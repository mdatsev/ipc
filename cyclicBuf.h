#ifndef _DUMB_SHM_STRUCT
#define _DUMB_SHM_STRUCT

#include <stdint.h>
#include "gen.h"

#define CYCLIC_BUFFER_SIZE (1 << 14)

struct cyclic_buf
{
	volatile uint64_t wr_pos;
	volatile int array[CYCLIC_BUFFER_SIZE][GEN_BLOCK_SIZE];
};

#endif
