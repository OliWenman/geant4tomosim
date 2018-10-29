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
		if(TypeOfObjects[nObject] == "Cube")
			{Box(nObject, World);}

		else if(TypeOfObjects[nObject] == "HollowCube")
			{HollowBox(nObject, World);}

		else if(TypeOfObjects[nObject] == "Sphere")
			{Sphere(nObject, World);}

		else if(TypeOfObjects[nObject] == "Cylinder")
			{Cylinder(nObject, World);} 
	} 

	++nImage;

	//Sphere(G4int ObjectNumber, G4double Radius, G4ThreeVector TargetPosition, G4String Material, G4LogicalVolume* MotherBox)

	/*
	G4Box* Box = new G4Box("Centre", 0.01*mm, 1*mm, 0.01*mm);

	//Fill the target with its material
	G4Material* BoxMaterial = FindMaterial("G4_AIR");
	G4LogicalVolume* logicBox = new G4LogicalVolume(Box, BoxMaterial, "logicCentre");

	//Create the target physical volume
	G4VPhysicalVolume* physBox = new G4PVPlacement(0,            
							     Centre_Cmd,    
							     logicBox,           //its logical volume
							     "Centre",               //its name
							     World,                     //its mother  volume
							     false,                 //no boolean operation
							     0,                     //copy number
							     false);		//overlaps checking    

	Visualization(logicBox, G4Colour::Red());  
	
	//Target geometry
	G4double innerRadius = 0*cm;
	G4double outerRadius = OffSetRadius_Cmd;
	G4double hz = 1*mm;
	G4double startAngle = 0.*deg;
	G4double spanningAngle = 360.*deg;

	G4RotationMatrix* RotateObjectAngle = new G4RotationMatrix();
	RotateObjectAngle->rotateX(90.*deg);
	RotateObjectAngle->rotateY(0.);
	RotateObjectAngle->rotateZ(0);
		
	//TARGET
   	G4Tubs* TargetTube = new G4Tubs("TargetGeometry",
				 	innerRadius,
                  		 	outerRadius,
                  		 	hz,
                  		 	startAngle,
                  		 	spanningAngle);

	//Fill the target with its material
	G4Material* TargetMaterial = FindMaterial("G4_AIR");
	G4LogicalVolume* logicTarget = new G4LogicalVolume(TargetTube, TargetMaterial, "Al_Target");
	//Create the target physical volume
	G4VPhysicalVolume* physTarget= new G4PVPlacement(RotateObjectAngle,            //no rotation
							 Centre_Cmd,       
							 logicTarget,           //its logical volume
							 "Target",               //its name
							 World,                     //its mother  volume
							 false,                 //no boolean operation
							 0,                     //copy number
							 true);		//overlaps checking 
     
	//Visualization attributes
  	G4VisAttributes* Target_Colour = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));	//White
  	Visualization(logicTarget, G4Colour::Blue());  */

	G4cout << G4endl << "The target has been created succesfully " << G4endl;  	
}

void TargetConstruction::Box(G4int ObjectNumber, G4LogicalVolume* MotherBox)
{
	//SetVectorPosition(ObjectNumber, TargetPosition);

	G4ThreeVector TargetSize = G4ThreeVector(Dimensions[ObjectNumber][0], 
						 Dimensions[ObjectNumber][1],
						 Dimensions[ObjectNumber][2]);

	G4String StringNumber = std::to_string(ObjectNumber);

	G4Box* Box = new G4Box("Box" + StringNumber, TargetSize.x()/2., TargetSize.y()/2., TargetSize.z()/2.);

	//Fill the target with its material
	G4Material* BoxMaterial = FindMaterial(Materials[ObjectNumber]);
	G4LogicalVolume* logicBox = new G4LogicalVolume(Box, BoxMaterial, "logicBox" + StringNumber);
	
	G4ThreeVector StartingRotation = GetVectorRotation(ObjectNumber);
	G4double DeltaAngle = RotateObject();

	G4RotationMatrix* RotateObjectAngle = new G4RotationMatrix();
	RotateObjectAngle->rotateX(90.*deg + StartingRotation.x());
	RotateObjectAngle->rotateY(0.*deg + StartingRotation.y());
	RotateObjectAngle->rotateZ(DeltaAngle + StartingRotation.z());
	//RotateObjectAngle->rotateZ(45*deg);
	
	G4ThreeVector NewTargetPosition = OffSetRotation(ObjectNumber, Positions[ObjectNumber], OffSetRadius_Cmd, DeltaAngle);

	//Create the target physical volume
	G4VPhysicalVolume* physBox = new G4PVPlacement(RotateObjectAngle,            
						       NewTargetPosition,    
						       logicBox,           //its logical volume
						       "phyBox" + StringNumber,               //its name
						       MotherBox,                     //its mother  volume
						       false,                 //no boolean operation
						       ObjectNumber,                     //copy number
						       OverlapCheck_Cmd);		//overlaps checking      

	//Visualization attributes
	Visualization(logicBox, G4Colour::White());
}

