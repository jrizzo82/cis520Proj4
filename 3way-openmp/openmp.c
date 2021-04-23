#include <stdlib.h>
#include <stdio.h>
#include <string.h>

float find_avg(char* line, int nchars) {
    int i, j;
    float sum = 0;

    for (i = 0; i < nchars; i++) {
        sum += ((int)line[i]);
    }

    if (nchars > 0)
        return sum / (float)nchars;
    else
        return 0.0;
}

int main()
{
    int nlines = 0, maxlines = 1000000;
    int i, j, err;
    float  sum = 0.0;
    float avg = 0.0;
    int nchars = 0;
    FILE* fd;
    char* line = (char*)malloc(2001); // no lines larger than 2000 chars

 // Read in the lines from the data file

    fd = fopen("/homes/dan/625/wiki_dump.txt", "r");

    #pragma omp parallel private(i,j,sum,avg,nchars,err,nlines,line)
    {
        #pragma omp for collapse(2)
        for (i = 0; i < maxlines; i++) {
            sum = 0.0
            err = fscanf(fd, "%[^\n]\n", line);
            if (err == EOF) break;
            nchars = strlen(line);
            for (j = 0; j < nchars; j++) {
                sum += ((int)line[i]);
            }

            if (nchars > 0)
                avg = sum / (float)nchars;
            else
                avg = 0.0;
            printf("%d: %.1f\n", nlines, avg);
            nlines++;
        }
    }

    fclose(fd);

}

