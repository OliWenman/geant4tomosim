# -*- coding: utf-8 -*-
import os 
this_directory = os.path.dirname(os.path.realpath(__file__))

import sys
sys.path.insert(0, this_directory + '/../settings')
import tomosim_input as tsi

"""
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


filepath = this_directory + "/../output/tomosimData.nxs"
"""
#===================================================================

#RUN THE SIMULATION
import sim
import numpy as np

print "verbose = ", tsi.verbose
"""
rotations = [0, 1.5708, 2*1.5708]   

tomosim = sim.G4TomoSim(tsi.verbose, 
                        tsi.interactive)
"""            
from mpi4py import MPI
comm        = MPI.COMM_WORLD

rank        = comm.Get_rank()
mpi_nprocesses = comm.Get_size()

#print "rank =", rank
#print "size =", mpi_nprocesses

data = {'a': 7, 'b': 3.14}
comm.send(data, dest=1, tag=11)

if rank == 0:
    #for i in range (mpi_nprocesses -1):   
    data = comm.recv(source=0, tag=11)


"""             
tomosim.addMacroFiles(tsi.macrofiles)                    

tomosim.runsingleprojection(1.e5,
                            False,
                            rotations[rank],
                            0.)
localdata = tomosim.absorptionData()

print rank, "Local data"
print localdata

comm.Send(localdata, tag=11, dest=0)

if rank == 0:
    for i in range (mpi_nprocesses):
        
        comm.Recv(localdata, source = 1 , tag = 11)
"""
    
    

                          
#print "Finished"

