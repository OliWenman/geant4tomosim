#ifndef TargetConstruction_h
#define TargetConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4UserLimits.hh"
#include "G4UImessenger.hh"
#include <vector>
#include "G4SystemOfUnits.hh"
#include "G4Colour.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class TargetConstructionMessenger;
class Data;
class TrackerSD;
class G4Material;

class TargetConstruction
{
	public:
		TargetConstruction(G4int);
		virtual ~TargetConstruction();

		virtual void Construct(G4LogicalVolume *logicWorld);

		virtual void MasterVolume(G4String Shape,  G4LogicalVolume* logicMotherBox);
		virtual void Box(G4String CheckMaster, G4ThreeVector TargetSize, G4ThreeVector TargetPosition, G4String Material, G4LogicalVolume* logicWorld);
		virtual void HollowBox(G4String CheckMaster, G4ThreeVector TargetSize, G4ThreeVector InnerSize, G4ThreeVector TargetPosition, G4String Material, G4LogicalVolume* logicWorld);
		virtual void Cylinder(G4ThreeVector WorldSize, G4String Material, G4LogicalVolume* TargetMotherBox, G4String Shape);
		virtual G4Material* FindMaterial(G4String MaterialName);
		virtual G4double RotateObject();
		virtual G4ThreeVector MasterOffSetRotation(G4String CheckMaster, G4ThreeVector, G4double, G4double);
		virtual G4ThreeVector OffSetRotation(G4String CheckMaster, G4ThreeVector Centre, G4double Radius, G4double Angle);
		virtual void Visualization(G4LogicalVolume*, G4Colour);
 
		inline void SetNumberOfObjects(G4int value){NumberOfObjects_Cmd = value;}
		inline void SetMasterPosition(G4ThreeVector value){MasterPosition_Cmd = value;}
		inline void SetTargetPosition(G4ThreeVector value){TargetPosition_Cmd = value;}
		inline void SetMasterTargetMaterial(G4String value){MasterTargetMaterial_Cmd = value;}
		inline void SetMasterTargetCopyNo(G4int value){MasterTargetCopyNo = value;}
		inline void SetMasterOffSetRadius(G4double value){MasterOffSetRadius_Cmd = value;}


		inline void SetCurrentImage(G4int value){CurrentImage = value;}
		inline void SetNoImages(G4int value){NoImages = value;}
		inline void SetVisualization(G4bool value){VisualizationValue = value;}

		G4int GetNumberOfObjects(){return NumberOfObjects_Cmd;}
		G4ThreeVector GetMasterPosition(){return MasterPosition_Cmd;}
		G4ThreeVector GetTargetPosition(){return TargetPosition_Cmd;}
		G4String GetMasterTargetMaterial(){return MasterTargetMaterial_Cmd;}
		G4int GetMasterTargetCopyNo(){return MasterTargetCopyNo;}
		G4double GetMasterOffSetRadius(){return MasterOffSetRadius_Cmd;}

		G4int GetCurrentImage(){return CurrentImage;}
		G4int GetNoImages(){return NoImages;}
		G4bool GetVisualization(){return VisualizationValue;}

		inline void SavePositions(std::vector<G4ThreeVector> value){Positions = value;}
		inline void SetPosition(G4int n, G4ThreeVector value){Positions[n] = value;}
		G4ThreeVector GetPosition(G4int n){return Positions[n];}

	private:
		TargetConstructionMessenger *TCMessenger;		

		G4VPhysicalVolume *hollowBox;

		//Target variables
		std::vector<G4ThreeVector> Positions;		

		G4int NumberOfObjects_Cmd;
		G4ThreeVector MasterPosition_Cmd;
		G4ThreeVector TargetPosition_Cmd;
		G4String MasterTargetMaterial_Cmd;
		G4int MasterTargetCopyNo;
		G4double MasterOffSetRadius_Cmd;

		//Data manipulation varaibles
		G4int CurrentImage;
		G4int NoImages;

		G4bool VisualizationValue;
};

#endif	
	
