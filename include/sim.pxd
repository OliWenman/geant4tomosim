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

#===================================================================================================
      
      #Print information to the user about the simulation
      void printinfo_pywrapped(string,
                               unsigned long long int, 
                               int, 
                               int)
      
      void setupvis_pywrapped(string,
		                      string)

      #Run the simulation 
      int                         run_pywrapped(unsigned long long int, 
                                                vector[int], 
                                                double,
                                                double)

#===================================================================================================
                                                
      int                         runsingleprojection_pywrapped(unsigned long long int,
                                                                bint,
                                                                double,
                                                                double )

#===================================================================================================
      
      #Get the data after the simulation is finished
      #Absorption data
      vector[int]                 getAbsorption_pywrapped()
      int                         getNumAbsXpixels_pywrapped()
      int                         getNumAbsYpixels_pywrapped()
      vector[double]              getAbsHalf3Dim_pywrapped()
      
      #Fluorescence data
      vector[int]                 getFullFieldFluore_pywrapped()
      vector[vector[vector[int]]] getFullMappingFluore_pywrapped()
      vector[double]              getFluoreEneBins_pywrapped()
      bint                        fluorFMactive_pywrapped()
      bint                        fluorFFactive_pywrapped()
      int                         getNumFluorbins_pywrapped()  
      
      #Diffraction data
      vector[vector[int]]         GetDiffractionData()

      #Beam energy data      
      vector[int]                 getBeamEnergy_pywrapped()
      vector[double]              getBeamEnergyBins_pywrapped()
      int                         getbeambins_pywrapped()
   
#===================================================================================================
  
      #Set functions
      void setSavingTime_pywrapped (double)
      
      #Free memory
      void freedataMemory_pywrapped()
      
      
