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
    def __cinit__(self):
       
        #Creates the default path to save file 
        WorkingDirectory = os.path.dirname(os.getcwd())
        BuildDirectory = "/Output/HDF5/"
        self.SaveFilePath = WorkingDirectory + BuildDirectory
        self.NexusName = "SimulationData.nxs"
        
        self.thisptr = new Simulation()

    #Delete the C++ class
    def __dealloc__(self):
        del self.nexusfile
        del self.thisptr
    """
    def outputOptions(self, bint fluoreFullField = False, bint fluoreFullMapping = False):
        self.FFF = fluoreFullField
        self.FFM = fluoreFullMapping
        self.thisptr.pyOutputOptions(self.FFF, self.FFM)

    #Get needed information from textfiles pySettings.mac and Geometry.mac
    def setupDetectors(self, int nDetY, int nDetZ, DetectorDimensions, int nBins = 2000):
        if nDetY >= 1 and nDetZ >= 1:

           self.Bins = nBins
           self.DetDimensions = DetectorDimensions

           #Call the C++ function
           self.thisptr.pySetupDetectors(nDetY, nDetZ, DetectorDimensions, self.Bins)
           
           #Make the number of detectors availiable for the Python class
           self.nDetectorsY = nDetY
           self.nDetectorsZ = nDetZ

           self.Ready = True

        else:
           print("\nError: The number of detectors for x and y should be greater or equal to 1! ")
    """           
    def addMacroFiles(self, macroFiles):
        self.thisptr.pyAddMacros(macroFiles)

    def run(self, TotalParticles, rotation_angles, nDarkFlatFields, energyArray, gunTypes):
        
        if TotalParticles >= 1:

           Path = './../Output/HDF5/'
           
           TotalImages = len(rotation_angles) + nDarkFlatFields

           self.thisptr.PrintInfo(TotalParticles, TotalImages, nDarkFlatFields)
           print "RECORDING THE FOLLOWING DATA: "
           print "- Absoprtion"
           print "- The beam energy"
           monoEnergySet = len(set(energyArray[0][:]))
           sigmaEnergySet = len(set(energyArray[:][1]))
           
           if gunTypes[0] != "Gauss":
              message = "  - Monochromatic energy:"
           else:
              message = "  - Polochromatic mean energy:"
           
           if monoEnergySet == 1:
               print message, energyArray[0][0]*1000, "keV"
           else:
               print message, "(energy will change throughout run)", "min =", min(energyArray[0][:]*1000), "keV, max =", max(energyArray[0][:]*1000),"keV"
               
           if gunTypes[0] != "Mono":    
              if sigmaEnergySet == 1:
                 print "  - Sigma energy:", energyArray[1][0]*1000, "keV"
              else:
                 print "  - Sigma energy:", "(energy will change throughout run)", " min =", min(energyArray[:][1]*1000), "keV, max =", max(energyArray[:][1]*1000),"keV"
               
           FMFluorescence = self.thisptr.FullMappingFluorescence()
           FFFluorescence = self.thisptr.FullFieldFluorescence()
               
           if FMFluorescence == True:
              print "- Full field fluorescence"
              
           if FFFluorescence == True:
              print "- Full mapping fluorescence"
           
           windowRows, windowColumns = os.popen('stty size', 'r').read().split()
           for i in range(int(windowColumns)):
              sys.stdout.write('-')
           
           self.nexusfile = NexusFormatter.NexusFormatter(self.SaveFilePath + self.NexusName)
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
               
               energyInfo = [energyArray[0][CurrentImage], energyArray[1][CurrentImage]]
               gunType = gunTypes[CurrentImage]
                            
               #pyRun returns the 1D array at the end of each run. Reshape it to make it 2D
               simOutput = np.reshape(self.thisptr.pyRun(TotalParticles, imageInfo, rotation_angle, energyInfo, gunType), (-1, xPixels))  
               
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
        
