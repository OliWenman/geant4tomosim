#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4UserLimits.hh"
#include "G4UImessenger.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class DetectorConstructionMessenger;
class Data;
class TrackerSD;
class G4Material;

//Detector construction class to define materials and geometry.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  	public:
    		DetectorConstruction(Data* data);
    		virtual ~DetectorConstruction();

    		virtual G4VPhysicalVolume* Construct();
		virtual void SetUpTarget(G4ThreeVector WorldSize, G4String Material, G4LogicalVolume* TargetMotherBox, G4String Shape);
		virtual void SetUpTargetBox(G4ThreeVector TargetSize, G4ThreeVector InnerSize, G4ThreeVector TargetPosition, G4String Material, G4LogicalVolume* logicMotherBox);
		virtual void SetUpDetectors(G4ThreeVector DetectorSize, G4int NoDetectorsY, G4int NoDetectorsZ, G4String Material, G4LogicalVolume* logicMotherBox);
		virtual G4Material* FindMaterial(G4String material);
		virtual G4double RotationMatrix();
		virtual void AttachSensitiveDetector(G4LogicalVolume* volume);
		
		//Set methods
		inline void SetWorldSize(G4ThreeVector value){WorldSize_Cmd = value;}

		inline void SetNoDetectorsY(G4int value){NoDetectorsY_Cmd = value;} 
		inline void SetNoDetectorsZ(G4int value){NoDetectorsZ_Cmd = value;}
		inline void SetDetectorSize(G4ThreeVector value){DetectorSize_Cmd = value;}
		inline void SetDetectorMaterial(G4String value){DetectorMaterial_Cmd = value;}

		inline void SetTargetPosition(G4ThreeVector value){TargetPosition_Cmd = value;}
		inline void SetTargetMaterial(G4String value){TargetMaterial_Cmd = value;}
		inline void SetTargetCopyNo(G4int value){TargetCopyNo = value;}

		inline void SetCurrentImage(G4int value){CurrentImage = value;}
		inline void SetNoImages(G4int value){NoImages = value;}

		//Get methods
		G4ThreeVector GetWorldSize(){return WorldSize_Cmd;}

		G4int GetNoDetectorsY(){return NoDetectorsY_Cmd;}
		G4int GetNoDetectorsZ(){return NoDetectorsZ_Cmd;}
		G4ThreeVector GetDetectorSize(){return DetectorSize_Cmd;}
		G4String GetDetectorMaterial(){return DetectorMaterial_Cmd;}

		G4ThreeVector GetTargetPosition(){return TargetPosition_Cmd;}
		G4String GetTargetMaterial(){return TargetMaterial_Cmd;}
		G4int GetTargetCopyNo(){return TargetCopyNo;}

		G4int GetCurrentImage(){return CurrentImage;}
		G4int GetNoImages(){return NoImages;}

  	protected:
		//Pointers to classes
		G4UserLimits* fStepLimit;            
		DetectorConstructionMessenger* detectorMessenger;
		Data* data;
		TrackerSD* aTrackerSD;

		//DATA
		//World variables
		G4ThreeVector WorldSize_Cmd;

		//Detector variables
		G4int NoDetectorsY_Cmd;
		G4int NoDetectorsZ_Cmd;
		G4ThreeVector DetectorSize_Cmd;
		G4String DetectorMaterial_Cmd;

		//Target variables
		G4ThreeVector TargetPosition_Cmd;
		G4String TargetMaterial_Cmd;
		G4int TargetCopyNo;

		//Data manipulation varaibles
		G4int CurrentImage;
		G4int NoImages;
};

#endif
