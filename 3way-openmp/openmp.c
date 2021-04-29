#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

#define ARRAY_SIZE 1000000
#define STRING_SIZE 2001

char char_array[ARRAY_SIZE][STRING_SIZE];
int char_sums[ARRAY_SIZE];
int line_lengths[ARRAY_SIZE];

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
    // Fill arrays with lines from file, line lengths, or zeros
    init_arrays();
    // Count values of the characters
    count_array();
    // Print out the results
   // print_results();
    printf("Done test2");
}

