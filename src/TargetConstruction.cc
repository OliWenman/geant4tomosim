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

TargetConstruction::TargetConstruction()
{	
	TCMessenger = new TargetConstructionMessenger(this);
	nImage = 0;
	TotalImages = 1;
	SubtractSolidCounter = 0;
	MasterVolume = 0;
	MasterCheck = false;
}

TargetConstruction::~TargetConstruction()
{
	delete TCMessenger;
}

void TargetConstruction::Construct(G4LogicalVolume* World)
{
	//Set the Starting positions (only once)
	if (nImage == 0)
		{StartingPositions = Positions;}

	//Loop through the number of objects there are to be created
	for(G4int nObject = 0; nObject < TypeOfObjects.size(); nObject++)
	{
		G4String StringNumber = std::to_string(nObject + 1);

		//Create the dimensions of the object and assign its logic volume (if it has one) only once depending on the shape it is
		if (nImage == 0)
		{
			if(TypeOfObjects[nObject] == "Cube")
				{Box(nObject, StringNumber);}

			else if(TypeOfObjects[nObject] == "HollowCube")
				{HollowBox(nObject, StringNumber);}

			else if(TypeOfObjects[nObject] == "Sphere")
				{Sphere(nObject, StringNumber);}

			else if(TypeOfObjects[nObject] == "Cylinder")
				{Cylinder(nObject, StringNumber);} 

			else if(TypeOfObjects[nObject] == "SubtractSolid")
				{SubtractSolid(nObject, StringNumber);}

			else if(TypeOfObjects[nObject] == "Trapezoid") 
				{Trapezoid(nObject, StringNumber);}

			else if(TypeOfObjects[nObject] == "Ellipsoid")
				{Ellipsoid(nObject, StringNumber);}

			else
				{G4cout << "\nERROR: Wrong TypesOfObjects String" << G4endl; exit(-1);}

			//Add its logical volume to that object
			AddLogicalVolume(nObject,TypeOfObjects[nObject]+StringNumber, Materials[nObject]);
		}

		//Place the object in the world volume for each image
		AddPhysicalVolume(nObject,TypeOfObjects[nObject]+StringNumber, World);
	} 

	//Update the image counter
	++nImage;	
}

void TargetConstruction::Box(G4int ObjectNumber, G4String StringNumber)
{
	//Get the dimensiosn for the box
	G4ThreeVector TargetSize = G4ThreeVector(Dimensions[ObjectNumber][0], 
						 Dimensions[ObjectNumber][1],
						 Dimensions[ObjectNumber][2]);

	//Create a G4VSolid of the box
	G4Box* Box = new G4Box("Cube" + StringNumber, TargetSize.x(), TargetSize.y(), TargetSize.z());
}

void TargetConstruction::HollowBox(G4int ObjectNumber, G4String StringNumber)
{
	//Get the dimensions for the outerBox
	G4ThreeVector outerBoxDimensions = G4ThreeVector(Dimensions[ObjectNumber][0], 
					       		 Dimensions[ObjectNumber][1],
					       		 Dimensions[ObjectNumber][2]);

	//Get the dimensions for the innerBox
	G4ThreeVector innerBoxDimensions = G4ThreeVector(Dimensions[ObjectNumber][3], 
					       		 Dimensions[ObjectNumber][4],
					       		 Dimensions[ObjectNumber][5]);

	//Create G4VSolids of the two boxes
	G4Box* outerBox = new G4Box("OuterBox" + StringNumber, 
				    outerBoxDimensions.x(), 
				    outerBoxDimensions.y(), 
				    outerBoxDimensions.z());

	G4Box* innerBox = new G4Box("InnerBox" + StringNumber,
				    innerBoxDimensions.x(),
				    innerBoxDimensions.y(),
				    innerBoxDimensions.z());

	//Subtract them from each other to get a hollow cube
	G4SubtractionSolid *HollowBox = new G4SubtractionSolid("HollowCube" + StringNumber, 
							       outerBox, 
							       innerBox);
}

