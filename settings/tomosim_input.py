"""
Script that contains the python variables used for run.py and mpi_run.py
"""

import os 
this_directory = os.path.dirname(os.path.realpath(__file__))

import sys
sys.path.insert(0, this_directory + '/../src')

import units as G4
import numpy as np

#========================================================================================

#General settings
verbose         = 1
interactive     = False

#Image variables
projections     = 5
ndarkflatfields = 0
particles       = 1.e6
#Rotation angles
startrotation   = 0*G4.deg
endrotation     = 180.*G4.deg
rotation_angles = np.linspace(start    = startrotation,  \
                              stop     = endrotation,    \
                              num      = projections,    \
                              endpoint = False)
startpos = +0.*G4.mm
endpos   = -0.*G4.mm
zpos     = np.linspace(start    = startpos,    \
                       stop     = endpos,      \
                       num      = projections, \
                       endpoint = True)

#Macro files to be included
physics         = this_directory + "/physics.mac"
materials       = this_directory + "/materials.mac"
detectors       = this_directory + "/detectors.mac"
beam            = this_directory + "/beam.mac"

#Most basic of samples to most complicated.
#Uncomment the sample to change.
sample    = this_directory + "/samples/cube.mac"
#sample   = this_directory + "/samples/Al_Ni_block.mac"
#sample   = this_directory + "/samples/objectA.mac"
#sample   = this_directory + "/samples/objectB.mac"
#sample   = this_directory + "/samples/objectA_objectB.mac"

macrofiles      = [physics,
                   materials,
                   detectors,
                   beam,
                   sample]
                   
#========================================================================================
