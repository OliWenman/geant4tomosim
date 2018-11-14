# creating a cython wrapper class
import numpy as np

cdef class PySim:
    cdef Simulation *thisptr      # hold a C++ instance which we're wrapping
    
    def __cinit__(self):
        self.thisptr = new Simulation()
        
    def __dealloc__(self):
        del self.thisptr
        
    def reset(self):
        self.thisptr.ResetSimulation()
        
    def initialize(self):
        self.thisptr.Initialize()
        
    def run(self):
        self.thisptr.RunSimulation()
        
    def Kill(self):
        self.thisptr.KillSimulation()
    
        
   # def Pointer1DSim(self, Input):
    #    self.thisptr.Pointer1DSim(Input)