void TargetConstruction::Cylinder(G4int ObjectNumber, G4String StringNumber)
{
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

void TargetConstruction::Sphere(G4int ObjectNumber, G4String StringNumber)
{
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

void TargetConstruction::Trapezoid(G4int ObjectNumber, G4String StringNumber)
{
	G4double dx1 = Dimensions[ObjectNumber][0];
        G4double dx2 = Dimensions[ObjectNumber][1];
        G4double dy1 = Dimensions[ObjectNumber][2];
        G4double dy2 = Dimensions[ObjectNumber][3];
        G4double dz  = Dimensions[ObjectNumber][4];

	G4Trd* Trapezoid = new G4Trd("Trapezoid" + StringNumber,
				     dx1,
          			     dx2,
            			     dy1,
            			     dy2,
            			     dz);
}

void TargetConstruction::Ellipsoid(G4int ObjectNumber, G4String StringNumber)
{
	G4double pxSemiAxis = Dimensions[ObjectNumber][0];
        G4double pySemiAxis = Dimensions[ObjectNumber][1];
        G4double pzSemiAxis = Dimensions[ObjectNumber][2];
        G4double pzBottomCut = Dimensions[ObjectNumber][3];
        G4double pzTopCut = Dimensions[ObjectNumber][4];

	G4Ellipsoid* Ellipsoid = new G4Ellipsoid("Ellipsoid"+StringNumber,
                    				 pxSemiAxis,
                    				 pySemiAxis,
                    				 pzSemiAxis,
                    				 pzBottomCut,
                    				 pzTopCut);
}

void TargetConstruction::SubtractSolid(G4int ObjectNumber, G4String StringNumber)
{
	//From the string, be able to take the needed paramenters out as seperate variables
	G4Tokenizer next(SubtractObject[SubtractSolidCounter]);

	//Names of the two objects being used
	G4String OuterObject = next();
	G4String InnerObject= next();

	//Find the positions of the inner object inside the string 
	G4double x = std::stod(next());
	G4double y = std::stod(next());
	G4double z = std::stod(next());
	G4String LengthUnit = next();
	TCMessenger -> CheckUnits("/Target/SubtractSolid", SubtractObject[SubtractSolidCounter], LengthUnit, "Length");

	//Get the correct unit for the length using the dictionary created in the TCMessenger
	x = x * TCMessenger -> GetLengthUnit(LengthUnit);
	y = y * TCMessenger -> GetLengthUnit(LengthUnit);
	z = z * TCMessenger -> GetLengthUnit(LengthUnit);

	//Find the rotation of the inner object insde the string
	G4double Rotx = std::stod(next());
	G4double Roty = std::stod(next());
	G4double Rotz = std::stod(next());
	G4String AngleUnit = next();
	TCMessenger -> CheckUnits("/Target/SubtractSolid", SubtractObject[SubtractSolidCounter], AngleUnit, "Angle");

	//Create a rotation matrix for the rotation and give it the correct units using the dictionary created in the TCMessenger
	G4RotationMatrix* RotateInnerObject = new G4RotationMatrix();
	RotateInnerObject -> rotateX(Rotx * TCMessenger -> GetAngleUnit(AngleUnit));
	RotateInnerObject -> rotateY(Rotz * TCMessenger -> GetAngleUnit(AngleUnit));
	RotateInnerObject -> rotateZ(Roty * TCMessenger -> GetAngleUnit(AngleUnit));	

	//Find the correct solids with the name of the objects inputted
	G4VSolid* OuterSolid = G4SolidStore::GetInstance() -> GetSolid(OuterObject, true); 
	G4VSolid* InnerSolid = G4SolidStore::GetInstance() -> GetSolid(InnerObject, true); 

	//Create the new solid from 
	G4SubtractionSolid *NewSolid = new G4SubtractionSolid("SubtractSolid" + StringNumber, 
							      OuterSolid, 
							      InnerSolid, 
							      RotateInnerObject, 
							      G4ThreeVector(x, y, z));

	//G4cout << "\n- Created solid: SubtractSolid" + StringNumber << " -> Made up of " << OuterSolid -> GetName() << " & " << InnerSolid -> GetName();
	++SubtractSolidCounter;
}

void TargetConstruction::AddLogicalVolume(G4int ObjectNumber, G4String SolidName, G4String Material)
{
	//If its logic volume value is true, add its logic volume
	if (LogicVolumeArray[ObjectNumber] == true)
	{
		//DefineMaterial();
		//Finds the right G4VSolid
		G4VSolid* Solid = G4SolidStore::GetInstance() -> GetSolid(SolidName, true); 
		G4LogicalVolume* logicObject = new G4LogicalVolume(Solid, FindMaterial(Material), "LV" + SolidName);

		//G4Region* targetRegion = new G4Region("targetRegion");
  		//logicObject->SetRegion(targetRegion);
  		//siloRegion->AddRootLogicalVolume(wallLog);
	}
}

void TargetConstruction::AddPhysicalVolume(G4int ObjectNumber, G4String Name, G4LogicalVolume* MotherBox)
{
	//If its logic volume value is true, add its physical volume
	if (LogicVolumeArray[ObjectNumber] == true)
	{
		if (MasterCheck == false)
		{
			MasterVolume = ObjectNumber;
			MasterCheck = true;
		}

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

G4ThreeVector TargetConstruction::OffSetRotation(G4int ObjectNumber, G4ThreeVector CurrentPosition, G4double Radius, G4double Angle)
{
	//If the object number is the master, it rotates around centre with radius
	if (ObjectNumber == MasterVolume)
	{
		if (TotalImages > 1 && Radius != 0)
		{
			G4double NewX = Centre_Cmd.x() + cos(Angle)*Radius;
			G4double NewZ = Centre_Cmd.z() + sin(Angle)*Radius;

			SetVectorPosition(ObjectNumber, G4ThreeVector(NewX,Centre_Cmd.y()+StartingPositions[ObjectNumber].y(),NewZ));

			return G4ThreeVector(NewX, Centre_Cmd.y()+StartingPositions[ObjectNumber].y() ,NewZ);
		}
		else 
			{return Positions[ObjectNumber];}
	}
	//If object number is greater than 0, it will rotate around the master volume
	else 
	{
		G4double NewX = Positions[MasterVolume].x() + (cos(Angle) * CurrentPosition.x()) + (-sin(Angle) * CurrentPosition.z());
		G4double NewZ = Positions[MasterVolume].z() + (sin(Angle) * CurrentPosition.x()) + (cos(Angle) * CurrentPosition.z());

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

void TargetConstruction::ReadOutInfo()
{
	/*G4cout << "\n--------------------------------------------------------------------"
		  "\nTHE GEOMETRY SETUP: " << G4endl;
 
	//Loop through the number of objects there are to be created
	for(G4int n = 0; n < TypeOfObjects.size(); n++)
	{
		int N = n + 1;
		if(TypeOfObjects[n] == "Cube")
		{
			G4cout << "\nCube" << N << ": " 
			          "\n  - Dimensions: x = " << G4BestUnit(Dimensions[n][0], "Length") 
					       << ", y = " << G4BestUnit(Dimensions[n][1], "Length")
					       << ", z = " << G4BestUnit(Dimensions[n][2], "Length") 

			       << "\n  - Material: " << Materials[n]
			       << "\n  - Initial position: " << G4BestUnit(StartingPositions[n], "Length") << G4endl;
		}
		else if(TypeOfObjects[n] == "HollowCube")
		{	
			G4cout << "\nHollowCube" << N << ": "
			          "\n  - Dimensions: outer cube: x = " << G4BestUnit(Dimensions[n][0], "Length") 
							   << ", y = " << G4BestUnit(Dimensions[n][1], "Length") 
							   << ", z = " << G4BestUnit(Dimensions[n][2], "Length") 
			       << "\n                inner cube: x = " << G4BestUnit(Dimensions[n][3], "Length") 
							   << ", y = " << G4BestUnit(Dimensions[n][4], "Length") 
							   << ", z = " << G4BestUnit(Dimensions[n][5], "Length")

			       << "\n  - Material: " << Materials[n]
			       << "\n  - Initial position: " << G4BestUnit(StartingPositions[n], "Length") << G4endl;
		}
		else if(TypeOfObjects[n] == "Sphere")
		{
			G4cout << "\nSphere" << N << ": "
                               << "\n  - Dimensions: inner radius = " << G4BestUnit(Dimensions[n][0], "Length")
					       << ", outer radius = " << G4BestUnit(Dimensions[n][1], "Length") << ", "
                               << "\n                inner phi = " << G4BestUnit(Dimensions[n][2], "Angle")
					       << ", outer phi = " << G4BestUnit(Dimensions[n][3], "Angle") << ", "
			       << "\n                inner theta = " << G4BestUnit(Dimensions[n][4], "Angle")
					       << ", outer theta = " << G4BestUnit(Dimensions[n][5], "Angle")   

			       << "\n  - Material: " << Materials[n]
			       << "\n  - Initial position: " << G4BestUnit(StartingPositions[n], "Length") << G4endl;
		}
		else if(TypeOfObjects[n] == "Cylinder")
		{	
			G4cout << "\nCylinder" << N << ": "
			       << "\n  - Dimensions: inner radius = " << G4BestUnit(Dimensions[n][0], "Length")
					       << ", outer radius = " << G4BestUnit(Dimensions[n][1], "Length") << ", "
			       << "\n                height = " << G4BestUnit(Dimensions[n][2], "Length") << ", "
			       << "\n                inner phi = " << G4BestUnit(Dimensions[n][3], "Angle")
					       << ", outer phi = " << G4BestUnit(Dimensions[n][4], "Angle") 

			       << "\n  - Material: " << Materials[n]
			       << "\n  - Initial position: " << G4BestUnit(StartingPositions[n], "Length") << G4endl;
		}*/

		/*else if(TypeOfObjects[n] == "Trapezoid") 
			{Trapezoid(n);}

		else if(TypeOfObjects[n] == "Ellipsoid")
			{Ellipsoid(n);}

		else if(TypeOfObjects[n] == "SubtractSolid")
			{SubtractSolid(n);}*/
	//}

}

void TargetConstruction::DefineMaterial()
{
	/*//Single elements
	G4double atomicWeight = 39.95*g/mole;
	G4double density = 1.390*g/cm3;
	G4int z = 18;
	G4String Name = "liquidArgon";
	G4Material* lAr = new G4Material(Name, z, atomicWeight, density);

	return lAr;*/

//=======================================================================

	/*//Molecules
	G4String name = "Hydrogen";
	G4String symbol = "H";
	G4int z = 1;
	G4double a = 1.01*g/mole;	
	G4Element* hydrogen = new G4Element(name, symbol, z, a); 

	name = "Oxygen";
	symbol = "O";
	z = 8;
	a = 16.00*g/mole; 
	G4Element* oxygen = new G4Element(name, symbol, z, a);

	name = "Water";
	G4double density = 1.000*g/cm3;
	G4int n_components = 2;
	G4Material* H2O = new G4Material(name, density, n_components);

	H2O -> AddElement(hydrogen, 2);
	H2O -> AddElement(oxygen, 1);

	return H2O;*/

//=====================================================================

	//Isotopes
	/*G4int z = 92;
	G4int A = 235;
	G4double a = 235.044*g/mole;
	G4String name = "U235";
	G4Isotope* isoU235 = new G4Isotope(name, z, A, a);

	z = 92;
	A = 238;
	a = 238.051*g/mole;
	name = "U238";
	G4Isotope* isoU238 = new G4Isotope(name, z, A, a);

	name = "enr. U";
	G4String symbol = "U";
	G4int nComponents = 2;
	G4Element* enrichedU = new G4Element(name, symbol, nComponents);

	enrichedU -> AddIsotope(isoU235, 80.*perCent);
	enrichedU -> AddIsotope(isoU238, 20.*perCent);

	G4double density = 19.1*g/cm3;
	nComponents = 1;
	G4Material* mat_enrichedU = new G4Material("enr. U", density, nComponents);
	G4int fraction_mass = 1;
	mat_enrichedU -> AddElement(enrichedU, fraction_mass);*/
}
