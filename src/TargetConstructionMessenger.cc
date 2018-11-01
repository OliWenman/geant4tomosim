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
	SubtractionSolid_Cmd -> SetGuidance("Choose two solids to be subtracted to create a new shape");
	
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
	OffSetRadius_Cmd -> SetUnitCategory("Length");
	OffSetRadius_Cmd -> SetDefaultUnit("cm");
	OffSetRadius_Cmd -> SetDefaultValue(0.5*cm);

	//Command to set the centre of this rotation if there is an offset
	Centre_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/OffSet/Centre", this);
	Centre_Cmd -> SetGuidance("Set the centre for the target to rotate round");
	Centre_Cmd -> SetUnitCandidates("um mm cm m ");
	Centre_Cmd -> SetDefaultUnit("m");
	Centre_Cmd -> SetDefaultValue(G4ThreeVector(0.0*m, 0.0*m, 0.0*m));

	//Create a dictionary for units
	mapOfUnits.insert(std::make_pair("m",m));
	mapOfUnits.insert(std::make_pair("cm",cm));
	mapOfUnits.insert(std::make_pair("mm",mm));
	mapOfUnits.insert(std::make_pair("um",um));
	mapOfUnits.insert(std::make_pair("nm",nm));

	mapOfUnits.insert(std::make_pair("rad",rad));
	mapOfUnits.insert(std::make_pair("deg",deg));
}

TargetConstructionMessenger::~TargetConstructionMessenger()
{
	delete TargetDirectory;

	delete CubeDimensions_Cmd;
	delete SphereDimensions_Cmd;
	delete CylinderDimensions_Cmd;
	delete HollowCubeDimensions_Cmd;
	delete SubtractionSolid_Cmd;

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

void TargetConstructionMessenger::AppendVectors(G4String ObjectName, std::vector<double> Array)
{
	//Append to all needed vectors to make them the same length. Fill as empty until filled with a variable
	TC -> AddDimensions(Array);
	TC -> AddTypeOfObjects(ObjectName);
	TC -> AddLogicVolumeArray(false);
	TC -> AddMaterial("EMPTY");
	TC -> AddVectorRotation(G4ThreeVector(0,0,0));
	TC -> AddVectorPosition(G4ThreeVector(0,0,0));

	++ObjectCounter;
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

		innerRadius = innerRadius*mapOfUnits[RadiusUnit];
		outerRadius = outerRadius*mapOfUnits[RadiusUnit];

		//Define each variable with in the string for angle
		G4double StartingPhi = std::stod(next());
		G4double EndPhi = std::stod(next());
		G4double StartingTheta = std::stod(next());
		G4double EndTheta = std::stod(next());	
		G4String AngleUnit = next();

		//Multiply the values by the correct unit
		StartingPhi = StartingPhi*mapOfUnits[AngleUnit];
		EndPhi = EndPhi*mapOfUnits[AngleUnit];
		StartingTheta = StartingTheta*mapOfUnits[AngleUnit];
		EndTheta = EndTheta*mapOfUnits[AngleUnit];

		//Turn the variables into an array and append to the dimensions vector
		std::vector<G4double> Array = {innerRadius, outerRadius, StartingPhi, EndPhi, StartingTheta, EndTheta};
		
		//Turn the variables into an array and append to the dimensions vector and all other vectors
		AppendVectors("Sphere", Array);
	}
	else if(command == CylinderDimensions_Cmd)
	{
		//Allows for multiple inputs for complex commands, seperates the variables out
		G4Tokenizer next(newValue);
		
		//Define each variable with in the string for length
		G4double innerRadius = std::stod(next());
		G4double outerRadius = std::stod(next());
		G4double length = std::stod(next());
		G4String RadiusUnit = next();

		//Multiply the values by the correct unit
		innerRadius = innerRadius*mapOfUnits[RadiusUnit];
		outerRadius = outerRadius*mapOfUnits[RadiusUnit];
		length = length*mapOfUnits[RadiusUnit];
		
		//Define each variable with in the string for angle
		G4double StartingPhi = std::stod(next());
		G4double EndPhi = std::stod(next());
		G4String AngleUnit = next();

		//Multiply the values by the correct unit
		StartingPhi = StartingPhi*mapOfUnits[AngleUnit];
		EndPhi = EndPhi*mapOfUnits[AngleUnit];
		
		//Turn the variables into an array and append to the dimensions vector
		std::vector<G4double> Array = {innerRadius, outerRadius, length, StartingPhi, EndPhi};
		
		//Turn the variables into an array and append to the dimensions vector and all other vectors
		AppendVectors("Cylinder", Array);
	}
	else if(command == HollowCubeDimensions_Cmd)
	{
		//Allows for multiple inputs for complex commands, seperates the variables out
		G4Tokenizer next(newValue);
		
		//Define each variable with in the string for length
		G4double outerX = std::stod(next());
		G4double outerY = std::stod(next());
		G4double outerZ = std::stod(next());

		G4double innerX = std::stod(next());
		G4double innerY = std::stod(next());
		G4double innerZ = std::stod(next());

		G4String Unit = next();

		//Multiply the values by the correct unit
		outerX = outerX*mapOfUnits[Unit];
		outerY = outerY*mapOfUnits[Unit];
		outerZ = outerZ*mapOfUnits[Unit];

		innerX = innerX*mapOfUnits[Unit];
		innerY = innerY*mapOfUnits[Unit];
		innerZ = innerZ*mapOfUnits[Unit];

		//Turn the variables into an array and append to the dimensions vector
		std::vector<G4double> Array = {outerX, outerY, outerZ, innerX, innerY, innerZ};
		
		//Turn the variables into an array and append to the dimensions vector and all other vectors
		AppendVectors("HollowCube", Array);
	}
	else if(command == CubeDimensions_Cmd)
	{
		//Convert the input to a standard vector to be compatible with the dimensions vector 
		G4ThreeVector Dimensions = CubeDimensions_Cmd -> GetNew3VectorValue(newValue);
		std::vector<G4double> Array = {Dimensions.x(), Dimensions.y(), Dimensions.z()};

		//Turn the variables into an array and append to the dimensions vector and all other vectors
		AppendVectors("Cube", Array);
	}
	else if(command == SubtractionSolid_Cmd)
	{
		TC -> AddSubtractObject(newValue);
		std::vector<G4double> Array; 
		AppendVectors("SubtractSolid", Array);
	}
	else if(command == TargetPosition_Cmd)
	{
		G4ThreeVector Position = TargetPosition_Cmd -> GetNew3VectorValue(newValue);
		TC -> SetVectorPosition(ObjectCounter -1, Position);	
		G4cout << "TargetConstruction -> AddTargetPosition command detected "<< G4endl;
	}
	else if(command == TargetRotation_Cmd)
	{
		G4ThreeVector Rotation = TargetRotation_Cmd -> GetNew3VectorValue(newValue);
		TC -> SetVectorRotation(ObjectCounter -1, Rotation);	
		G4cout << "TargetConstruction -> AddTargetRotation command detected "<< G4endl;
	}
	else if(command == TargetMaterial_Cmd )
	{
		TC -> SetMaterial(ObjectCounter -1, newValue);
		TC -> SetLogicVolumeArray(ObjectCounter - 1, true);
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
