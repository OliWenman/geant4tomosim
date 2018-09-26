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

TargetConstruction::TargetConstruction(G4int NoImages)
{	G4cout << G4endl << "TargetConstruction has been created" << G4endl;
	
	TCMessenger = new TargetConstructionMessenger(this);
}

TargetConstruction::~TargetConstruction()
{
	G4cout << G4endl << "TargetConstruction has been deleted" << G4endl;
	delete TCMessenger;
}

void TargetConstruction::Construct(G4LogicalVolume* World)
{
	if (GetCurrentImage() == 0)
	{
		std::vector<G4ThreeVector> iPositions;
		iPositions.resize(GetNumberOfObjects());
		SavePositions(iPositions);
		SetPosition(0, G4ThreeVector(0,0,0));
	}
	MasterVolume("Box", World);
}

void TargetConstruction::Box(G4String CheckMaster, G4ThreeVector TargetSize, G4ThreeVector TargetPosition, G4String Material, G4LogicalVolume* MotherBox)
{
	G4Box *Box = new G4Box("Box", TargetSize.x()/2., TargetSize.y()/2., TargetSize.z()/2.);

	//Fill the target with its material
	G4Material* BoxMaterial = FindMaterial("G4_Al");
	G4LogicalVolume* logicBox = new G4LogicalVolume(Box, BoxMaterial, "logicBox");
	
	G4double Rotation = RotateObject();
	G4RotationMatrix* RotateObjectAngle = new G4RotationMatrix();
	RotateObjectAngle->rotateX(90.*deg);
	RotateObjectAngle->rotateY(0.*deg);
	RotateObjectAngle->rotateZ(Rotation);

	G4ThreeVector NewTargetPosition = OffSetRotation(CheckMaster, TargetPosition, MasterOffSetRadius_Cmd, Rotation);

	//Create the target physical volume
	G4VPhysicalVolume* physHollowBox = new G4PVPlacement(RotateObjectAngle,            
							     NewTargetPosition,    
							     logicBox,           //its logical volume
							     "Box",               //its name
							     MotherBox,                     //its mother  volume
							     false,                 //no boolean operation
							     0,                     //copy number
							     false);		//overlaps checking      

	//Visualization attributes
	Visualization(logicBox, G4Colour::White());

	G4cout << G4endl << "The target has been created succesfully " << G4endl;  	

}

void TargetConstruction::HollowBox(G4String CheckMaster, G4ThreeVector TargetSize, G4ThreeVector InnerSize, G4ThreeVector TargetPosition, G4String Material, G4LogicalVolume* MotherBox)
{
	G4Box *outerBox = new G4Box("Outer Box", TargetSize.x()/2., TargetSize.y()/2., TargetSize.z()/2.);
	G4Box *innerBox = new G4Box("Inner Box",(TargetSize.x()-InnerSize.x())/2.,(TargetSize.y()-InnerSize.y())/2.,(TargetSize.z()-InnerSize.z())/2.);
	G4SubtractionSolid *HollowBox = new G4SubtractionSolid("Hollow Box", outerBox, innerBox);

	//Fill the target with its material
	G4Material* BoxMaterial = FindMaterial("G4_Al");
	G4LogicalVolume* logicHollowBox = new G4LogicalVolume(HollowBox, BoxMaterial, "logicBox");
	
	G4double Rotation = RotateObject();
	G4RotationMatrix* RotateObjectAngle = new G4RotationMatrix();
	RotateObjectAngle->rotateX(90.*deg);
	RotateObjectAngle->rotateY(0.*deg);
	RotateObjectAngle->rotateZ(Rotation);

	G4ThreeVector NewTargetPosition = OffSetRotation(CheckMaster, TargetPosition, MasterOffSetRadius_Cmd, Rotation);

	//Create the target physical volume
	G4VPhysicalVolume* physHollowBox = new G4PVPlacement(RotateObjectAngle,            
							     NewTargetPosition,    
							     logicHollowBox,           //its logical volume
							     "HollowBox",               //its name
							     MotherBox,                     //its mother  volume
							     false,                 //no boolean operation
							     0,                     //copy number
							     false);		//overlaps checking      

	//Visualization attributes
	Visualization(logicHollowBox, G4Colour::White());

	G4cout << G4endl << "The target has been created succesfully " << G4endl;  
}

