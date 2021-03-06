/* LICENSE
 * 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>

#include "logger.h"
#include "message.h"
#include "msgqueue.h"


/**
 * @brief add @p msg to @p queue
 * @param queue the queue to deal with
 * @param msg the ::message to add
 * @returns 0 on success, -1 on error.
 */
int enqueue_message(struct msgqueue *queue, message *msg) {
  msg_node *m;
  
  assert(msg!=NULL);
  
  pthread_mutex_lock(&(queue->control.mutex));
  if(queue->control.active) {
    if(!(m=malloc(sizeof(msg_node)))) {
      pthread_mutex_unlock(&(queue->control.mutex));
      print( ERROR, "malloc: %s", strerror(errno) );
      return -1;
    }
    m->next=NULL;
    m->msg=msg;
    
    queue_put(&(queue->list), (node *)m);
    pthread_mutex_unlock(&(queue->control.mutex));
    
    pthread_cond_broadcast(&(queue->control.cond));
    
    return 0;
  } else {
    pthread_mutex_unlock(&(queue->control.mutex));
    
    print( ERROR, "message queue disabled" );
    
    return -1;
  }
}