#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>


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

	// main program starts here haha
	// -----------------------------
	char* shm = (char*) ptr;

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
	    				printf("yes\n");
	    			}

	    			// dictionary full
	    			else if (dict.index == dict.length ) {	    				
	    				index = dict.index;

	    				// expand the dict
	    				morespace(&dict, buffersize);

						strcpy(dict.items[index].word, word);
						times = ++dict.items[index].occurance;
						dict.index += 1;
						printf("no\n");
	    			}

	    			// dictionary not full, just append the word
	    			else {
	    				index = dict.index;
	    				strcpy(dict.items[index].word, word);
	    				times = ++dict.items[index].occurance;
	    				dict.index += 1;
	    				printf("no\n");
	    			}

	    		}

	    		else if (spl[0] == '$') {
	    			printf("\n\n%15s\n", "TABLE");
	    			printf("%10s%15s\n", "word", "occurance");
	    			int i;
	    			for (i = 0; i < dict.index; i++) {
	    				printf("%10s%15d\n", dict.items[i].word, dict.items[i].occurance);
	    			}
	    			printf("\n\n");
	    		}

	    	    spl = strtok(NULL, " ");
	    	}
	    	shm[0] = '\0';
	    }
	}
_OUT:
	return 0;
}