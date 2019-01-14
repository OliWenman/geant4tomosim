# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../src')

#from G4Units import *
import G4Units as G4
import sim

nDetY = 25
nDetZ = 21
DetectorDimensions = [0.01, 0.05, 0.05]*G4.mm

nBins = 100
NumberOfImages = 5
TotalRotation = 180*G4.deg
nParticles = 1000

fluoreFF = False
fluoreFM = False
beamEnergy = False

Sim = sim.PySim()

#Sim.dataPaths("sdfjkjksdfa", "ajsdb", "ksdbjsd")
Sim.initialise(nDetY, nDetZ, DetectorDimensions, nBins)
Sim.outputOptions(fluoreFF, fluoreFM, beamEnergy)

Sim.run(nParticles, TotalRotation, NumberOfImages)

#print "Photon transmission = ", Sim.photonTransmission()

