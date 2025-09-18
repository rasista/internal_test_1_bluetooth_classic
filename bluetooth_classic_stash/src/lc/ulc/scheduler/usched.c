#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "usched.h"
#include "usched_port.h"

#define USCH_OVERHEAD 0 // TODO: measure where this comes from
uint32_t timer_invoke_count = 0;
uint8_t timer_role = 0;
struct usch_s usch;

/**
 * Clear scheduled task
 */
static void clear_scheduled_task(bool finished)
{
  if (bg_bit_isset(usch.flags, USCH_FLAG_INIT_OR_START_SENT)) {
    bg_bit_clr(&usch.flags, USCH_FLAG_INIT_OR_START_SENT);
    //if init is called then deinit task, but only if not already finished
    if (finished == false) {
      usch_sendEventToTask(usch_taskEvent_Stop, usch.scheduled);
    }
  }

  usch.scheduled = NULL;
}

/**
 * Add task to queue so that tasks are ordered by start time and priority
 * of scheduled tasks
 */
static void add_task_to_queue(usch_Task_t *newTask)
{
  BTC_PRINT("\n add_task_to_queue\n");
  if (bg_bit_isset(newTask->flags, USCH_TASK_FLAG_IMMEDIATE)) {
    // immediate task is expected to be scheduled now, therefore set current
    // time to task start time so that task is put to the head of queue
    newTask->start = usch_GetTimeCB();
  }
  usch_Task_t **queue = &usch.head;
  while (*queue) {
    // check that task is not already in queue
    BG_ASSERT(*queue != newTask);
    if (!usch_timeGreaterThan(newTask->start, (*queue)->start)) {
      break;
    }
    queue = &(*queue)->next;
  }
  newTask->next = *queue;
  *queue        = newTask;
}

//3 args - bestpossible start, worst poss end, minimum duration, max duration
//1. minimum time > best possible start
//2. start < worst poss end - minimum dura
//3. cap on found slot and update it only if bigger than the current slot is available
//4. if slot is >= max duration return the slot
//5. slot > minimum duration
uint32_t find_white_spaces(uint32_t best_possible_start,
                           uint32_t worst_possible_end,
                           uint32_t min_duration,
                           uint32_t max_duration,
                           uint32_t overhead,
                           usch_Task_t *task)
{
  usch_Task_t *queue     = usch.head;
  uint32_t start           = best_possible_start;
  // uint32_t old_start       = 0;
  while (queue) {
    if (queue != task) {
      //! Check if the current min_max_duration fits before the head start
      if ((start + max_duration + overhead) < queue->start) {
        return start;
      } else if ((start + min_duration + overhead) < queue->start) {
        return start;
      } else if (queue->priority > task->priority) { //! If Queue task priority is less than current task, return start
        queue = queue->next;
        continue;
      }

      if ((queue->start + queue->initTime + queue->maxRuntime + USCH_OVERHEAD + overhead <= start) && (queue->priority <= task->priority)) {
        //! Current start is greater than the queue task end, no need to update start, check for next Queued task and compare
        queue = queue->next;
        continue;
      }
      // old_start = start;
      if (usch.scheduled == queue) {
        start = usch.endTime + USCH_OVERHEAD + overhead;
      } else {
        start = queue->start + queue->initTime + queue->minRuntime + USCH_OVERHEAD + overhead;
      }
      if (start < (worst_possible_end - min_duration - overhead)) {
        //! Check with next node
      } else {
        //! Already worst end time. So continue with best possible start as proper start is not found
        start = best_possible_start;
        break;
      }
    }
    queue = queue->next;
  }
  return start;
}

/**
 * @brief Tries to add task to queue
 *
 * @param newTask
 */
