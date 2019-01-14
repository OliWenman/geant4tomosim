#include "TargetConstructionMessenger.hh"
#include "TargetConstruction.hh"

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
	ObjectCounter = 0;

	//Directory
	TargetDirectory = new G4UIdirectory("/Target/");
	TargetDirectory -> SetGuidance("Commands to control the detector variables. ");

	//Command to set the dimensions of a sphere
	SphereDimensions_Cmd = new G4UIcmdWithAString("/Target/Sphere/Dimensions", this);
	SphereDimensions_Cmd -> SetGuidance("Set the dimensions of a sphere you would like, inner radius, outer radius, unit, starting phi angle, delta phi angle, starting theta angle, delta theta angle and unit. ");

	//Command to set the dimensions of a sphere
	CylinderDimensions_Cmd = new G4UIcmdWithAString("/Target/Cylinder/Dimensions", this);
	CylinderDimensions_Cmd -> SetGuidance("Set the dimensions of a cylinder that you would like, inner radius, outer radius, length, delta phi angle and ending phi angle");

	//Command to set the dimensions of a hollow cube
	HollowCubeDimensions_Cmd = new G4UIcmdWithAString("/Target/HollowCube/Dimensions", this);
	HollowCubeDimensions_Cmd -> SetGuidance("Set the dimensions of a sphere you would like, outer x, y, z, unit, inner x, y, z unit");

	//Command to set the dimensions of a cube
	CubeDimensions_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/Cube/Dimensions", this);
	CubeDimensions_Cmd -> SetGuidance("Set the dimensions of a cube for x, y and z. ");
	CubeDimensions_Cmd -> SetUnitCandidates("mm cm m ");
	CubeDimensions_Cmd -> SetDefaultUnit("mm");
	CubeDimensions_Cmd -> SetDefaultValue(G4ThreeVector(0.3*mm, 0.3*mm, 0.0*mm));

	//Command to set the dimensions of a trapezoid
	TrapezoidDimensions_Cmd = new G4UIcmdWithAString("/Target/Trapezoid/Dimensions", this);
	TrapezoidDimensions_Cmd -> SetGuidance("Set the dimensions of a trapezoid you would like, dx1, dx2, dy1, dy2 and dz");

	//Command to set the dimensions of a ellipsoid
	EllipsoidDimensions_Cmd = new G4UIcmdWithAString("/Target/Ellipsoid/Dimensions", this);
	EllipsoidDimensions_Cmd -> SetGuidance("Set the dimensions of a ellipsoid you would like, pxSemiAxis, pySemiAxis, pzSemiAxis, pzBottomCut and pzTopCut");

	//Command to set the dimensions of subtraction solid
	SubtractionSolid_Cmd = new G4UIcmdWithAString("/Target/SubtractSolid", this);
	SubtractionSolid_Cmd -> SetGuidance("Choose two solids to be subtracted to create a new shape.\nName of solid1, name of solid 2, position vector within solid1 and unit, rotation vector of solid2 inside solid 1 and unit");
	
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

    CalibrationImages_Cmd = new G4UIcmdWithAnInteger("/data/CalibrateImages", this);
    

	//Command to set the centre of this rotation if there is an offset
	Centre_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/OffSet/Centre", this);
	Centre_Cmd -> SetGuidance("Set the centre for the target to rotate round");
	Centre_Cmd -> SetUnitCandidates("um mm cm m ");
	Centre_Cmd -> SetDefaultUnit("m");
	Centre_Cmd -> SetDefaultValue(G4ThreeVector(0.0*m, 0.0*m, 0.0*m));

	//Create a dictionary for units
	MapLengthUnits.insert(std::make_pair("m",m));
	MapLengthUnits.insert(std::make_pair("cm",cm));
	MapLengthUnits.insert(std::make_pair("mm",mm));
	MapLengthUnits.insert(std::make_pair("um",um));
	MapLengthUnits.insert(std::make_pair("nm",nm));
	
	MapAngleUnits.insert(std::make_pair("rad", rad));
	MapAngleUnits.insert(std::make_pair("deg", deg));
}

TargetConstructionMessenger::~TargetConstructionMessenger()
{
	delete TargetDirectory;

	delete CubeDimensions_Cmd;
	delete SphereDimensions_Cmd;
	delete CylinderDimensions_Cmd;
	delete HollowCubeDimensions_Cmd;
	delete SubtractionSolid_Cmd;
	delete TrapezoidDimensions_Cmd;
	delete EllipsoidDimensions_Cmd;

	delete TargetPosition_Cmd;
	delete TargetRotation_Cmd;

	delete TargetMaterial_Cmd;
	delete BooleanOp_Cmd;
	delete OverlapCheck_Cmd;

	delete FullRotationAngle_Cmd;
	delete OffSetRadius_Cmd;
	delete Centre_Cmd;
	
	delete CalibrationImages_Cmd;
}

