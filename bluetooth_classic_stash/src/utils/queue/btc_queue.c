#include "btc_queue.h"
#include "stdio.h"
#include "debug.h"

/* Function to add to the queue*/
void enqueue(queue_t *queue, node_t *node)
{
  ACQUIRE_LOCK(queue->plt_lock.lock);
  // If head is NULL, then the queue is empty
  if (queue->head == NULL) {
    queue->head = node;
    queue->tail = node;
    queue->size = 1;
  } else {
    // Add the node to the end of the queue
    queue->tail->next = node;
    queue->tail = node;
    queue->size++;
  }
  // Exit critical section
  RELEASE_LOCK(queue->plt_lock.lock);
}

// Function to remove from the queue
node_t *dequeue(queue_t *queue)
{
//enter critical section
  ACQUIRE_LOCK(queue->plt_lock.lock);
//If head is NULL, then the queue is empty
  if (queue->head == NULL) {
    RELEASE_LOCK(queue->plt_lock.lock);
    return NULL;
  } else {
    //Remove the node from the front of the queue
    node_t *node = queue->head;
    queue->head = queue->head->next;
    queue->size--;
    RELEASE_LOCK(queue->plt_lock.lock);
    node->next = NULL;    //! This is important to avoid memory leaks
    return node;
  }
}