static void add_task(usch_Task_t *newTask)
{
  newTask->next = NULL;
  if (bg_bit_isset(newTask->flags, USCH_TASK_FLAG_INIT_MEASURE_REQ)) {
    bg_bit_clr(&newTask->flags, USCH_TASK_FLAG_INIT_MEASURE_REQ);
  }
  add_task_to_queue(newTask);
  usch_Task_t *curr = usch.scheduled;
  if (curr == NULL) {
    return;
  }
  // Check if currently running task is lower priority than the newly added and
  // the new and currently scheduled tasks overlap.
  if (newTask->priority < curr->priority) {
    if (bg_bit_isset(newTask->flags, USCH_TASK_FLAG_IMMEDIATE)) {
      clear_scheduled_task(false);
    } else if (usch_timeGreaterThan(newTask->start, (curr->start + curr->maxRuntime)) == false) {
      clear_scheduled_task(false);
    }
  } else if (usch_timeGreaterThan(curr->start - ll_execTimingGetCurrentValue(curr->initTime),
                                  newTask->start + newTask->minRuntime)) {
    //If new task fits before already scheduler curr->task
    //Current task must not be started, and there must be enough time to initialize of current task again
    clear_scheduled_task(false);
  }
  BTC_PRINT("Time: %d, add_task\n", usch_GetTimeCB());
}

/**
 * @brief Triggers schedule fail procedure
 *
 * @param task
 * @param cause
 */
void task_fail(usch_Task_t *task, uint8_t cause)
{
  (void)cause;
  USCH_COUNTER(failures);
  usch_sendEventToTask(usch_taskEvent_ScheduleFail, task);
  // If task scheduling fails let's re-measure the init timing.
  // Once the task will be added to the scheduling queue next time
  // the counter will be set.
  //
  // TODO: the scheduler utest MOCKs should be improved such that
  //       the measurement could be enabled. Calling the MOCK task init
  //       function should advance the virtual time and the scheduler
  //       should be able to notice it. At the moment, if the measurements
  //       are enabled, the init time becomes zero and various tests start
  //       failing.
}

/**
 * @brief Adds task to queue and requests scheduling
 *
 * @param newTask
 */
void usch_AddTask(usch_Task_t *newTask)
{
  BTC_PRINT("%d/t Add task: Priority: %d, Start: %d, MinRuntime: %d, MaxRuntime: %d, Role: %d\n", usch_GetTimeCB(), newTask->priority, newTask->start, newTask->minRuntime, newTask->maxRuntime, newTask->task_id);
  BTC_PRINT("%d %d set_state %d %s\n", usch_GetTimeCB(), newTask->task_id, 1, "usch_AddTask");
  add_task(newTask);
  usch_ScheduleReqCB();
}

/**
 * @brief Removes/clears the task
 *
 * @param task
 * @param finished
 */
void usch_RemoveTask(usch_Task_t *task, bool finished)
{
  //Task is removed from list, not scheduled anymore
  bg_bit_clr(&task->flags, USCH_TASK_FLAG_SLACK_ENABLED);
  if (usch.scheduled == task) {
    clear_scheduled_task(finished);
  }

  usch_Task_t **p = &usch.head;

  while ((*p)) {
    if ((*p) == task) {
      //remove p from between
      (*p) = (*p)->next;
      usch_ScheduleReqCB();
      return;
    }
    p = &(*p)->next;
  }

  usch_ScheduleReqCB();
}

/**
 * @brief Checks if task is schedulable based on time and set flags
 *
 * @param task
 * @param task_start
 * @param curr_time
 */
inline static bool isTaskSchedulable(usch_Task_t *task, uint32_t task_start, uint32_t curr_time)
{
  int32_t diff = (task_start - ll_execTimingGetCurrentValue(task->initTime)) - curr_time;

  if (!(task->flags & USCH_TASK_FLAG_IMMOVABLE)) {
    diff += (task->maxRuntime - task->minRuntime);
  }

  return diff >= 0;
}

/**
 * @brief usch_TrySchedule tries to get next task for scheduling
 *
 * @return false if reschedule happened, call function again
 */
