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


#filepath = this_directory + "/../output/tomosimData.nxs"

#===================================================================

#RUN THE SIMULATION
from g4tomosim import G4TomoSim

tomosim = G4TomoSim(tsi.verbose, 
                    tsi.interactive)
                 
tomosim.execute_macrolist(tsi.macrofiles)

tomosim.simulatetomography(filepath        = filepath,
                           n_particles     = tsi.particles, 
                           nDarkFlatFields = tsi.ndarkflatfields,
                           rotation_angles = tsi.rotation_angles,
                           zpositions      = tsi.zpos )
print ("Finished")

"""
from g4tomosim import G4TomoSim

tomosim = G4TomoSim(verbose = 1)
                    
tomosim.execute_macrolist(macroFiles = ["detector.mac", 
                                        "physics.mac", 
                                        "beam.mac",       
                                        "sample.mac"])

tomosim.simulateprojection(n_particles    = 1.e9,
                           flatfield      = False,
                           rotation_angle = 21*deg,
                           zposition      = 0*m)

absorb_data = tomosim.absorptiondetector_getprojection()
fluor_data  = tomosim.fluorescencedetector_getfullmappingdata()
beamIn_data = tomosim.beam_getintensity()
beamEn_data = tomosim.beam_getenergybins()
"""
