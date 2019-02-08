# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../src')
import G4Units as G4
import sim

nDetY = 250
nDetZ = 210
DetectorDimensions = [0.001, 0.005, 0.005]*G4.mm

nBins = 1000
NumberOfImages = 100
TotalRotation = 180*G4.deg
nParticles = 500000000

fluoreFF = False
fluoreFM = False

Sim = sim.PySim()

Sim.initialise(nDetY, nDetZ, DetectorDimensions, nBins)
Sim.outputOptions(fluoreFF, fluoreFM)
#Sim.setFilePath("/scratch/Data/Complex Object/ObjectAB/AlSi10Mg_TC4/V4/")
#Sim.setFilePath("/home/xol73553/Documents/NeXusTesting/")
#Sim.setFilePath('/scratch/Data/Complex Object/ObjectAB/Daniil/')
#Sim.setFilePath("/scratch/Data/Complex Object/MultiSimTest/1/")
Sim.setFilePath('/scratch/Data/Complex_Object/ObjectAB/IN625_MS1/V1/')
Sim.run(nParticles, TotalRotation, NumberOfImages)

#Sim.printNexusTree()
#Sim.plotBeamEnergy()
#Sim.plotFinalFluorescence()

#print "Photon transmission = ", Sim.photonTransmission()

