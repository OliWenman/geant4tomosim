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

"""
G4TomoSim is for the simulation of tomography data using the C++ toolkit Geant4.
Designed to be accessed by Python, functions wrapped in Cython.

Author: Oliver Wenman

"""

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
   
    def addMacroFiles(self, list macroFiles):
        self.thisptr.addmacros_pywrapped(macroFiles)

    def simulatetomography(self, 
                           str        filepath,
                           int        n_particles, 
                           np.ndarray rotation_angles, 
                           int        nDarkFlatFields):

        """
        The main function to use if running the simulation on your local machine. Supply the filepsth to save the data. number of particles to simulate
        per image, a numpy.darray of rotation angles, and the number of dark and flat fields.
        
        The function will loop through the rotation_angles supplied, saving at the end of each projection the appropiate data in a nexus format.
        Other variables for the simulation can be controlled via macrofiles supplied before this function, such as the sample, physics used, 
        beam and detector configurations, and creation of materials. 
        
        """
        totalprojections = len(rotation_angles) + nDarkFlatFields
        
        if n_particles <= 1:
            print ("\nERROR: n_particles should be greater or equal to 1! ")
            return 0
            
        if nDarkFlatFields <= 0:
            print("\nERROR: nDarkFlatFields can't be negative! ")
            return 0
        
        stringLength = len(filepath)
    
        pathFound = False
        filepathokay = False;

        #Loop through the string to find the path and the name
        for i in reversed(range(stringLength)):
            char = filepath[i]
        
            #Finds the path and then can seperate the variables to path and name
            if char == '/':
  
                filename = filepath[i+1:stringLength]      
                path = filepath[0:i+1]
                pathFound = True
                filepathokay = True
        
                break
        
        nexusExtension = filepath[-4:stringLength]
        #If the file doesn't have the .nxs extension, ask for another input
        if nexusExtension != '.nxs':
            print "\nERROR: the extension", filename,"is not valid. Please input the file with a \'.nxs\' extension. "
            filepathokay = False
    
        pathOkay = os.path.isdir(path)
            
        if not pathOkay :
            print "\nERROR: the directory \'", path, "\' doesn't exist."
            filepathokay = False
    
        if not filepathokay :
            return 0   
    
        dotPosition  = filename.find('.')
        filepath_log = path + filename[0:dotPosition] + '_log.txt'
        
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
        fluoreBins = self.thisptr.getNumFluorbins_pywrapped()
        beamBins   = self.thisptr.getbeambins_pywrapped()
        xPixels = self.thisptr.getNumAbsXpixels_pywrapped()
        yPixels = self.thisptr.getNumAbsYpixels_pywrapped()
           
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
            elif projection >= totalprojections - nDarkFlatFields:
                 rotation_angle = 0
            
            #Group the image info into a list   
            imageInfo = [projection, nDarkFlatFields, totalprojections]
                            
            #Runs the simulation
            self.thisptr.run_pywrapped(n_particles, 
                                       imageInfo, 
                                       rotation_angle)  
               
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
        
    def plotDiffraction(self):
        
        print "function being used"
        data = self.thisptr.GetDiffractionData()
        data = np.array(data)
        
        #plt.figure(220)
        #plt.plot(data[0])
        #plt.show()
        #plt.figure(330)
        #plt.plot(data[1])
        #plt.show()
        plt.figure(440)
        plt.imshow(data, cmap="Greys")
        #plt.
        plt.show()
        
    def ApplyCommand(self, command):
        self.thisptr.applycommand_pywrapped(command)
        
    def ApplyMacroFile(self, macrofile):
        self.thisptr.applymacrofile_pywrapped(macrofile)
    
    def RunSingleProjection(self, 
                            n_particles, 
                            n_projection, 
                            n_darkflatfields,
                            totalprojections, 
                            rotation_angle):
    
        imageInfo = [n_projection, n_darkflatfields, totalprojections]
        self.thisptr.run_pywrapped(n_particles, imageInfo, rotation_angle)
        
     
