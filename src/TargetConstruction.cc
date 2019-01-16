//Own classes
#include "Data.hh"
#include "TargetConstruction.hh"
#include "TargetConstructionMessenger.hh"
//Output to console/write to file
#include "SettingsLog.hh"

//Different shapes for solids
#include "G4VSolid.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4Ellipsoid.hh"
#include "G4SolidStore.hh"

//Logical volume
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

//Physical volume
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4PhysicalVolumeStore.hh"

//Material database
#include "G4NistManager.hh"

//Units
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

//Update the geometery
#include "G4RunManager.hh"

//Seperate out variables from strings
#include "G4Tokenizer.hh"

#include "G4Region.hh"

//=================================================================================================
//  CONSTRUCTORS/DESTRUCTORS

TargetConstruction::TargetConstruction()
{	
	TCMessenger = new TargetConstructionMessenger(this);
	nImage = 0;
	TotalImages = 1;
	MasterVolume = 0;
	MasterCheck = false;
}

TargetConstruction::~TargetConstruction()
{
	delete TCMessenger;
}

//=================================================================================================
//  BUILD THE PHYSICAL VOLUMES OF OBJECTS THAT ARE IN THE DATABASE

void TargetConstruction::Construct(G4LogicalVolume* World)
{	
	for (int n = 0 ; n < ObjectDatabase.size() ; n++)
	{
	    if (ObjectDatabase[n].WorldPlacement == true)
	        AddPhysicalVolume(ObjectDatabase[n], World); 
	} 
	    
	//Update the image counter
	++nImage;	
}
//=================================================================================================
//  DIFFERENT OBJECT TYPES

void TargetConstruction::Box(G4String Name, std::vector<double> Dimensions)
{
	//Create a G4VSolid of the box
	G4Box* Box = new G4Box(Name, Dimensions[0], Dimensions[1], Dimensions[2]);
	
	G4VSolid* Solid = G4SolidStore::GetInstance() -> GetSolid(Name, true); 
	
	if (Solid)
	    G4cout << "\nSolid added successfully " << Solid -> GetName() << G4endl;
	else
	    G4cout << "\nERROR: Solid not added " << G4endl;
}

void TargetConstruction::Cylinder(G4String Name, std::vector<double> Dimensions)
{
	//Get the dimensions of the cylinder
	G4double innerRadius = Dimensions[0];
	G4double outerRadius = Dimensions[1];
	G4double height = Dimensions[2];
	G4double startAngle = Dimensions[3];
	G4double spanningAngle = Dimensions[4];
		
	//Create a G4VSolid of the cylinder
   	G4Tubs* TargetTube = new G4Tubs(Name,
				 	                innerRadius,
                  		 	        outerRadius,
                  		 	        height,
                  		 	        startAngle,
                  		 	        spanningAngle);
}

void TargetConstruction::Sphere(G4String Name, std::vector<double> Dimensions)
{
    G4cout << "\nSphere dimensions ";
    for (int n = 0 ; n < Dimensions.size() ; n++)
        G4cout << Dimensions[n] << " ";
    G4cout << G4endl;

	//Get the dimensions of the sphere
	G4double innerRadius = Dimensions[0];
	G4double outerRadius = Dimensions[1];
	G4double startingPhi = Dimensions[2];
	G4double endPhi = Dimensions[3];
	G4double startingTheta = Dimensions[4];
	G4double endTheta = Dimensions[5]; 

	//Create a G4VSolid of the sphere
	G4Sphere* Sphere = new G4Sphere(Name, 
				     	            innerRadius,
				     	            outerRadius,
				     	            startingPhi,
				     	            endPhi,
				     	            startingTheta,
				     	            endTheta);
}

void TargetConstruction::Trapezoid(G4String Name, std::vector<double> Dimensions)
{
	G4double dx1 = Dimensions[0];
    G4double dx2 = Dimensions[1];
    G4double dy1 = Dimensions[2];
    G4double dy2 = Dimensions[3];
    G4double dz  = Dimensions[4];

	G4Trd* Trapezoid = new G4Trd(Name,
				                 dx1,
          			             dx2,
            			         dy1,
            			         dy2,
            			         dz);
}

void TargetConstruction::Ellipsoid(G4String Name, std::vector<double> Dimensions)
{
	G4double pxSemiAxis = Dimensions[0];
    G4double pySemiAxis = Dimensions[1];
    G4double pzSemiAxis = Dimensions[2];
    G4double pzBottomCut = Dimensions[3];
    G4double pzTopCut = Dimensions[4];

	G4Ellipsoid* Ellipsoid = new G4Ellipsoid(Name,
                    				         pxSemiAxis,
                    				         pySemiAxis,
                    				         pzSemiAxis,
                    				         pzBottomCut,
                    				         pzTopCut);
}

