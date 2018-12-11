# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../build/src')

from G4Units import *
import sim
import numpy as np
import matplotlib.pyplot as plt
import h5py

nDetY = 5
nDetZ = 5
DetectorDimensions = [0.1, 0.32, 0.21]*mm

nBins = 2000
NumberOfImages = 2
TotalRotation = 180*deg
nParticles = 1000000

fluoreFF = True
fluoreFM = False
beamEnergy = True

Sim = sim.PySim()

Sim.initialise(nDetY, nDetZ, DetectorDimensions, nBins)
Sim.outputOptions(fluoreFF, fluoreFM, beamEnergy)

Sim.run(nParticles, NumberOfImages, TotalRotation)

#fm = Sim.fullMapping()


#Path = './../build/Output/HDF5/'

#Create a h5 file to view the data after the simulation is complete
#h5file1 = h5py.File(Path + 'FullMappingTest.h5', 'w')
#BuildDirectory = "/build/Output/HDF5/"

#fullMappingGroup = h5file1.create_group('FullMapping')
          
#fullMappingSet = fullMappingGroup.create_dataset('Mapping', shape=(nBins, nDetY, nDetZ), dtype = 'i4')
#fullMappingSet[:,:,:] = fm

#==========================================================================
"""
xLabel = 'Energy'
yLabel = 'Photons'

# create the HDF5 NeXus file
DataSet = h5py.File("./../build/Output/HDF5/DataTest.h5", "w")
DataSet.create_dataset('title', data = 'Image and Fluorescence data')

ImageGroup = DataSet.create_group('Projections')
ImageDataSet = h5file.create_dataset('Images', shape=(self.nDetectorsZ, self.nDetectorsY, 5))



# create the NXentry group
EData = DataSet.create_group('Fluorescence')
EData.attrs['NX_class'] = 'NXdata'

EData.attrs['axes'] = xLabel         # X axis of default plot
EData.attrs['signal'] = yLabel      # Y axis of default plot
EData.attrs[xLabel + '_indices'] = [0,]   # use "mr" as the first dimension of I00

# X axis data
Energy = EData.create_dataset(xLabel, shape = (2500,))

# Y axis data
nPhotons = EData.create_dataset(yLabel, shape = (2500, 5))

Energy[:] = x[:]
nPhotons[:, 2] = y[:]


EnergyDataSet.close() 
"""
