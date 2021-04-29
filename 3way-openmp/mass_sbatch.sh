#!/bin/bash

for i in 1 2 4 8 16
do
	 sbatch --constraint=elves --mem-per-cpu=2G --ntasks-per-node=$i --nodes=1 openmp_sbatch.sh
done