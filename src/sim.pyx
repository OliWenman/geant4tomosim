# creating a cython wrapper class
import numpy as np
import h5py
import time
import os
import gc
import getpass

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
        self.FFM = False
        self.BE = True
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

           #Check to see if file is open
           for obj in gc.get_objects():   # Browse through ALL objects
               if isinstance(obj, h5py.File):   # Just HDF5 files
                  try:
                     obj.close()
                  except:
                     pass # Was already closed
                     
           rotationArray = np.linspace(start = 0, stop = dTheta, num = NumberOfImages) 
           calibrationLine = np.zeros(nCalibrations*2)
           rotationArray = np.concatenate([calibrationLine, rotationArray])

           #Create a h5 file to view the data after the simulation is complete
           h5file1 = h5py.File(Path + 'SimulationData.nxs', 'w')
           #Create a h5 file to view the data after the simulation is complete

           h5file1.attrs['default'] = 'entry'

           #/entry/entry1
           nxentry = h5file1.create_group('entry1')
           nxentry.attrs['NX_class'] = 'NXentry'
           nxentry.attrs['default'] = 'data'
           
           #/entry/entry1/tomo_entry
           tomo_entry = nxentry.create_group('tomo_entry')
           tomo_entry.attrs['NX_class'] = 'NXsubentry'
           
           
           #/entry/entry1/tomo_entry/user
           user = tomo_entry.create_group('user')
           user.attrs['NX_class'] = 'NXuser'
           user.create_dataset('username', data = getpass.getuser())
           
           #/entry/entry1/tomo_entry/data
           data = tomo_entry.create_group('data')
           data.attrs['NX_class'] = 'NXdata'
           data.attrs['defination'] = 'NXtomo'

           #/entry/entry1/tomo_entry/data/Images
           imageSet = data.create_dataset('Images', shape=(TotalNumberOfProjections, self.nDetectorsZ, self.nDetectorsY), dtype = 'i4')

           #/entry/entry1/tomo_entry/data/rotation_angle
           rotation_angle = data.create_dataset('rotation_angle', data = rotationArray)
           rotation_angle.attrs['axis'] = '1'
           rotation_angle.attrs['label'] = '1'
           rotation_angle.attrs['units'] = 'radians'

           WorkingDirectory = os.path.dirname(os.getcwd())
           BuildDirectory = "/Output/HDF5/"
           FullPath = WorkingDirectory + BuildDirectory
           print "\nThe data will be saved in:", FullPath

           print("The simulation will record the following data: ")
           print("- Transmission")
           
           #If the energy data is to be recored, setup the h5file
           if self.FFF == True:

              print("- Full field fluorescence")     
              #Fluorescence data
              xLabel = "Energy(keV)"
              yLabel = "Photons"
              fluorescenceGroup = data.create_group('Fluorescence')
              fluorescenceGroup.attrs['NX_class'] = 'NXdata'

              fluorescenceGroup.attrs['axes'] = xLabel         # X axis of default plot
              #fluorescenceGroup.attrs['signal'] = yLabel      # Y axis of default plot
              #fluorescenceGroup.attrs[xLabel + '_indices'] = [0,]   # use "mr" as the first dimension of I00
              fluorescenceGroup.attrs[yLabel + '_indices'] = [1,]   # use "mr" as the first dimension of I00

              fluorescenceSet = fluorescenceGroup.create_dataset(xLabel, shape = (self.Bins,), dtype = 'f8')  # X axis data
              fluorPhotonSet = fluorescenceGroup.create_dataset(yLabel, shape = (NumberOfImages, self.Bins ), dtype = 'i4')  # Y axis data

           if self.FFM == True:

              print("- Full mapping fluorescence")
              #Fluorescence data
              xLabel = "Energy(keV)"
              yLabel = "Photons"
              fluorescenceFMGroup = data.create_group('FluorescenceFM')
              fluorescenceFMGroup.attrs['NX_class'] = 'NXdata'

              fluorescenceFMGroup.attrs['axes'] = xLabel         # X axis of default plot
              #fluorescenceFMGroup.attrs['signal'] = yLabel      # Y axis of default plot
              fluorescenceFMGroup.attrs[xLabel + '_indices'] = [1,]   # use "mr" as the first dimension of I00

              fluorescenceFMSet = fluorescenceFMGroup.create_dataset(xLabel, shape = (self.Bins,), dtype = 'f8')  # X axis data
              
              fluorPhotonFMSet = fluorescenceFMGroup.create_dataset(yLabel, shape = (NumberOfImages, self.Bins, self.nDetectorsY, self.nDetectorsZ), dtype = 'i4')  # Y axis data           

           if self.BE == True:

              print("- Beam energy")
              #Beam energy data
              xLabel = "Energy(keV)"
              yLabel = "Photons"
              beamGroup = data.create_group('BeamEnergy')
              beamGroup.attrs['NX_class'] = 'NXdata'

              beamGroup.attrs['axes'] = xLabel         # X axis of default plot
              beamGroup.attrs['signal'] = yLabel      # Y axis of default plot
              beamGroup.attrs[xLabel + '_indices'] = [0,]   # use "mr" as the first dimension of I00

              energybeamSet = beamGroup.create_dataset(xLabel, shape = (self.Bins,), dtype = 'f8')
              energyphotonSet = beamGroup.create_dataset(yLabel, shape = (self.Bins,), dtype = 'i4')

           iTime = time.time()
           
           Mode = "Calibrating"
           if nCalibrations == 0:
               self.thisptr.pyRun(0, dTheta, 0, nCalibrations, Mode), (-1, self.nDetectorsY)
           
           #Do flat field images
           for nImage in range(nCalibrations):
               #pyRun returns the 1D array at the end of each run. Reshape it to make it 2D
               calibrationOutPut = np.reshape(self.thisptr.pyRun(TotalParticles, dTheta, nImage, nCalibrations, Mode), (-1, self.nDetectorsY))  
               #Append the 2D Data to a 3D data set
               imageSet[nCalibrations + nImage, :, :] = calibrationOutPut[:, :]     
           
           #Run the simulation for the number of images that are required
           for nImage in range(NumberOfImages):

               Mode = "Simulating"
               #pyRun returns the 1D array at the end of each run. Reshape it to make it 2D
               simOutput = np.reshape(self.thisptr.pyRun(TotalParticles, dTheta, nImage, NumberOfImages, Mode), (-1, self.nDetectorsY))  
               
               #Append the 2D Data to a 3D data set
               imageSet[nImage + (nCalibrations*2), :, :] = simOutput[:, :]
               #print("\nTransmission data saved")
               iSavingTime = time.time()
               if nImage == 0:
                  energyBins = self.lastEnergyBins()
                  if self.FFF == True:
                     fluorescenceSet[:] = energyBins

                  if self.FFM == True:
                     fluorescenceFMSet[:] = energyBins

                  if self.BE == True:
                     energybeamSet[:] = energyBins
                     energyphotonSet[:] = self.beamEnergy()

               if self.FFF == True:
                  #Append the energy frequency to the 2D data
                  fluorPhotonSet[nImage, :] = self.lastEnergyFreq()
                  #print("Full field fluorescence data saved")

               if self.FFM == True:
                  fluorPhotonFMSet[nImage, :, :, :] = self.fullMapping()
                  
               eSavingTime = time.time()
               SavingTime  = eSavingTime - iSavingTime
               self.thisptr.SetSavingTime(SavingTime)
                  
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
