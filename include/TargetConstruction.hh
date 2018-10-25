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
		void Cylinder();
		void Sphere(G4int ObjectNumber, G4double Radius, G4ThreeVector TargetPosition, G4String Material, G4LogicalVolume* MotherBox);


		G4Material* FindMaterial(G4String MaterialName);
		G4double RotateObject();
		G4ThreeVector OffSetRotation(G4int ObjectNumber, G4ThreeVector Centre, G4double Radius, G4double Angle);
		void Visualization(G4LogicalVolume*, G4Colour);
 
		void SetNumberOfObjects(G4int value){NumberOfObjects_Cmd = value;}
		void SetOffSetRadius(G4double value){OffSetRadius_Cmd = value;}
		void SetCentrePosition(G4ThreeVector value){Centre_Cmd = value;}

		void SetCurrentImage(G4int value){CurrentImage = value;}
		void SetNoImages(G4int value){NoImages = value;}
		void SetVisualization(G4bool value){VisualizationValue = value;}

		G4int GetNumberOfObjects(){return NumberOfObjects_Cmd;}
		G4double GetOffSetRadius(){return OffSetRadius_Cmd;}

		G4int GetCurrentImage(){return CurrentImage;}
		G4int GetNoImages(){return NoImages;}
		G4bool GetVisualization(){return VisualizationValue;}

		inline void SaveTypeOfObects(std::vector<G4String> value){TypeOfObjects = value;}
		inline void AddTypeOfObjects(G4String value){TypeOfObjects.push_back (value);}
		inline void SetTypeOfObjects(G4int n, G4String value){TypeOfObjects[n] = value;}
		G4String GetTypeOfObjects(G4int n){return TypeOfObjects[n];}

		inline void SaveCubeDimensionsArray(std::vector<G4ThreeVector> value){CubeDimensions = value;}
		inline void AddCubeDimensionsArray(G4ThreeVector value){CubeDimensions.push_back (value);}
		inline void SetCubeDimensions(G4int n, G4ThreeVector value){CubeDimensions[n] = value;}
		G4ThreeVector GetCubeDimensions(G4int n){return CubeDimensions[n];}

		inline void SavePositionsArray(std::vector<G4ThreeVector> value){Positions = value;}
		inline void AddVectorPosition(G4ThreeVector value){Positions.push_back (value);}
		inline void SetVectorPosition(G4int n, G4ThreeVector value){Positions[n] = value;}
		G4ThreeVector GetVectorPosition(G4int n){return Positions[n];}

		inline void SaveRotationsArray(std::vector<G4ThreeVector> value){Rotations = value;}
		inline void AddVectorRotation(G4ThreeVector value){Rotations.push_back (value);}
		inline void SetVectorRotation(G4int n, G4ThreeVector value){Rotations[n] = value;}
		G4ThreeVector GetVectorRotation(G4int n){return Rotations[n];}

		inline void SaveMaterialArray(std::vector<G4String> value){Materials = value;}
		inline void AddMaterial(G4String value){Materials.push_back (value);}
		inline void SetMaterial(G4int n, G4String value){Materials[n] = value;} 
		G4String GetMaterial(G4int n){return Materials[n];}

	private:
		TargetConstructionMessenger *TCMessenger;		

		//Target variables
		std::vector<G4String> TypeOfObjects;
		std::vector<G4ThreeVector> CubeDimensions;
		std::vector<G4ThreeVector> Positions;	
		std::vector<G4ThreeVector> Rotations;
		std::vector<G4String> Materials;	

		G4int NumberOfObjects_Cmd;
		G4double OffSetRadius_Cmd;
		G4ThreeVector Centre_Cmd;
		std::vector<G4ThreeVector> StartingPositions;

		//Data manipulation varaibles
		G4int CurrentImage;
		G4int NoImages;

		G4bool VisualizationValue;
};

#endif	
	
