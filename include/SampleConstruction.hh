#ifndef TargetConstruction_h
#define TargetConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include <vector>
#include "G4ThreeVector.hh"
#include "CustomObject.hh"

//My own classes 
class SampleConstructionMessenger;
class Data;

//Solids, logic volume and physical volume for the geometry
class GVSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;

//Materials and colour classes
class G4Material;
class G4Colour;

class SampleConstruction
{
	public:
		SampleConstruction();
		~SampleConstruction();

		//Create the physical volumes
		void Box       (std::string name, std::vector<double> dimensions);
		void Sphere    (std::string name, std::vector<double> dimensions);
		void Cylinder  (std::string name, std::vector<double> dimensions);
		void Trapezoid (std::string name, std::vector<double> dimensions);
		void Ellipsoid (std::string name, std::vector<double> dimensions);

		void SubtractSolid(ComplexObject &complexObject);
		void UnionSolid(ComplexObject &complexObject);
		
    private:
        std::vector<G4PVPlacement*> samples;
        
        //Positioning functions
        G4double RotateObject(){return (FullRotationAngle_Cmd/TotalImages)*nImage;}
        G4double ApplyRotation(int image){return rotation_angles[image];}
		G4ThreeVector OffSetRotation2(CustomObject &object, G4double deltaAngle);

		//Functions for finding materials and adding visualization
		G4Material* FindMaterial(G4String MaterialName, G4String ObjectName);
		void Visualization(G4LogicalVolume*, G4Colour);
		
		//Pointer to its messenger class
		TargetConstructionMessenger *TCMessenger;		
        
};
