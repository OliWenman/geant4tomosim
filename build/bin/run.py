# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../src')

import sim
import numpy as np
from matplotlib import pyplot as plt
import h5py

Sim = sim.PySim()
Sim.initialize();
#Sim.initialize(nDetectorsY, nDetectorsZ, NumberOfImages, TotalAngle)
#Sim.PyRun(PhotonsPerImage)
NumberOfImages = 5

h5file = h5py.File('./../Output/HDF5/test2.h5', 'a')
dataset = h5file.create_dataset('data', shape=(210,250,NumberOfImages))

TotalRotation = 180

for nImage in range(NumberOfImages):
	Sim.PyRun(nImage, NumberOfImages, TotalRotation)
	Image = Sim.lastImage()
	dataset[:, :, nImage] = Image[:, :]

h5file.close()




