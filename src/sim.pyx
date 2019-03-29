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
    def __cinit__(self, verbose = 1):
       
        #Creates the default path to save file 
        WorkingDirectory = os.path.dirname(os.getcwd())
        BuildDirectory = "/Output/HDF5/"
        self.SaveFilePath = WorkingDirectory + BuildDirectory
        self.NexusName = "SimulationData.nxs"
        
        self.thisptr = new Simulation(verbose)
        self.nexusfile = NexusFormatter.NexusFormatter()

    #Delete the C++ class
    def __dealloc__(self):
        del self.thisptr
        del self.nexusfile
   
    def addMacroFiles(self, macroFiles):
        self.thisptr.pyAddMacros(macroFiles)

    def run(self, TotalParticles, rotation_angles, nDarkFlatFields):
        
        if TotalParticles >= 1:

           Path = './../Output/HDF5/'
           
           TotalImages = len(rotation_angles) + nDarkFlatFields

           self.thisptr.PrintInfo(TotalParticles, TotalImages, nDarkFlatFields)
           
           FMFluorescence = self.thisptr.FullMappingFluorescence()
           FFFluorescence = self.thisptr.FullFieldFluorescence()
           
           #windowRows, windowColumns = os.popen('stty size', 'r').read().split()
           #for i in range(int(windowColumns)):
           #   sys.stdout.write('-')
           self.thisptr.CalculateStorageSpace(TotalImages)
           
           self.nexusfile.openFile(self.SaveFilePath + self.NexusName)
           if self.nexusfile.setupSuccess == False:
              print "\nAborting run..." 
              return 0
           
           nBins = self.thisptr.GetNumberOfBins()
           xPixels = self.thisptr.GetNumberOfxPixels()
           yPixels = self.thisptr.GetNumberOfyPixels()
           
           self.nexusfile.CreateProjectionFolder(nDarkFlatFields, TotalImages, yPixels, xPixels, self.thisptr.GetAbsorptionDetectorHalfDimensions(), rotation_angles)
           self.nexusfile.CreateDataGroup("Beam_Energy", nImages = TotalImages, eBins = nBins)
           
           if FFFluorescence == True:
              self.nexusfile.CreateDataGroup("Fluorescence", nImages = TotalImages, eBins = nBins)
              
           if FMFluorescence == True:
              self.nexusfile.CreateDataGroup("Full_Mapping_Fluorescence", nImages = TotalImages, eBins = nBins, xBins = xPixels, yBins = yPixels)
           
           iTime = time.time()
           
           for CurrentImage in range(TotalImages):
           
               #pyRun returns the 1D array at the end of each run. Reshape it to make it 2D
               if CurrentImage < TotalImages - nDarkFlatFields:
                  rotation_angle = rotation_angles[CurrentImage]
                  
               elif CurrentImage >= TotalImages - nDarkFlatFields:
                  rotation_angle = 0
               
               imageInfo = [CurrentImage, nDarkFlatFields, TotalImages]
                            
               #pyRun returns the 1D array at the end of each run. Reshape it to make it 2D
               simOutput = self.thisptr.pyRun(TotalParticles, imageInfo, rotation_angle)  
               simOutput = np.reshape(simOutput, (-1, xPixels))        
               
               iSavingTime = time.time()
               
               self.nexusfile.AddProjectionData(simOutput, CurrentImage)
               
               if CurrentImage == 0:
                  energyBins = self.lastEnergyBins()
                  
                  self.nexusfile.AddxAxis("Beam_Energy", energyBins)
                  
                  if FFFluorescence == True:
                     self.nexusfile.AddxAxis("Fluorescence", energyBins)
                     
                  if FMFluorescence == True:
                     self.nexusfile.AddxAxis("Full_Mapping_Fluorescence", energyBins)
               
               self.nexusfile.AddData("Beam_Energy", self.beamEnergy(), nImage = CurrentImage)    
               
               if FFFluorescence == True:
                  self.nexusfile.AddData("Fluorescence", data = self.lastEnergyFreq(), nImage = CurrentImage)
                  
               if FMFluorescence == True:
                  self.nexusfile.AddData("Full_Mapping_Fluorescence", data = self.fullMapping(), nImage = CurrentImage)
               
               eSavingTime = time.time()
               SavingTime  = eSavingTime - iSavingTime
               self.thisptr.SetSavingTime(SavingTime)

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
            
        else:
           print("\nERROR: The number of particles and number of images should be greater or equal to 1! ")

    #Return the image data from the simulation
    #def lastImage(self):
    #    return np.reshape(self.thisptr.GetLastImage(), (-1, self.nDetectorsY))
    
    def photonTransmission(self):
        return np.sum(self.thisptr.GetLastImage())

    def lastEnergyBins(self):
        return np.array(self.thisptr.GetEnergyBins())

    def lastEnergyFreq(self):
        return np.array(self.thisptr.GetFluorescence())

    def beamEnergy(self):
        return np.array(self.thisptr.GetBeamEnergy())  

    def fullMapping(self):
        return np.array(self.thisptr.GetFullMapping())
        
    def printInfo(self, int verbose):
        self.thisptr.PrintInfo(verbose)
        
    def setFilePath(self, FilePath, NexusName, logName):
        self.SaveFilePath = FilePath
        self.NexusName = NexusName
        self.thisptr.SetSaveLogPath(FilePath, logName)
        
    def printNexusTree(self):
        self.nexusfile.DisplayTree()
        
    def plotBeamEnergy(self):
        plt.plot(self.thisptr.GetEnergyBins(), self.thisptr.GetBeamEnergy())
        plt.title("Beam energy distrubution")
        plt.xlabel("Energy (keV)")
        plt.ylabel("Photons")
        plt.show()
        
    def plotFinalFluorescence(self):
        plt.plot(self.thisptr.GetEnergyBins(), self.thisptr.GetFluorescence())
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
        self.thisptr.ApplyCommand(command)
        
    def ApplyMacroFile(self, macrofile):
        self.thisptr.ApplyMacroFile(macrofile)
