# creating a cython wrapper class
import numpy as np
import h5py
import time
import os
import gc
import getpass

import NexusFormatter

cdef class PySim:

    #Hold a C++ instance which we're wrapping
    cdef Simulation *thisptr     
    
    cdef public nexusfile

    #Makes the following available in Python-space:
    #Detector variables
    cdef public int nDetectorsY
    cdef public int nDetectorsZ
    cdef public int Bins

    #Output options for the simulation
    cdef public bint FFF 
    cdef public bint FFM 
    cdef public bint BE 

    #Simulation time
    cdef public double SimTime

    #Boolean if simulation is ready to go
    cdef public bint Ready
    
    cdef public str SaveFilePath
  
    #Constructor, create an instance of the C++ class
    def __cinit__(self):
        self.Ready = False
        self.FFF = False
        self.FFM = False
        self.BE = True
       
        #Creates the default path to save file 
        WorkingDirectory = os.path.dirname(os.getcwd())
        BuildDirectory = "/Output/HDF5/"
        self.SaveFilePath = WorkingDirectory + BuildDirectory
        
        self.thisptr = new Simulation()

    #Delete the C++ class
    def __dealloc__(self):
        del self.thisptr

    def outputOptions(self, bint fluoreFullField = False, bint fluoreFullMapping = True, bint beamEnergy = False):
        self.FFF = fluoreFullField
        self.FFM = fluoreFullMapping
        self.BE = beamEnergy
        self.thisptr.pyOutputOptions(self.FFF, self.FFM, self.BE)

    def dataPaths(self, a, b, c):
        self.thisptr.pyDataPaths(a, b, c)

    #Get needed information from textfiles pySettings.mac and Geometry.mac
    def initialise(self, int nDetY, int nDetZ, list DetectorDimensions, int nBins = 2000):
        if nDetY >= 1 and nDetZ >= 1:

           self.Bins = nBins

           #Call the C++ function
           self.thisptr.pyInitialise(nDetY, nDetZ, DetectorDimensions, self.Bins)
           
           #Make the number of detectors availiable for the Python class
           self.nDetectorsY = nDetY
           self.nDetectorsZ = nDetZ

           self.Ready = True

        else:
           print("\nError: The number of detectors for x and y should be greater or equal to 1! ")

    #Start the simulation
    def run(self, int TotalParticles, dTheta, int NumberOfImages):

        if TotalParticles >= 1 and NumberOfImages >= 1 and self.Ready == True:

           Path = './../Output/HDF5/'
           
           nCalibrations = self.thisptr.GetNumberCalibrations()
           TotalNumberOfProjections = NumberOfImages + (nCalibrations*2)

           self.nexusfile = NexusFormatter.NexusFormatter(self.SaveFilePath)
           self.nexusfile.CreateProjectionFolder(nCalibrations, TotalNumberOfProjections, self.nDetectorsZ, self.nDetectorsY)
           self.nexusfile.CreateRotationAngleData(dTheta, NumberOfImages, nCalibrations)
           
           print "The simulation will record the following data: "
           print "- Transmission"
           
           if self.FFF == True:
              self.nexusfile.CreateDataGroup("Fluorescence", nImages = NumberOfImages, eBins = self.Bins)
              print "- Fluorescence"
              
           if self.FFM == True:
              self.nexusfile.CreateDataGroup("Full_Mapping_Fluorescence", nImages = NumberOfImages, eBins = self.Bins, xBins = self.nDetectorsY, yBins = self.nDetectorsZ)
              print "- Full mapping fluorescence"
           
           if self.BE == True:
              self.nexusfile.CreateDataGroup("Beam_Energy", eBins = self.Bins)
              print "- The beam energy"
           
           iTime = time.time()
           
           Mode = "Calibrating"
           
           if nCalibrations == 0:
               self.thisptr.pyRun(0, dTheta, 0, nCalibrations, Mode), (-1, self.nDetectorsY)
           
           #Do flat field images
           for nImage in range(nCalibrations):
               #pyRun returns the 1D array at the end of each run. Reshape it to make it 2D
               calibrationOutPut = np.reshape(self.thisptr.pyRun(TotalParticles, dTheta, nImage, nCalibrations, Mode), (-1, self.nDetectorsY))  
               self.nexusfile.AddProjectionData(calibrationOutPut, nCalibrations + nImage)

           for nImage in range(NumberOfImages):

               Mode = "Simulating"
               #pyRun returns the 1D array at the end of each run. Reshape it to make it 2D
               simOutput = np.reshape(self.thisptr.pyRun(TotalParticles, dTheta, nImage, NumberOfImages, Mode), (-1, self.nDetectorsY))  
               
               iSavingTime = time.time()
               
               self.nexusfile.AddProjectionData(simOutput, nImage + (nCalibrations*2))
               
               if nImage == 0:
                  energyBins = self.lastEnergyBins()
                  
                  if self.FFF == True:
                     self.nexusfile.AddxAxis("Fluorescence", energyBins)
                     
                  if self.FFM == True:
                     self.nexusfile.AddxAxis("Full_Mapping_Fluorescence", energyBins)
                     
                  if self.BE == True:
                     self.nexusfile.AddxAxis("Beam_Energy", energyBins)
                     self.nexusfile.AddData("Beam_Energy", self.beamEnergy())                 
               
               if self.FFF == True:
                  self.nexusfile.AddData("Fluorescence", data = self.lastEnergyFreq(), nImage = nImage)
                  
               if self.FFM == True:
                  self.nexusfile.AddData("Full_Mapping_Fluorescence", data = self.fullMapping(), nImage = nImage)
                  
               eSavingTime = time.time()
               SavingTime  = eSavingTime - iSavingTime
               self.thisptr.SetSavingTime(SavingTime)

           #Ouput the time in the appropriate units
           eTime = time.time()
           self.SimTime = eTime -iTime

           message = "The total simulation time is"
           if self.SimTime < 60:
              print message, round(self.SimTime, 3), "seconds. "
           elif self.SimTime < 60*60:
              print message, round(self.SimTime/60, 3), "minutes. "
           else:
              print message, round(self.SimTime/(60*60), 3), "hours. "
            
        else:
           print("\nERROR: The number of particles and number of images should be greater or equal to 1! ")

    #Return the image data from the simulation
    def lastImage(self):
        return np.reshape(self.thisptr.GetLastImage(), (-1, self.nDetectorsY))
    
    def photonTransmission(self):
        return np.sum(self.thisptr.GetLastImage())

    def lastEnergyBins(self):
        return np.array(self.thisptr.GetEnergyBins())

    def lastEnergyFreq(self):
        return np.array(self.thisptr.GetEnergyFreq())

    def beamEnergy(self):
        return np.array(self.thisptr.GetBeamEnergyFreq())  

    def fullMapping(self):
        return np.array(self.thisptr.GetFullMapping())
        
    def setFilePath(self, FilePath):
        self.SaveFilePath = FilePath
        self.thisptr.SetSaveLogPath(FilePath)
        
    def printNexusTree(self):
        self.nexusfile.DisplayTree()
