#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_THREADS 4

#define ARRAY_SIZE 20
#define LINE_SIZE 2000	

char input_array[ARRAY_SIZE][LINE_SIZE];
float line_avgs[ARRAY_SIZE];
int actual_lines;

void init_arrays()
{
    int i, err;
    FILE* fd;

    // Read in the lines from the data file
    actual_lines = 0;
    fd = fopen("test.txt", "r");
    for (i = 0; i < 4; i++) {
        err = fscanf(fd, "%[^\n]\n", input_array[i]);
        actual_lines += 1;
        if (err == EOF) break;
    }
    fclose(fd);
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

void print_results()
{
    int i;

    for (i = 0; i < actual_lines; i++) {   
        line_avgs[i];
        printf("%d: %.1f\n", i, line_avgs[i]);
    }
}

void *count_array(void *myID)
{
    int i, j;
	int line, charIndex;
	int startPos = ((int) myID) * (actual_lines/ NUM_THREADS);
	int endPos = startPos + (actual_lines / NUM_THREADS);
    int line_length = 0;

    for (i = startPos; i < endPos; i++) {
        line_length = strlen(input_array[i]);
		if (line_length > 0){
            
            line_avgs[i] = find_avg(input_array[i], line_length);
        }
        else
            line_avgs[i] = 0; 
    }
	pthread_exit(NULL);
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

	gettimeofday(&t2, NULL);
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
	printf("DATA, %s, %f\n", getenv("SLURM_NTASKS"), elapsedTime);
	pthread_exit(NULL);
}
