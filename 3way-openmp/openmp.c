#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

#define ARRAY_SIZE 1000000
#define STRING_SIZE 2000

char char_array[ARRAY_SIZE][STRING_SIZE];
int char_sums[ARRAY_SIZE];
int line_lengths[ARRAY_SIZE];

//float find_avg(char* line, int nchars) {
//    int i;
//    float sum = 0;
//
//    for (i = 0; i < nchars; i++) {
//        sum += ((int)line[i]);
//    }
//
//    if (nchars > 0)
//        return sum / (float)nchars;
//    else
//        return 0.0;
//}

void init_arrays()
{
    int maxlines = 1000000;
    int i, j, err;
    float  charsum = 0.0;
    int nchars = 0;
    FILE* fd;
    char* line = (char*)malloc(2001); // no lines larger than 2000 chars

 // Read in the lines from the data file

    fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
    for (i = 0; i < maxlines; i++) {
        err = fscanf(fd, "%[^\n]\n", line);
        if (err == EOF) break;
        nchars = strlen(line);
        for (j = 0; j < nchars; j++)
        {
            char_array[i][j] = line[j];
        }
        line_lengths[i] = nchars;
        char_sums[i] = 0;
        //printf("%d: %.1f\n", nlines, find_avg(line, nchars));
    }

    fclose(fd);
}

void count_array()
{
    char theChar;
    int i, j;
    int local_char_sums[ARRAY_SIZE];

    #pragma omp parallel private(theChar,local_char_sums)
    {

        //printf("thread no %d\n", omp_get_thread_num());

        // init local count array
        for (i = 0; i < ARRAY_SIZE; i++) {
            local_char_count[i] = 0;
        }


        #pragma omp for collapse(2)
        for (i = 0; i < ARRAY_SIZE; i++) {
            for (j = 0; j < STRING_SIZE; j++) {
                theChar = char_array[i][j];
                local_char_sums[i] += ((int)theChar);
            }
        }

        #pragma omp critical
        {
            for (i = 0; i < ARRAY_SIZE; i++) {
                char_sums[i] += local_char_sums[i];
            }
        }
    } //omp parallel
}

void print_results()
{
    int i, total = 0;

    for (i = 0; i < ARRAY_SIZE; i++) {        
        printf("%d: %.1f\n", i, (char_sums[i])/((float)line_lengths[i]));
    }
}

int main()
{
    init_arrays();
    count_array();
    print_results();
}

