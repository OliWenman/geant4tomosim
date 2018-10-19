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
		TargetConstruction();
		~TargetConstruction();

		//Base class method
		void Construct(G4LogicalVolume *logicWorld);

		//Own class methods
		void Box(G4int ObjectNumber, G4ThreeVector TargetSize, G4ThreeVector TargetPosition, G4String Material, G4LogicalVolume* logicWorld);
		G4Material* FindMaterial(G4String MaterialName);
		G4double RotateObject();
		G4ThreeVector OffSetRotation(G4int ObjectNumber, G4ThreeVector Centre, G4double Radius, G4double Angle);
		void Visualization(G4LogicalVolume*, G4Colour);
 
		void SetNumberOfObjects(G4int value){NumberOfObjects_Cmd = value;}
		void SetTargetMaterial(G4String value){TargetMaterial_Cmd = value;}
		void SetOffSetRadius(G4double value){OffSetRadius_Cmd = value;}

		void SetCurrentImage(G4int value){CurrentImage = value;}
		void SetNoImages(G4int value){NoImages = value;}
		void SetVisualization(G4bool value){VisualizationValue = value;}

		G4int GetNumberOfObjects(){return NumberOfObjects_Cmd;}
		G4String GetTargetMaterial(){return TargetMaterial_Cmd;}
		G4double GetOffSetRadius(){return OffSetRadius_Cmd;}

		G4int GetCurrentImage(){return CurrentImage;}
		G4int GetNoImages(){return NoImages;}
		G4bool GetVisualization(){return VisualizationValue;}

		inline void SavePositionsArray(std::vector<G4ThreeVector> value){Positions = value;}
		inline void SetVectorPosition(G4int n, G4ThreeVector value){Positions[n] = value;}
		G4ThreeVector GetVectorPosition(G4int n){return Positions[n];}

	private:
		TargetConstructionMessenger *TCMessenger;		

		//Target variables
		std::vector<G4ThreeVector> Positions;		

		G4int NumberOfObjects_Cmd;
		G4String TargetMaterial_Cmd;
		G4double OffSetRadius_Cmd;

		//Data manipulation varaibles
		G4int CurrentImage;
		G4int NoImages;

		G4bool VisualizationValue;
};

#endif	
	
