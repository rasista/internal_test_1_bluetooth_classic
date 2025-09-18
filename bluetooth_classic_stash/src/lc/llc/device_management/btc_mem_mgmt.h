#ifndef BTC_MEM_MGMT_H_
#define BTC_MEM_MGMT_H_
#include "stdbool.h"
#include "btc_common_defines.h"

#define BUFFER_SIZE 1124
#define NUM_BUFFERS 1

void init_buffer_pool();
char* allocate_buffer();
void free_buffer(char* buffer);

#endif
