#include "Cube.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4Sphere.hh"
#include "G4Orb.hh"

#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4UserLimits.hh"
#include "G4RunManager.hh"
#include "G4RotationMatrix.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"


Cube::Cube()
{
	G4cout << G4endl << "Cube class has been created" << G4endl;
}

Cube::~Cube()
{
	G4cout << G4endl << "Cube class has been deleted" << G4endl;
}

void Cube::Construct(){}

void Cube::AddSolid(G4ThreeVector BoxDimensions)
{
	G4cout << G4endl << "Box Solid function";

	G4Box* Box = new G4Box("Box0", BoxDimensions.x()/2., BoxDimensions.y()/2., BoxDimensions.z()/2.);
}

void Cube::AddLogicalVolume()
{	
	if (LogicVolume == true)
	{
		G4cout << G4endl << "Box LV function";

		G4VSolid* Solid = G4SolidStore::GetInstance() -> GetSolid(Name, true); 
		G4LogicalVolume* logicCube = new G4LogicalVolume(Solid, FindMaterial(Material), "logic"+Name);
	}
}

void Cube::AddPhysicalVolume()
{
	if (PhysicalVolume == true)
	{
		G4cout << G4endl << "Box Physical function";

		G4LogicalVolume* MotherBoxLV = G4LogicalVolumeStore::GetInstance() -> GetVolume(MotherBox, true);
		G4LogicalVolume* Object = G4LogicalVolumeStore::GetInstance() -> GetVolume("logic"+Name, true);

		//Updates the rotation of the object
		G4RotationMatrix* RotateObjectAngle = new G4RotationMatrix();
		RotateObjectAngle->rotateX(Rotation.x());
		RotateObjectAngle->rotateY(Rotation.y());
		RotateObjectAngle->rotateZ(Rotation.z());

		//Create the target physical volume
		G4VPhysicalVolume* physCube = new G4PVPlacement(RotateObjectAngle,            
							  	Position,    
							 	Object,           //its logical volume
							 	"Box",               //its name
							 	MotherBoxLV,                     //its mother  volume
								false,                 //no boolean operation
							  	0,                     //copy number
							  	false);		//overlaps checking      

		//Visualization attributes
		G4VisAttributes* ObjectColour = new G4VisAttributes(G4Colour::White());	
  		Object -> SetVisAttributes(ObjectColour);
	}
}

G4Material* Cube::FindMaterial(G4String MaterialName)
{
	//Obtain pointer to NIST material manager and return the material  
	return G4NistManager::Instance() -> FindOrBuildMaterial(MaterialName);
}
