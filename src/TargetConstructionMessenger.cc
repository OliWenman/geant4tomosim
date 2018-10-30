#include "TargetConstructionMessenger.hh"
#include "TargetConstruction.hh"

#include "G4SystemOfUnits.hh"

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

#include "G4Tokenizer.hh"
#include "G4UnitsTable.hh"

TargetConstructionMessenger::TargetConstructionMessenger(TargetConstruction* Target): G4UImessenger(), TC(Target)
{	
	G4cout << G4endl << "TargetConstructionMessenger has been created" << G4endl;
	
	ObjectCounter = 0;

	//Directory
	TargetDirectory = new G4UIdirectory("/Target/");
	TargetDirectory -> SetGuidance("Commands to control the detector variables. ");

	//Command to set the dimensions of a sphere
	SphereDimensions_Cmd = new G4UIcmdWithAString("/Target/Sphere/Dimensions", this);
	SphereDimensions_Cmd -> SetGuidance("Set the dimensions of a sphere you would like, Inner radius, Outer radius, Starting Phi angle, Delta Phi angle, Starting Theta angle , Delta Theta angle");

	//Command to set the dimensions of a sphere
	CylinderDimensions_Cmd = new G4UIcmdWithAString("/Target/Cylinder/Dimensions", this);
	CylinderDimensions_Cmd -> SetGuidance("Set the dimensions of a cylinder that you would like, Inner radius, Outer radius, Length, Delta Phi angle and ending Phi angle");

	//Command to set the dimensions of a hollow cube
	HollowCubeDimensions_Cmd = new G4UIcmdWithAString("/Target/HollowCube/Dimensions", this);
	HollowCubeDimensions_Cmd -> SetGuidance("Set the dimensions of a sphere you would like, outer x, y, z , unit, inner x, y, z unit");

	//Command to set the dimensions of a cube
	CubeDimensions_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/Cube/Dimensions", this);
	CubeDimensions_Cmd -> SetGuidance("Set the dimensions of a cube for x, y and z. ");
	CubeDimensions_Cmd -> SetUnitCandidates("mm cm m ");
	CubeDimensions_Cmd -> SetDefaultUnit("mm");
	CubeDimensions_Cmd -> SetDefaultValue(G4ThreeVector(0.3*mm, 0.3*mm, 0.0*mm));

	SubtractionSolid_Cmd = new G4UIcmdWithAString("/Target/SubtractSolid", this);
	SubtractionSolid_Cmd -> SetGuidance("Choose two solids to be subtracted to create a new shape and give it a new name");
	
	//Command to set the position of an object
	TargetPosition_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/Position", this);
	TargetPosition_Cmd -> SetGuidance("Set the target position, x, y and z. ");
	TargetPosition_Cmd -> SetUnitCandidates("um mm cm m ");
	TargetPosition_Cmd -> SetDefaultUnit("m");
	TargetPosition_Cmd -> SetDefaultValue(G4ThreeVector(0.0*m, 0.0*m, 0.0*m));

	//Command to set the rotation of an object
	TargetRotation_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/Rotation", this);
	TargetRotation_Cmd -> SetGuidance("Set the starting rotation of the targert. ");
	TargetRotation_Cmd -> SetUnitCandidates("deg rad");
	TargetRotation_Cmd -> SetDefaultUnit("deg");
	TargetRotation_Cmd -> SetDefaultValue(G4ThreeVector(0.0*deg, 0.0*deg, 0.0*deg));

	//Command to set the material of an object
	TargetMaterial_Cmd = new G4UIcmdWithAString("/Target/Material/Database", this);
	TargetMaterial_Cmd -> SetGuidance("Set the material of the target ");
	TargetMaterial_Cmd -> SetDefaultValue("G4_Al");

	//Command to set any boolean operaions for the object
	BooleanOp_Cmd = new G4UIcmdWithABool("/Target/VolumeExclusion", this);
	BooleanOp_Cmd -> SetGuidance("Set the boolean operation for the physical volume of the object");
	BooleanOp_Cmd -> SetDefaultValue(false);

	//Command to check for overlaps in the geometry
	OverlapCheck_Cmd = new G4UIcmdWithABool("/Target/CheckAllOverlaps", this);
	OverlapCheck_Cmd -> SetGuidance("Choose if you would like the volumes to check for overlaps");
	OverlapCheck_Cmd -> SetDefaultValue(false);

	//Command to set the full roation angle for when the target rotates
	FullRotationAngle_Cmd = new G4UIcmdWithADoubleAndUnit("/Target/TotalRotationAngle", this);
	FullRotationAngle_Cmd -> SetGuidance("Set the full rotation angle for the target");

	//Command to set the off set radius of an object when it rotates between projections
	OffSetRadius_Cmd = new G4UIcmdWithADoubleAndUnit("/Target/OffSet/Radius", this);
	OffSetRadius_Cmd -> SetGuidance("Set the off set of the poisition of the target with a radius");
	OffSetRadius_Cmd -> SetDefaultUnit("cm");
	OffSetRadius_Cmd -> SetDefaultValue(0.5*cm);

	//Command to set the centre of this rotation if there is an offset
	Centre_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/OffSet/Centre", this);
	Centre_Cmd -> SetGuidance("Set the centre for the target to rotate round");
	Centre_Cmd -> SetUnitCandidates("um mm cm m ");
	Centre_Cmd -> SetDefaultUnit("m");
	Centre_Cmd -> SetDefaultValue(G4ThreeVector(0.0*m, 0.0*m, 0.0*m));
}

