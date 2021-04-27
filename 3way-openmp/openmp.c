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
    int i, j, err;
    int nchars = 0;
    FILE* fd;
    char* line = (char*)malloc(2001); // no lines larger than 2000 chars

 // Read in the lines from the data file

    fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
    for (i = 0; i < ARRAY_SIZE; i++) {
        err = fscanf(fd, "%[^\n]\n", line);
        if (err == EOF) break;
        nchars = strlen(line);
        char_array[i] = line;
        /*for (j = 0; j < nchars; j++)
        {
            char_array[i][j] = line[j];
        }*/
        line_lengths[i] = nchars;
        char_sums[i] = 0;
    }

    fclose(fd);
}

void count_array()
{
    char theChar;
    int i, j;

    #pragma omp parallel private(theChar)
    {
        #pragma omp for collapse(2)
        for (i = 0; i < ARRAY_SIZE; i++) {
            for (j = 0; j < STRING_SIZE; j++) {
                theChar = char_array[i][j];
                char_sums[i] += ((int)theChar);
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
    init_arrays();
    count_array();
    print_results();
}

