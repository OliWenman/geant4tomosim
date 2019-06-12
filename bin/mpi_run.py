# -*- coding: utf-8 -*-

"""
Python mpi script to run the g4tomosim package. HDF5 Parralisation not supported.
Author: Oliver Jack Wenman
 
"""
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

#===================================================================

import g4tomosim
import numpy as np
import NexusFormatter
import time
import random

from mpi4py import MPI
comm           = MPI.COMM_WORLD
rank           = comm.Get_rank()
mpi_nprocesses = comm.Get_size()

nframes = len(tsi.rotation_angles) -1 + tsi.ndarkflatfields

#Define the channels that each data type will be recieved on
absportionchannel = 111
#beamintensity_tag
#fluorescence_fullmapping_tag = 

tomosim = None

if rank == 0: 
    
    tomosim = g4tomosim.G4TomoSim(verbose = tsi.verbose, interactive = False)     
    tomosim.execute_macrolist(tsi.macrofiles)     
         
    xpixels = tomosim.absorptiondetector_getxpixels()
    ypixels = tomosim.absorptiondetector_getypixels()
    
    nexusfile = NexusFormatter.NexusFormatter()
    
    print ("Opening file:", filepath)
    
    nexusfile.openFile(filepath, False, comm = comm)
    
    if nexusfile.setupSuccess == False:
        print ("\nAborting run...") 
        sys.exit()
    
    #Create the path to save the projection data
    nexusfile.CreateProjectionFolder(n_darkflatfields    = tsi.ndarkflatfields, 
                                     n_projections       = nframes + 1, 
                                     ypixels             = ypixels, 
                                     xpixels             = xpixels, 
                                     detector_dimensions = tomosim.absorptiondetector_getdimensions(), 
                                     rotation            = tsi.rotation_angles)
else:
    tomosim = g4tomosim.G4TomoSim(verbose     = 0, interactive = False)           
    tomosim.execute_macrolist(tsi.macrofiles)        

if rank == 0:
    print ("Starting simulation")
    itime = time.time()

tomosim.set_seed(random.randint(1, 1.e9)*(mpi_nprocesses + 1))

i     = 0
frame = 0
while frame <= nframes:

    projection = mpi_nprocesses*(i) + rank
    frame = mpi_nprocesses*i + rank
    
    if frame <= nframes:
    
        doflatfield = False
    
        if frame > len(tsi.rotation_angles) -1:
            doflatfield    = True
            rotation_angle = 0.
            zposition      = 0.
        else:
            rotation_angle = tsi.rotation_angles[frame]
            zposition      = tsi.zpos[frame]
    
        tomosim.simulateprojection(n_particles     = tsi.particles,
                                    flatfield      = doflatfield,
                                    rotation_angle = rotation_angle,
                                    zposition      = zposition,
                                    resetdata      = False)

        absdata = tomosim.absorptiondetector_getprojection() 

        if rank != 0:
            comm.send(absdata, 
                      dest = 0, 
                      tag  = absportionchannel)      

        if rank == 0:
            if frame == 0:
                print ("===============================================================")
            
            for nrank in range(mpi_nprocesses):
            
                if nrank != 0 :
                    if frame+nrank <= nframes:
                        absdata = comm.recv(source = nrank, 
                                            tag    = absportionchannel)
            
                if frame+nrank <= nframes:
                    #Save the projection data                          
                    print ("projection", frame + nrank + 1, "complete") 
                    nexusfile.AddProjectionData(absdata, frame + nrank)
    i += 1

if rank == 0:
    nexusfile.LinkData()
    nexusfile.closeFile()
    
    print ("Simulation finished")
    print ("Data saved in :", filepath)
    
    etime         = time.time()
    simtime       = etime -itime
    simtime_units = "s" 
    
    if simtime < 60:
        simtime = round(simtime, 3)
        simtime_units = "s"       
    elif simtime < 60*60:
        simtime_units = "mins" 
        simtime = round(simtime/60, 3) 
    else:
        simtime_units = "hrs"
        simtime = round(simtime/(60*60), 3) 
    print ("Simulation time:", simtime, simtime_units)
    
    finished_message = open(this_directory + "/../output/finished.txt", "w")
    finished_message.write("Finished simulation")
    finished_message.write("\nSimulation time :" + str(simtime) + simtime_units)
    finished_message.close()
        

