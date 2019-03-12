#ifndef DiffractionDetector_h
#define DiffractionDetector_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "SettingsLog.hh"
#include <vector>

//My own classes
class Data;
class DiffractionSD;
//class AbsorptionDetectorMessenger;

//Solids, logic volume and physical volume for the geometry
class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;

//For efficient geometry containing millions of solids
class G4PhantomParameterisation;

//Materials and colour classes
class G4Material;
class G4Colour;

class DiffractionDetector
{
    public:
        DiffractionDetector();
        ~DiffractionDetector();
    
        void CreateVolumes();
        void AddProperties(Data* data, G4bool GraphicsOn);
        void PlaceDetectors(G4LogicalVolume* MotherBox, G4ThreeVector position);
        
        void ReadOutInfo(SettingsLog& log);
        
        //SetFunctions
        void SetHalfDimensions(G4ThreeVector value){halfdimensions = value;}
        void SetHalfDimenions(std::vector<double> value){halfdimensions.setX(value[0]);halfdimensions.setY(value[1]);halfdimensions.setZ(value[2]);}
        void SetRows(G4int value){rows = value;}
        void SetColumns(G4int value){columns = value;}
        void SetNumberOfxPixels(int value){rows = value;}
        void SetNumberOfyPixels(int value){columns = value;}
        
        //GetFunctions
        G4int GetRows(){return rows;}
        G4int GetColumns(){return columns;}
        
        G4int GetNumberOfxPixels()             {return rows;}
        G4int GetNumberOfyPixels()             {return columns;}
        G4ThreeVector GetHalfDimenions()       {return halfdimensions;}
        std::vector<double> GetHalfDimensions(){return {halfdimensions.x(),halfdimensions.y(),halfdimensions.z()};}
        
    private:
        //Pointers
        //AbsorptionDetectorMessenger* ADMessenger;
        G4PhantomParameterisation* param;
        G4Box* DetectorContainer;
        G4Box* DetectorCells;
        G4LogicalVolume* DetectorContainerLV;
		G4LogicalVolume* DetectorCellsLV;
		
		DiffractionSD* diffraSD;
    
        //Variables
        G4int rows;
		G4int columns;
		G4ThreeVector halfdimensions;
		G4String materialName;
		G4bool realDetector;
};
#endif
