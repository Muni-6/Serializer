#pragma once

#include "serializer/serializer-api.h"

// Child-care problem data structures
serializer_t *ser;
crowd_t *caregiver_arrive_crowd;
crowd_t *child_arrive_crowd;
crowd_t *caregiver_depart_crowd ;
crowd_t *child_depart_crowd;

int CAREGIVER_ARRIVED;
int CAREGIVER_READY_DEPART;
int CHILD_ARRIVED;
int CHILD_READY_DEPART;
int CHILD_DEPART;
int CAREGIVER_DEPART;
int CHILD_READY_DEPARTING;


queue_t *caregiver_arrive_queue;
queue_t *caregiver_depart_queue;
queue_t *child_arrive_queue;
queue_t *child_depart_queue;