//=================================================================================================
//  CREATE THE OBJECTS AND ITS ATTRIBUTES, SAVE TO DATABASE. BUILD THE MODEL

//Create a simple object provided via Geant4 classes
void TargetConstruction::CreateObject(G4String Name, G4String Type, std::vector<double> Dimensions)
{
    CustomObject newObject;
    newObject.Name = Name;
    newObject.Type = Type;
    newObject.Dimensions = Dimensions;
    newObject.WorldPlacement = false;
    
    int ObjectNumber = ObjectDatabase.size() + 1;
    newObject.Number = ObjectNumber;
    
    ObjectDatabase.push_back (newObject);
    
    if (Type == "Cube")
        Box(Name, Dimensions);
    else if (Type == "Sphere")
        Sphere(Name, Dimensions);
    else if (Type == "Cylinder")
        Cylinder(Name, Dimensions);
    else if (Type == "Trapezoid")
        Trapezoid(Name, Dimensions);
    else if (Type == "Ellipsoid")
        Ellipsoid(Name, Dimensions);
}

//Create complex objects by combining or subtracting components from objects
void TargetConstruction::CreateObject(G4String Name, G4String ComponentName1, G4String ComponentName2, G4String Type, G4ThreeVector innerPosition, G4ThreeVector innerRotation)
{
    ComplexObject newObject;
    
    //Custom object properties that ComplexObject inherits from
    newObject.Name = Name;
    newObject.Type = Type;
    newObject.WorldPlacement = false;
    
    int ObjectNumber = ObjectDatabase.size() + 1;
    newObject.Number = ObjectNumber;
    
    //ComplexObject properties
    newObject.ComponentName1 = ComponentName1;
    newObject.ComponentName2 = ComponentName2;
    newObject.InsidePosition = innerPosition;
    newObject.InsideRotation = innerRotation;
    
    ObjectDatabase.push_back (newObject);
    
    if (Type == "SubtractSolid")
    {
        SubtractSolid(newObject);
    }
}

//=================================================================================================
//  CREATE COMPLEX OBJECT

void TargetConstruction::SubtractSolid(ComplexObject &complexObject)
{
	//Create a rotation matrix for the rotation and give it the correct units using the dictionary created in the TCMessenger
	G4RotationMatrix* RotateInnerObject = new G4RotationMatrix();
	RotateInnerObject -> rotateX(complexObject.InsideRotation.x());
	RotateInnerObject -> rotateY(complexObject.InsideRotation.y());
	RotateInnerObject -> rotateZ(complexObject.InsideRotation.z());	

	//Find the correct solids with the name of the objects inputted
	G4VSolid* OuterSolid = G4SolidStore::GetInstance() -> GetSolid(complexObject.ComponentName1, true); 
	G4VSolid* InnerSolid = G4SolidStore::GetInstance() -> GetSolid(complexObject.ComponentName2, true); 

	//Create the new solid from 
	G4SubtractionSolid *NewSolid = new G4SubtractionSolid(complexObject.Name, 
							                              OuterSolid, 
							                              InnerSolid, 
							                              RotateInnerObject, 
							                              complexObject.InsidePosition);
	
	G4VSolid* Solid = G4SolidStore::GetInstance() -> GetSolid(complexObject.Name, true); 
	
	if (Solid)
	    G4cout << "\nSolid added successfully " << Solid -> GetName() << G4endl;
	else
	    G4cout << "\nERROR: Solid not added " << G4endl;
	
}

//=================================================================================================
//  CREATE A LOGICAL VOLUME AND ASSIGNING IT A MATERIAL
//  ADD ITS POSITION TO AN OBJECT
//  ADD ITS ROTATION TO AN OBJECT

G4Material* TargetConstruction::FindMaterial(G4String MaterialName)
{
	//Obtain pointer to NIST material manager to find the build materials 
	return G4NistManager::Instance() -> FindOrBuildMaterial(MaterialName);
}

void TargetConstruction::AddMaterial(G4String Name, G4String Material)
{
    bool Success = false;
    
    for (int n = 0 ; n < ObjectDatabase.size() ; n++)
    {   if (ObjectDatabase[n].Name == Name)
        {   ObjectDatabase[n].Material = Material;
            ObjectDatabase[n].WorldPlacement = true;
        
            G4VSolid* Solid = G4SolidStore::GetInstance() -> GetSolid(Name, true); 
		    G4LogicalVolume* logicObject = new G4LogicalVolume(Solid, FindMaterial(Material), "LV" + Name);
		    
		    Success = true;
		    
            break;
        }
    }
    if (Success = false)
    {   G4cout << "ERROR: couldn't find object \"" << Name << "\" in the database to add its material \"" << Material << "\"" << G4endl;
        exit(0);
    }
} 

