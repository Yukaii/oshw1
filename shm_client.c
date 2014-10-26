#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main(int argc, char* argv[]) {
	key_t shmkey; /* key to be passed to shmget() */ 
	int shmid; /* return value from shmget() */ 
	int size = 20; /* size to be passed to shmget() */ 
	pid_t pid;
	char* shm;

	if ((shmkey = ftok(".", 'a')) == (key_t) -1) {
		printf("key generate error\n");
		exit(1);
	}

	if ((shmid = shmget(shmkey, size,  IPC_CREAT | 0600)) < 0) {
		printf("error create shared memory: %d\n", shmid);
		exit(1);
	}

	if ((shm = (char*)shmat(shmid, NULL, 0)) == (char *) -1) {
		printf("shmat error\n");
		exit(1);
	}

	while(gets(shm) != EOF) {
		printf("> ");
	}
}