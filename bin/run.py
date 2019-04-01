# -*- coding: utf-8 -*-
import os 
this_directory = os.path.dirname(os.path.realpath(__file__))

import sys
sys.path.insert(0, this_directory + '/../settings')
import tomosim_input as tsi

#Can optionally add a file path and name for the data to be saved when running script
if __name__ == '__main__':
    default = this_directory + "/../Output/HDF5/SimulationData.nxs"
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
#RUN THE SIMULATION
import sim

print "verbose = ", tsi.verbose

Sim = sim.PySim(tsi.verbose, tsi.interactive)
Sim.addMacroFiles(tsi.macrofiles)
Sim.setFilePath(FilePath, FileName, logName)

Sim.run(tsi.nParticles, tsi.rotation_angles, tsi.nDarkFlatFields)


print "Finished"

#Sim.printNexusTree()
#Sim.plotBeamEnergy()
#Sim.plotFinalFluorescence()

#print "Photon transmission = ", Sim.photonTransmission()

