#include "Data.hh"
#include "TargetConstruction.hh"
#include "TargetConstructionMessenger.hh"

#include <math.h>	//Needed for sin and cos
#include <algorithm>    // std::replace

#include "G4NistManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4VSolid.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4Sphere.hh"
#include "G4Orb.hh"

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
{	
	G4cout << G4endl << "TargetConstruction has been created" << G4endl;
	
	TCMessenger = new TargetConstructionMessenger(this);
	nImage = 0;
	SubtractSolidCounter = 0;
}

TargetConstruction::~TargetConstruction()
{
	G4cout << G4endl << "TargetConstruction has been deleted";
	delete TCMessenger;
}

void TargetConstruction::Construct(G4LogicalVolume* World)
{
	if (nImage == 0)
		{StartingPositions = Positions;}

	for(G4int nObject = 0; nObject < TypeOfObjects.size(); nObject++)
	{
		G4String StringNumber = std::to_string(nObject);
		if(TypeOfObjects[nObject] == "Cube")
		{	Box(nObject, World);
			AddLogicalVolume(nObject,"Cube"+StringNumber, Materials[nObject]);
			AddPhysicalVolume(nObject,"Cube"+StringNumber, World);
		}
		else if(TypeOfObjects[nObject] == "HollowCube")
		{	HollowBox(nObject, World);
			AddLogicalVolume(nObject,"HollowCube"+StringNumber, Materials[nObject]);
			AddPhysicalVolume(nObject,"HollowCube"+StringNumber, World);
		}

		else if(TypeOfObjects[nObject] == "Sphere")
		{	Sphere(nObject, World);
			AddLogicalVolume(nObject,"Sphere"+StringNumber, Materials[nObject]);
			AddPhysicalVolume(nObject,"Sphere"+StringNumber, World);
		}
		else if(TypeOfObjects[nObject] == "Cylinder")
		{	Cylinder(nObject, World);
			AddLogicalVolume(nObject,"Cylinder"+StringNumber, Materials[nObject]);
			AddPhysicalVolume(nObject,"Cylinder"+StringNumber, World);
		} 
		else if(TypeOfObjects[nObject] == "SubtractSolid")
		{	SubtractSolid(nObject, World);
			AddLogicalVolume(nObject,"SubtractSolid"+StringNumber, Materials[nObject]);
			AddPhysicalVolume(nObject,"SubtractSolid"+StringNumber, World);
		}
	} 

	++nImage;

	G4cout << G4endl << "The target has been created succesfully " << G4endl;  	
}

void TargetConstruction::Box(G4int ObjectNumber, G4LogicalVolume* MotherBox)
{
	//Convert the ObjectNumber to a string
	G4String StringNumber = std::to_string(ObjectNumber);

	//Get the dimensiosn for the box
	G4ThreeVector TargetSize = G4ThreeVector(Dimensions[ObjectNumber][0], 
						 Dimensions[ObjectNumber][1],
						 Dimensions[ObjectNumber][2]);

	//Create a G4VSolid of the box
	G4Box* Box = new G4Box("Cube" + StringNumber, TargetSize.x()/2., TargetSize.y()/2., TargetSize.z()/2.);
}

void TargetConstruction::HollowBox(G4int ObjectNumber, G4LogicalVolume* MotherBox)
{
	//Convert the ObjectNumber to a string
	G4String StringNumber = std::to_string(ObjectNumber);

	//Get the dimensions for the outerBox
	G4ThreeVector outerBoxDimensions = G4ThreeVector(Dimensions[ObjectNumber][0], 
					       		 Dimensions[ObjectNumber][1],
					       		 Dimensions[ObjectNumber][2]);

	//Get the dimensions for the innerBox
	G4ThreeVector innerBoxDimensions = G4ThreeVector(Dimensions[ObjectNumber][3], 
					       		 Dimensions[ObjectNumber][4],
					       		 Dimensions[ObjectNumber][5]);

	//Create G4VSolids of the two boxes
	G4Box *outerBox = new G4Box("OuterBox" + StringNumber, 
				    outerBoxDimensions.x(), 
				    outerBoxDimensions.y(), 
				    outerBoxDimensions.z());

	G4Box *innerBox = new G4Box("InnerBox" + StringNumber,
				    outerBoxDimensions.x()-innerBoxDimensions.x(),
				    outerBoxDimensions.y()-innerBoxDimensions.y(),
				    outerBoxDimensions.z()-innerBoxDimensions.z());

	//Subtract them from each other to get a hollow cube
	G4SubtractionSolid *HollowBox = new G4SubtractionSolid("HollowCube" + StringNumber, outerBox, innerBox);
}

