# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../src')

#from G4Units import *
import G4Units as G4
import sim

nDetY = 250
nDetZ = 210
DetectorDimensions = [0.001, 0.005, 0.005]*G4.mm

nBins = 1000
NumberOfImages = 1
TotalRotation = 180*G4.deg
nParticles = 10000000

fluoreFF = False
fluoreFM = False
beamEnergy = False

Sim = sim.PySim()

#Sim.dataPaths("sdfjkjksdfa", "ajsdb", "ksdbjsd")
Sim.initialise(nDetY, nDetZ, DetectorDimensions, nBins)
Sim.outputOptions(fluoreFF, fluoreFM, beamEnergy)
#Sim.setFilePath("/home/xol73553/Documents/NeXusTesting")

Sim.run(nParticles, TotalRotation, NumberOfImages)

#print "Photon transmission = ", Sim.photonTransmission()

