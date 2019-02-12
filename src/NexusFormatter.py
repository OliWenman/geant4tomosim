import numpy as np
import h5py
import time
import os
import gc
import getpass
import sys

class NexusFormatter:
    
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
        #features = self.h5file1.create_group('features')
        #features.attrs['NX_class'] = 'features'
        
        #/entry/entry1/tomo_entry
        tomo_entry = nxentry.create_group('tomo_entry')
        tomo_entry.attrs['NX_class'] = 'NXsubentry'
           
        #/entry/entry1/tomo_entry/user
        user = nxentry.create_group('user')
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
        
    #When class is destroyed, close the file if it isn't already closed
    def __del__(self):
        
        if self.fileOpen == True:
            print "\nClosing NeXus file..."
            self.h5file1.close()
            print "File closed!"
        
    #The dataset that stores the information to do with the transmission data
    def CreateProjectionFolder(self, nCalibrations, NumberOfImages, nDetectorsZ, nDetectorsY, detectorDimensions, rotation):
        
        #entry/entry1/tomo_entry/data/instrument/detector/data
        DetectorPath = '/entry1/tomo_entry/instrument/detector/'
        #detector = self.h5file1[DetectorPath]
               
        self.detectordata = self.h5file1.create_dataset(DetectorPath + 'data', shape=(NumberOfImages + (nCalibrations), nDetectorsZ, nDetectorsY), dtype = 'i4')
        
        #entry/entry1/tomo_entry/data/instrument/detector/image_key
        darkFieldKey = np.full(shape = (nCalibrations,), fill_value = 2)
        flatFieldKey = np.full(shape = (nCalibrations,), fill_value = 1)
        dataKey = np.zeros(NumberOfImages - nCalibrations)
        
        self.image_key = self.h5file1.create_dataset(DetectorPath + 'image_key', data = np.concatenate((dataKey, flatFieldKey, darkFieldKey)))
        
        x_pixel_sizeN = detectorDimensions[1]
        y_pixel_sizeN = detectorDimensions[2]
        
        x_pixel_size = self.h5file1.create_dataset(DetectorPath + 'x_pixel_size', data = x_pixel_sizeN)
        y_pixel_size = self.h5file1.create_dataset(DetectorPath + 'y_pixel_size', data = y_pixel_sizeN)
        
        rotation = np.concatenate([rotation, np.full(nCalibrations*2, rotation[NumberOfImages - nCalibrations - 1])])
        
        sample = self.h5file1['/entry1/tomo_entry/sample/']
        
        self.rotation_angle = sample.create_dataset('rotation_angle', data = np.rad2deg(rotation))
        self.rotation_angle.attrs['axis'] = '1'
        self.rotation_angle.attrs['label'] = '1'
        self.rotation_angle.attrs['units'] = 'degrees'
        
       
    #Appened to the transmission data
    def AddProjectionData(self, theData, nImage):
        
        dataSet = self.h5file1['/entry1/tomo_entry/instrument/detector/data']
        dataSet[nImage, :, :] = theData[:, :]
     
    #Create the rotation angles    
    def CreateRotationAngleData(self, Theta, NumberOfImages, nCalibrations):
        
        #/entry/entry1/tomo_entry/sample/rotation_angle
        
        PathToData = '/entry1/tomo_entry/sample/'
        
        rotationArray = np.linspace(start = 0, stop = Theta, num = NumberOfImages) 
        calibrationLine = np.zeros(nCalibrations*2)
        rotationArray = np.concatenate([calibrationLine, rotationArray])
        
        sample = self.h5file1[PathToData]
        
        self.rotation_angle = sample.create_dataset('rotation_angle', data = np.rad2deg(rotationArray))
        self.rotation_angle.attrs['axis'] = '1'
        self.rotation_angle.attrs['label'] = '1'
        self.rotation_angle.attrs['units'] = 'degrees'
  
    #Function to create the data for the beam energy, fluorescence or full mapping fluorescence
    def CreateDataGroup(self, dataType, nImages = 1, eBins = 1, xBins = 1, yBins = 1):
        
        #DataPath = '/entry1/tomo_entry/data/'
        xLabel = "energy"
        yLabel = "photons"
        
        #entry/entry1/tomo_entry/data/instrument/
        
        DataPath = 'entry1/tomo_entry/instrument/'
        
        if dataType == "Beam_Energy":
        
            dataGroup = self.h5file1.create_group(DataPath +'detector_BE/')
            dataGroup.attrs['NX_class'] = 'NXdata'
            dataGroup.attrs['axes'] = xLabel         # X axis of default plot
            xScaleDataSet = dataGroup.create_dataset(xLabel, shape = (eBins,), dtype = 'f8')  # X axis data
        
            dataGroup.attrs['signal'] = yLabel      # Y axis of default plot
            dataGroup.attrs[xLabel + '_indices'] = [0,] 
            dataSet = dataGroup.create_dataset(yLabel, shape = (eBins,), dtype = 'i4')  # Y axis data 
                    
        elif dataType == "Fluorescence":
        
            dataGroup = self.h5file1.create_group(DataPath +'detector_Fluor/')
            dataGroup.attrs['NX_class'] = 'NXdata'
            #dataGroup.attrs['axes'] = xLabel         # X axis of default plot
            #xScaleDataSet = dataGroup.create_dataset(xLabel, shape = (eBins,), dtype = 'f8')  # X axis data
        
            #dataGroup.attrs[xLabel + '_indices'] = [1,] 
            dataSet = dataGroup.create_dataset(yLabel, shape = (nImages, eBins), dtype = 'i4')  # Y axis data  
            
        elif dataType == "Full_Mapping_Fluorescence":
            
            dataGroup = self.h5file1.create_group(DataPath +'detector_FluorFM/')
            dataGroup.attrs['NX_class'] = 'NXdata'
            #dataGroup.attrs['axes'] = xLabel         # X axis of default plot
            #xScaleDataSet = dataGroup.create_dataset(xLabel, shape = (eBins,), dtype = 'f8')  # X axis data
            
            #dataGroup.attrs[xLabel + '_indices'] = [3,] 
            dataSet = dataGroup.create_dataset(yLabel, shape = (nImages, xBins, yBins, eBins), dtype = 'i4')  # Y axis data
        
        else:
            print "Unrecognised dataType", dataType,"using CreateDataGroup."
            sys.exit()  
     
    #Function to add the x axis to the data   
    def AddxAxis(self, dataType, data):
        
        DataPath = 'entry1/tomo_entry/instrument/detector'
        
        if dataType == 'Fluorescence':
        
            DataPath = DataPath + '_Fluor/'
            dataGroup = self.h5file1[DataPath]
            
            xLabel = "energy"
            dataGroup.attrs['axes'] = xLabel         # X axis of default plot
            dataGroup['energy'] = self.h5file1['entry1/tomo_entry/instrument/detector_BE/energy']
        
            dataGroup.attrs[xLabel + '_indices'] = [1,] 
            
        elif dataType == 'Full_Mapping_Fluorescence':
        
            DataPath = DataPath + '_FluorFM/'
            dataGroup = self.h5file1[DataPath]
            
            xLabel = "energy"
            dataGroup.attrs['axes'] = xLabel         # X axis of default plot
            dataGroup['energy'] = self.h5file1['entry1/tomo_entry/instrument/detector_BE/energy']
        
            dataGroup.attrs[xLabel + '_indices'] = [3,] 
            
        elif dataType == 'Beam_Energy':
            DataPath = DataPath + '_BE'
        
        dataSet = self.h5file1[DataPath + '/energy']
        dataSet[:] = data
        
    def AddxAxisTest(self, dataType):
    
        data = self.h5file1['/entry1/tomo_entry/data/Beam_Energy/energy']
        dataSet = self.h5file1['/entry1/tomo_entry/data/'+ dataType + '/energy']
        dataSet[:] = data
        
    #Function to appened the data
    def AddData(self, dataType, data, nImage = 1):
        
        #dataSet = self.h5file1['/entry1/tomo_entry/data/'+ dataType +'/photons']
        
        if dataType == "Beam_Energy":
            dataSet = self.h5file1['entry1/tomo_entry/instrument/detector_BE/photons']
            dataSet[:] = data
            
        elif dataType == "Fluorescence":
            dataSet = self.h5file1['entry1/tomo_entry/instrument/detector_Fluor/photons']
            dataSet[nImage, :] = data
            
        elif dataType == "Full_Mapping_Fluorescence":
            dataSet = self.h5file1['entry1/tomo_entry/instrument/detector_FluorFM/photons']
            dataSet[nImage, :, :, :] = data
        
        else:
            print "Unrecognised dataType", dataType,"using AddData."
            sys.exit()  
            
    def LinkData(self):
    
        #/entry/entry1/tomo_entry/data/data
        DataPath = '/entry1/tomo_entry/data'
        data = self.h5file1[DataPath]
           
        data['data'] = self.detectordata
        data['rotation_angle'] = self.rotation_angle
        data['image_key'] = self.image_key
          
    #Display the tree of the nexus file  
    def DisplayTree(self):
    
        def printname(name):
            print name
    
        print "The nexus file layout: "    
        self.h5file1.visit(printname)
        
