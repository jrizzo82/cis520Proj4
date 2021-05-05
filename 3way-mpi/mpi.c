#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

#include "sys/types.h"
#include "sys/sysinfo.h"

void parprocess(MPI_File *in, const int rank, const int size, const int overlap)
{
	MPI_Offset globalstart;
	int mysize;
	char *chunk;
	
	MPI_Offset globalend;
	MPI_Offset filesize;
	
	MPI_File_get_size(*in, &filesize);
	filesize--;
	mysize = filesize / size;
	globalstart = rank * mysize;
	globalend = globalstart + mysize - 1;
	
	if (rank == size - 1)
	{
		globalend = filesize - 1;
	}
	
	if (rank != size - 1)
	{
		globalend += overlap;
	}
	
	mysize = globalend - globalstart + 1;
	
	chunk = malloc((mysize + 1) * sizeof(char));
	
	MPI_File_read_at_all(*in, globalstart, chunk, mysize, MPI_CHAR, MPI_STATUS_IGNORE);
	chunk[mysize] = '\0';
	
	int locstart = 0;
	int locend = mysize - 1;
	
	if (rank != 0)
	{
		locend -= overlap;
		while (chunk[locend] != '\n')
		{
			locend++;
		}
	}
	mysize = locend - locstart + 1;
	
	int total = 0;
	int count = 0;
	int average;
	
	for (int i = locstart; i <= locend; i++ )
	{
		total += chunk[i];
		count++;
	}
	
	average = total / count;
	
	printf("%d", average);
	
	return;
}

int main(int argc, char *argv[])
{
	MPI_File in;
	int numtasks, taskid, len;
	const int overlap = 100;
	char hostname[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	MPI_Get_processor_name(hostname, &len);
	
	MPI_File_open(MPI_COMM_WORLD, "/homes/dan/625/wiki_dump.txt", MPI_MODE_RDONLY, MPI_INFO_NULL, &in);
	
	parprocess(&in, taskid, numtasks, overlap);
	
	MPI_File_close(&in);
	
	MPI_Finalize();
	return 0;
}