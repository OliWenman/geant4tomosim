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

NumberOfImages = 1
nDarkFlatFields = 0
nParticles = 1000000

startRotation = 0*G4.deg
TotalRotation = 180*G4.deg
rotation_angles = np.linspace(start = startRotation, stop = TotalRotation, num = NumberOfImages, endpoint = False)

#===================================================================
#EXTRA DATA OPTIONS

fluoreFF = True
fluoreFM = False

#===================================================================
#ENERGY VARIABLES

minEnergy = 70*G4.keV
maxEnergy = 70*G4.keV
minSigmaEnergy = 0.5*G4.keV
maxSigmaEnergy = 5*G4.keV
Gun = "Mono"
nBins = 1000

energyArray = []
monoEnergies = np.linspace(start = minEnergy, stop = maxEnergy, num = NumberOfImages + nDarkFlatFields, endpoint = True)
energyArray.append(monoEnergies)
sigmaEnergies = np.linspace(start = minSigmaEnergy, stop = maxSigmaEnergy, num = NumberOfImages + nDarkFlatFields, endpoint = True)
energyArray.append(sigmaEnergies)
gunTypes = []
for e in range(NumberOfImages + nDarkFlatFields):
    gunTypes.append(Gun)
energyArray.append(gunTypes)

#===================================================================
#RUN THE SIMULATION

Sim = sim.PySim()

Sim.initialise(nDetY, nDetZ, DetectorDimensions, nBins)
Sim.outputOptions(fluoreFF, fluoreFM)
Sim.addMacroFiles(["./../scripts/pySettings.mac", "./../scripts/Geometry.mac"])
Sim.setFilePath("/scratch/Data/Complex_Object/ObjectAB/Absorption_Test/V4/")
Sim.run(nParticles, rotation_angles, nDarkFlatFields, energyArray)

#Sim.printNexusTree()
#Sim.plotBeamEnergy()
#Sim.plotFinalFluorescence()

#print "Photon transmission = ", Sim.photonTransmission()

