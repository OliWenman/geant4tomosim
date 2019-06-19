#!/bin/bash


module load hamilton

NUM_PROCESSORS=200
MPI_RECIPE=/home/xol73553/git/geant4tomosim/mpi_recipe.sh
LOG_FILE=/dls/tmp/xol73553/simulations/V1/bob.log
rm -rf $LOG_FILE
PATH_TO_SCRIPT=/home/xol73553/git/rh7/py3.7/bin/mpi_run.py
PATH_TO_DATA=/dls/tmp/xol73553/simulations/V1/86254.nxs
qsub -pe openmpi $NUM_PROCESSORS -o $LOG_FILE -e $LOG_FILE -l exclusive,gpu=4,m_mem_free=9G -P tomography $MPI_RECIPE $NUM_PROCESSORS $PATH_TO_SCRIPT $PATH_TO_DATA $LOG_FILE

