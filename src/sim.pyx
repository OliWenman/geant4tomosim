# creating a cython wrapper class
import numpy as np
import h5py
import time
import os

cdef class PySim:

    #Hold a C++ instance which we're wrapping
    cdef Simulation *thisptr     

    #Makes the following available in Python-space:
    cdef public int nDetectorsY
    cdef public int nDetectorsZ

    cdef public double SimTime

    cdef public bint Ready
  
    #Constructor, create an instance of the C++ class
    def __cinit__(self):
        self.Ready = False
        self.thisptr = new Simulation()

    #Delete the C++ class
    def __dealloc__(self):
        del self.thisptr

    #Get needed information from textfiles settings.mac and Geometry.mac
    def initialise(self, int nDetY, int nDetZ):
        if nDetY >= 1 and nDetZ >= 1:

           #Make the number of detectors availiable for the Python class
           self.nDetectorsY = nDetY
           self.nDetectorsZ = nDetZ

           #Call the C++ function
           self.thisptr.pyInitialise(self.nDetectorsY, self.nDetectorsZ)

           self.Ready = True

        else:
           print("\nError: The number of detectors for x and y should be greater or equal to 1! ")

    #Start the simulation
    def run(self, int TotalParticles, int NumberOfImages, dTheta):

        if TotalParticles >= 1 and NumberOfImages >= 1 and self.Ready == True:

           #Create a h5 file to view the data after the simulation is complete
           h5file = h5py.File('./../build/Output/HDF5/ProjectionTest.h5', 'w')

           #print(os.path.dirname(os.path.abspath(__file__)))
           #print(os.chdir(os.path.dirname(os.getcwd())))

           dataset = h5file.create_dataset('TomographyData', shape=(self.nDetectorsZ, self.nDetectorsY, NumberOfImages))
        
           iTime = time.time()

           #Run the simulation for the number of images that are required
           for nImage in range(NumberOfImages):

               #pyRun returns the 1D array at the end of each run. Reshape it to make it 2D
               Image = np.reshape(self.thisptr.pyRun(TotalParticles, nImage, NumberOfImages, dTheta), (-1, self.nDetectorsY))  

               #Append the 2D Data to a 3D data set
               dataset[:, :, nImage] = Image[:, :]

           #Close the file
           h5file.close()

           #Ouput the time in the appropriate units
           eTime = time.time()
           self.SimTime = eTime -iTime

           message = "The total simulation time is"
           if self.SimTime < 60:
              print message, round(self.SimTime, 3), "seconds. "
           elif self.SimTime < 60*60:
              print message, round(self.SimTime/60, 3), "minutes. "
           else:
              print message, round(self.SimTime/60*60, 3), "hours. "
            
        else:
           print("\nERROR: The number of particles and number of images should be greater or equal to 1! ")

    #Return the image data from the simulation
    def lastImage(self):
        return np.reshape(self.thisptr.GetLastImage(), (-1, self.nDetectorsY))