TargetConstructionMessenger::~TargetConstructionMessenger()
{
	delete TargetDirectory;

	delete CubeDimensions_Cmd;
	delete SphereDimensions_Cmd;
	delete CylinderDimensions_Cmd;
	delete HollowCubeDimensions_Cmd;

	delete TargetPosition_Cmd;
	delete TargetRotation_Cmd;

	delete TargetMaterial_Cmd;
	delete BooleanOp_Cmd;
	delete OverlapCheck_Cmd;

	delete FullRotationAngle_Cmd;
	delete OffSetRadius_Cmd;
	delete Centre_Cmd;

	G4cout << G4endl << "TargetConstructionMessenger has been deleted ";
}

void TargetConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command == SphereDimensions_Cmd)
	{
		//Allows for multiple inputs for complex commands, seperates the variables out
		G4Tokenizer next(newValue);
		
		//Define each variable with in the string for length
		G4double innerRadius = std::stod(next());
		G4double outerRadius = std::stod(next());
		G4String RadiusUnit = next();

		//Assign them the correct unit
		if( RadiusUnit == "mm")
		{
			innerRadius = innerRadius*mm;
			outerRadius = outerRadius*mm;
		}
		else if( RadiusUnit == "cm")
		{
			innerRadius = innerRadius*cm;
			outerRadius = outerRadius*cm;
		}
		else if ( RadiusUnit == "m")
		{
			innerRadius = innerRadius*m;
			outerRadius = outerRadius*m;
		}
		else 
		{
			G4cout << G4endl << "***** INVALID INPUT </Target/Cube/Dimensions>*****" <<
			G4endl << "***** Batch is interrupted!! *****" << G4endl;
			exit(1);
		}

		//Define each variable with in the string for angle
		G4double StartingPhi = std::stod(next());
		G4double EndPhi = std::stod(next());
		G4double StartingTheta = std::stod(next());
		G4double EndTheta = std::stod(next());	
		G4String AngleUnit = next();

		//Assigen them the correct unit
		if( AngleUnit == "deg")
		{
			StartingPhi = StartingPhi*deg;
			EndPhi = EndPhi*deg;
			StartingTheta = StartingTheta*deg;
			EndTheta = EndTheta*deg;	
		}
		else if( AngleUnit == "rad")
		{
			StartingPhi = StartingPhi*rad;
			EndPhi = EndPhi*rad;
			StartingTheta = StartingTheta*rad;
			EndTheta = EndTheta*rad;	
		}
		else
		{
			G4cout << G4endl << "***** INVALID INPUT </Target/Cube/Dimensions> *****" <<
			G4endl << "***** Batch is interrupted!! *****" << G4endl;
			exit(1);
		}

		//Turn the variables into an array and append to the dimensions vector
		std::vector<G4double> Array = {innerRadius, outerRadius, StartingPhi, EndPhi, StartingTheta, EndTheta};
		TC -> AddDimensions(Array);
		TC -> AddTypeOfObjects("Sphere" + std::to_string(ObjectCounter));
		++ObjectCounter;
	}
	else if(command == CylinderDimensions_Cmd)
	{
		//Allows for multiple inputs for complex commands, seperates the variables out
		G4Tokenizer next(newValue);
		
		//Define each variable with in the string for length
		G4double innerRadius = std::stod(next());
		G4double outerRadius = std::stod(next());
		G4double length = std::stod(next())/2;
		G4String RadiusUnit = next();

		//Assign them the correct unit
		if( RadiusUnit == "mm")
		{
			innerRadius = innerRadius*mm;
			outerRadius = outerRadius*mm;
			length = length*mm;
		}
		else if( RadiusUnit == "cm")
		{
			innerRadius = innerRadius*cm;
			outerRadius = outerRadius*cm;
			length = length*cm;
		}
		else if ( RadiusUnit == "m")
		{
			innerRadius = innerRadius*m;
			outerRadius = outerRadius*m;
			length = length*m;
		}
		else 
		{
			G4cout << G4endl << "***** INVALID INPUT </Target/Cylinder/Dimensions>*****" <<
			G4endl << "***** Batch is interrupted!! *****" << G4endl;
			exit(1);
		}

		//Define each variable with in the string for angle
		G4double StartingPhi = std::stod(next());
		G4double EndPhi = std::stod(next());
		G4String AngleUnit = next();

		//Assigen them the correct unit
		if( AngleUnit == "deg")
		{
			StartingPhi = StartingPhi*deg;
			EndPhi = EndPhi*deg;
		}
		else if( AngleUnit == "rad")
		{
			StartingPhi = StartingPhi*rad;
			EndPhi = EndPhi*rad;
		}
		else
		{
			G4cout << G4endl << "***** INVALID INPUT </Target/Cylinder/Dimensions> *****" <<
			G4endl << "***** Batch is interrupted!! *****" << G4endl;
			exit(1);
		}

		//Turn the variables into an array and append to the dimensions vector
		std::vector<G4double> Array = {innerRadius, outerRadius, length, StartingPhi, EndPhi};
		TC -> AddDimensions(Array);
		TC -> AddTypeOfObjects("Cylinder" + std::to_string(ObjectCounter));
		++ObjectCounter;
	}
	else if(command == HollowCubeDimensions_Cmd)
	{
		//Allows for multiple inputs for complex commands, seperates the variables out
		G4Tokenizer next(newValue);
		
		//Define each variable with in the string for length
		G4double outerX = std::stod(next())/2;
		G4double outerY = std::stod(next())/2;
		G4double outerZ = std::stod(next())/2;

		G4double innerX = std::stod(next())/2;
		G4double innerY = std::stod(next())/2;
		G4double innerZ = std::stod(next())/2;

		G4String Unit = next();

		//Assign them the correct unit
		if( Unit == "mm")
		{
			outerX = outerX*mm;
			outerY = outerY*mm;
			outerZ = outerZ*mm;

			innerX = innerX*mm;
			innerY = innerY*mm;
			innerZ = innerZ*mm;
		}
		else if( Unit == "cm")
		{
			outerX = outerX*cm;
			outerY = outerY*cm;
			outerZ = outerZ*cm;

			innerX = innerX*cm;
			innerY = innerY*cm;
			innerZ = innerZ*cm;
		}
		else if ( Unit == "m")
		{
			outerX = outerX*m;
			outerY = outerY*m;
			outerZ = outerZ*m;

			innerX = innerX*m;
			innerY = innerY*m;
			innerZ = innerZ*m;
		}
		else 
		{
			G4cout << G4endl << "***** INVALID INPUT </Target/HollowCube/Dimensions>*****" <<
			G4endl << "***** Batch is interrupted!! *****" << G4endl;
			exit(1);
		}

		//Turn the variables into an array and append to the dimensions vector
		std::vector<G4double> Array = {outerX, outerY, outerZ, innerX, innerY, innerZ};
		TC -> AddDimensions(Array);
		TC -> AddTypeOfObjects("HollowCube" + std::to_string(ObjectCounter));
		++ObjectCounter;
	}
	else if(command == CubeDimensions_Cmd)
	{
		//Convert the input to a standard vector to be compatible with the dimensions vector 
		G4ThreeVector Dimensions = CubeDimensions_Cmd -> GetNew3VectorValue(newValue);
		std::vector<G4double> Array = {Dimensions.x(), Dimensions.y(), Dimensions.z()};

		//Turn the variables into an array and append to the dimensions vector
		TC -> AddDimensions(Array);
		TC -> AddTypeOfObjects("Cube" + std::to_string(ObjectCounter));	
		G4cout << "TargetConstruction -> AddCubeDimensions command detected "<< G4endl;
		++ObjectCounter;
	}
	else if(command == TargetPosition_Cmd)
	{
		G4ThreeVector Position = TargetPosition_Cmd -> GetNew3VectorValue(newValue);
		TC -> AddVectorPosition(Position);	
		G4cout << "TargetConstruction -> AddTargetPosition command detected "<< G4endl;
	}
	else if(command == TargetRotation_Cmd)
	{
		G4ThreeVector Rotation = TargetRotation_Cmd -> GetNew3VectorValue(newValue);
		TC -> AddVectorRotation(Rotation);	
		G4cout << "TargetConstruction -> AddTargetRotation command detected "<< G4endl;
	}
	else if(command == TargetMaterial_Cmd )
	{
		TC -> AddMaterial(newValue);	
		G4cout << "TargetConstruction -> AddTargetMaterial command detected "<< G4endl;
	}
	else if(command == BooleanOp_Cmd )
	{
		TC -> AddBooleanOp(newValue);	
		G4cout << "TargetConstruction -> AddBooleanOp command detected "<< G4endl;
	}
	else if(command == OverlapCheck_Cmd)
	{
		TC -> SetOverlapCheck(OverlapCheck_Cmd -> GetNewBoolValue(newValue));
		G4cout << "TargetConstruction -> SetOverlapCheck command detected " << G4endl;
	}
	else if(command == FullRotationAngle_Cmd)
	{
		FullRotationAngle_Cmd -> GetNewUnitValue(newValue);
		TC -> SetFullRotationAngle(FullRotationAngle_Cmd -> GetNewDoubleValue(newValue));
		G4cout << "TargetConstruction -> FullRotationAngle_Cmd command detected " << G4endl;
	}
	else if(command == OffSetRadius_Cmd )
	{
		OffSetRadius_Cmd -> GetNewUnitValue(newValue);
		TC -> SetOffSetRadius(OffSetRadius_Cmd -> GetNewDoubleValue(newValue));	
		G4cout << "TargetConstruction -> OffSetRadius_Cmd command detected "<< G4endl;
	}
	else if(command == Centre_Cmd)
	{
		G4ThreeVector Centre = Centre_Cmd -> GetNew3VectorValue(newValue);
		TC -> SetCentrePosition(Centre);	
		G4cout << "TargetConstruction -> SetRadius command detected "<< G4endl;
	}
}
