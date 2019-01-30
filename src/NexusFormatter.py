import numpy as np
import h5py
import time
import os
import gc
import getpass
import sys

class NexusFormatter:
    
    h5file1 = None
    
    fileOpen = False
    #data = None
    
    #When class is created
    def __init__(self, SaveFilePath):
        
        FileName = 'SimulationData.nxs'
        
        print "\nOpening Nexus file:", SaveFilePath+FileName
        
        #Check if file exists       
        if os.path.isfile(SaveFilePath+FileName):
        
            #If it exits, check it's size
            fileInfo = os.stat(SaveFilePath+FileName)            
            fileSize = fileInfo.st_size/(10**9) #In GB
            
            #If file size is over 1 GB, ask user if they are sure they want to override this data
            if fileSize >= 1:
                
                print "The file", FileName, "size is", fileSize, "GB. \nAre you sure you want to override this?\n"              
                contin = False
                
                while contin != True:
                    answer = raw_input('yes/no?\n')
                
                    if answer == 'y' or answer == 'yes':
                        contin = True                      
                    elif answer == 'n' or answer == 'no':
                        contin = True
                        sys.exit()                      
                    else:
                        print "Input yes or no to continue."
                        contin = False
                
                print "\n"
        
        #Try to open file, delete file if can't open it as it's going to override it anyway       
        try:     
            self.h5file1 = h5py.File(SaveFilePath + FileName, 'w')
        
        except OSError:
            os.remove(SaveFilePath + FileName)
            self.h5file1 = h5py.File(SaveFilePath + FileName, 'w')
        
        self.fileOpen = True
        
        self.h5file1.attrs['default'] = 'entry'

        #Create the different groups to conform to the NXtomo format

        #/entry/entry1
        nxentry = self.h5file1.create_group('entry1')
        nxentry.attrs['NX_class'] = 'NXentry'
        nxentry.attrs['default'] = 'data'
        
        #/entry/entry1/features
        features = self.h5file1.create_group('features')
        features.attrs['NX_class'] = 'features'
        
        #/entry/entry1/tomo_entry
        tomo_entry = nxentry.create_group('tomo_entry')
        tomo_entry.attrs['NX_class'] = 'NXsubentry'
           
        #/entry/entry1/tomo_entry/user
        user = tomo_entry.create_group('user')
        user.attrs['NX_class'] = 'NXuser'
        user.create_dataset('username', data = getpass.getuser())
           
        #entry/entry1/tomo_entry/data/instrument
        instrument = tomo_entry.create_group('instrument')
        instrument.attrs['NX_class'] = 'NXinstrument'
        
        #entry/entry1/tomo_entry/data/instrument/detector
        detector = instrument.create_group('detector')
        detector.attrs['NX_class'] = 'NXdetector'
        
        #/entry/entry1/tomo_entry/sample
        sample = tomo_entry.create_group('sample')
        sample.attrs['NX_class'] = 'NXsample'
    
        #/entry/entry1/tomo_entry/data
        data = tomo_entry.create_group('data')
        data.attrs['NX_class'] = 'NXdata'
        data.attrs['defination'] = 'NXtomo'

        #/entry/entry1/tomo_entry/data/data
        imageSet = data.create_group('data')
        imageSet.attrs['NX_class'] = 'NXdata'
        
    #When class is destroyed, close the file if it isn't already closed
    def __del__(self):
        
        if self.fileOpen == True:
            print "\nClosing NeXus file..."
            self.h5file1.close()
            print "File closed!"
        
    #The dataset that stores the information to do with the transmission data
    def CreateProjectionFolder(self, nCalibrations, NumberOfImages, nDetectorsZ, nDetectorsY):
        
        #entry/entry1/tomo_entry/data/instrument/detector/data
        DetectorPath = '/entry1/tomo_entry/data/instrument/detector/'
        detectordata = self.h5file1.create_dataset(DetectorPath + 'data', shape=(NumberOfImages, nDetectorsZ, nDetectorsY), dtype = 'i4')
        
        #entry/entry1/tomo_entry/data/instrument/detector/image_key
        darkKey = np.full(shape = (nCalibrations,), fill_value = 2)
        lightKey = np.full(shape = (nCalibrations,), fill_value = 1)
        dataKey = np.zeros(NumberOfImages)
        image_key = self.h5file1.create_dataset(DetectorPath + 'image_key', data = np.concatenate((darkKey, lightKey, dataKey)))
        
        #/entry/entry1/tomo_entry/data/data
        #DataPath = '/entry/entry1/tomo_entry/data/'
        #imageSet = self.h5file1.create_group(DataPath + 'data')
        #imageSet.attrs['NX_class'] = 'NXdata'
           
        #source_addr = "/entry1/tomo_entry/instrument/detector/data"
        #target_addr = "/entry/entry1/tomo_entry/data/data"
        #imageSet.attrs['target'] = source_addr
        
    #Appened to the transmission data
    def AddProjectionData(self, theData, nImage):
        
        dataSet = self.h5file1['/entry1/tomo_entry/data/instrument/detector/data']
        dataSet[nImage, :, :] = theData[:, :]
     
    #Create the rotation angles    
    def CreateRotationAngleData(self, Theta, NumberOfImages, nCalibrations):
        
        #/entry/entry1/tomo_entry/sample/rotation_angle
        
        PathToData = '/entry1/tomo_entry/sample/'
        
        rotationArray = np.linspace(start = 0, stop = Theta, num = NumberOfImages) 
        calibrationLine = np.zeros(nCalibrations*2)
        rotationArray = np.concatenate([calibrationLine, rotationArray])
        
        sample = self.h5file1[PathToData]
        
        rotation_angle = sample.create_dataset('rotation_angle', data = rotationArray)
        rotation_angle.attrs['axis'] = '1'
        rotation_angle.attrs['label'] = '1'
        rotation_angle.attrs['units'] = 'radians'
  
    #Function to create the data for the beam energy, fluorescence or full mapping fluorescence
    def CreateDataGroup(self, dataType, nImages = 1, eBins = 1, xBins = 1, yBins = 1):
        
        DataPath = '/entry1/tomo_entry/data/'
        xLabel = "energy"
        yLabel = "photons"
        
        dataGroup = self.h5file1.create_group(DataPath + dataType)
        dataGroup.attrs['NX_class'] = 'NXdata'
        dataGroup.attrs['axes'] = xLabel         # X axis of default plot
        dataGroup.attrs['signal'] = yLabel      # Y axis of default plot
        xScaleDataSet = dataGroup.create_dataset(xLabel, shape = (eBins,), dtype = 'f8')  # X axis data
        
        if dataType == "Beam_Energy":
            dataGroup.attrs[xLabel + '_indices'] = [0,] 
            dataSet = dataGroup.create_dataset(yLabel, shape = (eBins,), dtype = 'i4')  # Y axis data 
                    
        elif dataType == "Fluorescence":
            dataGroup.attrs[xLabel + '_indices'] = [1,] 
            dataSet = dataGroup.create_dataset(yLabel, shape = (nImages, eBins), dtype = 'i4')  # Y axis data  
            
        elif dataType == "Full_Mapping_Fluorescence":
            dataGroup.attrs[xLabel + '_indices'] = [3] 
            dataSet = dataGroup.create_dataset(yLabel, shape = (nImages, xBins, yBins, eBins), dtype = 'i4')  # Y axis data
        
        else:
            print "Unrecognised dataType", dataType,"using CreateDataGroup."
            sys.exit()  
     
    #Function to add the x axis to the data   
    def AddxAxis(self, dataType, data):
        
        dataSet = self.h5file1['/entry1/tomo_entry/data/'+ dataType +'/energy']
        dataSet[:] = data
        
    #Function to appened the data
    def AddData(self, dataType, data, nImage = 1):
        
        dataSet = self.h5file1['/entry1/tomo_entry/data/'+ dataType +'/photons']
        
        if dataType == "Beam_Energy":
            dataSet[:] = data
            
        elif dataType == "Fluorescence":
            dataSet[nImage, :] = data
            
        elif dataType == "Full_Mapping_Fluorescence":
            dataSet[nImage, :, :, :] = data
        
        else:
            print "Unrecognised dataType", dataType,"using AddData."
            sys.exit()  
          
    #Display the tree of the nexus file  
    def DisplayTree(self):
    
        def printname(name):
            print name
    
        print "The nexus file layout: "    
        self.h5file1.visit(printname)
        
