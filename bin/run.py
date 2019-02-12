# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../src')
import G4Units as G4
import sim
import numpy as np

nDetY = 250
nDetZ = 210
DetectorDimensions = [0.001, 0.005, 0.005]*G4.mm

NumberOfImages = 1
nDarkFlatFields = 1
TotalRotation = 180*G4.deg
nParticles = 10000000

maxEnergy = 100*G4.keV
minEnergy = 70*G4.keV
maxSigmaEnergy = 0*G4.keV
minSigmaEnergy = 0*G4.keV
Gun = "Mono"

energyArray = []

nBins = 1000

monoEnergies = np.linspace(start = minEnergy, stop = maxEnergy, num = NumberOfImages)
energyArray.append(monoEnergies)
sigmaEnergies = np.linspace(start = minSigmaEnergy, stop = maxSigmaEnergy, num = NumberOfImages)
energyArray.append(sigmaEnergies)
gunTypes = []
for e in range(NumberOfImages):
    gunTypes.append(Gun)
energyArray.append(gunTypes)

print energyArray

rotation_angles = np.linspace(start = 0, stop = TotalRotation, num = NumberOfImages)

fluoreFF = False
fluoreFM = False

Sim = sim.PySim()

Sim.initialise(nDetY, nDetZ, DetectorDimensions, nBins)
Sim.outputOptions(fluoreFF, fluoreFM)
#Sim.setFilePath("/scratch/Data/Complex Object/ObjectAB/AlSi10Mg_TC4/V4/")
#Sim.setFilePath("/home/xol73553/Documents/NeXusTesting/")
#Sim.setFilePath('/scratch/Data/Complex Object/ObjectAB/Daniil/')
#Sim.setFilePath("/scratch/Data/Complex Object/MultiSimTest/1/")
#Sim.setFilePath('/scratch/Data/Complex_Object/ObjectAB/IN625_MS1/V1/')
#Sim.run(nParticles, TotalRotation, NumberOfImages)
Sim.runTest(nParticles, rotation_angles, nDarkFlatFields, energyArray)

#Sim.printNexusTree()
#Sim.plotBeamEnergy()
#Sim.plotFinalFluorescence()

#print "Photon transmission = ", Sim.photonTransmission()

