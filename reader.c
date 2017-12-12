#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>
#include "cyclicBuf.h"
#include "gen.h"

int main()
{
	int memFd = shm_open("homework_memory", O_RDONLY, 0);
	if (memFd == -1)
	{
		perror("Can't open file");
		return 1;
	}

	struct cyclic_buf *buffer = mmap(NULL, sizeof(struct cyclic_buf), PROT_READ, MAP_SHARED, memFd, 0);
	if (buffer == NULL)
	{
		perror("Can't mmap");
		return -1;
	}

	uint64_t pos = buffer->wr_pos;
	uint32_t seed, prev_seed;
	bool first = true;
	while (true)
	{
		if (buffer->wr_pos > pos + CYCLIC_BUFFER_SIZE)
		{
			printf("wtf reader:%lu writer:%lu\n", pos, buffer->wr_pos);
			printf("resynchronizing\n");
			pos = buffer->wr_pos;
			first = true;
		}
		while (buffer->wr_pos == pos)
			;
		prev_seed = seed;
		seed = verify((void *)buffer->array[pos % CYCLIC_BUFFER_SIZE]);
		printf("verified [%lu] - seed %d\n", pos, seed);
		if (seed == -1)
		{
			printf("block inconsistent");
			return 1;
		}
		if (!first && seed != prev_seed + 1)
		{
			printf("blocks discontinuous");
			return 2;
		}
		pos++;
		first = false;
	}

	return 0;
}
