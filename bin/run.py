# -*- coding: utf-8 -*-
import os 
this_directory = os.path.dirname(os.path.realpath(__file__))

import sys
sys.path.insert(0, this_directory + '/../settings')
import tomosim_input as tsi

#Can optionally add a file path and name for the data to be saved when running script
if __name__ == '__main__':
    defaultpath = this_directory + "/../Output/HDF5/SimulationData.nxs"
    
    #Get the filepath from the first arguement
    try:
        filepath = sys.argv[1]
    
    #If no filepath given, save the data in the default filepath
    except IndexError:
        filepath = defaultpath
        print "\nSaving data in the default place", filepath


#===================================================================
#RUN THE SIMULATION
import sim

print "verbose = ", tsi.verbose

tomosim = sim.G4TomoSim(tsi.verbose, 
                        tsi.interactive)
                
tomosim.addMacroFiles(tsi.macrofiles)

tomosim.simulatetomography(filepath        = filepath,
                           n_particles     = tsi.nParticles, 
                           rotation_angles = tsi.rotation_angles, 
                           nDarkFlatFields = tsi.nDarkFlatFields)
                      
"""
Sim.RunSingleProjection(1.e2,
                        0,
                        0,
                        10,
                        0)
"""
print "Finished"

