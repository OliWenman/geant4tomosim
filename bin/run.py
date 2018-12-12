# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../build/src')

from G4Units import *
import sim
import numpy as np
import matplotlib.pyplot as plt
import h5py

nDetY = 250
nDetZ = 210
DetectorDimensions = [0.1, 0.32, 0.21]*mm

nBins = 20
NumberOfImages = 2
TotalRotation = 180*deg
nParticles = 1000000

fluoreFF = True
fluoreFM = True
beamEnergy = True

Sim = sim.PySim()

Sim.initialise(nDetY, nDetZ, DetectorDimensions, nBins)
Sim.outputOptions(fluoreFF, fluoreFM, beamEnergy)

Sim.run(nParticles, NumberOfImages, TotalRotation)
