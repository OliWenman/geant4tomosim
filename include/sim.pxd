from libcpp.vector cimport vector
import numpy as np

# c++ interface to cython
cdef extern from "Simulation.hh":
   cdef cppclass Simulation:
      Simulation() except +
      
      void Initialize()
      void RunSimulation()
      void ResetSimulation()
      void KillSimulation()