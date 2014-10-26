#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int occurance;
	char word[50];
} item;

typedef struct {
	item* items;
	int length;
	int index;
} dictionary;

int contains(dictionary dict, char* word) {
	int len = dict.length;
	int i;
	int flag = 0;

	for (i = 0; i < len; i++) {
		if (strcmp(dict.items[i].word, word) == 0) {
			return i;
		}
	}
	return -1;
}

void initialdict(dictionary* dict, int buffersize) {
	dict->items = (item*) malloc(buffersize * sizeof(item));
	int i;
	for (i = 0; i < buffersize; i++) {
		dict->items[i].occurance = 0;
		strcpy(dict->items[i].word, "");
	}
	dict->index = 0;
	dict->length = buffersize;
}

void morespace(dictionary* dict, int moresize) {
	dict->length += moresize;
	dict->items = (item*) realloc(dict->items, dict->length * sizeof(item));
	int i;
	for (i = dict->length - moresize; i < dict->length; i++) {
		dict->items[i].occurance = 0;
		strcpy(dict->items[i].word, "");		
	}
}

int main(int argc, char* argv[]) {
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

	// main program starts here haha
	// -----------------------------

	dictionary dict;
	int buffersize = 20;
	initialdict(&dict, buffersize);
	
	while(1) {
		
	    if (strlen(shm) != 0) {

	    	// if one line has multiple inputs, explode it >///<
	    	char* spl = strtok(shm, " ");

	    	while(spl != NULL) {
	    		// input in correct form, %word
	    		if (spl[0] == '%') {
	    			int index;
	    			int times;

	    			char* word = spl + 1; //skip %

	    			// find word in dictttt!
	    			if ((index = contains(dict, word)) != -1) {
	    				times = ++dict.items[index].occurance;
	    			}

	    			// dictionary full
	    			else if (dict.index == dict.length ) {	    				
	    				index = dict.index;

	    				// expand the dict
	    				morespace(&dict, buffersize);

						strcpy(dict.items[index].word, word);
						times = ++dict.items[index].occurance;
						dict.index += 1;
	    			}

	    			// dictionary not full, just append the word
	    			else {
	    				index = dict.index;
	    				strcpy(dict.items[index].word, word);
	    				times = ++dict.items[index].occurance;
	    				dict.index += 1;
	    			}

	    			printf("%s: %d times\n", word, times);
	    		}

	    	    spl = strtok(NULL, " ");
	    	}
	    	shm[0] = '\0';
	    }
	}
	return 0;
}