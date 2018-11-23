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
  
    #Constructor, create an instance of the C++ class
    def __cinit__(self):
        self.thisptr = new Simulation()

    #Delete the C++ class
    def __dealloc__(self):
        del self.thisptr

    #Get needed information from textfiles settings.mac and Geometry.mac
    def initialise(self, nDetY, nDetZ):

        #Make the number of detectors availiable for the Python class
        self.nDetectorsY = nDetY
        self.nDetectorsZ = nDetZ

        #Call the C++ function
        self.thisptr.pyInitialise(self.nDetectorsY, self.nDetectorsZ)

    #Start the simulation
    def run(self, TotalParticles, NumberOfImages, dTheta):

        #Create a h5 file to view the data after the simulation is complete
        h5file = h5py.File('./../build/Output/HDF5/ProjectionTest.h5', 'w')

        #print(os.path.dirname(os.path.abspath(__file__)))
        #print(os.chdir(os.path.dirname(os.getcwd())))

        dataset = h5file.create_dataset('TomographyData', shape=(self.nDetectorsZ, self.nDetectorsY, NumberOfImages))
        
        iTime = time.time()

        #Run the simulation for the number of images that are required
        for nImage in range(NumberOfImages):
            self.thisptr.pyRun(TotalParticles, nImage, NumberOfImages, dTheta)

            #Get the image from the simulation before it gets wiped
            Image = self.lastImage()       

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
            
    #Return the image data from the simulation
    def lastImage(self):
        return np.reshape(self.thisptr.GetLastImage(), (-1, self.nDetectorsY))

