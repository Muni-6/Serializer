#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "child-care-api.h"
#include "child-care.h"


void init_child_care() {
    ser = create_serializer();
    
    caregiver_arrive_queue=create_queue(ser);
    caregiver_depart_queue=create_queue(ser);
    caregiver_arrive_crowd=create_crowd(ser);
    caregiver_depart_crowd=create_crowd(ser);

    child_arrive_queue=create_queue(ser);
    child_depart_queue=create_queue(ser);
    child_arrive_crowd=create_crowd(ser);
    child_depart_crowd=create_crowd(ser);
    // Create only the necessary crowds
     CAREGIVER_ARRIVED = 0;
     CAREGIVER_READY_DEPART = 0;
     CHILD_ARRIVED = 0;
     CHILD_READY_DEPART = 0;
     CHILD_INTO_DEPART = 0;
     CHILD_DEPART = 0;
     CAREGIVER_DEPART = 0;
     CHILD_READY_DEPARTING =0;

}

void finish_child_care() {
    // Destroy only the initialized crowds and queues
    printf("Destroying\n\n\n");
    destroy_crowd(ser, caregiver_arrive_crowd);
    destroy_crowd(ser, child_arrive_crowd);
    destroy_crowd(ser, caregiver_depart_crowd);
    destroy_crowd(ser, child_depart_crowd);
    
    destroy_queue(ser, caregiver_arrive_queue);
    destroy_queue(ser, caregiver_depart_queue);
    destroy_queue(ser, child_arrive_queue);
    destroy_queue(ser, child_depart_queue);
    
    destroy_serializer(ser);
}

bool caregiver_arrive_cond(void *arg) {
    return true;
}

bool child_depart_cond(void *arg) {
    // return CHILD_READY_DEPART>0;
    //childReadyDeparting>0 && CHILDreaddepart>=child ready departing
    return (CHILD_READY_DEPARTING>0) && (CHILD_READY_DEPART>0);
}

bool caregiver_depart_cond(void *arg) {
//  //childArived>0 && ChildDepart>0; caregiverarrived>0 && caregiverdepart>1; caregiverReaadyDepart>0;
//  bool cond1 = ((CHILD_ARRIVED - CHILD_READY_DEPART)==0) && (CHILD_INTO_DEPART==0);
//  printf("THE CONDITION1 is %d\n", cond1);
//  bool cond2 = (CAREGIVER_ARRIVED>0) || (CAREGIVER_READY_DEPART>1);
//  printf("THE CONDITION2 is %d\n", cond2);
//  bool cond3 = CAREGIVER_READY_DEPART>0;
//  printf("THE CONDITION3 is %d\n", cond3);

//  return cond3 && (cond1 || cond2);
//childArived==0 && ChildReadyDeparting==0 && childdepart==0; caregiverarrived>1 && caregiverarrived>0; 
    bool cond1 = (CHILD_ARRIVED == 0) && (CHILD_READY_DEPARTING ==0) && (CHILD_DEPART == 0) && (CHILD_READY_DEPART == 0);
    bool cond2 = CAREGIVER_ARRIVED > 1;
    bool cond3 = CAREGIVER_ARRIVED > 0;

    return (cond3) && (cond1 || cond2);
}

bool child_arrive_cond(void *arg) {
//   //atleast onecare giver CAREGIVER_ARRIVED>0
//   return CAREGIVER_ARRIVED>0 || CAREGIVER_READY_DEPART>0;

    return CAREGIVER_ARRIVED > 0;
}

void *caregiver_arrive_thread(void *arg) {
    tdata_t *td = (tdata_t *)arg;

    enter_serializer(ser);
    
    enqueue(ser, caregiver_arrive_queue, caregiver_arrive_cond);//return true
    
    join_crowd(ser, caregiver_arrive_crowd, td->body, td->body_arg);

    CAREGIVER_ARRIVED++; // CARE GIVER ARRIVED
    exit_serializer(ser);
    pthread_exit(NULL);
}

void *caregiver_depart_thread(void *arg) {
    tdata_t *td = (tdata_t *)arg;

    enter_serializer(ser);
    printf("Care giver departing  queue\n");
    enqueue(ser, caregiver_depart_queue, caregiver_depart_cond);//childArived==0 && ChildReadyDeparting==0 && childdepart==0; caregiverarrived>1 && caregiverarrived>0; //
    CAREGIVER_ARRIVED--;
    CAREGIVER_DEPART++;
    printf("Care giver joining departing  crowd\n");
    join_crowd(ser, caregiver_depart_crowd, td->body, td->body_arg);
    CAREGIVER_DEPART--;
    printf("EXIT caregiver departed\n");
    exit_serializer(ser);
   
    pthread_exit(NULL);
}

void *child_arrive_thread(void *arg) {
    tdata_t *td = (tdata_t *)arg;

    enter_serializer(ser);
    printf("Child entering the arriving queue\n");
     CHILD_ARRIVED++; //Child arrived here
    enqueue(ser, child_arrive_queue, child_arrive_cond);//atleast onecare giver CAREGIVER_ARRIVED>0

   
    printf("Child entering the arrival join crowd queue\n");
    join_crowd(ser, child_arrive_crowd, td->body, td->body_arg);

    CHILD_ARRIVED--; //when he is exiting means he is ready to depart and is no more in arrival;
    CHILD_READY_DEPART++;//he is now ready to depart
    printf("EXIT child arrive exited\n"); 
    exit_serializer(ser);
    pthread_exit(NULL);
}

void *child_depart_thread(void *arg) {
    tdata_t *td = (tdata_t *)arg;

    enter_serializer(ser); 
    printf("Child entering the departing  queue\n");
    
    CHILD_READY_DEPARTING++;
    enqueue(ser, child_depart_queue, child_depart_cond);//childReadyDeparting>0 && CHILDreaddepart>0
    
    //CHILD_READY_DEPART--; // now not in the depart queue
    CHILD_DEPART++; // Joined the depart queue and performing the functionality;
    CHILD_READY_DEPART--;   
    CHILD_READY_DEPARTING--;
    printf("Child entering the departing  crowd\n");
    join_crowd(ser, child_depart_crowd, td->body, td->body_arg);
    CHILD_DEPART--; //now departed the child center completely
    printf("EXIT child depart\n");
    exit_serializer(ser);

    pthread_exit(NULL);
}