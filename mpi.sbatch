#!/bin/bash

#SBATCH -p tornado
#SBATCH --ntasks=50
#SBATCH --cpus-per-task=1
#SBATCH --nodes=4
#SBATCH --time=00:02:00

echo "Date                           = $(date)"
echo "Hostname                       = $(hostname -s)"
echo "Working Directory              = $(pwd)"
echo ""
echo "Number of Nodes Allocated      = $SLURM_JOB_NUM_NODES"
echo "Number of Tasks Allocated      = $SLURM_NTASKS"
echo "Number of Cores/Task Allocated = $SLURM_CPUS_PER_TASK"

module load mpi/openmpi/4.0.1/gcc/8
mpirun -q ./mpi.out 1000
