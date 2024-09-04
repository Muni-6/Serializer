#pragma once

#include "serializer/serializer-api.h"

// Reader-writer problem data structures
serializer_t *ser;
crowd_t *read_c;
crowd_t *write_c;
queue_t *read_q;
queue_t *write_q;
