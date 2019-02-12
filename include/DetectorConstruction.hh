#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "SettingsLog.hh"

//My own classes
class DetectorConstructionMessenger;
class Data;
class TomographySD;
class TomographyVSD;
class FluorescenceSD;
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
        DetectorConstruction(Data *data);
    	~DetectorConstruction();

		//Base class method
    	G4VPhysicalVolume* Construct();
		
		//Set methods
		void SetWorldSize(G4ThreeVector value){WorldSize_Cmd = value;}
		void SetWorldMaterial(G4String value){WorldMaterial_Cmd = value;}
		void SetVisualization(G4bool value){Visualization_Cmd = value;}

		void SetNoDetectorsY(G4int value){NoDetectorsY_Cmd = value;} 
		void SetNoDetectorsZ(G4int value){NoDetectorsZ_Cmd = value;}
		void SetDetectorSize(G4ThreeVector value){DetectorSize_Cmd = value;}
		void SetDetectorMaterial(G4String value){DetectorMaterial_Cmd = value;}
		void SetDetectorEfficiency(G4bool value){DetectorEfficiency_Cmd = value;}
		void SetFluorescenceDet(G4bool value){FluorescenceDet = value;}
		void SetFluorDetSize(G4ThreeVector value){FluorDetSize_Cmd = value;}
		
		void SetCurrentImage(int value){nImage = value;}

		//Get methods
		G4ThreeVector GetWorldSize() const {return WorldSize_Cmd;}
		G4LogicalVolume* GetWorldLV() {return logicWorld;}
		G4bool GetVisualization(){return Visualization_Cmd;}

		G4int GetNoDetectorsY(){return NoDetectorsY_Cmd;}
		G4int GetNoDetectorsZ(){return NoDetectorsZ_Cmd;}
		G4ThreeVector GetDetectorSize(){return DetectorSize_Cmd;}
		G4String GetDetectorMaterial(){return DetectorMaterial_Cmd;}
		G4bool GetDetectorEfficiency(){return DetectorEfficiency_Cmd;}

		FluorescenceSD* GetFluoreDetector(){return fluorescenceSD;}

		void RelayToTC(int NumberOfImages, double TotalAngle);
	
		void ReadOutInfo(SettingsLog& log);
		
		TargetConstruction *GetTargetConstruction();

  	private:

		//Own class functions
		void SolidDetectors();
		void LVDetectors();
		void PVDetectors(G4LogicalVolume* logicMotherBox);

		G4Material* FindMaterial(G4String material);
		void AttachSensitiveDetector(G4LogicalVolume* volume, G4String TypeDetector);
		void Visualization(G4LogicalVolume*, G4Colour);

		//Pointers to my own classes 
		DetectorConstructionMessenger* detectorMessenger;
		Data* data;
		TomographySD* tomoSD;
		TomographyVSD* tomoVSD;
		FluorescenceSD* fluorescenceSD;
		TargetConstruction* TC;

		//Pointers to detector geometry (solid and logic)
		G4PhantomParameterisation* param;
		G4Box* SolidContainer;
		G4Box* SolidPhantomBoxes;
		G4LogicalVolume* container_logic;
		G4LogicalVolume* PhantomBoxes_logic;

		G4Box* SolidFluoDet;
		G4LogicalVolume* FluoDet_logic; 

		//World variables
		G4Box* solidWorld;
		G4LogicalVolume* logicWorld;
		G4ThreeVector WorldSize_Cmd;
		G4String WorldMaterial_Cmd;
		G4bool Visualization_Cmd;

		//Detector variables
		G4int NoDetectorsY_Cmd;
		G4int NoDetectorsZ_Cmd;
		G4ThreeVector DetectorSize_Cmd;
		G4String DetectorMaterial_Cmd;
		G4bool DetectorEfficiency_Cmd;

		G4ThreeVector FluorDetSize_Cmd;
		G4ThreeVector FluorDetPos_Cmd;

		//Image variables
		G4int nImage;

		bool FluorescenceDet;
		
		bool WarningChecked;
		
		
};

#endif
