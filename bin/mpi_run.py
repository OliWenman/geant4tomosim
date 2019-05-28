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
        print "\nSaving data in the default place", filepath

#===================================================================

#RUN THE SIMULATION
import sim
import numpy as np
import NexusFormatter
import time

#print "verbose = ", tsi.verbose

tomosim = sim.G4TomoSim(tsi.verbose, 
                        tsi.interactive)            

from mpi4py import MPI
comm        = MPI.COMM_WORLD
rank        = comm.Get_rank()
mpi_nprocesses = comm.Get_size()
"""
data = rank + 10

if rank != 0:
    comm.send(data, dest = 0)

if rank == 0:
    for i in range(mpi_nprocesses):
        if i != 0 :
            data = comm.recv(source = i)
        
        print data
"""

nframes = len(tsi.rotation_angles) -1 + tsi.ndarkflatfields

#Define the channels that each data type will be recieved on
absportionchannel = 111
#beamintensity_tag

if rank == 0: 
    #filepath = "/home/xol73553/git/mpiTest/output/tomosim_data.nxs"#"/scratch/Data/I12/simulations/86254/V3/5e8_200proj.nxs"
    #this_directory + "/../output/tomosimData3.nxs"
    nexusfile = NexusFormatter.NexusFormatter()
    nexusfile.openFile(filepath, False)
    if nexusfile.setupSuccess == False:
        print "\nAborting run..." 
        sys.exit()
    
    xpixels = 302
    ypixels = 434
    
    #Create the path to save the projection data
    nexusfile.CreateProjectionFolder(tsi.ndarkflatfields, 
                                     nframes + 1, 
                                     xpixels, 
                                     ypixels, 
                                     [5, 5, 5], 
                                     tsi.rotation_angles)    
    
tomosim.addMacroFiles(tsi.macrofiles)        

if rank == 0:
    print "Starting simulation"
    itime = time.time()

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
    
        tomosim.runsingleprojection(tsi.particles,
                                    doflatfield,
                                    rotation_angle,
                                    zposition)

        absdata = tomosim.absorptionData() 

        if rank != 0:
            comm.send(absdata, 
                      dest = 0, 
                      tag  = absportionchannel)      

        if rank == 0:
            if frame == 0:
                print "==============================================================="
            
            for nrank in range(mpi_nprocesses):
            
                if nrank != 0 :
                    if frame+nrank <= nframes:
                        absdata = comm.recv(source = nrank, 
                                            tag    = absportionchannel)
            
                if frame+nrank <= nframes:
                    #Save the projection data                          
                    print "frame =", frame + nrank, "complete" 
                    nexusfile.AddProjectionData(absdata, frame + nrank)
    i += 1

if rank == 0:
    print "Data saved in :", filepath
    nexusfile.LinkData()
    nexusfile.closeFile()
    etime = time.time()
    print "Simulation finished"
    simtime = etime -itime
    message = "Simulation time:",
    if simtime < 60:
        print message, round(simtime, 3), "seconds. "
    elif simtime < 60*60:
        print message, round(simtime/60, 3), "minutes. "
    else:
        print message, round(simtime/(60*60), 3), "hours. "
    
    import datetime
    File_object = open("/home/xol73553/git/mpiTest/output/finished.txt", "w")
    File_object.write("Finished")
    File_object.write("\nTime: " + datetime.datetime.now() + " hrs")
    File_object.write("\nsim time :" + str(simtime) + " s")
    File_object.close()
        



"""
import g4tomosim

#Create an instance of the object
tomosim = g4tomosim.G4TomoSim(verbose, interactiveOn)

#Pass a list filled with macro files to the object to setup 
#the simulation settings
tomosim.execute_macrofiles(macro_list)

#Run a single projection 
tomoism.simulateprojection(nparitcles,      #number of particles
                           flatfield,       #True/False do a flat field
                           rotation_angle,  #the rotation_angle of the sample
                           zposition)       #the z position of the sample

#Collect the data from the absorption detectors
data = tomosim.get_absorptiondata()
"""


"""       
tomosim.addMacroFiles(tsi.macrofiles)                    

tomosim.runsingleprojection(1.e5,
                            False,
                            tsi.rotation_angles[rank],
                            0.)
localdata = tomosim.absorptionData()

print rank, "Local data"
print localdata
"""
"""
comm.Send(localdata, tag=11, dest=0)

if rank == 0:
    for i in range (mpi_nprocesses):
        
        comm.Recv(localdata, source = 1 , tag = 11)
"""
    
    

                          
#print "Finished"