void TargetConstruction::AddPosition(G4String Name, G4ThreeVector Position)
{
    bool Success = false;
    
    for (int n = 0 ; n < ObjectDatabase.size() ; n++)
    {   if (ObjectDatabase[n].Name == Name)
        {   
            ObjectDatabase[n].StartingPosition = Position;
            ObjectDatabase[n].Position = Position;
		    
		    Success = true;
		    
            G4cout << "\nPosition \"" << ObjectDatabase[n].Position << "\" added to object \"" << ObjectDatabase[n].Name << "\" " << G4endl;
            break;
        }
    }
    if (Success = false)
    {   G4cout << "ERROR: couldn't find object \"" << Name << "\" in the database to add its position \"" << Position << "\"" << G4endl;
        exit(0);
    }
}

void TargetConstruction::AddRotation(G4String Name, G4ThreeVector Rotation)
{
    bool Success = false;
    
    for (int n = 0 ; n < ObjectDatabase.size() ; n++)
    {   if (ObjectDatabase[n].Name == Name)
        {   
            ObjectDatabase[n].StartingRotation = Rotation;
            ObjectDatabase[n].Rotation = Rotation;
		    
		    Success = true;
		    
            G4cout << "\nRotation \"" << ObjectDatabase[n].Rotation << "\" added to object \"" << ObjectDatabase[n].Name << "\" " << G4endl;
            break;
        }
    }
    if (Success = false)
    {   G4cout << "ERROR: couldn't find object \"" << Name << "\" in the database to add its rotation \"" << Rotation << "\"" << G4endl;
        exit(0);
    }
}


//=================================================================================================
//  BUILD THE GEOMETRY IN THE WORLD VOLUME

void TargetConstruction::AddPhysicalVolume(CustomObject &object, G4LogicalVolume* MotherBox)
{
    if (SimMode == "Simulating")
    {
        if (MasterCheck == false)
		{
			MasterVolume = object.Number;
			MasterCheck = true;
		}

        G4String Name = object.Name;
		//Finds the right logic volume
		G4LogicalVolume* LVObject = G4LogicalVolumeStore::GetInstance() -> GetVolume("LV"+ Name, true);

		//Get its starting rotation angle
	    G4ThreeVector StartingRotation = object.Rotation;
		
		//Calculate how much the object rotates between each image
		G4double DeltaAngle = RotateObject();

		//Updates the rotation of the object
		G4RotationMatrix* RotateObjectAngle = new G4RotationMatrix();
		RotateObjectAngle->rotateX(object.StartingRotation.x());
		RotateObjectAngle->rotateY(object.StartingRotation.y());
		RotateObjectAngle->rotateZ(DeltaAngle + object.StartingRotation.z());

		//Offsets the rotation
		G4ThreeVector NewTargetPosition = OffSetRotation2(object, DeltaAngle);

		//Create the target physical volume
		G4VPhysicalVolume* physObject = new G4PVPlacement(RotateObjectAngle,            
							 	                          NewTargetPosition,    
							  	                          LVObject,           //its logical volume
							  	                          "phy" + object.Name,               //its name
							  	                          MotherBox,                     //its mother  volume
							 	                          false,                 //no boolean operation
							 	                          object.Number,                     //copy number
							 	                          OverlapCheck_Cmd);		//overlaps checking      
							 	                          
							 	           

		//Visualization attributes
		Visualization(LVObject, G4Colour::White());

		//Let the runManager know the geometry has changed between runs
		G4RunManager::GetRunManager()->GeometryHasBeenModified();
	}
}

G4ThreeVector TargetConstruction::OffSetRotation2(CustomObject &object, G4double deltaAngle)
{
    //If the object number is the master, it rotates around centre with radius
	if (object.Number == MasterVolume)
	{
		if (TotalImages > 1 && OffSetRadius_Cmd != 0)
		{
			G4double NewX = Centre_Cmd.x() + cos(deltaAngle)*OffSetRadius_Cmd;
			G4double NewZ = Centre_Cmd.z() + sin(deltaAngle)*OffSetRadius_Cmd;

			G4ThreeVector NewPosition = G4ThreeVector(NewX, Centre_Cmd.y() + object.StartingPosition.y(), NewZ);
			object.Position = NewPosition;

			return NewPosition;
		}
		else 
			{return object.Position;}
	}
	//If object number is greater not the master volume, it will rotate around the master volume
	else 
	{
        G4double NewX = object.Position.x() + (cos(deltaAngle) * object.Position.x()) + (-sin(deltaAngle) * object.Position.z());
        G4double NewZ = object.Position.z() + (sin(deltaAngle) * object.Position.x()) + (cos(deltaAngle) * object.Position.z()); 

		return G4ThreeVector(NewX, object.StartingPosition.y(), NewZ);
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

void TargetConstruction::ReadOutInfo()
{

}
