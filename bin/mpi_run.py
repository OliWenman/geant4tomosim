# -*- coding: utf-8 -*-

"""
Python mpi script to run the g4tomosim package. HDF5 Parralisation not supported in this code and therefore 
this is not the most optimial way of parralising G4TomoSim.
Saves absorption data.

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
        
    stringLength = len(filepath)
    
    #Loop through the string to find the path and the name
    for i in reversed(range(stringLength)):
        char = filepath[i]
    
        #Finds the path and then can seperate the variables to path and name
        if char == '/':

            _file       = filepath[i+1:stringLength]    
            dotposition = _file.find('.')
            _filelog    = _file[0:dotposition] + '_log.txt'
              
            _path = filepath[0:i+1]
    
            break
#===================================================================

import g4tomosim
import numpy as np
import NexusFormatter
import time
import random
import math

#Initialise the mpi
from mpi4py import MPI
comm           = MPI.COMM_WORLD
rank           = comm.Get_rank()
mpi_nprocesses = comm.Get_size()

nframes = len(tsi.rotation_angles) -1 + tsi.ndarkflatfields

#Define the channels that each data type will be recieved on
absportionchannel = 111

tomosim = None

if rank == 0: 
    #Create the instance of G4TomoSim and setup
    tomosim = g4tomosim.G4TomoSim(verbose = tsi.verbose, interactive = False)     
    tomosim.execute_macrolist(tsi.macrofiles)  
    
    #Set the seed randomly, make all other seeds a multiple of this seed to make sure each seed is different for each rank   
    tomosim.set_seed(random.randint(1, 1.e9)*(mpi_nprocesses + 1))
    
    #log the setup of the simulation used
    tomosim.log_setup(filepath         = _path + _filelog,
                      n_particles      = tsi.particles,
                      n_projections    = len(tsi.rotation_angles),
                      n_darkflatfields = tsi.ndarkflatfields)
         
    xpixels = tomosim.absorptiondetector_getxpixels()
    ypixels = tomosim.absorptiondetector_getypixels()
             
    #Setup the place to save the data
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
    #Create an instance of G4TomoSim but with verbose as quiet
    tomosim = g4tomosim.G4TomoSim(verbose     = 0, interactive = False)           
    tomosim.execute_macrolist(tsi.macrofiles)  
    tomosim.set_seed(random.randint(1, 1.e9)*(mpi_nprocesses + 1))      

if rank == 0:
    print ("Number of cores used =", mpi_nprocesses)
    print ("Starting simulation")
    itime = time.time()

i     = 0
frame = 0

#Calculate how each rank is assigned a rotation angle
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
    
        #USED TO REDUCE VERBOSE 
        #/run/initialize is done automatically inside simulateprojection. However to reduce output, commands below not valid
        #until run/initialize active so is done manually.
        if i == 0:
            tomosim.execute_command("/run/initialize")
            tomosim.execute_command("/process/em/verbose 0")
            tomosim.execute_command("/process/verbose 0")
        
        #Simulate a projection
        tomosim.simulateprojection(n_particles     = tsi.particles,
                                    flatfield      = doflatfield,
                                    rotation_angle = rotation_angle,
                                    zposition      = zposition,
                                    resetdata      = True)

        #Collect the data from the simulation
        absdata = tomosim.absorptiondetector_getprojection() 

        #Send the data to rank 0 to be saved
        if rank != 0:
            comm.send(absdata, 
                      dest = 0, 
                      tag  = absportionchannel)      

        if rank == 0:
            
             #Recieve the data from all other ranks and save it
            for nrank in range(mpi_nprocesses):
            
                if nrank != 0 :
                    if frame+nrank <= nframes:
                        absdata = comm.recv(source = nrank, 
                                            tag    = absportionchannel)
            
                if frame+nrank <= nframes:
                    #Save the projection data                          
                    
                    nexusfile.AddProjectionData(absdata, frame + nrank)
                
            print("======== batch", i+1, "of", math.ceil(nframes/mpi_nprocesses), "complete ========")
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
    
    finished_message = open(_path + "/finished.txt", "w")
    finished_message.write("Finished simulation")
    finished_message.write("\nSimulation time :" + str(simtime) + simtime_units)
    finished_message.close()
        

