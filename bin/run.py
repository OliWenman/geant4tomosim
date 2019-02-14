# -*- coding: utf-8 -*-
#Used to find the .so file built using CMake
import sys
sys.path.insert(0, './../src')
import G4Units as G4
import sim
import numpy as np
import os

#Can optionally add a file path and name for the data to be saved when runninh script
if __name__ == '__main__':
    #Input equals the option given or the default one  
    try:
        Input = sys.argv[1]
    except IndexError:
        Input = "./../Output/HDF5/SimulationData.nxs"
        print "\nSaving data in the default place", Input

    stringLength = len(Input)
    
    pathFound = False
    success = True;

    #Loop through the string to find the path and the name
    for i in reversed(range(stringLength)):
        char = Input[i]
        
        #Finds the path and then can seperate the variables to path and name
        if char == '/':
  
            FileName = Input[i+1:stringLength]      
            FilePath = Input[0:i+1]
            pathFound = True
        
            break
    
    #If no path found, save it in the current directory
    if pathFound == False:    
        FileName = Input
        FilePath = os.getcwd() + '/'
    
    extension = Input[-4:stringLength]
    #If the file doesn't have the .nxs extension, ask for another input
    if extension != '.nxs':
        print "\nERROR: the extension", FileName,"is not valid. Please input the file with a \'.nxs\' extension. If no input given, will automatically save data as ./../Output/HDF5/SimulationData.nxs"
        success = False
    
    pathOkay =  os.path.isdir(FilePath)
            
    if pathOkay == False:
        print "\nERROR: the directory \'", FilePath, "\' doesn't exist."
        success = False
    
    if success == False:
        sys.exit()    
    
    dotPosition = FileName.find('.')
    logName = FileName[0:dotPosition] + '_log.txt'
    
#===================================================================
#DETECTOR VARIABLES

nDetY = 250
nDetZ = 210
DetectorDimensions = [0.001, 0.005, 0.005]*G4.mm

#===================================================================
#IMAGE VARIABLES

NumberOfImages = 10
nDarkFlatFields = 5
nParticles = 1000000

startRotation = 180*G4.deg
TotalRotation = 0*G4.deg
rotation_angles = np.linspace(start = startRotation, stop = TotalRotation, num = NumberOfImages, endpoint = False)

#===================================================================
#EXTRA DATA OPTIONS

fluoreFF = False
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
Sim.setFilePath(FilePath, FileName, logName)
Sim.run(nParticles, rotation_angles, nDarkFlatFields, energyArray)

#Sim.printNexusTree()
#Sim.plotBeamEnergy()
#Sim.plotFinalFluorescence()

#print "Photon transmission = ", Sim.photonTransmission()

