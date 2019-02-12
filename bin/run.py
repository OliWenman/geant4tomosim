# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../src')
import G4Units as G4
import sim
import numpy as np

#===================================================================
#DETECTOR VARIABLES

nDetY = 250
nDetZ = 210
DetectorDimensions = [0.001, 0.005, 0.005]*G4.mm

#===================================================================
#IMAGE VARIABLES

NumberOfImages = 10
nDarkFlatFields = 0
nParticles = 1000000

TotalRotation = 180*G4.deg
rotation_angles = np.linspace(start = 0, stop = TotalRotation, num = NumberOfImages, endpoint = False)

#===================================================================
#EXTRA DATA OPTIONS

fluoreFF = True
fluoreFM = True

#===================================================================
#ENERGY VARIABLES

minEnergy = 25*G4.keV
maxEnergy = 25*G4.keV
minSigmaEnergy = 0.5*G4.keV
maxSigmaEnergy = 5*G4.keV
Gun = "Gauss"
nBins = 1000

energyArray = []
monoEnergies = np.linspace(start = minEnergy, stop = maxEnergy, num = NumberOfImages, endpoint = True)
energyArray.append(monoEnergies)
sigmaEnergies = np.linspace(start = minSigmaEnergy, stop = maxSigmaEnergy, num = NumberOfImages, endpoint = True)
energyArray.append(sigmaEnergies)
gunTypes = []
for e in range(NumberOfImages):
    gunTypes.append(Gun)
energyArray.append(gunTypes)

#===================================================================
#RUN THE SIMULATION

Sim = sim.PySim()

Sim.initialise(nDetY, nDetZ, DetectorDimensions, nBins)
Sim.outputOptions(fluoreFF, fluoreFM)
#Sim.setFilePath("/scratch/Data/Complex Object/ObjectAB/AlSi10Mg_TC4/V4/")
#Sim.setFilePath("/home/xol73553/Documents/NeXusTesting/")
#Sim.setFilePath('/scratch/Data/Complex Object/ObjectAB/Daniil/')
#Sim.setFilePath("/scratch/Data/Complex Object/MultiSimTest/1/")
#Sim.setFilePath('/scratch/Data/Complex_Object/ObjectAB/IN625_MS1/V1/')
#Sim.run(nParticles, TotalRotation, NumberOfImages)
Sim.run(nParticles, rotation_angles, nDarkFlatFields, energyArray)

#Sim.printNexusTree()
#Sim.plotBeamEnergy()
#Sim.plotFinalFluorescence()

#print "Photon transmission = ", Sim.photonTransmission()

