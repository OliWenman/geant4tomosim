# distutils: language = c++
# distutils: sources = Simulation.cc

"""
G4TomoSim is for the simulation of tomography data using the C++ toolkit Geant4.
Designed to be accessed by Python, functions wrapped in Cython.

Author: Oliver Wenman

"""

#Creating a cython wrapper class
import numpy as np
cimport numpy as np
import h5py
import time
import os
import gc
import matplotlib.pyplot as plt
import sys

import NexusFormatter

cdef class G4TomoSim:

    cdef Simulation *thisptr        
  
    #Constructor, create an instance of the C++ class
    def __cinit__(self, 
                  verbose = 1, 
                  interactive = True):
                  
        self.thisptr = new Simulation(verbose, interactive)

    #Delete the C++ class
    def __dealloc__(self):
        del self.thisptr

    #===============================================================================================
    """
    Functions for executing macro files (.mac). Used to control the settings of the simulation by
    containing a list of commands.
    """

    #Execute a list containing paths to macro files
    def execute_macrolist(self, list macroFiles):  
        return self.thisptr.Execute_Macrolist_pyw(macroFiles)
    
    #Execute a single macro file by providing the path to the file    
    def execute_macro(self, str macrofile):
        return self.thisptr.Execute_Macro_pyw(macrofile)

    #Execute a command. Error checking in command done via C++ in "Simulation.hh".
    def execute_command(self, str command):
        return self.thisptr.Execute_Command_pyw (command)
    
    
    #===============================================================================================
    def setup_visualization(self,
                            str path,
                            str filename):
        
        self.thisptr.Setup_Visualization_pyw(path, filename)
    
    #===============================================================================================
    """
    Absorption detector functions to return data
    """
    
    def absorptiondetector_getprojection(self):
        """
        Return the projection. Is orginally 1D, reshapes it too 2D.  
        """       
        return np.reshape(np.array(self.thisptr.AbsorptionDetector_GetProjection_pyw(), dtype = np.uint16), 
                          (-1, self.thisptr.AbsorptionDetector_GetXPixels_pyw()))
    
    def absorptiondetector_getxpixels(self):
        return self.thisptr.AbsorptionDetector_GetXPixels_pyw()
        
    def absorptiondetector_getypixels(self):
        return self.thisptr.AbsorptionDetector_GetYPixels_pyw()
        
    def absorptiondetector_getdimensions(self):
        return self.thisptr.AbsorptionDetector_GetDimensions_pyw()
    
    #===============================================================================================
    """
    Fluorescence detector functions to return information and data
    """
    
    def fluorescencedetector_getfullmappingdata(self):
        return np.array(self.thisptr.FluorescenceDetector_GetFullMapping_pyw(), dtype = np.uint64)
        
    def fluorescencedetector_getfullfielddata(self):
        return np.array(self.thisptr.FluorescenceDetector_GetFullField_pyw(), dtype = np.uint64)
    
    def fluorescencedetector_getenergybins(self):
        return np.array(self.thisptr.FluorescenceDetector_GetEnergyBins_pyw(), dtype = np.uint16)
    
    def fluorescencedetector_getnoenergybins(self):
        return 0
        
    def fluorescencedetector_getposition(self):
        return 0
        
    def fluorescencedetector_getdimensions(self):
        return 0   
            
    def fluorescencedetector_fullmappingActive(self):
        return self.thisptr.FluorescenceDetector_FullMappingActive_pyw()
        
    def fluorescencedetector_fullfieldOn(self):
        return self.thisptr.FluorescenceDetector_FullFieldActive_pyw()
    
    #===============================================================================================
    """
    Beam functions to return information and data
    """
    def beam_getnobins(self):
        return 0
    
    def beam_getenergybins(self):
        return np.array(self.thisptr.Beam_GetEnergyBins_pyw(), dtype = np.uint16)
        
    def beam_getintensity(self):
        return np.array(self.thisptr.Beam_GetIntensity_pyw(), dtype = np.uint64)  

    #===============================================================================================

    def simulateprojection(self, 
                           int n_particles, 
                           bint flatfields,
                           double rotation_angle,
                           double zposition):
        
        """
        Run a single projection of a simulation. Can be used for wrapping the simulation in other software such as Savu.
        
        """
        
        if n_particles < 1:
            raise Exception("n_particles can't be less than 1.")
        
        return self.thisptr.Run_Projection_pyw(n_particles,
                                               flatfields,
                                               rotation_angle,
                                               zposition)

    def simulatetomography(self, 
                           str        filepath,
                           int        n_particles, 
                           int        nDarkFlatFields,
                           np.ndarray rotation_angles,
                           np.ndarray zpositions = None ):

        """
        The main function to use if running the simulation on your local machine. Supply the filepath to save the data, number of particles to simulate
        per projection, the number of dark and flat fields, a numpy.darray of rotation angles, and optionally a numpy.darray of zpositions for a helical
        scan.
        
        The function will loop through the rotation_angles supplied, saving at the end of each projection the appropiate data in a nexus format.
        Other variables for the simulation can be controlled via macrofiles supplied before this function, such as the sample, physics used, 
        beam and detector configurations, and creation of materials. 
        
        """

        #Error check the inputs to make sure they are correct
        if zpositions is not None:
            if len(zpositions) != len(rotation_angles):
                raise Exception("zpositions and rotation_angles should be the same shape.")
            
            if zpositions.ndim != 1:
                raise Exception("zpositions should be 1D.")
        
        if rotation_angles.ndim != 1:
            raise Exception ("rotation_angles should be 1D.")
            
        if n_particles < 1:
            raise Exception("n_particles can't be less than 1.")
            
        if nDarkFlatFields < 0:
            raise Exception("nDarkFlatFields can't be negative.")

        totalprojections = len(rotation_angles) + nDarkFlatFields
        
        stringLength = len(filepath)
    
        pathfound = False
        filepathokay = False;

        #Loop through the string to find the path and the name
        for i in reversed(range(stringLength)):
            char = filepath[i]
        
            #Finds the path and then can seperate the variables to path and name
            if char == '/':
  
                filename = filepath[i+1:stringLength]      
                path = filepath[0:i+1]
                pathfound = filepathokay = True
        
                break
        
        nexusExtension = filepath[-4:stringLength]
        
        #If the file doesn't have the .nxs extension, ask for another input
        if nexusExtension != '.nxs':
            raise Exception ("the extension", filename,"is not valid for file. Please input the file with a \'.nxs\' extension.")
        
        #Checks to see if path exists
        if not os.path.isdir(path) :
            raise Exception ("the directory \'", path, "\' doesn't exist.")
    
        #Create the log file name
        dotPosition  = filename.find('.')
        filepath_log = path + filename[0:dotPosition] + '_log.txt'
        
        #Setup any external visualisation
        self.thisptr.Setup_Visualization_pyw(path,
                                             filename[0:dotPosition])
        
        #Print information about the settings of the simulation to the user and save in a log file
        self.thisptr.Print_SimulationInfo_pyw(filepath_log,
                                              n_particles, 
                                              totalprojections, 
                                              nDarkFlatFields)   
                 
        #Checks to see if the nexus template was created successfully  
        nexusfile = NexusFormatter.NexusFormatter()
        nexusfile.openFile(filepath)
        if nexusfile.setupSuccess == False:
            print ("\nAborting run...") 
            return 0
        
        #Get needed variables from the C++ side    
        FMFluorescence = self.thisptr.FluorescenceDetector_FullMappingActive_pyw()
        FFFluorescence = self.thisptr.FluorescenceDetector_FullFieldActive_pyw()
        fluoreBins     = self.thisptr.FluorescenceDetector_GetNoEnergyBins_pyw()
        beamBins       = self.thisptr.Beam_GetNoEnergyBins_pyw()
        xPixels        = self.absorptiondetector_getxpixels()
        yPixels        = self.absorptiondetector_getypixels()
           
        #Create the path to save the projection data
        nexusfile.CreateProjectionFolder(nDarkFlatFields, 
                                         totalprojections, 
                                         yPixels, 
                                         xPixels, 
                                         self.absorptiondetector_getdimensions(), 
                                         rotation_angles)
                                                 
        #Create the other data groups in the nexus file template   
        nexusfile.CreateDataGroup("Beam_Energy", 
                                   nImages = totalprojections, 
                                   eBins = beamBins)
           
        if FFFluorescence == True:
            nexusfile.CreateDataGroup("Fluorescence", 
                                       nImages = totalprojections, 
                                       eBins = fluoreBins)
              
        if FMFluorescence == True:
            nexusfile.CreateDataGroup("Full_Mapping_Fluorescence", 
                                       nImages = totalprojections, 
                                       eBins = fluoreBins, 
                                       xBins = xPixels, 
                                       yBins = yPixels)
        
        iTime = time.time()
        
        #Start the loop for the simulation
        for projection in range(totalprojections):
           
            #Get the rotation_angle
            if projection < totalprojections - nDarkFlatFields:        
                rotation_angle = rotation_angles[projection]
                                 
                if zpositions is None:
                    zposition = 0
                else :
                    zposition = zpositions[projection]
                
            elif projection >= totalprojections - nDarkFlatFields:
                rotation_angle = 0
                zposition      = 0
            
            #Group the image info into a list   
            imageInfo = [projection, nDarkFlatFields, totalprojections]
                       
            #Runs the simulation
            self.thisptr.Run_Tomography_pyw(n_particles, 
                                            imageInfo, 
                                            rotation_angle,
                                            zposition)  
               
            iSavingTime = time.time()

            #Save the projection data                           
            nexusfile.AddProjectionData(self.absorptiondetector_getprojection(), projection)
               
            #Add axes to the other data
            if projection == 0:
                energy_xaxis = self.beam_getenergybins()
                  
                nexusfile.AddxAxis("Beam_Energy", energy_xaxis)
                  
                energy_xaxis = self.thisptr.FluorescenceDetector_GetEnergyBins_pyw()
                if FFFluorescence == True:
                    nexusfile.AddxAxis("Fluorescence", energy_xaxis)
                     
                if FMFluorescence == True:
                    nexusfile.AddxAxis("Full_Mapping_Fluorescence", energy_xaxis)
               
            #Save other data
            nexusfile.AddData("Beam_Energy", 
                               self.beam_getintensity(), 
                               nImage = projection)    
               
            if FFFluorescence == True:
                nexusfile.AddData("Fluorescence", 
                                   data = self.fluorescencedetector_getfullfielddata(), 
                                   nImage = projection)
                  
            if FMFluorescence == True:
                data = self.fluorescencedetector_getfullmappingdata()
                nexusfile.AddData("Full_Mapping_Fluorescence", 
                                   data = data, 
                                   nImage = projection)
            
            #Record the saving time, estimated time can be adjusted with this into account   
            eSavingTime = time.time()
            SavingTime  = eSavingTime - iSavingTime
            self.thisptr.Set_SavingTime_pyw (SavingTime)

        #Ouput the time in the appropriate units
        eTime = time.time()
        simtime = eTime -iTime

        message = "The total simulation time is"
        if simtime < 60:
            print (message, round(simtime, 3), "seconds. ")
        elif simtime < 60*60:
            print (message, round(simtime/60, 3), "minutes. ")
        else:
            print (message, round(simtime/(60*60), 3), "hours. ")
            
        nexusfile.LinkData()   
        
        print ("\nData was saved in", filepath) 
        nexusfile.closeFile()
        del nexusfile
        
        #Frees the memory by reducing the arrays to store the data back to a size of 0.   
        self.thisptr.Free_DetectorMemory_pyw()
        

