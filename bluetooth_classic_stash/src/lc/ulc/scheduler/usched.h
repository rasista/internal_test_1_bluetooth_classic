#ifndef USCHED_H
#define USCHED_H

#include <stdint.h>
#include <string.h>

extern struct usch_s usch;
typedef uint32_t exec_timing_t;

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define USCH_FLAG_SCHEDULING_BUSY    2 //!< Rescheduling is already busy
#define USCH_FLAG_INIT_OR_START_SENT 4 //!< Init or Start event sent to the task
#define USCH_FLAG_PENDING            8 //!< Task is pending to be scheduled
#define SL_STATUS_SUCCESS            0
#define USCH_TASK_FLAG_IMMOVABLE                1 //<!Task start point can't be moved
#define USCH_TASK_FLAG_INIT_MEASURE_REQ         2 //!< Init time (re)measurement requested for the task
#define USCH_TASK_FLAG_DISABLE_INIT_MEASUREMENT 3 //!< Keep init time measurements disabled for the task
#define USCH_TASK_FLAG_IMMEDIATE                4 //!< Request to execute task immediately
#define USCH_TASK_FLAG_SLACK_ENABLED            5 //!< Apply slack only once.

#define USCH_SCHED_FAIL_TOO_LATE 1
#define USCH_SCHED_FAIL_OVERLAP  2
#define USCH_SCHED_FAIL_HALT     3
#define INVALID_TASK              BIT(0)
#define REMOVE_TASK               BIT(1)

enum usch_taskEvent {
  /**
   * Task should prepare to be started
   * -Called at least initTime before start
   */
  usch_taskEvent_Init,
  /**
   * Request Stopping task
   * This may be sent anytime after init or start has been sent
   * - Task must release all shared resources used
   * - Task must not call any other scheduler functions when handling this event
   */
  usch_taskEvent_Stop,
  /**
   * Unable to schedule task
   * - Task must reschedule itself
   * - Task must not modify any shared resource(radio)
   * - This is only sent when task has not been scheduled yet
   */
  usch_taskEvent_ScheduleFail,
};

typedef struct usch_Task {
  struct usch_Task *next; //<! Used internally by Scheduler
  uint32_t start;           //<! Absolute time when task must start, Remove task from scheduler before updating
  exec_timing_t
    initTime; //<! Dynamically updated initialization time the task requires before calling start, if 0 init not called
  uint32_t minRuntime; //<! Minimum running time for task, must be greater than 0
  uint32_t maxRuntime; //<! Maximum running time for task, must be equal or greater than minRuntime
  uint32_t flags;      //<! Task configuration flags
  uint32_t priority;
  uint8_t task_id;
  uint32_t delay;
  uint8_t status_flags;
  void (*handler)(uint8_t event, struct usch_Task *);
  void *user_data;
} usch_Task_t;

struct usch_s {
  uint32_t flags;                    /* 0-15  for scheduler internal use
                                      * 16-31 for scheduler configuration options
                                      * Perhaps should split flags to to 2 x 16 bits? */
  usch_Task_t *head;               //!<linked list of tasks, sorted by startup time
  usch_Task_t *waiting_queue_head; //!<linked list of tasks pending to be scheduled, sorted by priority
  usch_Task_t *scheduled;          //!< Task currently scheduled
  uint32_t endTime;                  //!< current task end time
  struct {
    uint16_t tasks;
    uint16_t slips;
    uint16_t failures;
    uint16_t overdues;
    uint16_t overlappings;
    uint16_t late_recovered;
    uint16_t late_ends;
  } counters;
#define USCH_COUNTER(NAME) \
  do {                     \
    usch.counters.NAME++;  \
  } while (0)
};

uint32_t find_white_spaces(uint32_t best_possible_start,
                           uint32_t worst_possible_end,
                           uint32_t min_duration,
                           uint32_t max_duration,
                           uint32_t overhead,
                           usch_Task_t *task);
void usch_RemoveTask(usch_Task_t *task, bool finished);
void usch_AddTask(usch_Task_t *newTask);
void usch_ScheduleProcess(void);
void usch_future_task(void);
void usch_remove_future_task(void);
void usch_SendEvent(uint8_t event);
void task_fail(usch_Task_t *task, uint8_t cause);
#endif
