# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../build/src')

from G4Units import *
import sim
import numpy as np
import matplotlib.pyplot as plt
import numpy as np
import h5py

nDetY = 25
nDetZ = 21
DetectorDimensions = [0.05, 0.05, 0.05]*mm

nBins = 2500
NumberOfImages = 3
TotalRotation = 180*deg
nParticles = 5000000

Sim = sim.PySim()
Sim.initialise(nDetY, nDetZ, DetectorDimensions)

#Sim.run(nParticles, NumberOfImages, TotalRotation, nBins)

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
