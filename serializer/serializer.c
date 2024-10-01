#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

#include "serializer-api.h"
#include "serializer.h"

// typedef struct queue_node_t {
//     bool (*condition)(void*);
//     struct queue_node_t* next; 
// } Queue_Node;

// typedef struct queue_t {
//      Queue_Node* front;
//      Queue_Node* rear;
// }queue_t;

// typedef struct crowd_t {
//     int count;               
// } crowd_t;

// typedef struct serializer_t {
//     queue_t** queues;             
//     crowd_t** crowds; 
//     int queue_size;
//     int crowd_size;   
//     bool isPossessed;         
//     pthread_mutex_t lock;        
//     pthread_cond_t serializer_cond;    
// } serializer_t;

serializer_t *create_serializer() {
    serializer_t* serial = (serializer_t*)malloc(sizeof(serializer_t));
    // printf("Created a Serializer\n");
    if (serial == NULL) {
        printf("Memory allocation for serializer failed!\n");
        exit(1);
    }
    pthread_mutex_init(&serial->lock, NULL);
    pthread_cond_init(&serial->serializer_cond, NULL);
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
    serial->isPossessed = false;
    return serial;
}

void destroy_serializer(serializer_t *serial) {

    if (serial) {
        // for (int i = 0; i < serial->queue_size; i++) {
        //     free(serial->queues[i]);
        // }
        // free(serial->queues);

        // for (int i = 0; i <serial; i++) {
        //     free(serial->crowds[i]);
        // }
        // free(serial->crowds);
        pthread_cond_destroy(&serial->serializer_cond);
        pthread_mutex_destroy(&serial->lock);
        free(serial);
    }
    
}

void enter_serializer(serializer_t *serial) {
   pthread_mutex_lock(&serial->lock);
   printf("entered Serializer\n");
   while(serial->isPossessed){
     pthread_cond_wait(&serial->serializer_cond, &serial->lock);
   }

   serial->isPossessed = true;
   pthread_mutex_unlock(&serial->lock);
}

void exit_serializer(serializer_t *serial) {
    printf("Exiting serializer\n");
    pthread_mutex_lock(&serial->lock);
    serial->isPossessed = false;
    pthread_cond_broadcast(&serial->serializer_cond);
    pthread_mutex_unlock(&serial->lock);
}

crowd_t *create_crowd(serializer_t *serial) {
    crowd_t *crowd = (crowd_t *)malloc(sizeof(crowd_t));
    printf("Created a Crowd\n");
    if (crowd) {
        crowd->count = 0;
        serial->crowds[serial->crowd_size] = crowd;
        serial->crowd_size++;
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
     pthread_mutex_lock(&serial->lock);
     printf("Joining the Crowd\n\n");
     crowd->count++;

    // Release possession of the serializer
    serial->isPossessed = false;
    pthread_cond_broadcast(&serial->serializer_cond);
    pthread_mutex_unlock(&serial->lock);
    // Execute list of statements
    body(body_args);

    // Perform leave_crowd operation
    pthread_mutex_lock(&serial->lock);
    printf("Gained possen again after crowd\n");
    // while(!serial->isPossessed){
    //     printf("going in loop here???\n");
    //     pthread_cond_wait(&serial->serializer_cond, &serial->lock);
    // }
    serial->isPossessed = true;
    crowd->count--;
    pthread_mutex_unlock(&serial->lock);
}

int crowd_count(serializer_t *serial, crowd_t *crowd) {
    printf("Entering the crowd Count function\n");
    int ret = crowd->count;
    return ret;
}

queue_t *create_queue(serializer_t *serial) {
    queue_t* new_queue = (queue_t*)malloc(sizeof(queue_t));
    printf("Creating the Queue\n");
    if (new_queue == NULL) {
        printf("Memory allocation for new queue failed!\n");
        pthread_mutex_unlock(&serial->lock);  // Unlock before returning
       exit(1);
    }
    new_queue->front = NULL;
    new_queue->rear = NULL;
    serial->queues[serial->queue_size] = new_queue;
    serial->queue_size++;

    return new_queue;
}

void destroy_queue(serializer_t *serial, queue_t *queue) {

  if (queue == NULL) {
        return;
    }
    pthread_mutex_lock(&serial->lock);

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
    pthread_mutex_unlock(&serial->lock);

}

void enqueue(serializer_t *serial, queue_t *queue, bool (*cond)(void *)) {
    // Place the executing thread into end of queue
   pthread_mutex_lock(&serial->lock);
   printf("Enqueuing the process\n");
//    printf("cond addresss while entering %p\n", cond);
   Queue_Node* temp = (Queue_Node*)malloc(sizeof(Queue_Node));
    if (temp == NULL) {
        printf("Memory allocation failed\n");
        exit(1); 
    }
    // printf("Queue Node Created\n");
    temp->condition = cond;    // node ---> cond, next
    temp->next = NULL;  
    printf("%p queue->rear, %p queue->front\n", queue->rear, queue->front);
    if (queue->rear == NULL && queue->front == NULL) {
        queue->front = temp;
        printf("queue->front %p, temp %p\n", queue->front, temp);
        queue->rear = temp;
    }
    else{
        printf("In else queue->front %p, temp %p\n", queue->front, temp);
         queue->rear->next = temp;
         queue->rear = temp;
    }
    // printf("Out of if and else queue->front %p, temp %p\n", queue->front, temp);
    serial->isPossessed = false;
    pthread_cond_broadcast(&serial->serializer_cond);
    // Check condition when head of queue is reached
    int index = -1;
    printf("Serial->queue_size is %d\n", serial->queue_size);
    // for (int i = 0; i < serial->queue_size; i++) {
    //     printf("%p == %p\n", serial->queues[i], queue);
    //         if (serial->queues[i] == queue) {
    //             printf("%p == %p\n", serial->queues[i], queue);
    //             index = i;
    //             break;
    //         }
    // }
     //printf("The value of the condition call is %d",queue->front->condition(NULL));
    // printf("cond address is %p\n",queue->front->condition);
    printf("queue->front is %p, temp is %p\n", queue->front, temp);
   
    while((queue->front != temp) || (!(queue->front->condition(NULL)))){
        printf("Is it in loop for ever\n");
        // pthread_cond_signal(&serial->serializer_cond);
        pthread_cond_wait(&serial->serializer_cond, &serial->lock);
    }
 // Perform dequeue operation
    serial->isPossessed = true;
    printf("Before dequeue %p\n", queue->front);
    Queue_Node* temp2 = queue->front;
    if(queue->front == queue->rear){
        queue->front = NULL;
        queue->rear = NULL;
    }else{
        queue->front = queue->front->next;
    }
    
    printf("After dequeue %p\n", queue->front);
    pthread_mutex_unlock(&serial->lock); 
}
