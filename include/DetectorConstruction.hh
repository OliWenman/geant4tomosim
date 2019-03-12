#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "SettingsLog.hh"
#include "AbsorptionDetector.hh"
#include "FluorescenceDetector.hh"
#include "DiffractionDetector.hh"
#include "TargetConstruction.hh"

//My own classes
class DetectorConstructionMessenger;
class Data;
class AbsorptionDetector;
class FluorescenceDetector;
class DiffractionDetector;
class TargetConstruction;

//Solids, logic volume and physical volume for the geometry
class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;

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

		/*void SetNoDetectorsY(G4int value){NoDetectorsY_Cmd = value;} 
		void SetNoDetectorsZ(G4int value){NoDetectorsZ_Cmd = value;}
		void SetDetectorSize(G4ThreeVector value){DetectorSize_Cmd = value;}
		void SetDetectorMaterial(G4String value){DetectorMaterial_Cmd = value;}
		void SetDetectorEfficiency(G4bool value){DetectorEfficiency_Cmd = value;}
		void SetFluorescenceDet(G4bool value){FluorescenceDet = value;}
		void SetFluorDetSize(G4ThreeVector value){FluorDetSize_Cmd = value;}*/
		
		void SetCurrentImage(int value){nImage = value;}

		//Get methods
		G4ThreeVector GetWorldSize() const {return WorldSize_Cmd;}
		G4LogicalVolume* GetWorldLV() {return logicWorld;}
		G4bool GetVisualization(){return Visualization_Cmd;}
		
		G4int GetAbsoptionDetectorRows(){return absDetector -> GetRows();}
		G4int GetAbsoptionDetectorColumns(){return absDetector -> GetColumns();}

		void RelayToTC(int NumberOfImages, double TotalAngle);
	
		void ReadOutInfo(SettingsLog& log);
		
		TargetConstruction *GetTargetConstruction()     {return TC;}
		AbsorptionDetector *GetAbsorptionDetector()     {return absDetector;}
		FluorescenceDetector *GetFluorescenceDetector() {return fluorescenceDetector;}
		DiffractionDetector *GetDiffractionDetector()   {return diffractionDetector;}

  	private:

		G4Material* FindMaterial(G4String material);
		void Visualization(G4LogicalVolume*, G4Colour);

		//Pointers to my own classes 
		DetectorConstructionMessenger* detectorMessenger;
		Data* data;
		TargetConstruction* TC;

		//World variables
		G4Box* solidWorld;
		G4LogicalVolume* logicWorld;
		G4ThreeVector WorldSize_Cmd;
		G4String WorldMaterial_Cmd;
		G4bool Visualization_Cmd;

		//Image variables
		G4int nImage;
		
		AbsorptionDetector* absDetector;
		FluorescenceDetector* fluorescenceDetector;		
		DiffractionDetector* diffractionDetector; 
};

#endif
