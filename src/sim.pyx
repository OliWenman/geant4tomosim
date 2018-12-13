# creating a cython wrapper class
import numpy as np
import h5py
import time
import os

cdef class PySim:

    #Hold a C++ instance which we're wrapping
    cdef Simulation *thisptr     

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
  
    #Constructor, create an instance of the C++ class
    def __cinit__(self):
        self.Ready = False
        self.FFF = False
        self.FFM = True
        self.BE = False
        self.thisptr = new Simulation()

    #Delete the C++ class
    def __dealloc__(self):
        del self.thisptr

    def outputOptions(self, bint fluoreFullField = False, bint fluoreFullMapping = True, bint beamEnergy = False):
        self.FFF = fluoreFullField
        self.FFM = fluoreFullMapping
        self.BE = beamEnergy
        self.thisptr.pyOutputOptions(self.FFF, self.FFM, self.BE)

    #Get needed information from textfiles pySettings.mac and Geometry.mac
    def initialise(self, int nDetY, int nDetZ, list DetectorDimensions, int nBins = 2000):
        if nDetY >= 1 and nDetZ >= 1:

           #Make the number of detectors availiable for the Python class
           self.nDetectorsY = nDetY
           self.nDetectorsZ = nDetZ

           self.Bins = nBins

           #Call the C++ function
           self.thisptr.pyInitialise(self.nDetectorsY, self.nDetectorsZ, DetectorDimensions, self.Bins)

           self.Ready = True

        else:
           print("\nError: The number of detectors for x and y should be greater or equal to 1! ")

    #Start the simulation
    def run(self, int TotalParticles, int NumberOfImages, dTheta):

        if TotalParticles >= 1 and NumberOfImages >= 1 and self.Ready == True:

           Path = './../build/Output/HDF5/'

           #Create a h5 file to view the data after the simulation is complete
           h5file1 = h5py.File(Path + 'Projections.h5', 'w')

           WorkingDirectory = os.path.dirname(os.getcwd())
           BuildDirectory = "/build/Output/HDF5/"
           FullPath = WorkingDirectory + BuildDirectory
           print "The data will be saved in:", FullPath

           imageGroup = h5file1.create_group('Projections')
           #imageGroup.attrs['NX_class'] = 'NXdata'
           imageSet = imageGroup.create_dataset('Images', shape=(self.nDetectorsZ, self.nDetectorsY, NumberOfImages), dtype = 'i4')

           print("\nThe simulation will record the following data: ")
           print("- Transmission")
           #If the energy data is to be recored, setup the h5file
           if self.FFF == True:

              print("- Full field fluorescence")     
              #Fluorescence data
              xLabel = "Energy(keV)"
              yLabel = "Photons"
              fluorescenceGroup = h5file1.create_group('Fluorescence')
              fluorescenceGroup.attrs['NX_class'] = 'NXdata'

              fluorescenceGroup.attrs['axes'] = xLabel         # X axis of default plot
              fluorescenceGroup.attrs['signal'] = yLabel      # Y axis of default plot
              fluorescenceGroup.attrs[xLabel + '_indices'] = [0,]   # use "mr" as the first dimension of I00

              fluorescenceSet = fluorescenceGroup.create_dataset(xLabel, shape = (self.Bins,), dtype = 'f8')  # X axis data
              fluorPhotonSet = fluorescenceGroup.create_dataset(yLabel, shape = (self.Bins, NumberOfImages), dtype = 'i4')  # Y axis data

           if self.FFM == True:

              print("- Full mapping fluorescence")
              #Fluorescence data
              xLabel = "Energy(keV)"
              yLabel = "Photons"
              fluorescenceFMGroup = h5file1.create_group('FluorescenceFM')
              fluorescenceFMGroup.attrs['NX_class'] = 'NXdata'

              fluorescenceFMGroup.attrs['axes'] = xLabel         # X axis of default plot
              fluorescenceFMGroup.attrs['signal'] = yLabel      # Y axis of default plot
              fluorescenceFMGroup.attrs[xLabel + '_indices'] = [0,]   # use "mr" as the first dimension of I00

              fluorescenceFMSet = fluorescenceFMGroup.create_dataset(xLabel, shape = (self.Bins,), dtype = 'f8')  # X axis data
              fluorPhotonFMSet = fluorescenceFMGroup.create_dataset(yLabel, shape = (self.Bins, self.nDetectorsY, self.nDetectorsZ, NumberOfImages), dtype = 'i4')  # Y axis data           

           if self.BE == True:

              print("- Beam energy")
              #Beam energy data
              xLabel = "Energy(keV)"
              yLabel = "Photons"
              beamGroup = h5file1.create_group('BeamEnergy')
              beamGroup.attrs['NX_class'] = 'NXdata'

              beamGroup.attrs['axes'] = xLabel         # X axis of default plot
              beamGroup.attrs['signal'] = yLabel      # Y axis of default plot
              beamGroup.attrs[xLabel + '_indices'] = [0,]   # use "mr" as the first dimension of I00

              energybeamSet = beamGroup.create_dataset(xLabel, shape = (self.Bins,), dtype = 'f8')
              energyphotonSet = beamGroup.create_dataset(yLabel, shape = (self.Bins,), dtype = 'i4')

           iTime = time.time()

           #Run the simulation for the number of images that are required
           for nImage in range(NumberOfImages):

               #pyRun returns the 1D array at the end of each run. Reshape it to make it 2D
               simOutput = np.reshape(self.thisptr.pyRun(TotalParticles, nImage, NumberOfImages, dTheta), (-1, self.nDetectorsY))  

               #Append the 2D Data to a 3D data set
               imageSet[:, :, nImage] = simOutput[:, :]
               #print("\nTransmission data saved")
               
               if nImage == 0:
                  energyBins = self.lastEnergyBins()
                  if self.FFF == True:
                     fluorescenceSet[:] = energyBins

                  if self.FFM == True:
                     fluorescenceFMSet[:] = energyBins

                  if self.BE == True:
                     energybeamSet[:] = energyBins
                     energyphotonSet[:] = self.beamEnergy()
                     print("Beam energy data saved")

               if self.FFF == True:
                  #Append the energy frequency to the 2D data
                  fluorPhotonSet[:, nImage] = self.lastEnergyFreq()
                  #print("Full field fluorescence data saved")

               if self.FFM == True:
                  fluorPhotonFMSet[:, :, :, nImage] = self.fullMapping()
                  print("Full mapping fluorescence data saved")
                  
           #Close the files
           h5file1.close()

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

    def lastEnergyBins(self):
        return np.array(self.thisptr.GetEnergyBins())

    def lastEnergyFreq(self):
        return np.array(self.thisptr.GetEnergyFreq())

    def beamEnergy(self):
        return np.array(self.thisptr.GetBeamEnergyFreq())  

    def fullMapping(self):
        return np.array(self.thisptr.GetFullMapping())