void TargetConstruction::HollowBox(G4int ObjectNumber, G4LogicalVolume* MotherBox)
{
	G4String StringNumber = std::to_string(ObjectNumber);

	G4ThreeVector outerBoxDimensions = G4ThreeVector(Dimensions[ObjectNumber][0], 
					       		 Dimensions[ObjectNumber][1],
					       		 Dimensions[ObjectNumber][2]);

	G4ThreeVector innerBoxDimensions = G4ThreeVector(Dimensions[ObjectNumber][3], 
					       		 Dimensions[ObjectNumber][4],
					       		 Dimensions[ObjectNumber][5]);

	G4Box *outerBox = new G4Box("Outer Box" + StringNumber, 
				    outerBoxDimensions.x(), 
				    outerBoxDimensions.y(), 
				    outerBoxDimensions.z());

	G4Box *innerBox = new G4Box("Inner Box" + StringNumber,
				    outerBoxDimensions.x()-innerBoxDimensions.x(),
				    outerBoxDimensions.y()-innerBoxDimensions.y(),
				    outerBoxDimensions.z()-innerBoxDimensions.z());

	G4SubtractionSolid *HollowBox = new G4SubtractionSolid("Hollow Box" + StringNumber, outerBox, innerBox);

	//Fill the target with its material
	G4Material* BoxMaterial = FindMaterial(Materials[ObjectNumber]);
	G4LogicalVolume* logicHollowBox = new G4LogicalVolume(HollowBox, BoxMaterial, "logic_HollowBox" + StringNumber);
	
	G4ThreeVector StartingRotation = GetVectorRotation(ObjectNumber);
	G4double DeltaAngle = RotateObject();

	G4RotationMatrix* RotateObjectAngle = new G4RotationMatrix();
	RotateObjectAngle->rotateX(90.*deg + StartingRotation.x());
	RotateObjectAngle->rotateY(0.*deg + StartingRotation.y());
	RotateObjectAngle->rotateZ(DeltaAngle + StartingRotation.z());

	G4ThreeVector NewTargetPosition = OffSetRotation(ObjectNumber, Positions[ObjectNumber], OffSetRadius_Cmd, DeltaAngle);

	//Create the target physical volume
	G4VPhysicalVolume* physHollowBox = new G4PVPlacement(RotateObjectAngle,            
							     NewTargetPosition,    
							     logicHollowBox,           //its logical volume
							     "phyHollowBox" + StringNumber,               //its name
							     MotherBox,                     //its mother  volume
							     false,                 //no boolean operation
							     0,                     //copy number
							     OverlapCheck_Cmd);		//overlaps checking      
	//Visualization attributes
  	G4VisAttributes* HollowBoxColour = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));	//White
  	logicHollowBox -> SetVisAttributes(HollowBoxColour);
}

void TargetConstruction::Cylinder(G4int ObjectNumber, G4LogicalVolume* MotherBox)
{
	G4String StringNumber = std::to_string(ObjectNumber);

	//Target geometry
	G4double innerRadius = Dimensions[ObjectNumber][0];
	G4double outerRadius = Dimensions[ObjectNumber][1];
	G4double hz = Dimensions[ObjectNumber][2];
	G4double startAngle = Dimensions[ObjectNumber][3];
	G4double spanningAngle = Dimensions[ObjectNumber][4];
		
	//TARGET
   	G4Tubs* TargetTube = new G4Tubs("Cylinder" + StringNumber ,
				 	innerRadius,
                  		 	outerRadius,
                  		 	hz,
                  		 	startAngle,
                  		 	spanningAngle);

	//Fill the target with its material
	G4Material* TargetMaterial = FindMaterial("G4_AIR");
	G4LogicalVolume* logicTarget = new G4LogicalVolume(TargetTube, TargetMaterial, "logicCylinder" + StringNumber);

	G4ThreeVector StartingRotation = GetVectorRotation(ObjectNumber);
	G4double DeltaAngle = RotateObject();

	G4RotationMatrix* RotateObjectAngle = new G4RotationMatrix();
	RotateObjectAngle->rotateX(90.*deg + StartingRotation.x());
	RotateObjectAngle->rotateY(0.*deg + StartingRotation.y());
	RotateObjectAngle->rotateZ(DeltaAngle + StartingRotation.z());
	
	G4ThreeVector NewTargetPosition = OffSetRotation(ObjectNumber, Positions[ObjectNumber], OffSetRadius_Cmd, DeltaAngle);

	//Create the target physical volume
	G4VPhysicalVolume* physTarget= new G4PVPlacement(RotateObjectAngle,            //no rotation
							 NewTargetPosition,       
							 logicTarget,           //its logical volume
							 "phyCyliner" + StringNumber,               //its name
							 MotherBox,                     //its mother  volume
							 false,                 //no boolean operation
							 0,                     //copy number
							 OverlapCheck_Cmd);		//overlaps checking 
     
	//Visualization attributes
  	G4VisAttributes* Target_Colour = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));	//White
  	Visualization(logicTarget, G4Colour::White());  
}

