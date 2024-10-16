#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

#include "serializer-api.h"
#include "serializer.h"

serializer_t *create_serializer() {
    serializer_t* serial = (serializer_t*)malloc(sizeof(serializer_t));
    // printf("Created a Serializer\n");
    if (serial == NULL) {
        printf("Memory allocation for serializer failed!\n");
        exit(1);
    }
    pthread_mutex_init(&serial->lock, NULL);
    serial->queues = (queue_t**)malloc(10 * sizeof(queue_t*));
    serial->crowds = (crowd_t**)malloc(sizeof(crowd_t *) * 10); 
    if (serial->queues == NULL) {
        printf("Memory allocation for queue vector failed!\n");
        exit(1);
    }
     if (serial->crowds == NULL) {
        printf("Memory allocation for crowd vector failed!\n");
        exit(1);
    }
    serial->queue_size = 0;
    serial->crowd_size = 0;
    return serial;
}

void destroy_serializer(serializer_t *serial) {
    if (serial) {
        pthread_mutex_destroy(&serial->lock);
        free(serial);
    }
    
}

void unlock_signal(serializer_t *serial){
    //printf("SIGNALING\n");
     for(int i=0;i<serial->queue_size;i++){
        if(serial->queues[i]->front!=NULL && serial->queues[i]->front->condition(NULL)){
            pthread_cond_signal(&serial->queues[i]->front->node_condV);
            break;
        }
    }
    //printf("FINISHED SIGNALING\n");
}


void enter_serializer(serializer_t *serial) {
   pthread_mutex_lock(&serial->lock);
   //printf("Entered Serializer\n");
}

void exit_serializer(serializer_t *serial) {
    //printf("Exiting serializer\n\n");
    unlock_signal(serial);
    pthread_mutex_unlock(&serial->lock);
}

crowd_t *create_crowd(serializer_t *serial) {
    crowd_t *crowd = (crowd_t *)malloc(sizeof(crowd_t));
   // printf("Created a Crowd\n");
    if (crowd) {
        crowd->count = 0;
        serial->crowds[serial->crowd_size] = crowd;
        serial->crowd_size++;
        if (serial->crowd_size >= 10) {
        serial->crowds = (crowd_t **)realloc(serial->crowds, (serial->crowd_size + 1) * sizeof(crowd_t *));
        
}
    }

    return crowd;
}

void destroy_crowd(serializer_t *serial, crowd_t *crowd) {

    // Free memory...
    if (crowd) {
        int index = -1;
        for (int i = 0; i < serial->crowd_size; i++) {
            if (serial->crowds[i] == crowd) {
                index = i;
                break;
            }
        }
        if (index != -1) {
            free(crowd);
            for (int i = index; i < serial->crowd_size - 1; i++) {
                serial->crowds[i] = serial->crowds[i + 1];
            }
            serial->crowd_size--;
        }
    }

}

void join_crowd(serializer_t *serial, crowd_t *crowd, void (*body)(void *), void *body_args) {
    // Put the executing thread into crowd
    // pthread_mutex_lock(&serial->lock);
     //printf("Joining the Crowd\n");
     crowd->count++;

    // Release possession of the serializer
    
    unlock_signal(serial);
    pthread_mutex_unlock(&serial->lock);
    // Execute list of statements
    body(body_args);

    // Perform leave_crowd operation
    pthread_mutex_lock(&serial->lock);
    //printf("Crowd count is %d\n", crowd->count);
    crowd->count--;
    //printf("LEAVE CROWD\n");
  //  pthread_mutex_unlock(&serial->lock);
}

int crowd_count(serializer_t *serial, crowd_t *crowd) {
    //printf("Entering the crowd Count function\n");
    int ret = crowd->count;
    return ret;
}
int queue_size(queue_t *queue){
    return queue->size;
}
queue_t *create_queue(serializer_t *serial) {
    queue_t* new_queue = (queue_t*)malloc(sizeof(queue_t));
    // printf("The address of the queue created is %p", new_queue);
    // printf("Creating the Queue\n");
    // printf("The address of the queue created is %p\n\n", new_queue);
    if (new_queue == NULL) {
        // printf("Memory allocation for new queue failed!\n");
        pthread_mutex_unlock(&serial->lock);  // Unlock before returning
       exit(1);
    }
    new_queue->front = NULL;
    new_queue->rear = NULL;
    new_queue->nodes = 0;
    new_queue->size = 0;
    serial->queues[serial->queue_size] = new_queue;
    serial->queue_size++;
    // if(serial->queue_size>10){
    //     serial->queues = (queue_t*)realloc(serial->queues, 20 * sizeof(queue_t));
    // }
    if (serial->queue_size >= 10) {
        serial->queues = (queue_t**)realloc(serial->queues, (serial->queue_size + 1) * sizeof(queue_t*));
    }
    return new_queue;
}

void destroy_queue(serializer_t *serial, queue_t *queue) {

    if (queue == NULL) {
        return;
    }
    Queue_Node* current = queue->front;
    while (current != NULL) {
        Queue_Node* temp = current;
        current = current->next;
        free(temp);  
    }

    for (int i = 0; i < serial->queue_size; i++) {
        if (serial->queues[i] == queue) {
            for (int j = i; j < serial->queue_size - 1; j++) {
                serial->queues[j] = serial->queues[j + 1];
            }
            serial->queue_size--;
            break;
        }
    }
    free(queue);
}
void enqueue(serializer_t *serial, queue_t *queue, bool (*cond)(void *)) {
    // Place the executing thread into end of queue
   //printf("Enqueuing the process\n");
//    printf("cond addresss while entering %p\n", cond);
   Queue_Node* temp = (Queue_Node*)malloc(sizeof(Queue_Node));
    if (temp == NULL) {
        printf("Memory allocation failed\n");
        exit(1); 
    }
    // printf("Queue Node Created\n");
    temp->condition = cond;    // node ---> cond, next
    temp->next = NULL;  
    temp->id = queue->nodes++;
    pthread_cond_init(&temp->node_condV,NULL); 
    queue->size++;
    // printf("the queue to which it's enqueueing is %p\n", queue);
    if (queue->rear == NULL && queue->front == NULL) {
        queue->front = temp;
        // printf("queue->front %p, temp %p\n", queue->front, temp);
        queue->rear = temp;
    }
    else{
        // printf("In else queue->front %p, temp %p\n", queue->front, temp);
         queue->rear->next = temp;
         queue->rear = temp;
    }
    // printf("Out of if and else queue->front %p, temp %p\n", queue->front, temp);
    // Check condition when head of queue is reached
    while((queue->front != temp) || (!(queue->front->condition(NULL)))){
        //printf("Head at %d and im at %d\n",queue->front->id, temp->id);
        //printf("Is it in loop for ever\n");
        unlock_signal(serial);
        pthread_cond_wait(&temp->node_condV, &serial->lock);
    }
     //printf("Head at %d while dequeueing is \n",queue->front->id);
 // Perform dequeue operation
    // printf("Before dequeue %p\n", queue->front);
    queue->size--;
    if(queue->front == queue->rear){
        queue->front = NULL;
        queue->rear = NULL;
    }else{
        queue->front = queue->front->next;
    }
    free(temp);
    //printf("After dequeue %p\n\n", queue->front);
   //pthread_mutex_unlock(&serial->lock); 
}