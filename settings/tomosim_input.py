# Python script that contains the values to control the python varaibles for the simulation

import os 
this_directory = os.path.dirname(os.path.realpath(__file__))

import sys
sys.path.insert(0, this_directory + '/../src')

import units as G4
import numpy as np

#========================================================================================

#General settings
verbose         = 5
interactive     = True

#Image variables
NumberOfImages  = 1
nDarkFlatFields = 0
nParticles      = 1*10**5

#Rotation angles
startRotation   = 0*G4.deg
TotalRotation   = 180.*G4.deg
rotation_angles = np.linspace(start    = startRotation,  \
                              stop     = TotalRotation,  \
                              num      = NumberOfImages, \
                              endpoint = False)

#Macro files to be included
physics         = this_directory + "/physics.mac"
materials       = this_directory + "/materials.mac"
detectors       = this_directory + "/detectors.mac"
beam            = this_directory + "/beam.mac"
sampleGeometry  = this_directory + "/sampleGeometry.mac"
samplePlacement = this_directory + "/samplePlacement.mac"

macrofiles      = [physics,       
                   materials,     
                   detectors,     
                   beam,          
                   sampleGeometry, 
                   samplePlacement]
                   
#========================================================================================
