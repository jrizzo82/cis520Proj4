#!/bin/bash -l
##$ -l h_rt=0:01:00
#SBATCH --constraint=dwarves
# #SBATCH --nodes=1 --ntasks-per-node=4
#SBATCH -o pt2.out

module load OpenMPI

mpirun /homes/dan/625/pt2
