#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"

//My own classes
class DetectorConstructionMessenger;
class Data;
class Input;
class TrackerSD;
class VisTrackerSD;
class TargetConstruction;

//Solids, logic volume and physical volume for the geometry
class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;

//For efficient geometry containing millions of solids
class G4PhantomParameterisation;

//Materials and colour classes
class G4Material;
class G4Colour;

//Detector construction class to define materials and geometry.
class DetectorConstruction : public G4VUserDetectorConstruction
{
  	public:
    		DetectorConstruction(Data* data, Input* input);
    		~DetectorConstruction();

		//Base class method
    		G4VPhysicalVolume* Construct();
		
		//Own class functions
		void SolidDetectors();
		void LVDetectors();
		void PVDetectors(G4LogicalVolume* logicMotherBox);

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

		//Get methods
		G4ThreeVector GetWorldSize() const {return WorldSize_Cmd;}
		G4bool GetVisualization(){return Visualization_Cmd;}

		G4int GetNoDetectorsY(){return NoDetectorsY_Cmd;}
		G4int GetNoDetectorsZ(){return NoDetectorsZ_Cmd;}
		G4ThreeVector GetDetectorSize(){return DetectorSize_Cmd;}
		G4String GetDetectorMaterial(){return DetectorMaterial_Cmd;}
		G4bool GetDetectorEfficiency(){return DetectorEfficiency_Cmd;}

		void RelayToTC(int NumberOfImages, double TotalAngle);

  	protected:
		//Pointers to my own classes 
		DetectorConstructionMessenger* detectorMessenger;
		Data* data;
		Input* input;
		TrackerSD* aTrackerSD;
		VisTrackerSD* VTrackerSD;
		TargetConstruction* TC;

		//Pointers to the solid and logic of the world
		G4Box* solidWorld;
		G4LogicalVolume* logicWorld;

		//Pointers to detector geometry (solid and logic)
		G4PhantomParameterisation* param;
		G4Box* SolidContainer;
		G4Box* SolidPhantomBoxes;
		G4LogicalVolume* container_logic;
		G4LogicalVolume* PhantomBoxes_logic;

		//World variables
		G4ThreeVector WorldSize_Cmd;
		G4bool Visualization_Cmd;

		//Detector variables
		G4int NoDetectorsY_Cmd;
		G4int NoDetectorsZ_Cmd;
		G4ThreeVector DetectorSize_Cmd;
		G4String DetectorMaterial_Cmd;
		G4bool DetectorEfficiency_Cmd;

		//Image variables
		G4int nImage;
};

#endif
