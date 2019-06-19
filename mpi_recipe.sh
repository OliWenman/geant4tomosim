#!/bin/bash

module load python/3.7





mpiexec -n $1 python $2 $3
