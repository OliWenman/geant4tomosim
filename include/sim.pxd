from libcpp.vector cimport vector
from libcpp.string cimport string
import numpy as np

#C++ interface to cython
cdef extern from "Simulation.hh":
   cdef cppclass Simulation:
      Simulation() except +
      
      void pyOutputOptions(bint, bint)
      void pySetupDetectors(int, int, vector[double], int)
      void pyAddMacros(vector[string])
      vector[int] pyRun(unsigned long long int, vector[int], double, vector[double], string)
      
      void PrintInfo(unsigned long long int TotalParticles, int NumberOfImages, int nDarkFlatFields)
      void PrintInfo(int)

      vector[int]                 GetLastImage()
      vector[int]                 GetFluorescence()
      vector[double]              GetEnergyBins()
      vector[int]                 GetBeamEnergy()
      vector[vector[vector[int]]] GetFullMapping()
      vector[vector[int]]         GetDiffractionData()
      
      int GetNumberOfBins()
      int GetNumberOfxPixels()
      int GetNumberOfyPixels()
      vector[double] GetAbsorptionDetectorHalfDimensions()
      
      void SetSavingTime(double)
      void SetSaveLogPath(string, string)
      
      bint FullMappingFluorescence()
      bint FullFieldFluorescence()