void TargetConstruction::Cylinder(G4int ObjectNumber, G4LogicalVolume* MotherBox)
{
	//Convert the ObjectNumber to a string
	G4String StringNumber = std::to_string(ObjectNumber);

	//Get the dimensions of the cylinder
	G4double innerRadius = Dimensions[ObjectNumber][0];
	G4double outerRadius = Dimensions[ObjectNumber][1];
	G4double hz = Dimensions[ObjectNumber][2];
	G4double startAngle = Dimensions[ObjectNumber][3];
	G4double spanningAngle = Dimensions[ObjectNumber][4];
		
	//Create a G4VSolid of the cylinder
   	G4Tubs* TargetTube = new G4Tubs("Cylinder" + StringNumber ,
				 	innerRadius,
                  		 	outerRadius,
                  		 	hz,
                  		 	startAngle,
                  		 	spanningAngle);
}

void TargetConstruction::Sphere(G4int ObjectNumber, G4LogicalVolume* MotherBox)
{
	//Convert the ObjectNumber to a string
	G4String StringNumber = std::to_string(ObjectNumber);

	//Get the dimensions of the sphere
	G4double innerRadius = Dimensions[ObjectNumber][0];
	G4double outerRadius = Dimensions[ObjectNumber][1];
	G4double startingPhi = Dimensions[ObjectNumber][2];
	G4double endPhi = Dimensions[ObjectNumber][3];
	G4double startingTheta = Dimensions[ObjectNumber][4];
	G4double endTheta = Dimensions[ObjectNumber][5]; 

	//Create a G4VSolid of the sphere
	G4Sphere* Sphere = new G4Sphere("Sphere" + StringNumber, 
				     	innerRadius,
				     	outerRadius,
				     	startingPhi,
				     	endPhi,
				     	startingTheta,
				     	endTheta);
}

void TargetConstruction::SubtractSolid(G4int ObjectNumber, G4LogicalVolume* MotherBox)
{
	G4String StringNumber = std::to_string(ObjectNumber);

	G4Tokenizer next(SubtractObject[SubtractObject.size()-1]);

	G4String Object1 = next();
	G4String Object2 = next();

	//G4ThreeVector InnerPosition = G4ThreeVector(next(),next(),next());

	G4VSolid* Solid1 = G4SolidStore::GetInstance() -> GetSolid(Object1, true); 
	G4VSolid* Solid2 = G4SolidStore::GetInstance() -> GetSolid(Object2, true); 

	G4SubtractionSolid *NewSolid = new G4SubtractionSolid("SubtractSolid" + StringNumber, Solid1, Solid2, 0, G4ThreeVector(0,0,0));
}

void TargetConstruction::AddLogicalVolume(G4int ObjectNumber, G4String SolidName, G4String Material)
{
	//If its logic volume value is true, add its logic volume
	if (LogicVolumeArray[ObjectNumber] == true)
	{
		//Finds the right G4VSolid
		G4VSolid* Solid = G4SolidStore::GetInstance() -> GetSolid(SolidName, true); 
		G4LogicalVolume* logicObject = new G4LogicalVolume(Solid, FindMaterial(Material), "LV" + SolidName);
	}
}