static bool usch_TrySchedule(void)
{
  usch_Task_t *task = usch.head;

  if (!task) {
    return true; //nothing to do
  }

  uint32_t curr_time    = usch_GetTimeCB();
  uint32_t task_runtime = task->maxRuntime;

  //check if there is slack to adjust delta
  int32_t delta = 0;

  if (bg_bit_isset(task->flags, USCH_TASK_FLAG_IMMEDIATE)) {
    task->start = curr_time + ll_execTimingGetCurrentValue(task->initTime);
  } else {
    delta = (task->start - ll_execTimingGetCurrentValue(task->initTime)) - curr_time;
  }

  // delta ~ "how much too late we are now"
  if (((task->flags & USCH_TASK_FLAG_IMMOVABLE) == 0) && (delta < 0)
      && (!(bg_bit_isset(task->flags, USCH_TASK_FLAG_SLACK_ENABLED)))) {
    int32_t slack = task->maxRuntime - task->minRuntime; // How much we can shorten the task
    //add slack to delta
    delta += slack;
    // If this takes us back to positive time, start immediately & reduce duration
    if (delta >= 0) {
      uint32_t start = curr_time + ll_execTimingGetCurrentValue(task->initTime);
      // Decrease task run time by how much we postponed it
      BG_ASSERT(task_runtime >= (start - task->start));
      task_runtime -= start - task->start;
      task->start = start;
      USCH_COUNTER(late_recovered);
      bg_bit_set(&task->flags, USCH_TASK_FLAG_SLACK_ENABLED);
    }
  }

  //instant passed, scheduling failed
  if (delta < 0) {
    USCH_COUNTER(overdues);
    task_fail(task, USCH_SCHED_FAIL_TOO_LATE);
    //Return false for task scheduler to restart
    return false;
  }

  //loop through rest of tasks and check for overlap
  usch_Task_t *it;
  for (it = task->next; it != NULL; it = it->next) {
    uint32_t it_start = it->start;

    if (bg_bit_isset(it->flags, USCH_TASK_FLAG_IMMEDIATE)) {
      it_start = curr_time + ll_execTimingGetCurrentValue(it->initTime);
    }

    //absolute time when iterated task must be init at the latest
    uint32_t it_init = it_start - ll_execTimingGetCurrentValue(it->initTime);
    if ((!(it->flags & USCH_TASK_FLAG_IMMOVABLE)) && (!(bg_bit_isset(task->flags, USCH_TASK_FLAG_SLACK_ENABLED)))
        && (task->priority < it->priority)) {
      it_init += (it->maxRuntime - it->minRuntime);
    }

    //how much time to start of iterated task from current task end
    int32_t time_overlapping = task_runtime + task->start - it_init;

    //This task is not overlapping, but there might be more tasks that can overlap this
    if (time_overlapping <= 0) {
      continue;
    }

    //If task overlaps too much, so that not even minimum time is left include scheduling overhead in minimum time
    if (task_runtime <= (uint32_t)time_overlapping /* if this wraps, the next check fails */
        || (task_runtime - time_overlapping) < (task->minRuntime + USCH_OVERHEAD)) {
      if (!(task->flags & USCH_TASK_FLAG_IMMOVABLE)) {
        // schedule second task first if current task has enough slip time
        uint32_t start_time_candidate = it_start + it->minRuntime + ll_execTimingGetCurrentValue(task->initTime);
        int32_t task_time_candidate     = task->maxRuntime - (start_time_candidate - task->start);
        if (task_time_candidate > (int32_t)(task->minRuntime + USCH_OVERHEAD) && isTaskSchedulable(it, it_start, curr_time)
            && (task->priority > it->priority)) {
          USCH_COUNTER(slips);
          task         = it;
          task->start  = it_start;
          task_runtime = it->minRuntime;
          continue;
        }
      }
      //Task priority check to find out which task is allowed to run
      //later overlapping task will fail
      if (it->priority < task->priority) {
        if (!isTaskSchedulable(it, it_start, curr_time)) {
          task_fail(it, USCH_SCHED_FAIL_TOO_LATE);
          return false;
        }
        if (bg_bit_isset(task->flags, USCH_TASK_FLAG_IMMEDIATE)) {
          task         = it;
          task->start  = it_start;
          task_runtime = it->minRuntime;
          continue;
        }
        USCH_COUNTER(overlappings);
        task_fail(task, USCH_SCHED_FAIL_OVERLAP);
        return false;
      }
    } else {
      BG_ASSERT(task_runtime >= (uint32_t)time_overlapping);
      task_runtime -= time_overlapping;
    }
  }
  usch.endTime   = task_runtime + task->start + USCH_OVERHEAD;
  usch.scheduled = task;

  return true;
}

