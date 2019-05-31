#Cython file for wrapping my C++ functions 

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
      void Execute_Macrolist_pyw      (vector[string])
      void Execute_Macro_pyw          (string)
      void Execute_Command_pyw        (string)

#===================================================================================================
      
      #Print information to the user about the simulation
      void Print_SimulationInfo_pyw(string,
                                    unsigned long long int, 
                                    int, 
                                    int)
      
      void Setup_Visualization_pyw(string, string)

      #Run the simulation 
      int Run_Tomography_pyw(unsigned long long int, 
                             vector[int], 
                             double,
                             double)

#===================================================================================================
                                                
      int Run_Projection_pyw(unsigned long long int,
                             bint,
                             double,
                             double )

#===================================================================================================
      
      #Get the data after the simulation is finished
      #Absorption data
      vector[int]                 AbsorptionDetector_GetProjection_pyw()
      int                         AbsorptionDetector_GetXPixels_pyw()
      int                         AbsorptionDetector_GetYPixels_pyw()
      vector[double]              AbsorptionDetector_GetDimensions_pyw()
      
      #Fluorescence data
      vector[int]                 FluorescenceDetector_GetFullField_pyw()
      vector[vector[vector[int]]] FluorescenceDetector_GetFullMapping_pyw()
      vector[double]              FluorescenceDetector_GetEnergyBins_pyw()
      int                         FluorescenceDetector_GetNoEnergyBins_pyw()  
      bint                        FluorescenceDetector_FullMappingActive_pyw()
      bint                        FluorescenceDetector_FullFieldActive_pyw()
      
      #Diffraction data
      vector[vector[int]]         GetDiffractionData()

      #Beam energy data      
      vector[int]                 Beam_GetIntensity_pyw()
      vector[double]              Beam_GetEnergyBins_pyw()
      int                         Beam_GetNoEnergyBins_pyw()
   
#===================================================================================================
  
      #Set functions
      void Set_SavingTime_pyw (double)
      
      #Free memory
      void Free_DetectorMemory_pyw()
   
