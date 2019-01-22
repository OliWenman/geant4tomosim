#ifndef TargetConstruction_h
#define TargetConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include <vector>
#include "G4ThreeVector.hh"
#include "CustomObject.hh"

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
		
		//Set functions to do with the mode of simulation
		void SetSimMode(G4String value){SimMode = value; nImage = 0;}
		void SetCalibrationImages(int value){CalibrationImages = value;}

		//Set functions to do with setting the offset of rotation
		void SetFullRotationAngle(G4double value){FullRotationAngle_Cmd = value;}
		void SetOffSetRadius(G4double value){OffSetRadius_Cmd = value;}
		void SetCentrePosition(G4ThreeVector value){Centre_Cmd = value;}

		//Other set functions
		void SetTotalImages(G4int value){TotalImages = value;}
		void SetOverlapCheck(G4bool value){OverlapCheck_Cmd = value;}
		void SetVisualization(G4bool value){VisualizationValue = value;}
		
		//Get functions
		int GetCalibrationImages(){return CalibrationImages;}
		G4int GetTotalImages(){return TotalImages;}
		G4double GetOffSetRadius(){return OffSetRadius_Cmd;}
		G4bool GetOverlapCheck(){return OverlapCheck_Cmd;}
		
		std::vector<CustomObject> GetObjectDatabase(){return ObjectDatabase;}
		
		//Create objects from TCMessenger
		void CreateObject(G4String Name, G4String Type, std::vector<double> dimensions);
		void CreateObject(G4String Name, G4String ComponentName1, G4String ComponentName2, G4String Type, G4ThreeVector innerPosition, G4ThreeVector innerRotation);
		
		//Add attributes to those objects
		void AddMaterial(G4String Name, G4String Material);
		void AddPhysicalVolume(CustomObject &object, G4LogicalVolume* MotherBox);
		void AddPosition(G4String Name, G4ThreeVector Position);
		void AddRotation(G4String Name, G4ThreeVector Rotation);
		
		void AddColour(G4String Name, G4String Material);
		
		//Info
		void ReadOutInfo();

	private:

		//Functions for creating different shaped objects
		void Box(G4String Name, std::vector<double> dimensions);
		void Sphere(G4String Name, std::vector<double> dimensions);
		void Cylinder(G4String Name, std::vector<double> dimensions);
		void Trapezoid(G4String Name, std::vector<double> dimensions);
		void Ellipsoid(G4String Name, std::vector<double> dimensions);
		//Complex objects
		void SubtractSolid(ComplexObject &complexObject);
		void UnionSolid(ComplexObject &complexObject);

        //Positioning functions
        G4double RotateObject(){return (FullRotationAngle_Cmd/TotalImages)*nImage;}
		G4ThreeVector OffSetRotation2(CustomObject &object, G4double deltaAngle);

		//Functions for finding materials and adding visualization
		G4Material* FindMaterial(G4String MaterialName, G4String ObjectName);
		void Visualization(G4LogicalVolume*, G4Colour);
		
		//Pointer to its messenger class
		TargetConstructionMessenger *TCMessenger;		

        //Check variables
        G4String SimMode;
		G4bool VisualizationValue;
		G4bool OverlapCheck_Cmd;
		
		G4int MasterVolume;
		G4bool MasterCheck;

		//Commands to do with the rotation of the object between images
		G4double FullRotationAngle_Cmd;
		G4double OffSetRadius_Cmd;
		G4ThreeVector Centre_Cmd;

		//Image variables,  counter and total number of images
		G4int nImage;
		G4int TotalImages;
		int CalibrationImages;
		
		//Database of objects
		std::vector<CustomObject> ObjectDatabase;
};

#endif	
	
