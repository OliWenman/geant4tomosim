#ifndef TargetConstruction_h
#define TargetConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include <vector>
#include "G4ThreeVector.hh"

//My own classes 
class TargetConstructionMessenger;
class Data;

//Solids, logic volume and physical volume for the geometry
class GVSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;

//Materials and colour classes
class G4Material;
class G4Colour;

class TargetConstruction
{
	public:
		TargetConstruction();
		~TargetConstruction();

		//Base class method
		void Construct(G4LogicalVolume *logicWorld);

		//Appends to a 2D vector another vector filled with an objects geometry 
		inline void AddDimensions(std::vector<G4double> value){Dimensions.push_back (value);}	

		//Functions to do with an properities of objects being used
		inline void AddTypeOfObjects(G4String value){TypeOfObjects.push_back (value);}
		inline void SetTypeOfObjects(G4int n, G4String value){TypeOfObjects[n] = value;}

		inline void AddLogicVolumeArray(bool value){LogicVolumeArray.push_back (value);}
		inline void SetLogicVolumeArray(int n, bool value){LogicVolumeArray[n] = value;}

		inline void AddVectorPosition(G4ThreeVector value){Positions.push_back (value);}
		inline void SetVectorPosition(G4int n, G4ThreeVector value){Positions[n] = value;}

		inline void AddVectorRotation(G4ThreeVector value){Rotations.push_back (value);}
		inline void SetVectorRotation(G4int n, G4ThreeVector value){Rotations[n] = value;}

		inline void AddMaterial(G4String value){Materials.push_back (value);}
		inline void SetMaterial(G4int n, G4String value){Materials[n] = value;} 

		inline void AddBooleanOp(bool value){BooleanOp.push_back (value);}
		inline void SetBooleanOp(G4int n, bool value){BooleanOp[n] = value;} 

		inline void AddSubtractObject(G4String value){SubtractObject.push_back (value);}
		inline void SetSubtractObject(G4int n, G4String value){SubtractObject[n] = value;}

		//Functions to do with setting the offset of rotation
		void SetFullRotationAngle(G4double value){FullRotationAngle_Cmd = value;}

		void SetOffSetRadius(G4double value){OffSetRadius_Cmd = value;}
		G4double GetOffSetRadius(){return OffSetRadius_Cmd;}

		void SetCentrePosition(G4ThreeVector value){Centre_Cmd = value;}

		//Get and set the total number of images being used
		void SetTotalImages(G4int value){TotalImages = value;}
		G4int GetTotalImages(){return TotalImages;}

		void SetOverlapCheck(G4bool value){OverlapCheck_Cmd = value;}
		G4bool GetOverlapCheck(){return OverlapCheck_Cmd;}
		
		//Get and set the visualization settings
		void SetVisualization(G4bool value){VisualizationValue = value;}
		void ReadOutInfo();

	private:

		//Functions for creating different shaped objects
		void Box(G4int ObjectNumber, G4String StringNumber);
		void HollowBox(G4int ObjectNumber, G4String StringNumber);
		void Cylinder(G4int ObjectNumber, G4String StringNumber);
		void Sphere(G4int ObjectNumber, G4String StringNumber);
		void Trapezoid(G4int ObjectNumber, G4String StringNumber);
		void Ellipsoid(G4int ObjectNumber, G4String StringNumber);

		//Complex objects
		void SubtractSolid(G4int ObjectNumber, G4String StringNumber);

		//Add its material and place it in the world
		void AddLogicalVolume(G4int ObjectNumber, G4String, G4String Material);
		void AddPhysicalVolume(int, G4String Name, G4LogicalVolume* MotherBox);

		//Functions used through out class
		G4Material* FindMaterial(G4String MaterialName);
		void DefineMaterial();
		G4double RotateObject(){return (FullRotationAngle_Cmd/TotalImages)*nImage;}
		G4ThreeVector OffSetRotation(G4int ObjectNumber, G4ThreeVector Centre, G4double Radius, G4double Angle);
		void Visualization(G4LogicalVolume*, G4Colour);

		//Pointer to its messenger class
		TargetConstructionMessenger *TCMessenger;		

		//Vector of the different types of objects
		std::vector<G4String> TypeOfObjects;

		//Properties to do with the solid
		std::vector<std::vector<G4double> > Dimensions;
		std::vector<G4String> SubtractObject;
		G4int SubtractSolidCounter;

		//Properties to do with the logic volume
		std::vector<bool> LogicVolumeArray;
		std::vector<G4String> Materials;
		G4bool VisualizationValue;

		//Properties to do with the physical volume
		std::vector<G4ThreeVector> StartingPositions;
		std::vector<G4ThreeVector> Positions;	
		std::vector<G4ThreeVector> Rotations;
		std::vector<bool> BooleanOp;	
		G4bool OverlapCheck_Cmd;
		G4int MasterVolume;
		G4bool MasterCheck;

		//Commands to do with the rotation of the object between images
		G4double FullRotationAngle_Cmd;
		G4double OffSetRadius_Cmd;
		G4ThreeVector Centre_Cmd;

		//Image counter and total number of images
		G4int nImage;
		G4int TotalImages;
};

#endif	
	