void TargetConstructionMessenger::CheckUnits(G4UIcommand* command, G4String newValue, G4String Unit, G4String TypeOfUnit)
{
    //Function to check if the units exist. If it doesn't, print an error and stop program
    if(TypeOfUnit == "Length" && MapLengthUnits.count(Unit) == 0)
    {
        G4cout << "\nERROR: " << command -> GetCommandPath() << " " << newValue << " -> Invalid length unit!\nGuidance: "
               << command -> GetGuidanceLine(0) << "\nAvailable units: ";
        
        for (std::map<std::string, double>::iterator it = MapLengthUnits.begin(); it != MapLengthUnits.end(); ++it)
            G4cout << it -> first << " ";
        G4cout << G4endl;
        
        exit(0);
        
    }
    else if (TypeOfUnit == "Angle" && MapAngleUnits.count(Unit) == 0)
    {
        G4cout << "\nERROR: " << command -> GetCommandPath() << " " << newValue << " -> Invalid angle unit!\nGuidance: "
               << command -> GetGuidanceLine(0) << "\nAvailable units: ";
        
        for (std::map<std::string, double>::iterator it = MapAngleUnits.begin(); it != MapAngleUnits.end(); ++it)
            G4cout << it -> first << " ";
        G4cout << G4endl;
        
        exit(0);
    }
}

