#ifndef FluorescenceDetector_h
#define FluorescenceDetector_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "SettingsLog.hh"

//My own classes
class Data;
class FluorescenceSD;
class FluorescenceDetectorMessenger;

//Solids, logic volume and physical volume for the geometry
class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;

class FluorescenceDetector
{
    public:
        FluorescenceDetector();
        ~FluorescenceDetector();
    
        void CreateVolumes();
        void CreateVolumes(G4ThreeVector halfDimensions);
        void AddProperties(Data* data, G4bool GraphicsOn);
        void PlaceDetectors(G4LogicalVolume* MotherBox, G4ThreeVector position);
        
        void ReadOutInfo(SettingsLog& log);

        //Set
        void SetHalfDimensions(G4ThreeVector value){halfdimensions = value;}
        
        //Get
        FluorescenceSD* GetSensitiveDetector() {return sensitiveDetector;}

    private:
        FluorescenceDetectorMessenger* FDMessenger;
    
        G4Box* DetectorVolume;
        G4LogicalVolume *DetectorLV;
        FluorescenceSD* sensitiveDetector;        
        
        G4ThreeVector halfdimensions;
        G4ThreeVector position;
};
#endif