void TargetConstruction::Cylinder(G4ThreeVector TargetPosition, G4String Material, G4LogicalVolume* logicMotherBox, G4String Shape)
{
	//Target geometry
	G4double innerRadius = 1*cm;
	G4double outerRadius = 1.50*cm;
	G4double hz = 0.5*m;
	G4double startAngle = 0.*deg;
	G4double spanningAngle = 360.*deg;

	//TARGET
   	G4Tubs* TargetTube = new G4Tubs("TargetGeometry",
				 	innerRadius,
              			 	outerRadius,
               			 	hz,
               			 	startAngle,
               			 	spanningAngle);

	//Fill the target with its material
	G4LogicalVolume* logicTarget = new G4LogicalVolume(TargetTube, FindMaterial("G4_Al"), "Al_Target");


	//Create the target physical volume
	G4VPhysicalVolume* physTarget= new G4PVPlacement(0,            //no rotation
						 	TargetPosition,       
						 	logicTarget,           //its logical volume
						 	"Target",               //its name
						 	logicMotherBox,                     //its mother  volume
						 	false,                 //no boolean operation
						 	0,                     //copy number
						 	true);		//overlaps checking      
	//Visualization attributes
	Visualization(logicTarget, G4Colour::White());
	
	G4cout << G4endl << "The target has been created succesfully ";  
	         
}

G4Material* TargetConstruction::FindMaterial(G4String MaterialName)
{
	//Obtain pointer to NIST material manager
	G4NistManager* nist = G4NistManager::Instance();
		
	//Build materials 
	G4Material* Material = nist -> FindOrBuildMaterial(MaterialName);

	return Material;
}

void TargetConstruction::MasterVolume(G4String Shape,  G4LogicalVolume* logicMotherBox)
{
	if (Shape == "HollowBox")
	{
		HollowBox("Master", G4ThreeVector(1.05*mm, 1.05*mm, 1.25*mm), G4ThreeVector(0.0*m, 0.0*m, 0.0*m), GetPosition(0), GetMasterTargetMaterial(), logicMotherBox);
	}
	else if (Shape == "Box")
	{
		Box("Master", G4ThreeVector(0.5*mm, 0.5*mm, 0.5*mm), GetPosition(0), GetMasterTargetMaterial(), logicMotherBox);
	}
}

G4double TargetConstruction::RotateObject()
{
	if (GetNoImages() > 1)
	{	
		G4GeometryManager::GetInstance()->OpenGeometry();
		G4double deltaTheta = 180*deg/(GetNoImages());
		
		G4double RotateY = deltaTheta * GetCurrentImage();
		G4GeometryManager::GetInstance()->CloseGeometry();
		G4RunManager::GetRunManager()->GeometryHasBeenModified();

		/*//Delete and re-build the relevant physical volumes
  		G4PhysicalVolumeStore* store = G4PhysicalVolumeStore::GetInstance();

  		//Delete...
  		G4VPhysicalVolume* myVol = store->GetVolume("RODetectorPhys");
  		store->DeRegister(myVol);
  		//..and rebuild
  		G4VPhysicalVolume *RODetectorPhys = new G4PVPlacement(0,
							detectorToWorldPosition,
							RODetectorLog,
							"RODetectorPhys",
							worldLogical,							
							false,0);*/
		return RotateY;
	}
	else 
	{	
		G4double RotateY = 0.*deg;
		return RotateY;
	}
}

G4ThreeVector TargetConstruction::MasterOffSetRotation(G4String VolumeType, G4ThreeVector Centre, G4double Radius, G4double Angle)
{
	if (GetNoImages() > 1 && Radius != 0)
	{	
		G4double x = (Centre.x()+Radius)*cos(Angle);
		G4double y = (Centre.y()+Radius)*sin(Angle);

		SetMasterPosition(G4ThreeVector(x,0,y));

		return G4ThreeVector(x, 0 ,y);
	}
	else 
		{return G4ThreeVector(0,0,0);}
}

G4ThreeVector TargetConstruction::OffSetRotation(G4String CheckMaster, G4ThreeVector Centre, G4double Radius, G4double Angle)
{
	if (CheckMaster == "SubVolume")
	{
		if (GetNoImages() > 1 && Radius != 0)
		{	
			G4double x = (GetMasterPosition().x()+Radius)*cos(Angle);
			G4double y = (GetMasterPosition().x()+Radius)*sin(Angle);

			return G4ThreeVector(x, 0 ,y);
		}
		else 
			{return G4ThreeVector(0,0,0);}
	}
	else if(CheckMaster == "Master")
	{
		if (GetNoImages() > 1 && Radius != 0)
		{	
			G4double x = (Centre.x()+Radius)*cos(Angle);
			G4double y = (Centre.y()+Radius)*sin(Angle);

			SetMasterPosition(G4ThreeVector(x,0,y));

			return G4ThreeVector(x, 0 ,y);
		}
		else 
			{return G4ThreeVector(0,0,0);}
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



