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
    def addMacroFiles(self, list macroFiles):
        self.thisptr.addmacros_pywrapped(macroFiles)
    
    #Execute a single macro file by providing the path to the file    
    def execute_macro(self, str macrofile):
        self.thisptr.applymacrofile_pywrapped(macrofile)

    #Execute a command. Error checking in command done via C++ in "Simulation.hh".
    def execute_command(self, str command):
        self.thisptr.applycommand_pywrapped(command)

    #===============================================================================================

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
            
        if n_particles <= 1:
            raise Exception("n_particles can't be less than 1.")
            
        if nDarkFlatFields <= 0:
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
        self.thisptr.setupvis_pywrapped(path,
                                        filename[0:dotPosition])
        
        #Print information about the settings of the simulation to the user and save in a log file
        self.thisptr.printinfo_pywrapped(filepath_log,
                                         n_particles, 
                                         totalprojections, 
                                         nDarkFlatFields)   
                 
        #Checks to see if the nexus template was created successfully  
        nexusfile = NexusFormatter.NexusFormatter()
        nexusfile.openFile(filepath)
        if nexusfile.setupSuccess == False:
            print "\nAborting run..." 
            return 0
        
        #Get needed variables from the C++ side    
        FMFluorescence = self.thisptr.fluorFMactive_pywrapped()
        FFFluorescence = self.thisptr.fluorFFactive_pywrapped()
        fluoreBins     = self.thisptr.getNumFluorbins_pywrapped()
        beamBins       = self.thisptr.getbeambins_pywrapped()
        xPixels        = self.thisptr.getNumAbsXpixels_pywrapped()
        yPixels        = self.thisptr.getNumAbsYpixels_pywrapped()
           
        #Create the path to save the projection data
        nexusfile.CreateProjectionFolder(nDarkFlatFields, 
                                         totalprojections, 
                                         yPixels, 
                                         xPixels, 
                                         self.thisptr.getAbsHalf3Dim_pywrapped(), 
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
            self.thisptr.run_pywrapped(n_particles, 
                                       imageInfo, 
                                       rotation_angle,
                                       zposition)  
               
            iSavingTime = time.time()

            #Save the projection data                           
            nexusfile.AddProjectionData(self.absorptionData(), projection)
               
            #Add axes to the other data
            if projection == 0:
                energy_xaxis = self.beamEnergyBins()
                  
                nexusfile.AddxAxis("Beam_Energy", energy_xaxis)
                  
                energy_xaxis = self.thisptr.getFluoreEneBins_pywrapped()
                if FFFluorescence == True:
                    nexusfile.AddxAxis("Fluorescence", energy_xaxis)
                     
                if FMFluorescence == True:
                    nexusfile.AddxAxis("Full_Mapping_Fluorescence", energy_xaxis)
               
            #Save other data
            nexusfile.AddData("Beam_Energy", 
                               self.beamEnergyData(), 
                               nImage = projection)    
               
            if FFFluorescence == True:
                nexusfile.AddData("Fluorescence", 
                                   data = self.fullfieldfluoreData(), 
                                   nImage = projection)
                  
            if FMFluorescence == True:
                data = self.fullmappingfluoreData()
                nexusfile.AddData("Full_Mapping_Fluorescence", 
                                   data = data, 
                                   nImage = projection)
            
            #Record the saving time, estimated time can be adjusted with this into account   
            eSavingTime = time.time()
            SavingTime  = eSavingTime - iSavingTime
            self.thisptr.setSavingTime_pywrapped (SavingTime)

        #Ouput the time in the appropriate units
        eTime = time.time()
        simtime = eTime -iTime

        message = "The total simulation time is"
        if simtime < 60:
            print message, round(simtime, 3), "seconds. "
        elif simtime < 60*60:
            print message, round(simtime/60, 3), "minutes. "
        else:
            print message, round(simtime/(60*60), 3), "hours. "
            
        nexusfile.LinkData()   
        
        print "\nData was saved in", 
        nexusfile.closeFile()
        del nexusfile
        
        #Frees the memory by reducing the arrays to store the data back to a size of 0.   
        self.thisptr.freedataMemory_pywrapped()

    def runsingleprojection(self, 
                            int n_particles, 
                            bint flatfields,
                            double rotation_angle,
                            double zposition):
        
        """
        Run a single projection of a simulation. Can be used for wrapping the simulation in other software such as Savu.
        
        """
        
        if n_particles <= 1:
            raise Exception("n_particles can't be less than 1.")
        
        return self.thisptr.runsingleprojection_pywrapped(n_particles,
                                                          flatfields,
                                                          rotation_angle,
                                                          zposition)
       
    def absorptionData(self):
        """
        Return the absorption data. Is orginally 1D, reshapes it too 2D.  
        """
        return np.reshape(self.thisptr.getAbsorption_pywrapped(), 
                         (-1, self.thisptr.getNumAbsXpixels_pywrapped()))
    
    def photonTransmission(self):
        return np.sum(self.thisptr.getAbsorption_pywrapped())

    def beamEnergyBins(self):
        return np.array(self.thisptr.getBeamEnergyBins_pywrapped())

    def fullfieldfluoreData(self):
        return np.array(self.thisptr.getFullFieldFluore_pywrapped())

    def beamEnergyData(self):
        return np.array(self.thisptr.getBeamEnergy_pywrapped())  

    def fullmappingfluoreData(self):
        return np.array(self.thisptr.getFullMappingFluore_pywrapped())
        
    def plotBeamEnergy(self):
        plt.plot(self.beamEnergyBins(), self.thisptr.getBeamEnergy_pywrapped())
        plt.title("Beam energy distrubution")
        plt.xlabel("Energy (keV)")
        plt.ylabel("Photons")
        plt.show()
        
    def plotFinalFluorescence(self):
        plt.plot(self.thisptr.getFluoreEneBins_pywrapped(), self.thisptr.getFullFieldFluore_pywrapped())
        plt.title("Fluorescence")
        plt.xlabel("Energy (keV)")
        plt.ylabel("Photons")
        plt.show()
     
