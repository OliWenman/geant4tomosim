#include "Data.hh"
#include "TargetConstruction.hh"
#include "TargetConstructionMessenger.hh"

#include <math.h>	//Needed for sin and cos
#include <algorithm>    // std::replace

#include "G4NistManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"

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

TargetConstruction::TargetConstruction()
{	G4cout << G4endl << "TargetConstruction has been created" << G4endl;
	
	TCMessenger = new TargetConstructionMessenger(this);
}

TargetConstruction::~TargetConstruction()
{
	G4cout << G4endl << "TargetConstruction has been deleted";
	delete TCMessenger;
}

void TargetConstruction::Construct(G4LogicalVolume* World)
{
	if (GetCurrentImage() == 0)
	{
		std::vector<G4ThreeVector> iPositions(GetNumberOfObjects());
		SavePositionsArray(iPositions);
	}
	
	//G4ThreeVector ObjectSize0 = G4ThreeVector(0.3*mm, 0.3*mm, 0.3*mm);
	G4ThreeVector ObjectSize0 = G4ThreeVector(0.3*mm, 0.3*mm, 0.3*mm);
	G4ThreeVector ObjectPosition0 = G4ThreeVector(0.0*mm, 0.0*mm, 0.0*mm);

	//G4ThreeVector ObjectSize1 = G4ThreeVector(0.3*mm, 0.6*mm, 0.4*mm);
	//G4ThreeVector ObjectPosition1 = G4ThreeVector(0.2*mm, 0.5*mm, 0.0*mm);	

	G4String ObjectMaterial = GetTargetMaterial();

	Box(0, ObjectSize0, ObjectPosition0, ObjectMaterial, World);
	//Box(1, ObjectSize1, ObjectPosition1, ObjectMaterial, World);
}

void TargetConstruction::Box(G4int ObjectNumber, G4ThreeVector TargetSize, G4ThreeVector TargetPosition, G4String Material, G4LogicalVolume* MotherBox)
{
	SetVectorPosition(ObjectNumber, TargetPosition);

	G4Box* Box = new G4Box("Box", TargetSize.x()/2., TargetSize.y()/2., TargetSize.z()/2.);

	//Fill the target with its material
	G4Material* BoxMaterial = FindMaterial(Material);
	G4LogicalVolume* logicBox = new G4LogicalVolume(Box, BoxMaterial, "logicBox");
	
	G4double DeltaAngle = RotateObject();
	G4RotationMatrix* RotateObjectAngle = new G4RotationMatrix();
	RotateObjectAngle->rotateX(90.*deg);
	RotateObjectAngle->rotateY(0.*deg);
	RotateObjectAngle->rotateZ(DeltaAngle);

	G4ThreeVector NewTargetPosition = OffSetRotation(ObjectNumber, TargetPosition, OffSetRadius_Cmd, DeltaAngle);

	//Create the target physical volume
	G4VPhysicalVolume* physBox = new G4PVPlacement(RotateObjectAngle,            
							     NewTargetPosition,    
							     logicBox,           //its logical volume
							     "Box",               //its name
							     MotherBox,                     //its mother  volume
							     false,                 //no boolean operation
							     ObjectNumber,                     //copy number
							     false);		//overlaps checking      

	//Visualization attributes
	Visualization(logicBox, G4Colour::White());

	G4cout << G4endl << "The target has been created succesfully " << G4endl;  	

}

G4Material* TargetConstruction::FindMaterial(G4String MaterialName)
{
	//Obtain pointer to NIST material manager
	//Build materials 
	return G4NistManager::Instance() -> FindOrBuildMaterial(MaterialName);;
}

G4double TargetConstruction::RotateObject()
{
	if (GetNoImages() > 1)
	{	
		G4GeometryManager::GetInstance()->OpenGeometry();
		G4double deltaTheta = 180*deg/(GetNoImages());
		
		G4GeometryManager::GetInstance()->CloseGeometry();
		G4RunManager::GetRunManager()->GeometryHasBeenModified();

		return deltaTheta * GetCurrentImage();
	}
	else 
		{return 0.*deg;}
}

G4ThreeVector TargetConstruction::OffSetRotation(G4int ObjectNumber, G4ThreeVector CurrentPosition, G4double Radius, G4double Angle)
{
	G4double intialY = GetVectorPosition(ObjectNumber).y();

	//If the object number is 0 (master), it rotates around centre with radius
	if (ObjectNumber == 0)
	{
		if (GetNoImages() > 1 && Radius != 0)
		{	
			G4double NewX = (CurrentPosition.x()+Radius)*cos(Angle);
			G4double NewZ = (CurrentPosition.z()+Radius)*sin(Angle);

			SetVectorPosition(ObjectNumber, G4ThreeVector(NewX,intialY,NewZ));

			return G4ThreeVector(NewX, intialY ,NewZ);
		}
		else 
			{return GetVectorPosition(ObjectNumber);}
	}
	//If object number is greater than 0, it will rotate around the master volume(0)
	else 
	{
		G4double NewX = GetVectorPosition(0).x() + (cos(Angle) * CurrentPosition.x()) + (-sin(Angle) * CurrentPosition.z());
		G4double NewZ = GetVectorPosition(0).z() + (sin(Angle) * CurrentPosition.x()) + (cos(Angle) * CurrentPosition.z());
 
		SetVectorPosition(ObjectNumber, G4ThreeVector(NewX,intialY,NewZ));

		return G4ThreeVector(NewX, intialY ,NewZ);
	}
}

void TargetConstruction::Visualization(G4LogicalVolume* LV, G4Colour Colour)
{
	if (GetVisualization() == true)
	{
		G4VisAttributes* ObjectColour = new G4VisAttributes(G4Colour(Colour));	
  		LV -> SetVisAttributes(ObjectColour);
	}
}



