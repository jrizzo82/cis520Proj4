  
#!/bin/bash

for i in 1 2 4 8 16
do
	 sbatch --nodes=1 --constraint=elves --mem-per-cpu=1500M --ntasks-per-node=$i --nodes=1 pthreads_sbatch.sh
done