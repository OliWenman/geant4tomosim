from libcpp.vector cimport vector
import numpy as np

#C++ interface to cython
cdef extern from "Simulation.hh":
   cdef cppclass Simulation:
      Simulation() except +
      
      void pyOutputOptions(bint, bint, bint)
      void pyInitialise(int, int, vector[double], int)
      vector[int] pyRun(unsigned long long int, int, int, double)

      vector[int] GetLastImage()

      vector[int] GetEnergyFreq()
      vector[double] GetEnergyBins()

      vector[int] GetBeamEnergyFreq()

      vector[vector[vector[int]]] GetFullMapping()
