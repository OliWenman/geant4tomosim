#ifndef Cube_h
#define Cube_h 1

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

class Cube
{
	public:
		Cube();
		~Cube();
	
		void Construct();
		
		void AddSolid(G4ThreeVector BoxDimensions);
		void AddLogicalVolume();
		void AddPhysicalVolume();	

		G4Material* FindMaterial(G4String Material);

		void SetDimensions(G4ThreeVector value){Dimensions = value;}
		void SetPosition(G4ThreeVector value){Position = value;}
		void SetRotation(G4ThreeVector value){Rotation = value;}
		void SetMaterial(G4String value){Material = value;}
		void SetName(G4String value){Name = value;}
	
	private:

		G4ThreeVector Dimensions;
		G4ThreeVector Position;
		G4ThreeVector Rotation;
		G4String Material;

		G4String Name;
		G4String MotherBox;

		bool LogicVolume;
		bool PhysicalVolume;
};

#endif

	
