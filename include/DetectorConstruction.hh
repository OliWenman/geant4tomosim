#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4UserLimits.hh"
#include "G4UImessenger.hh"
#include "G4Colour.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class DetectorConstructionMessenger;
class Data;
class Input;
class TrackerSD;
class VisTrackerSD;
class G4Material;
class TargetConstruction;

//Detector construction class to define materials and geometry.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  	public:
    		DetectorConstruction(Data* data, Input* input);
    		~DetectorConstruction();

		//Base class method
    		G4VPhysicalVolume* Construct();
		
		//Own class functions
		void SetUpDetectors(G4ThreeVector DetectorSize, G4int NoDetectorsY, G4int NoDetectorsZ, G4String Material, G4LogicalVolume* logicMotherBox);
		G4Material* FindMaterial(G4String material);
		void AttachSensitiveDetector(G4LogicalVolume* volume);
		void Visualization(G4LogicalVolume*, G4Colour);
		
		//Set methods
		void SetWorldSize(G4ThreeVector value){WorldSize_Cmd = value;}
		void SetVisualization(G4bool value){Visualization_Cmd = value;}

		void SetNoDetectorsY(G4int value){NoDetectorsY_Cmd = value;} 
		void SetNoDetectorsZ(G4int value){NoDetectorsZ_Cmd = value;}
		void SetDetectorSize(G4ThreeVector value){DetectorSize_Cmd = value;}
		void SetDetectorMaterial(G4String value){DetectorMaterial_Cmd = value;}
		void SetDetectorEfficiency(G4bool value){DetectorEfficiency_Cmd = value;}

		void SetCurrentImage(G4int value){CurrentImage = value;}
		void SetNoImages(G4int value){NoImages = value;}

		//Get methods
		G4ThreeVector GetWorldSize(){return WorldSize_Cmd;}
		G4bool GetVisualization(){return Visualization_Cmd;}

		G4int GetNoDetectorsY(){return NoDetectorsY_Cmd;}
		G4int GetNoDetectorsZ(){return NoDetectorsZ_Cmd;}
		G4ThreeVector GetDetectorSize(){return DetectorSize_Cmd;}
		G4String GetDetectorMaterial(){return DetectorMaterial_Cmd;}
		G4bool GetDetectorEfficiency(){return DetectorEfficiency_Cmd;}

		G4int GetCurrentImage(){return CurrentImage;}
		G4int GetNoImages(){return NoImages;}

  	protected:
		//Pointers to classes
		G4UserLimits* fStepLimit;            
		DetectorConstructionMessenger* detectorMessenger;
		Data* data;
		Input* input;

		TrackerSD* aTrackerSD;
		VisTrackerSD* VTrackerSD;

		TargetConstruction* TC;

		//DATA
		//World variables
		G4ThreeVector WorldSize_Cmd;
		G4bool Visualization_Cmd;

		//Detector variables
		G4int NoDetectorsY_Cmd;
		G4int NoDetectorsZ_Cmd;

		G4ThreeVector DetectorSize_Cmd;
		G4String DetectorMaterial_Cmd;
		G4bool DetectorEfficiency_Cmd;

		//Data manipulation varaibles
		G4int nImage;
		G4int CurrentImage;
		G4int NoImages;
};

#endif
