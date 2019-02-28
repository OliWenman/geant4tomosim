# -*- coding: utf-8 -*-
import sys
sys.path.insert(0, './../src')
import G4Units as G4
import numpy as np

#VARIABLES TO CHANGE
#===================================================================
#DETECTOR VARIABLES

nDetY = 375
nDetZ = 315

fullLengthDet_x = 1*G4.um
fullLengthDet_y = 1.25*G4.cm
fullLengthDet_z = 1.05*G4.cm

DetectorDimensions = np.array([fullLengthDet_x, fullLengthDet_y/nDetY, fullLengthDet_z/nDetZ])

#-------------------------------------------------------------------
#IMAGE VARIABLES

NumberOfImages = 1
nDarkFlatFields = 0
nParticles = 1#5*10**7#1*10**7#5*10**8

startRotation = 0*G4.deg
TotalRotation = 180*G4.deg
rotation_angles = np.linspace(start = startRotation, stop = TotalRotation, num = NumberOfImages, endpoint = False)

#-------------------------------------------------------------------
#RECORD EXTRA DATA

fullfield_fluorescence = False 
fullmapping_fluorescence = False

#-------------------------------------------------------------------
#ENERGY VARIABLES

minEnergy = 20*G4.keV
maxEnergy = 20*G4.keV
minSigmaEnergy = 0*G4.keV
maxSigmaEnergy = 0*G4.keV
Gun = "Mono"
nBins = 2000

#-------------------------------------------------------------------
#Macrofiles to add
macrofile1 = "./../scripts/Settings.mac" 
macrofile2 = "./../scripts/Geometry.mac"

macrofiles = [macrofile1, macrofile2]

#==================================================================
import os

#Can optionally add a file path and name for the data to be saved when running script
if __name__ == '__main__':
    default = "./../Output/HDF5/SimulationData.nxs"
    #Input equals the option given or the default one  
    try:
        Input = sys.argv[1]
    except IndexError:
        Input = default
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
    
    nexusExtension = Input[-4:stringLength]
    #If the file doesn't have the .nxs extension, ask for another input
    if nexusExtension != '.nxs':
        print "\nERROR: the extension", FileName,"is not valid. Please input the file with a \'.nxs\' extension. If no input given, will automatically save data as", default
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
#CREATE THE ENERGY ARRAY    

energyArray = []
monoEnergies = np.linspace(start = minEnergy, stop = maxEnergy, num = NumberOfImages + nDarkFlatFields, endpoint = True)
energyArray.append(monoEnergies)
sigmaEnergies = np.linspace(start = minSigmaEnergy, stop = maxSigmaEnergy, num = NumberOfImages + nDarkFlatFields, endpoint = True)
energyArray.append(sigmaEnergies)
gunTypes = []
for e in range(NumberOfImages + nDarkFlatFields):
    gunTypes.append(Gun)
#energyArray.append(gunTypes)

#===================================================================
#RUN THE SIMULATION
import sim

Sim = sim.PySim()

Sim.setupDetectors(nDetY, nDetZ, DetectorDimensions, nBins)
Sim.outputOptions(fullfield_fluorescence, fullmapping_fluorescence)
Sim.addMacroFiles(macrofiles)
Sim.setFilePath(FilePath, FileName, logName)
Sim.run(nParticles, rotation_angles, nDarkFlatFields, energyArray, gunTypes)

#Sim.printNexusTree()
#Sim.plotBeamEnergy()
#Sim.plotFinalFluorescence()

#print "Photon transmission = ", Sim.photonTransmission()