void TargetConstructionMessenger:: CheckUnits(G4String command, G4String commandInput, G4String Unit, G4String TypeOfUnit)
{
    //Function to check if the units exist. If it doesn't, print an error and stop program
    if(TypeOfUnit == "Length" && MapLengthUnits.count(Unit) == 0)
    {
        G4String Guidance = "Choose two solids to be subtracted to create a new shape.\nName of solid1, name of solid 2, position vector within solid1 and unit, rotation vector of solid2 inside solid 1 and unit";
    
        G4cout << "\nERROR: " << command << " " << commandInput << " -> Invalid length unit!\nGuidance: "
               << Guidance << "\nAvailable units: ";
        
        for (std::map<std::string, double>::iterator it = MapLengthUnits.begin(); it != MapLengthUnits.end(); ++it)
            G4cout << it -> first << " ";
        G4cout << G4endl;
        
        exit(0);
        
    }
    else if (TypeOfUnit == "Angle" && MapAngleUnits.count(Unit) == 0)
    {
        G4String Guidance = "Choose two solids to be subtracted to create a new shape.\nName of solid1, name of solid 2, position vector within solid1 and unit, rotation vector of solid2 inside solid 1 and unit";
    
        G4cout << "\nERROR: " << command << " " << commandInput << " -> Invalid angle unit!\nGuidance: "
               << Guidance << "\nAvailable units: ";
        
        for (std::map<std::string, double>::iterator it = MapAngleUnits.begin(); it != MapAngleUnits.end(); ++it)
            G4cout << it -> first << " ";
        G4cout << G4endl;
        
        exit(0);
    }
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
		CheckUnits(command, newValue, RadiusUnit, "Length");

		innerRadius = innerRadius*MapLengthUnits[RadiusUnit];
		outerRadius = outerRadius*MapLengthUnits[RadiusUnit];

		//Define each variable with in the string for angle
		G4double StartingPhi = std::stod(next());
		G4double EndPhi = std::stod(next());
		G4double StartingTheta = std::stod(next());
		G4double EndTheta = std::stod(next());	
		G4String AngleUnit = next();
    	CheckUnits(command, newValue, AngleUnit, "Angle");

		//Multiply the values by the correct unit
		StartingPhi = StartingPhi*MapAngleUnits[AngleUnit];
		EndPhi = EndPhi*MapAngleUnits[AngleUnit];
		StartingTheta = StartingTheta*MapAngleUnits[AngleUnit];
		EndTheta = EndTheta*MapAngleUnits[AngleUnit];

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
		CheckUnits(command, newValue, RadiusUnit, "Length");

		//Multiply the values by the correct unit
		innerRadius = innerRadius*MapLengthUnits[RadiusUnit];
		outerRadius = outerRadius*MapLengthUnits[RadiusUnit];
		length = length*MapLengthUnits[RadiusUnit];
		
		//Define each variable with in the string for angle
		G4double StartingPhi = std::stod(next());
		G4double EndPhi = std::stod(next());
		G4String AngleUnit = next();
		CheckUnits(command, newValue, AngleUnit, "Angle");

		//Multiply the values by the correct unit
		StartingPhi = StartingPhi*MapAngleUnits[AngleUnit];
		EndPhi = EndPhi*MapAngleUnits[AngleUnit];
		
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
		CheckUnits(command, newValue, Unit, "Length");

		//Multiply the values by the correct unit
		outerX = outerX*MapLengthUnits[Unit];
		outerY = outerY*MapLengthUnits[Unit];
		outerZ = outerZ*MapLengthUnits[Unit];

		innerX = innerX*MapLengthUnits[Unit];
		innerY = innerY*MapLengthUnits[Unit];
		innerZ = innerZ*MapLengthUnits[Unit];

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
	else if(command == TrapezoidDimensions_Cmd)
	{
		G4Tokenizer next(newValue);
		
		G4double dx1 = std::stod(next());
		G4double dx2 = std::stod(next());
		G4double dy1 = std::stod(next());
		G4double dy2 = std::stod(next());
		G4double dz  = std::stod(next());
		G4String Unit = next();
		CheckUnits(command, newValue, Unit, "Length");

		dx1 = dx1*MapLengthUnits[Unit];
		dx2 = dx2*MapLengthUnits[Unit];
		dy1 = dy1*MapLengthUnits[Unit];
		dy2 = dy2*MapLengthUnits[Unit];
		dz  = dz*MapLengthUnits[Unit];
		
		std::vector<G4double> Array = {dx1, dx2, dy1, dy2, dz};

		AppendVectors("Trapezoid", Array);	
	}
	else if(command ==  EllipsoidDimensions_Cmd)
	{
		G4Tokenizer next(newValue);

		G4double pxSemiAxis = std::stod(next());
        G4double pySemiAxis = std::stod(next());
        G4double pzSemiAxis = std::stod(next());
        G4double pzBottomCut = std::stod(next());
        G4double pzTopCut = std::stod(next());
		G4String Unit = next();
		CheckUnits(command, newValue, Unit, "Length");

		pxSemiAxis = pxSemiAxis * MapLengthUnits[Unit];
		pySemiAxis = pySemiAxis * MapLengthUnits[Unit];
		pzSemiAxis = pzSemiAxis * MapLengthUnits[Unit];
		pzBottomCut = pzBottomCut * MapLengthUnits[Unit];
		pzTopCut = pzTopCut * MapLengthUnits[Unit];

		std::vector<G4double> Array = {pxSemiAxis, pySemiAxis, pzSemiAxis, pzBottomCut, pzTopCut};
	
		AppendVectors("Ellipsoid", Array); 
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
	}
	else if(command == TargetRotation_Cmd)
	{
		G4ThreeVector Rotation = TargetRotation_Cmd -> GetNew3VectorValue(newValue);
		TC -> SetVectorRotation(ObjectCounter -1, Rotation);	
	}
	else if(command == TargetMaterial_Cmd )
	{
		TC -> SetMaterial(ObjectCounter -1, newValue);
		TC -> SetLogicVolumeArray(ObjectCounter - 1, true);
	}
	else if(command == BooleanOp_Cmd )
	{
		TC -> AddBooleanOp(newValue);	
	}
	else if(command == OverlapCheck_Cmd)
	{
		TC -> SetOverlapCheck(OverlapCheck_Cmd -> GetNewBoolValue(newValue));
	}
	else if(command == FullRotationAngle_Cmd)
	{
		FullRotationAngle_Cmd -> GetNewUnitValue(newValue);
		TC -> SetFullRotationAngle(FullRotationAngle_Cmd -> GetNewDoubleValue(newValue));
	}
	else if(command == OffSetRadius_Cmd )
	{
		OffSetRadius_Cmd -> GetNewUnitValue(newValue);
		TC -> SetOffSetRadius(OffSetRadius_Cmd -> GetNewDoubleValue(newValue));	
	}
	else if(command == Centre_Cmd)
	{
		G4ThreeVector Centre = Centre_Cmd -> GetNew3VectorValue(newValue);
		TC -> SetCentrePosition(Centre);	
	}
	else if( command == CalibrationImages_Cmd)
	{
	    TC -> SetCalibrationImages(CalibrationImages_Cmd -> GetNewIntValue(newValue));
	}
}
