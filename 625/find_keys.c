#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <omp.h>

double myclock();

int main()
{
   int nwords = 0, maxwords = 50000;
   int nlines = 0, maxlines = 1000000;
   int i, k, n, err, *count, nthreads = 0;
   double nchars = 0;
   double tstart, ttotal;
   FILE *fd;
   char **word, **line;

// Malloc space for the word list and lines

   word = (char **) malloc( maxwords * sizeof( char * ) );
   count = (int *) malloc( maxwords * sizeof( int ) );
   for( i = 0; i < maxwords; i++ ) {
      word[i] = malloc( 10 );
      count[i] = 0;
   }

   line = (char **) malloc( maxlines * sizeof( char * ) );
   for( i = 0; i < maxlines; i++ ) {
      line[i] = malloc( 2001 );
   }


// Read in the dictionary words

   fd = fopen( "/homes/dan/625/keywords.txt", "r" );
   for ( i = 0; i < maxwords; i++ )  {
      err = fscanf( fd, "%[^\n]\n", word[i] );
      if( err == EOF ) break;
         nwords++;
   }
   fclose( fd );

   printf( "Read in %d words\n", nwords);


// Read in the lines from the data file

   fd = fopen( "/homes/dan/625/wiki_dump.txt", "r" );
   for ( i = 0; i < maxlines; i++ )  {
      err = fscanf( fd, "%[^\n]\n", line[i] );
      if( err == EOF ) break;
         nlines++;
         nchars += (double) strlen( line[i] );
   }

   printf( "Read in %d lines averaging %.0lf chars/line\n", nlines, nchars / nlines);


// Loop over the word list

   tstart = myclock();  // Set the zero time
   tstart = myclock();  // Start the clock

//   omp_set_num_threads( nthreads );
   nthreads = omp_get_max_threads();

#pragma omp parallel for schedule( dynamic ) private(i,k)
   for( i = 0; i < nwords; i++ ) {

      //for( k = 0; k < 10000; k++ ) {
      for( k = 0; k < nlines; k++ ) {
         if( strstr( line[k], word[i] ) != NULL ) count[i]++;
      }

   }

   ttotal = myclock() - tstart;
   printf( "The run on %d cores took %lf seconds for %d words\n",
           nthreads, ttotal, nwords);

// Dump out the word counts

   fd = fopen( "wiki.out", "w" );
   for( i = 0; i < nwords; i++ ) {
      fprintf( fd, "%d %s %d\n", i, word[i], count[i] );
   }
   fprintf( fd, "The run on %d cores took %lf seconds for %d words\n",
           nthreads, ttotal, nwords);
   fclose( fd );

}

double myclock() {
   static time_t t_start = 0;  // Save and subtract off each time

   struct timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   if( t_start == 0 ) t_start = ts.tv_sec;

   return (double) (ts.tv_sec - t_start) + ts.tv_nsec * 1.0e-9;
}
