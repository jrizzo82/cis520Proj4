#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <sys/time.h>
#include <stdint.h>

#include "sys/types.h"
#include "sys/sysinfo.h"

#define ARRAY_SIZE 1000000
#define STRING_SIZE 2001

char char_array[ARRAY_SIZE][STRING_SIZE];
int char_sums[ARRAY_SIZE];
int line_lengths[ARRAY_SIZE];

typedef struct {
    uint32_t virtualMem;
    uint32_t physicalMem;
} processMem_t;

int parseLine(char* line) {
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p < '0' || *p > '9') p++;
    line[i - 3] = '\0';
    i = atoi(p);
    return i;
}

void GetProcessMemory(processMem_t* processMem) {
    FILE* file = fopen("/proc/self/status", "r");
    char line[128];

    while (fgets(line, 128, file) != NULL) {
        //printf("%s", line);
        if (strncmp(line, "VmSize:", 7) == 0) {
            processMem->virtualMem = parseLine(line);
        }

        if (strncmp(line, "VmRSS:", 6) == 0) {
            processMem->physicalMem = parseLine(line);
        }
    }
    fclose(file);
}

void init_arrays()
{
    int i, err;
    FILE* fd;

    // Read in the lines from the data file

    fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
    for (i = 0; i < ARRAY_SIZE; i++) {
        err = fscanf(fd, "%[^\n]\n", char_array[i]);
        if (err == EOF) break;
        line_lengths[i] = strlen(char_array[i]);
        char_sums[i] = 0;
    }

    fclose(fd);
}

void count_array()
{
    int i, j;

    #pragma omp parallel private(i,j) num_threads(4)
    {
        #pragma omp for collapse(2)
        for (i = 0; i < ARRAY_SIZE; i++) {
            for (j = 0; j < STRING_SIZE; j++) {
                char_sums[i] += ((int)char_array[i][j]);
            }
        }
    } //omp parallel
}

void print_results()
{
    int i;
    float total = 0;

    for (i = 0; i < ARRAY_SIZE; i++) {   
        if (line_lengths[i] > 0)
            total = (char_sums[i]) / ((float)line_lengths[i]);
        else
            total = 0;
        printf("%d: %.1f\n", i, total);
    }
}

int main()
{
    int i, j = 0;
    struct timeval t1, t2;
    double elapsedTime;
    int numSlots, myVersion = 1;
    processMem_t myMem;

    gettimeofday(&t1, NULL);

    printf("DEBUG: starting loop on %s\n", getenv("HOSTNAME"));

    // Fill arrays with lines from file, line lengths, or zeros
    init_arrays();
    // Count values of the characters
    count_array();
    // Print out the results
   // print_results();
    gettimeofday(&t2, NULL);

    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
    printf("DATA, %d, %s, %f\n", myVersion, getenv("SLURM_NTASKS"), elapsedTime);    

    GetProcessMemory(&myMem);

    printf("Memory: vMem %u KB, pMem %u KB\n", myMem.virtualMem, myMem.physicalMem);
}