void TargetConstruction::AddPhysicalVolume(G4int ObjectNumber, G4String Name, G4LogicalVolume* MotherBox)
{
	//If its logic volume value is true, add its physical volume
	if (LogicVolumeArray[ObjectNumber] == true)
	{
		//Finds the right logic volume
		G4LogicalVolume* Object = G4LogicalVolumeStore::GetInstance() -> GetVolume("LV"+Name, true);

		//Get its starting rotation angle
		G4ThreeVector StartingRotation = Rotations[ObjectNumber];
		
		//Calculate how much the object rotates between each image
		G4double DeltaAngle = RotateObject();

		//Updates the rotation of the object
		G4RotationMatrix* RotateObjectAngle = new G4RotationMatrix();
		RotateObjectAngle->rotateX(StartingRotation.x());
		RotateObjectAngle->rotateY(DeltaAngle + StartingRotation.y());
		RotateObjectAngle->rotateZ(StartingRotation.z());

		//Offsets the rotation
		G4ThreeVector NewTargetPosition = OffSetRotation(ObjectNumber, Positions[ObjectNumber], OffSetRadius_Cmd, DeltaAngle);

		//Create the target physical volume
		G4VPhysicalVolume* physObject = new G4PVPlacement(RotateObjectAngle,            
							 	 NewTargetPosition,    
							  	 Object,           //its logical volume
							  	 "phy"+Name,               //its name
							  	 MotherBox,                     //its mother  volume
							 	 false,                 //no boolean operation
							 	 ObjectNumber,                     //copy number
							 	 OverlapCheck_Cmd);		//overlaps checking      

		//Visualization attributes
		Visualization(Object, G4Colour::White());

		//Let the runManager know the geometry has changed between runs
		G4RunManager::GetRunManager()->GeometryHasBeenModified();
	}
}

G4Material* TargetConstruction::FindMaterial(G4String MaterialName)
{
	//Obtain pointer to NIST material manager to find the build materials 
	return G4NistManager::Instance() -> FindOrBuildMaterial(MaterialName);
}

G4double TargetConstruction::RotateObject()
	{return (FullRotationAngle_Cmd/NoImages)*nImage;}

G4ThreeVector TargetConstruction::OffSetRotation(G4int ObjectNumber, G4ThreeVector CurrentPosition, G4double Radius, G4double Angle)
{
	G4ThreeVector Start = StartingPositions[0];
	G4double intialY = Start.y();

	//If the object number is 0 (master), it rotates around centre with radius
	if (ObjectNumber == 0)
	{
		if (NoImages > 1 && Radius != 0)
		{
			G4double NewX = Centre_Cmd.x() + cos(Angle)*Radius;
			G4double NewZ = Centre_Cmd.z() + sin(Angle)*Radius;

			SetVectorPosition(ObjectNumber, G4ThreeVector(NewX,Centre_Cmd.y()+intialY,NewZ));

			return G4ThreeVector(NewX, Centre_Cmd.y()+intialY ,NewZ);
		}
		else 
			{return Positions[ObjectNumber];}
	}
	//If object number is greater than 0, it will rotate around the master volume(0)
	else 
	{
		G4double NewX = Positions[0].x() + (cos(Angle) * CurrentPosition.x()) + (-sin(Angle) * CurrentPosition.z());
		G4double NewZ = Positions[0].z() + (sin(Angle) * CurrentPosition.x()) + (cos(Angle) * CurrentPosition.z());
 
		//SetVectorPosition(ObjectNumber, G4ThreeVector(NewX,intialY,NewZ));

		return G4ThreeVector(NewX, StartingPositions[ObjectNumber].y() ,NewZ);
	}
}

void TargetConstruction::Visualization(G4LogicalVolume* LV, G4Colour Colour)
{
	if (VisualizationValue == true)
	{
		G4VisAttributes* ObjectColour = new G4VisAttributes(G4Colour(Colour));	
  		LV -> SetVisAttributes(ObjectColour);
	}
}

