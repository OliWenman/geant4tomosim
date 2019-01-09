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
DetectorDimensions = [0.001, 0.005, 0.005]*mm

nBins = 1000
NumberOfImages = 1
TotalRotation = 180*deg
nParticles = 10000000

fluoreFF = True
fluoreFM = True
beamEnergy = True

Sim = sim.PySim()

#Sim.dataPaths("sdfjkjksdfa", "ajsdb", "ksdbjsd")
Sim.initialise(nDetY, nDetZ, DetectorDimensions, nBins)
Sim.outputOptions(fluoreFF, fluoreFM, beamEnergy)

Sim.run(nParticles, NumberOfImages, TotalRotation)

print "Photon transmission = ", Sim.photonTransmission()

