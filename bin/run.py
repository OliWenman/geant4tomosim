# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
import matplotlib.pyplot as plt
sys.path.insert(0, './../build/src')

from G4Units import *
import sim
import numpy as np

nDetY = 25
nDetZ = 21
DetectorDimensions = [0.05, 0.05, 0.05]*mm

nBins = 250
NumberOfImages = 1
TotalRotation = 180*deg
nParticles = 10000000

Sim = sim.PySim()
Sim.initialise(nDetY, nDetZ, DetectorDimensions)

Sim.run(nParticles, NumberOfImages, TotalRotation, nBins)

#x = Sim.lastEnergyBins()
#y = Sim.lastEnergyFreq()
#y[nBins -1] = 0
#print(x,"\n")
#print(y)

#plt.plot(x,y)
#plt.show()

