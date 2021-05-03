#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_THREADS 12

#define ARRAY_SIZE 8000000
#define LINE_SIZE 2000	

char input_array[ARRAY_SIZE][LINE_SIZE];
int char_sums[ARRAY_SIZE];
int line_lengths[ARRAY_SIZE];
int line_avgs[ARRAY_SIZE];

pthread_mutex_t mutexsum;

void init_arrays()
{
    int i, err;
    FILE* fd;

    // Read in the lines from the data file

    fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
    for (i = 0; i < ARRAY_SIZE; i++) {
        err = fscanf(fd, "%[^\n]\n", input_array[i]);
        if (err == EOF) break;
        char_sums[i] = 0;
    }

    fclose(fd);
}

void count_array(void *myID)
{
    int i, j;
	int line, charIndex;
	int startPos = ((int) myID) * (ARRAY_SIZE/ NUM_THREADS);
	int endPos = startPos + (ARRAY_SIZE / NUM_THREADS);

    for (i = startPos; i < endPos; i++) {
		line_lengths[i] = strlen(input_array[i]);
        for (j = 0; j < LINE_SIZE; j++) {
            char_sums[i] += ((int)input_array[i][j]);
        }
		if (line_lengths[i] > 0)
            line_avgs[i] = (char_sums[i]) / ((float)line_lengths[i]);
        else
            line_avgs[i] = 0; 
    }
	
	pthread_exit(NULL);
}

void print_results()
{
    int i;
    for (i = 0; i < ARRAY_SIZE; i++) {   
        printf("%d: %.1f\n", i, line_avgs[i]);
    }
}


float find_avg(char* line, int nchars) {
   int i, j;
   float sum = 0;

   for ( i = 0; i < nchars; i++ ) {
      sum += ((int) line[i]);
   }

   if (nchars > 0) 
	return sum / (float) nchars;
   else
	return 0.0;
}
	
main() {
	int i, rc;
	struct timeval t1, t2;
    double elapsedTime;
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	void *status;
	
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	gettimeofday(&t1, NULL);
	
	printf("Debug: starting on %s\n", getenv("HOSTNAME"));
	
	init_arrays();
	
	for(i = 0; i < NUM_THREADS; i++){
		rc = pthread_create(&threads[i], &attr, count_array, (void *)i);
		if(rc){
			printf("Thread error: %d\n", rc);
			exit(-1);
		}
	}
	
	for(i=0; i < NUM_THREADS; i++) {
		rc = pthread_join(threads[i], &status);
		if(rc) {
			printf("Thread error: %d\n", rc);
			exit(-1);
		}
	}
	
	print_results();
	
	pthread_mutex_destroy(&mutexsum);
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
	printf("DATA, %s, %f\n", getenv("SLURM_NTASKS"), elapsedTime);
	pthread_exit(NULL);
}
