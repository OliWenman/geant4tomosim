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
		//Functions for creating different shaped objects
		void Box(G4int ObjectNumber, G4LogicalVolume* logicWorld);
		void HollowBox(G4int ObjectNumber, G4LogicalVolume* logicWorld);
		void Cylinder(G4int ObjectNumber, G4LogicalVolume* MotherBox);
		void Sphere(G4int ObjectNumber, G4LogicalVolume* MotherBox);
		void SubtractSolid(G4int ObjectNumber, G4LogicalVolume* MotherBox);

		void AddLogicalVolume(G4String SolidName, G4String Material);
		void AddPhysicalVolume(G4String LogicalName);

		//Functions used through out class
		G4Material* FindMaterial(G4String MaterialName);
		G4double RotateObject();
		G4ThreeVector OffSetRotation(G4int ObjectNumber, G4ThreeVector Centre, G4double Radius, G4double Angle);
		void Visualization(G4LogicalVolume*, G4Colour);

		//Appends to a 2D vector another vector filled with an objects geometry 
		inline void AddDimensions(std::vector<G4double> value){Dimensions.push_back (value);}	

		//Functions to do with an array of objects being used
		inline void SaveTypeOfObects(std::vector<G4String> value){TypeOfObjects = value;}
		inline void AddTypeOfObjects(G4String value){TypeOfObjects.push_back (value);}
		inline void SetTypeOfObjects(G4int n, G4String value){TypeOfObjects[n] = value;}
		G4String GetTypeOfObjects(G4int n){return TypeOfObjects[n];}	

		//Functions to do with an array of the positions of the objects being used
		inline void SavePositionsArray(std::vector<G4ThreeVector> value){Positions = value;}
		inline void AddVectorPosition(G4ThreeVector value){Positions.push_back (value);}
		inline void SetVectorPosition(G4int n, G4ThreeVector value){Positions[n] = value;}
		G4ThreeVector GetVectorPosition(G4int n){return Positions[n];}

		//Functions to do with an array of rotations of the objects being used
		inline void SaveRotationsArray(std::vector<G4ThreeVector> value){Rotations = value;}
		inline void AddVectorRotation(G4ThreeVector value){Rotations.push_back (value);}
		inline void SetVectorRotation(G4int n, G4ThreeVector value){Rotations[n] = value;}
		G4ThreeVector GetVectorRotation(G4int n){return Rotations[n];}

		//Functions to do with an array of materials of the objects being used
		inline void SaveMaterialArray(std::vector<G4String> value){Materials = value;}
		inline void AddMaterial(G4String value){Materials.push_back (value);}
		inline void SetMaterial(G4int n, G4String value){Materials[n] = value;} 
		G4String GetMaterial(G4int n){return Materials[n];}

		//Functions to do with an array of boolean operations for the object being used
		inline void SaveBooleanOpArray(std::vector<bool> value){BooleanOp = value;}
		inline void AddBooleanOp(bool value){BooleanOp.push_back (value);}
		inline void SetBooleanOp(G4int n, bool value){BooleanOp[n] = value;} 
		bool GetBooleanOp(G4int n){return BooleanOp[n];}

		//Functions to do with setting the offset of rotation
		void SetFullRotationAngle(G4double value){FullRotationAngle_Cmd = value;}

		void SetOffSetRadius(G4double value){OffSetRadius_Cmd = value;}
		G4double GetOffSetRadius(){return OffSetRadius_Cmd;}

		void SetCentrePosition(G4ThreeVector value){Centre_Cmd = value;}

		//Get and set the total number of images being used
		void SetNoImages(G4int value){NoImages = value;}
		G4int GetNoImages(){return NoImages;}

		void SetOverlapCheck(G4bool value){OverlapCheck_Cmd = value;}
		G4bool GetOverlapCheck(){return OverlapCheck_Cmd;}
		
		//Get and set the visualization settings
		void SetVisualization(G4bool value){VisualizationValue = value;}

	private:
		//Pointer to its messenger class
		TargetConstructionMessenger *TCMessenger;		

		//Vector of the different types of objects
		std::vector<G4String> TypeOfObjects;

		//2D vector of the dimensions of the different objects
		std::vector<std::vector<G4double> > Dimensions;

		//Vectors of G4ThreeVectors for position and rotation of the objects
		std::vector<G4ThreeVector> Positions;	
		std::vector<G4ThreeVector> Rotations;

		//Vector of the string of the different types of material used in the objects
		std::vector<G4String> Materials;

		//Vector of the boolean operations for the different objects
		std::vector<bool> BooleanOp;	

		//Vector of the starting positions of the objects
		std::vector<G4ThreeVector> StartingPositions;

		//Data variables
		G4double FullRotationAngle_Cmd;
		G4double OffSetRadius_Cmd;
		G4ThreeVector Centre_Cmd;
		G4int nImage;
		G4int NoImages;
		G4bool VisualizationValue;
		G4bool OverlapCheck_Cmd;
};

#endif	
	
