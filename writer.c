#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include "cyclicBuf.h"
#include "gen.h"
#include <signal.h>

static volatile bool keepRunning = 1;

void signalHandler(int signo)
{
	//printf("exiting...");
	if (signo == SIGINT)
		keepRunning = 0;
}

int main()
{
	int memFd = shm_open("homework_memory", O_CREAT | O_RDWR, S_IRWXU);
	if (memFd == -1)
	{
		perror("Can't open file");
		return 1;
	}

	int res;
	res = ftruncate(memFd, sizeof(struct cyclic_buf));
	if (res == -1)
	{
		perror("Can't truncate file");
		return res;
	}

	struct cyclic_buf *buffer = mmap(NULL, sizeof(struct cyclic_buf), PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
	if (buffer == NULL)
	{
		perror("Can't mmap");
		return -1;
	}
	uint32_t seed = 0;
	//uint64_t initial_pos = 1337 % CYCLIC_BUFFER_SIZE;
	//generate(&mem->array[initial_pos], 0);
	//mem->array[initial_pos][0] = seed;
	//buffer->wr_pos = initial_pos;
	int writes = 0;
	struct sigaction sa;
	sa.sa_handler = signalHandler;
	sigaction(SIGINT, &sa, NULL);

	while (keepRunning)
	{
		//printf("%d\n", writes++);
		//usleep(1000);
		//sleep(1);
		generate((void *)buffer->array[buffer->wr_pos], seed);

		printf("generated at [%lu] with seed %d\n", buffer->wr_pos, seed);
		buffer->wr_pos++;
		buffer->wr_pos %= CYCLIC_BUFFER_SIZE;
		//if (writes++ == 9000) continue; //test discontinuity check
		seed++;
	}
	//cleanup?
	return 0;
}
