# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../src')
import matplotlib.pyplot as plt

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
fluoreFM = True
beamEnergy = True

Sim = sim.PySim()

Sim.initialise(nDetY, nDetZ, DetectorDimensions, nBins)
Sim.outputOptions(fluoreFF, fluoreFM, beamEnergy)
#Sim.setFilePath("/scratch/Data/Complex Object/ObjectAB/AlSi10Mg_TC4/V2/")

Sim.run(nParticles, TotalRotation, NumberOfImages)

#Sim.printNexusTree()
#Sim.plotBeamEnergy()
#Sim.plotFinalFluorescence()

#print "Photon transmission = ", Sim.photonTransmission()

