#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 800000
#define STRING_SIZE 32

char char_array[ARRAY_SIZE][STRING_SIZE];
int char_counts[26];

char getRandomChar()
{
	int randNum = 0;
	char randChar = ' ';

	randNum = 26 * (rand() / (RAND_MAX + 1.0)); 	// pick number 0 < # < 25
	randNum = randNum + 97;				// scale to 'a'
	randChar = (char) randNum;

	// printf("%c", randChar);
	return randChar;
}

void init_arrays()
{
  int i, j, randNum; 
  char randChar;

  for ( i = 0; i < ARRAY_SIZE; i++) {
	for ( j = 0; j < STRING_SIZE; j++ ) {
		 char_array[i][j] = getRandomChar();
	}
  }

  for ( i = 0; i < 26; i++ ) {
  	char_counts[i] = 0;
  }
}

void count_array()
{
  char theChar;
  int i, j, charLoc;
  int local_char_count[STRING_SIZE];
   
  #pragma omp parallel private(theChar,charLoc,local_char_count) num_threads(4)
  {

	printf("thread no %d\n", omp_get_thread_num());
	
						// init local count array
	  for ( i = 0; i < STRING_SIZE; i++ ) {
		local_char_count[i] = 0;
	  }


	  #pragma omp for collapse(2)
		for ( i = 0; i < ARRAY_SIZE; i++) {
		  for ( j = 0; j < STRING_SIZE; j++ ) {
				 theChar = char_array[i][j];
			 charLoc = ((int) theChar) - 97;
			 local_char_count[charLoc]++;
		  }
		}
	  
	  #pragma omp critical
	  {
		for ( i = 0; i < STRING_SIZE; i++ ) {
			char_counts[i] += local_char_count[i];
		  }
	  }
  } //omp parallel
}


void count_array2()
{
  char theChar;
  int i, j, charLoc;

  for ( i = 0; i < 26; i++ ) {
  	char_counts[i] = 0;
  }

  for ( i = 0; i < ARRAY_SIZE; i++) {
	for ( j = 0; j < STRING_SIZE; j++ ) {
	         theChar = char_array[i][j];
		 charLoc = ((int) theChar) - 97;
		 char_counts[charLoc]++;
	}
  }
}


void print_results()
{
  int i, total = 0;

  for ( i = 0; i < 26; i++ ) {
     total += char_counts[i];
     printf(" %c %d\n", (char) (i + 97), char_counts[i]);
  }
  printf("\nTotal characters:  %d\n", total);
}

main() {
	init_arrays();
	count_array();
	print_results();
	
	count_array2(); //repeat using serial algorithm for debugging - comment out for perf modeling
	print_results();
}


