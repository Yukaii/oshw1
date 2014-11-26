#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {

	const int SIZE = 4096;
	const char *name = "dict";
	int shm_fd;
	void *ptr;
	char buf[1000] = {0};

	shm_fd = shm_open(name, O_CREAT|O_RDWR, 0666);
	ftruncate(shm_fd, SIZE);

	ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		return -1;
	}

	do {
		printf("> ");
		gets(buf);
		sprintf(ptr, "%s", buf);
	} while(1);

	return 0;
}