void TargetConstruction::Sphere(G4int ObjectNumber, G4LogicalVolume* MotherBox)
{
	G4String StringNumber = std::to_string(ObjectNumber);

	G4double innerRadius = Dimensions[ObjectNumber][0];
	G4double outerRadius = Dimensions[ObjectNumber][1];
	G4double startingPhi = Dimensions[ObjectNumber][2];
	G4double endPhi = Dimensions[ObjectNumber][3];
	G4double startingTheta = Dimensions[ObjectNumber][4];
	G4double endTheta = Dimensions[ObjectNumber][5]; 

	G4Sphere* Sphere = new G4Sphere("Sphere" + StringNumber, 
				     	innerRadius,
				     	outerRadius,
				     	startingPhi,
				     	endPhi,
				     	startingTheta,
				     	endTheta);

	G4LogicalVolume* logicSphere = new G4LogicalVolume(Sphere, FindMaterial(Materials[ObjectNumber]), "logicSphere" + StringNumber);

	G4ThreeVector StartingRotation = GetVectorRotation(ObjectNumber);
	G4double DeltaAngle = RotateObject();

	G4RotationMatrix* RotateObjectAngle = new G4RotationMatrix();
	RotateObjectAngle->rotateX(90.*deg + StartingRotation.x());
	RotateObjectAngle->rotateY(0.*deg + StartingRotation.y());
	RotateObjectAngle->rotateZ(DeltaAngle + StartingRotation.z());

	G4ThreeVector NewTargetPosition = OffSetRotation(ObjectNumber, Positions[ObjectNumber], OffSetRadius_Cmd, DeltaAngle);

	//Create the target physical volume
	G4VPhysicalVolume* physSphere = new G4PVPlacement(RotateObjectAngle,            
							  NewTargetPosition,    
							  logicSphere,           //its logical volume
							  "phySphere" + StringNumber,               //its name
							  MotherBox,                     //its mother  volume
							  false,                 //no boolean operation
							  ObjectNumber,                     //copy number
							  OverlapCheck_Cmd);		//overlaps checking      

	//Visualization attributes
	Visualization(logicSphere, G4Colour::White());
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

		return deltaTheta*nImage;
	}
	else 
		{return 0.*deg;}
}

G4ThreeVector TargetConstruction::OffSetRotation(G4int ObjectNumber, G4ThreeVector CurrentPosition, G4double Radius, G4double Angle)
{
	G4ThreeVector Start = StartingPositions[0];
	G4double intialY = Start.y();

	//If the object number is 0 (master), it rotates around centre with radius
	if (ObjectNumber == 0)
	{
		if (GetNoImages() > 1 && Radius != 0)
		{
			G4double NewX = Centre_Cmd.x() + cos(Angle)*Radius;
			G4double NewZ = Centre_Cmd.z() + sin(Angle)*Radius;

			SetVectorPosition(ObjectNumber, G4ThreeVector(NewX,Centre_Cmd.y()+intialY,NewZ));

			return G4ThreeVector(NewX, Centre_Cmd.y()+intialY ,NewZ);
		}
		else 
			{return GetVectorPosition(ObjectNumber);}
	}
	//If object number is greater than 0, it will rotate around the master volume(0)
	else 
	{
		G4double NewX = GetVectorPosition(0).x() + (cos(Angle) * CurrentPosition.x()) + (-sin(Angle) * CurrentPosition.z());
		G4double NewZ = GetVectorPosition(0).z() + (sin(Angle) * CurrentPosition.x()) + (cos(Angle) * CurrentPosition.z());
 
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



