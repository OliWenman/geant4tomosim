"""
Short script for running G4TomoSim with or without visualization on
and saving the data automatically with the simulatetomograpy function
"""

# -*- coding: utf-8 -*-
import os 
this_directory = os.path.dirname(os.path.realpath(__file__))

import sys
sys.path.insert(0, this_directory + '/../settings')
import tomosim_input as tsi


#Can optionally add a file path and name for the data to be saved when running script
if __name__ == '__main__':
    defaultpath = this_directory + "/../output/tomosimData.nxs"
    
    #Get the filepath from the first arguement
    try:
        filepath = sys.argv[1]
    
    #If no filepath given, save the data in the default filepath
    except IndexError:
        filepath = defaultpath
        print ("\nSaving data in the default place", filepath)


#===================================================================

import random

#RUN THE SIMULATION
from g4tomosim import G4TomoSim

tomosim = G4TomoSim(tsi.verbose, 
                    tsi.interactive)

tomosim.execute_macrolist(tsi.macrofiles)
tomosim.set_seed(random.randint(1, 1.e9))
"""
#For visualization, make there aren't many pixels otherwise will crash 
#HepRApp viewer
 
tomosim.execute_command("/detector/absorption/xpixels 5")
tomosim.execute_command("/detector/absorption/ypixels 3")

#Creates a directory to save all the vis data. Each projection simulated creates a 
#new vis file. 
tomosim.setup_visualization(path = this_directory + "/../output/",
                            filename = "simdata_vis")

tomosim.simulatetomography(filepath         = filepath,
                           n_particles      = 100,
                           rotation_angles  = tsi.rotation_angles,
                           n_darkflatfields = tsi.ndarkflatfields,
                           zpositions       = tsi.zpos)

"""
tomosim.simulatetomography(filepath         = filepath,
                           n_particles      = tsi.particles,
                           rotation_angles  = tsi.rotation_angles,
                           n_darkflatfields = tsi.ndarkflatfields,
                           zpositions      = tsi.zpos)                        

print ("Finished")