/**
 * @brief Tries to schedule the task based on set flags
 */
void usch_ScheduleProcess(void)
{
  if ((bg_bit_isset(usch.flags, USCH_FLAG_SCHEDULING_BUSY))
      || (bg_bit_isset(usch.flags, USCH_FLAG_INIT_OR_START_SENT))) {
    return; //task already scheduled
  }
  usch_Task_t *old_scheduled_task = usch.scheduled;

  bg_bit_set_atomic(&usch.flags, USCH_FLAG_SCHEDULING_BUSY);
  //loop until next task has been found
  while (usch_TrySchedule() == false) {
  }
  bg_bit_clr_atomic(&usch.flags, USCH_FLAG_SCHEDULING_BUSY);
  usch_Task_t *task = usch.scheduled;
  if (!task) { //no tasks to schedule
    return;
  }
  //! If new task is scheduled, then start timer, else skip this
  if (old_scheduled_task != usch.scheduled) {
    if (usch.scheduled == NULL) {
      return;
      /* Not expected -  Timer is expected to be stopped in usch_future_task() when queue is empty*/
    }
    if (usch.scheduled->start > usch_GetTimeCB() + 2) {
      timer_invoke_count = usch.scheduled->start;
      timer_role = usch.scheduled->task_id;
      // set the pending flag
      bg_bit_set(&usch.flags, USCH_FLAG_PENDING);
    } else {
      // ! else sart task straight away
      timer_invoke_count = 0;
      timer_role = 0;
      bg_bit_clr(&usch.flags, USCH_FLAG_PENDING);
      USCH_COUNTER(tasks);
      // Save the counter value as the task can modify it during init.
      bg_bit_set(&usch.flags, USCH_FLAG_INIT_OR_START_SENT);
      usch_sendEventToTask(usch_taskEvent_Init, task);
    }
  }
}

//! function to send usch_taskEvent_Init, when timer expires clean the flag
void usch_future_task(void)
{
  usch_Task_t *task = usch.scheduled;
  //print which task is it
  BTC_PRINT("%d/t usch_future_task: Priority: %d, Start: %d, MinRuntime: %d, MaxRuntime: %d\n", usch_GetTimeCB(), task->priority, task->start, task->minRuntime, task->maxRuntime);
  timer_invoke_count = 0;
  bg_bit_clr(&usch.flags, USCH_FLAG_PENDING);
  USCH_COUNTER(tasks);
  // Save the counter value as the task can modify it during init.
  bg_bit_set(&usch.flags, USCH_FLAG_INIT_OR_START_SENT);
  usch_sendEventToTask(usch_taskEvent_Init, task);
}

//! function to remove the future task when called
void usch_remove_future_task(void)
{
  if (usch.scheduled != NULL && bg_bit_isset(usch.flags, USCH_FLAG_PENDING)) {
    bg_bit_clr(&usch.flags, USCH_FLAG_PENDING);
    timer_invoke_count = 0;
    timer_role = 0;
    usch_RemoveTask(usch.scheduled, false);
    // Optionally, clear the scheduled task pointer
    // usch.scheduled = NULL;
  } else {
    BTC_PRINT("No future task scheduled or not in pending state.\n");
  }
}
