# creating a cython wrapper class
import numpy as np
import h5py
import time
import os
import gc
import getpass
import matplotlib.pyplot as plt
import sys

import NexusFormatter

cdef class PySim:

    cdef Simulation *thisptr        
    cdef public nexusfile

    #Simulation time
    cdef public double SimTime
    
    cdef public str SaveFilePath  
    cdef public str NexusName
  
    #Constructor, create an instance of the C++ class
    def __cinit__(self, verbose = 1, interactive = True):
       
        #Creates the default path to save file 
        WorkingDirectory = os.path.dirname(os.getcwd())
        BuildDirectory = "/Output/HDF5/"
        self.SaveFilePath = WorkingDirectory + BuildDirectory
        self.NexusName = "SimulationData.nxs"
        
        self.thisptr = new Simulation(verbose, interactive)
        self.nexusfile = NexusFormatter.NexusFormatter()

    #Delete the C++ class
    def __dealloc__(self):
        del self.thisptr
        del self.nexusfile
   
    def addMacroFiles(self, macroFiles):
        self.thisptr.addmacros_pywrapped(macroFiles)

    def run(self, TotalParticles, rotation_angles, nDarkFlatFields):
        
        if TotalParticles >= 1:

           Path = './../Output/HDF5/'
           
           TotalImages = len(rotation_angles) + nDarkFlatFields

           self.thisptr.printinfo_pywrapped(TotalParticles, TotalImages, nDarkFlatFields)
           
           FMFluorescence = self.thisptr.fluorFMactive_pywrapped()
           FFFluorescence = self.thisptr.fluorFFactive_pywrapped()
           
           self.nexusfile.openFile(self.SaveFilePath + self.NexusName)
           if self.nexusfile.setupSuccess == False:
              print "\nAborting run..." 
              return 0
           
           fluoreBins = self.thisptr.getNumFluorbins_pywrapped()
           beamBins   = self.thisptr.getbeambins_pywrapped()
           xPixels = self.thisptr.getNumAbsXpixels_pywrapped()
           yPixels = self.thisptr.getNumAbsYpixels_pywrapped()
           
           self.nexusfile.CreateProjectionFolder(nDarkFlatFields, TotalImages, yPixels, xPixels, self.thisptr.getAbsHalf3Dim_pywrapped(), rotation_angles)
           self.nexusfile.CreateDataGroup("Beam_Energy", nImages = TotalImages, eBins = beamBins)
           
           if FFFluorescence == True:
              self.nexusfile.CreateDataGroup("Fluorescence", nImages = TotalImages, eBins = fluoreBins)
              
           if FMFluorescence == True:
              self.nexusfile.CreateDataGroup("Full_Mapping_Fluorescence", nImages = TotalImages, eBins = fluoreBins, xBins = xPixels, yBins = yPixels)
           
           iTime = time.time()
           
           for CurrentImage in range(TotalImages):
           
               #pyRun returns the 1D array at the end of each run. Reshape it to make it 2D
               if CurrentImage < TotalImages - nDarkFlatFields:
                  rotation_angle = rotation_angles[CurrentImage]
                  
               elif CurrentImage >= TotalImages - nDarkFlatFields:
                  rotation_angle = 0
               
               imageInfo = [CurrentImage, nDarkFlatFields, TotalImages]
                            
               #pyRun returns the 1D array at the end of each run. Reshape it to make it 2D
               self.thisptr.run_pywrapped(TotalParticles, imageInfo, rotation_angle)  
               
               iSavingTime = time.time()
               
               self.nexusfile.AddProjectionData(self.absorptionData(), CurrentImage)
               
               if CurrentImage == 0:
                  energy_xaxis = self.beamEnergyBins()
                  
                  self.nexusfile.AddxAxis("Beam_Energy", energy_xaxis)
                  
                  energy_xaxis = self.thisptr.getFluoreEneBins_pywrapped()
                  if FFFluorescence == True:
                     self.nexusfile.AddxAxis("Fluorescence", energy_xaxis)
                     
                  if FMFluorescence == True:
                     self.nexusfile.AddxAxis("Full_Mapping_Fluorescence", energy_xaxis)
               
               self.nexusfile.AddData("Beam_Energy", self.beamEnergyData(), nImage = CurrentImage)    
               
               if FFFluorescence == True:
                  self.nexusfile.AddData("Fluorescence", data = self.fullfieldfluoreData(), nImage = CurrentImage)
                  
               if FMFluorescence == True:
                  data = self.fullmappingfluoreData()

                  self.nexusfile.AddData("Full_Mapping_Fluorescence", data = data, nImage = CurrentImage)
               
               eSavingTime = time.time()
               SavingTime  = eSavingTime - iSavingTime
               self.thisptr.setSavingTime_pywrapped (SavingTime)

           #Ouput the time in the appropriate units
           eTime = time.time()
           self.SimTime = eTime -iTime
           
           self.nexusfile.LinkData()   

           message = "The total simulation time is"
           if self.SimTime < 60:
              print message, round(self.SimTime, 3), "seconds. "
           elif self.SimTime < 60*60:
              print message, round(self.SimTime/60, 3), "minutes. "
           else:
              print message, round(self.SimTime/(60*60), 3), "hours. "
            
           print "\nData was saved in", self.SaveFilePath 
           self.nexusfile.closeFile()
           
           self.thisptr.freedataMemory_pywrapped()
            
        else:
           print("\nERROR: The number of particles and number of images should be greater or equal to 1! ")

    
    def absorptionData(self):
        return np.reshape(self.thisptr.getAbsorption_pywrapped(), (-1, self.thisptr.getNumAbsXpixels_pywrapped()))
    
    def photonTransmission(self):
        return np.sum(self.thisptr.getAbsorption_pywrapped())

    def beamEnergyBins(self):
        return np.array(self.thisptr.getBeamEnergyBins_pywrapped())

    def fullfieldfluoreData(self):
        return np.array(self.thisptr.getFullFieldFluore_pywrapped())

    def beamEnergyData(self):
        return np.array(self.thisptr.getBeamEnergy_pywrapped())  

    def fullmappingfluoreData(self):
        return np.array(self.thisptr.getFullMappingFluore_pywrapped())
        
    def setFilePath(self, FilePath, NexusName, logName):
        self.SaveFilePath = FilePath
        self.NexusName = NexusName
        self.thisptr.setlogfile_pywrapped (FilePath, logName)
        
    def printNexusTree(self):
        self.nexusfile.DisplayTree()
        
    def plotBeamEnergy(self):
        plt.plot(self.beamEnergyBins(), self.thisptr.getBeamEnergy_pywrapped())
        plt.title("Beam energy distrubution")
        plt.xlabel("Energy (keV)")
        plt.ylabel("Photons")
        plt.show()
        
    def plotFinalFluorescence(self):
        plt.plot(self.thisptr.getFluoreEneBins_pywrapped(), self.thisptr.getFullFieldFluore_pywrapped())
        plt.title("Fluorescence")
        plt.xlabel("Energy (keV)")
        plt.ylabel("Photons")
        plt.show()
        
    def plotDiffraction(self):
        
        print "function being used"
        data = self.thisptr.GetDiffractionData()
        data = np.array(data)
        
        #plt.figure(220)
        #plt.plot(data[0])
        #plt.show()
        #plt.figure(330)
        #plt.plot(data[1])
        #plt.show()
        plt.figure(440)
        plt.imshow(data, cmap="Greys")
        #plt.
        plt.show()
        
    def ApplyCommand(self, command):
        self.thisptr.applycommand_pywrapped(command)
        
    def ApplyMacroFile(self, macrofile):
        self.thisptr.applymacrofile_pywrapped(macrofile)
