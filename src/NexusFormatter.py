import numpy as np
import h5py
import time
import os
import gc
import getpass
import sys
import datetime

class NexusFormatter:
        
    #When class is created
    def __init__(self):
            
        self.fileOpen = False
        self.SimReady = False
        self.setupSuccess = False
        
    def openFile(self, filepath, interactivity = True):
        
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
            
        #Check if file exists       
        if os.path.isfile(filepath) and interactivity == True:
                
            #If it exits, check it's size
            fileInfo = os.stat(filepath)
            Modified_time = datetime.datetime.fromtimestamp(fileInfo.st_mtime)
            fileSizeB  = fileInfo.st_size #In B 
            fileSizeKB = fileInfo.st_size/(10**3) #In KB 
            fileSizeMB = fileInfo.st_size/(10**6) #In MB           
            fileSizeGB = fileInfo.st_size/(10**9) #In GB
                        
            if fileSizeB > 10**3:
                if fileSizeKB > 10**3:
                    if fileSizeMB > 10**3: 
                       fileSize = fileSizeGB
                       Unit = "Gb"
                    else:
                       fileSize = fileSizeMB
                       Unit = "Mb"
                else:
                    fileSize = fileSizeKB
                    Unit = "kb"
            else:
                fileSize = fileSizeB
                Unit = "bytes"
            
            print ("\nThis file already exits. \nSize:", fileSize, Unit, "\nLast modified:", Modified_time,"\nAre you sure you want to override this?\n")              
            contin = False
                
            while contin != True:
                #raw_input for python2, input for python3
                try:
                    answer = raw_input('yes/no?\n')
                except NameError:
                    answer = input('yes/no?\n')
                
                if answer == 'y' or answer == 'yes' or answer == 'Y':
                   contin = True   
                        
                elif answer == 'n' or answer == 'no' or answer == 'N':
                   contin = True
                   self.setupSuccess = False 
                   return                
                else:
                   print ("Input yes or no to continue.")
                   contin = False
                
                print ("\n")
            
        #Try to open file, delete file if can't open it as it's going to override it anyway       
        try:     
            self.h5file1 = h5py.File(filepath, 'w')
        
        except OSError:
            os.remove(filepath)
            self.h5file1 = h5py.File(filepath, 'w')
        
        self.fileOpen = True
        
        self.h5file1.attrs['default'] = 'entry'

        #Create the different groups to conform to the NXtomo format

        #/entry/entry1
        nxentry = self.h5file1.create_group('entry1')
        nxentry.attrs['NX_class'] = 'NXentry'
        nxentry.attrs['default'] = 'data'
        
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
        
        self.setupSuccess = True       
        
    #When class is destroyed, close the file if it isn't already closed
    def __del__(self):
        
        if self.fileOpen == True:
            print ("\nClosing NeXus file...")
            self.h5file1.close()
            self.fileOpen = False
            print ("File closed!")
        
    #The dataset that stores the information to do with the transmission data
    def CreateProjectionFolder(self, 
                               nCalibrations, 
                               NumberOfImages, 
                               nDetectorsZ, 
                               nDetectorsY, 
                               detectorDimensions, 
                               rotation):
        
        #entry/entry1/tomo_entry/data/instrument/detector/data
        DetectorPath = '/entry1/tomo_entry/instrument/detector/'
               
        self.detectordata = self.h5file1.create_dataset(DetectorPath + 'data', shape=(NumberOfImages + (nCalibrations), nDetectorsZ, nDetectorsY), dtype = 'i4')
        
        #entry/entry1/tomo_entry/data/instrument/detector/image_key
        darkFieldKey = np.full(shape = (nCalibrations,), fill_value = 2)
        flatFieldKey = np.full(shape = (nCalibrations,), fill_value = 1)
        dataKey = np.zeros(NumberOfImages - nCalibrations)
        
        self.image_key = self.h5file1.create_dataset(DetectorPath + 'image_key', data = np.concatenate((dataKey, flatFieldKey, darkFieldKey)))
        
        #x_pixel_sizeN = detectorDimensions[1]
        #y_pixel_sizeN = detectorDimensions[2]
        
        #x_pixel_size = self.h5file1.create_dataset(DetectorPath + 'x_pixel_size', data = x_pixel_sizeN)
        #y_pixel_size = self.h5file1.create_dataset(DetectorPath + 'y_pixel_size', data = y_pixel_sizeN)
        
        rotation = np.concatenate([rotation, np.full(nCalibrations*2, rotation[NumberOfImages - nCalibrations - 1])])
        
        sample = self.h5file1['/entry1/tomo_entry/sample/']
        
        self.rotation_angle = sample.create_dataset('rotation_angle', data = np.rad2deg(rotation))
        self.rotation_angle.attrs['axis'] = '1'
        self.rotation_angle.attrs['label'] = '1'
        self.rotation_angle.attrs['units'] = 'degrees'
        
       
    #Appened to the transmission data
    def AddProjectionData(self, 
                          theData, 
                          nImage):
        
        dataSet = self.h5file1['/entry1/tomo_entry/instrument/detector/data']
        dataSet[nImage, :, :] = theData[:, :]
    
    #Function to create the data for the beam energy, fluorescence or full mapping fluorescence
    def CreateDataGroup(self, 
                        dataType, 
                        nImages = 1, 
                        eBins = 1, 
                        xBins = 1, 
                        yBins = 1):
        
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
        
            dataGroup.attrs[xLabel + '_indices'] = [1,] 
            dataSet = dataGroup.create_dataset(yLabel, shape = (nImages, eBins), dtype = 'i4')  # Y axis data 
                    
        elif dataType == "Fluorescence":
        
            dataGroup = self.h5file1.create_group(DataPath +'detector_Fluor/')
            dataGroup.attrs['NX_class'] = 'NXdata'
            dataGroup.attrs['axes'] = xLabel         # X axis of default plot
            xScaleDataSet = dataGroup.create_dataset(xLabel, shape = (eBins,), dtype = 'f8')  # X axis data
        
            dataGroup.attrs[xLabel + '_indices'] = [1,] 
            dataSet = dataGroup.create_dataset(yLabel, shape = (nImages, eBins), dtype = 'i4')  # Y axis data 
            
        elif dataType == "Full_Mapping_Fluorescence":
            
            dataGroup = self.h5file1.create_group(DataPath +'detector_FluorFM/')
            dataGroup.attrs['NX_class'] = 'NXdata'
            dataGroup.attrs['axes'] = xLabel         # X axis of default plot
            xScaleDataSet = dataGroup.create_dataset(xLabel, shape = (eBins,), dtype = 'f8')  # X axis data
            
            dataGroup.attrs[xLabel + '_indices'] = [3,] #
            dataSet = dataGroup.create_dataset(yLabel, shape = (nImages, yBins, xBins, eBins), dtype = 'i4')  # Y axis data
        
        else:
            print ("Unrecognised dataType", dataType,"using CreateDataGroup.")
            sys.exit()  
     
    #Function to add the x axis to the data   
    def AddxAxis(self, 
                 dataType, 
                 data):
        
        DataPath = 'entry1/tomo_entry/instrument/detector'
        
        if dataType == 'Fluorescence':
        
            DataPath = DataPath + '_Fluor/'
            dataSet = self.h5file1[DataPath + '/energy']
            dataSet[:] = data
            """
            dataGroup = self.h5file1[DataPath]
            
            xLabel = "energy"
            dataGroup.attrs['axes'] = xLabel         # X axis of default plot
            dataGroup['energy'] = self.h5file1['entry1/tomo_entry/instrument/detector_BE/energy']
        
            dataGroup.attrs[xLabel + '_indices'] = [1,] 
            """
        elif dataType == 'Full_Mapping_Fluorescence':
        
            DataPath = DataPath + '_FluorFM/'
            dataSet = self.h5file1[DataPath + '/energy']
            dataSet[:] = data
            """
            dataGroup = self.h5file1[DataPath]
            
            xLabel = "energy"
            dataGroup.attrs['axes'] = xLabel         # X axis of default plot
            dataGroup['energy'] = self.h5file1['entry1/tomo_entry/instrument/detector_BE/energy']
        
            dataGroup.attrs[xLabel + '_indices'] = [3,] 
            """
        elif dataType == 'Beam_Energy':
            DataPath = DataPath + '_BE'
            dataSet = self.h5file1[DataPath + '/energy']
            dataSet[:] = data
        #dataSet = self.h5file1[DataPath + '/energy']
        #dataSet[:] = data
        
    def AddxAxisTest(self, dataType):
    
        data = self.h5file1['/entry1/tomo_entry/data/Beam_Energy/energy']
        dataSet = self.h5file1['/entry1/tomo_entry/data/'+ dataType + '/energy']
        dataSet[:] = data
        
    #Function to appened the data
    def AddData(self, 
                dataType, 
                data, 
                nImage):
        
        if dataType == "Beam_Energy":
            dataSet = self.h5file1['entry1/tomo_entry/instrument/detector_BE/photons']
            dataSet[nImage, :] = data
            
        elif dataType == "Fluorescence":
            dataSet = self.h5file1['entry1/tomo_entry/instrument/detector_Fluor/photons']
            dataSet[nImage, :] = data
            
        elif dataType == "Full_Mapping_Fluorescence":
            dataSet = self.h5file1['entry1/tomo_entry/instrument/detector_FluorFM/photons']
            dataSet[nImage, :, :, :] = data
        
        else:
            print ("Unrecognised dataType", dataType,"using AddData.")
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
            print (name)
    
        print ("The nexus file layout: ")    
        self.h5file1.visit(printname)
        
    def closeFile(self):
        print ("\nClosing NeXus file...")
        self.h5file1.close()
        self.fileOpen = False
        print ("File closed!")
