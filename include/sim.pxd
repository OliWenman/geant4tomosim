from libcpp.vector cimport vector
from libcpp.string cimport string
import numpy as np

#C++ interface to cython
cdef extern from "Simulation.hh":
   cdef cppclass Simulation:
      Simulation() except +
      Simulation(int, bint)

#===================================================================================================
      
      #Functions to apply macro files or commands to the simulation
      void addmacros_pywrapped(vector[string])
      void applymacrofile_pywrapped(string)
      void applycommand_pywrapped(string)
      
      #Print information to the user about the simulation
      void printinfo_pywrapped(unsigned long long int TotalParticles, int NumberOfImages, int nDarkFlatFields)

#===================================================================================================

      #Run the simulation 
      int                         run_pywrapped(unsigned long long int, vector[int], double)
      
      #Get the data after the simulation is finished
      #Absorption data
      vector[int]                 getAbsorption_pywrapped()
      
      #Fluorescence data
      vector[int]                 getFullFieldFluore_pywrapped()
      vector[vector[vector[int]]] getFullMappingFluore_pywrapped()
      vector[double]              getFluoreEneBins_pywrapped()
      bint FullMappingFluorescence()
      bint FullFieldFluorescence()
      
      #Diffraction data
      vector[vector[int]]         GetDiffractionData()

      #Beam energy data      
      vector[int]                 getBeamEnergy_pywrapped()
      vector[double]              getBeamEnergyBins_pywrapped()
   
#===================================================================================================
  
      #Set functions
      void setSavingTime_pywrapped (double)
      void setlogfile_pywrapped (string, string)
      
      #Get functions
      int getNumFluoreEneBins_pywrapped()                        
      int getNumAbsXpixels_pywrapped()
      int getNumAbsYpixels_pywrapped()
      vector[double] getAbsHalf3Dim_pywrapped()
