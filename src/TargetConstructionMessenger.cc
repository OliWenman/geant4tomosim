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

#include "CustomObject.hh"
#include <vector>

TargetConstructionMessenger::TargetConstructionMessenger(TargetConstruction* Target): G4UImessenger(), TC(Target)
{	
	//Directory
	TargetDirectory = new G4UIdirectory("/Target/");
	TargetDirectory -> SetGuidance("Commands to build your sample. ");

//=================================================================================================
//  DIFFERENT OBJECT SHAPES

	//Command to set the dimensions of a sphere
	SphereDimensions_Cmd = new G4UIcmdWithAString("/Target/Sphere/Dimensions", this);
	SphereDimensions_Cmd -> SetGuidance("Set the dimensions of a sphere you would like: \n"
	                                    "Name innerRadius outerRadius lengthUnit startPh endPhi startTheta endTheta angleUnit. ");

	//Command to set the dimensions of a sphere
	CylinderDimensions_Cmd = new G4UIcmdWithAString("/Target/Cylinder/Dimensions", this);
	CylinderDimensions_Cmd -> SetGuidance("Set the dimensions of a cylinder that you would like: \n" 
	                                      "Name innerRadius outerRadius height lengthUnit startPhi endPhi angleUnit");

	//Command to set the dimensions of a cube
	CubeDimensions_Cmd = new G4UIcmdWithAString("/Target/Cube/Dimensions", this);
	CubeDimensions_Cmd -> SetGuidance("Set the dimensions of a cube: \n"
	                                  "Name x y z lengthUnit");

	//Command to set the dimensions of a trapezoid
	TrapezoidDimensions_Cmd = new G4UIcmdWithAString("/Target/Trapezoid/Dimensions", this);
	TrapezoidDimensions_Cmd -> SetGuidance("Set the dimensions of a trapezoid you would like: \n" 
	                                       "Name dx1 dx2 dy1 dy2 dz lengthUnit");

	//Command to set the dimensions of a ellipsoid
	EllipsoidDimensions_Cmd = new G4UIcmdWithAString("/Target/Ellipsoid/Dimensions", this);
	EllipsoidDimensions_Cmd -> SetGuidance("Set the dimensions of a ellipsoid you would like: \n"
	                                       "Name pxSemiAxis pySemiAxis pzSemiAxis pzBottomCut pzTopCut lengthUnit");

//=================================================================================================
//  SUBTRACT SOLID COMMANDS

	//Command to set the dimensions of subtraction solid
	SubtractionSolid_Cmd = new G4UIcmdWithAString("/Target/SubtractSolid", this);
	SubtractionSolid_Cmd -> SetGuidance("Choose two solids to be subtracted to create a new shape: \n"
	                                    "Name ComponenentName1 ComponenetName2 x y z lengthUnit xTheta yTheta zTheta angleUnit");
	                                    
    UnionSolid_Cmd = new G4UIcmdWithAString("/Target/UnionSolid", this);
    UnionSolid_Cmd -> SetGuidance("Choose two solids to be joinded together to create a new shape: \n"
	                              "Name ComponenentName1 ComponenetName2 x y z lengthUnit xTheta yTheta zTheta angleUnit");

//=================================================================================================
//  POSITION, ROTATION AND MATERIAL COMMANDS	

	//Command to set the position of an object
	TargetPosition_Cmd = new G4UIcmdWithAString("/Target/Position", this);
	TargetPosition_Cmd -> SetGuidance("Set the position of a solid: \n"
	                                  "Name x y z lengthUnit");

	//Command to set the rotation of an object
	TargetRotation_Cmd = new G4UIcmdWithAString("/Target/Rotation", this);
	TargetRotation_Cmd -> SetGuidance("Set the starting rotation of a solid: \n "
	                                  "Name xTheta yTheta zTheta angleUnit");

	//Command to set the material of an object
	TargetMaterial_Cmd = new G4UIcmdWithAString("/Target/Material", this);
	TargetMaterial_Cmd -> SetGuidance("Set the material of a solid: \n" 
	                                  "Name Material (Material must already exist in the database)");

//=================================================================================================
//  OVERLAP CHECKING COMMAND

	//Command to set any boolean operaions for the object
	BooleanOp_Cmd = new G4UIcmdWithABool("/Target/VolumeExclusion", this);
	BooleanOp_Cmd -> SetGuidance("Set the boolean operation for the physical volume of the object");
	BooleanOp_Cmd -> SetDefaultValue(false);

	//Command to check for overlaps in the geometry
	OverlapCheck_Cmd = new G4UIcmdWithABool("/Target/CheckAllOverlaps", this);
	OverlapCheck_Cmd -> SetGuidance("Choose if you would like the volumes to check for overlaps");
	OverlapCheck_Cmd -> SetDefaultValue(false);

//=================================================================================================
//  OBJECT IMAGE AND ROTATION COMMANDS

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

//=================================================================================================
//  UNIT DICTIONARY

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
	delete TrapezoidDimensions_Cmd;
	delete EllipsoidDimensions_Cmd;
	delete SubtractionSolid_Cmd;
	delete UnionSolid_Cmd;

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

void TargetConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command == SphereDimensions_Cmd)
	{
	    G4String Type = "Sphere";
	
		//Allows for multiple inputs for complex commands, seperates the variables out
		G4Tokenizer next(newValue);
		G4String Name = CheckName(next, command, newValue);
		
		//Define each variable with in the string for length
		G4double innerRadius = ConvertToDouble(next, newValue, command);
		G4double outerRadius = ConvertToDouble(next, newValue, command);
		G4double RadiusUnit = CheckUnits(next, command, newValue, "Length");
		innerRadius = innerRadius*RadiusUnit;
		outerRadius = outerRadius*RadiusUnit;

		//Define each variable with in the string for angle
		G4double StartingPhi = ConvertToDouble(next, newValue, command);
		G4double EndPhi = ConvertToDouble(next, newValue, command);
		G4double StartingTheta = ConvertToDouble(next, newValue, command);
		G4double EndTheta = ConvertToDouble(next, newValue, command);
		G4double AngleUnit = CheckUnits(next, command, newValue, "Length");

		//Multiply the values by the correct unit
		StartingPhi = StartingPhi*AngleUnit;
		EndPhi = EndPhi*AngleUnit;
		StartingTheta = StartingTheta*AngleUnit;
		EndTheta = EndTheta*AngleUnit;

		//Turn the variables into an array and append to the dimensions vector
		std::vector<G4double> Dimensions = {innerRadius, outerRadius, StartingPhi, EndPhi, StartingTheta, EndTheta};
		
		TC -> CreateObject(Name, Type, Dimensions);
	}
	else if(command == CylinderDimensions_Cmd)
	{
	    G4String Type = "Cylinder"; 
	
		//Allows for multiple inputs for complex commands, seperates the variables out
		G4Tokenizer next(newValue);
		G4String Name = next();
		
		//Define each variable with in the string for length
		G4double innerRadius = ConvertToDouble(next, newValue, command);
		G4double outerRadius = ConvertToDouble(next, newValue, command);
		G4double length = ConvertToDouble(next, newValue, command);
		G4double RadiusUnit = CheckUnits(next, command, newValue, "Length");
		//Multiply the values by the correct unit
		innerRadius = innerRadius*RadiusUnit;
		outerRadius = outerRadius*RadiusUnit;
		length = length*RadiusUnit;
		
		//Define each variable with in the string for angle
		G4double StartingPhi = ConvertToDouble(next, newValue, command);
		G4double EndPhi = ConvertToDouble(next, newValue, command);
		G4double AngleUnit = CheckUnits(next, command, newValue, "Angle");
		//Multiply the values by the correct unit
		StartingPhi = StartingPhi*AngleUnit;
		EndPhi = EndPhi*AngleUnit;
		
		//Turn the variables into an array and append to the dimensions vector
		std::vector<G4double> Dimensions = {innerRadius, outerRadius, length, StartingPhi, EndPhi};
		
		TC -> CreateObject(Name, Type, Dimensions);
	}
	else if(command == CubeDimensions_Cmd)
	{
		G4String Type = "Cube";
		
		G4Tokenizer next(newValue);
		G4String Name = CheckName(next, command, newValue);
		
		G4double x = ConvertToDouble(next, newValue, command);
		G4double y = ConvertToDouble(next, newValue, command);
		G4double z = ConvertToDouble(next, newValue, command);
		G4double Unit = CheckUnits(next, command, newValue, "Length");
		
		x = x*Unit; 
		y = y*Unit; 
		z = z*Unit;
		
		//Turn the variables into an array and append to the dimensions vector
		std::vector<G4double> Dimensions = {x, y, z};
		
		TC -> CreateObject(Name, Type, Dimensions);
	}
	else if(command == TrapezoidDimensions_Cmd)
	{
	    G4String Type = "Trapezoid";
	    
		G4Tokenizer next(newValue);
		G4String Name = CheckName(next, command, newValue);
		
		G4double dx1 = ConvertToDouble(next, newValue, command);
		G4double dx2 = ConvertToDouble(next, newValue, command);
		G4double dy1 = ConvertToDouble(next, newValue, command);
		G4double dy2 = ConvertToDouble(next, newValue, command);
		G4double dz  = ConvertToDouble(next, newValue, command);
		G4double Unit = CheckUnits(next, command, newValue, "Length");

		dx1 = dx1*Unit;
		dx2 = dx2*Unit;
		dy1 = dy1*Unit;
		dy2 = dy2*Unit;
		dz  = dz*Unit;
		
		std::vector<G4double> Dimensions = {dx1, dx2, dy1, dy2, dz};

        TC -> CreateObject(Name, Type, Dimensions);
	}
	else if(command ==  EllipsoidDimensions_Cmd)
	{
	    G4String Type = "Ellipsoid";
	
		G4Tokenizer next(newValue);

        G4String Name = CheckName(next, command, newValue);

		G4double pxSemiAxis = ConvertToDouble(next, newValue, command);
        G4double pySemiAxis = ConvertToDouble(next, newValue, command);
        G4double pzSemiAxis = ConvertToDouble(next, newValue, command);
        G4double pzBottomCut = ConvertToDouble(next, newValue, command);
        G4double pzTopCut = ConvertToDouble(next, newValue, command);
		G4double Unit = CheckUnits(next, command, newValue, "Length");

		pxSemiAxis = pxSemiAxis * Unit;
		pySemiAxis = pySemiAxis * Unit;
		pzSemiAxis = pzSemiAxis * Unit;
		pzBottomCut = pzBottomCut * Unit;
		pzTopCut = pzTopCut * Unit;

		std::vector<G4double> Dimensions = {pxSemiAxis, pySemiAxis, pzSemiAxis, pzBottomCut, pzTopCut};
		
		TC -> CreateObject(Name, Type, Dimensions);
	}

	else if(command == SubtractionSolid_Cmd)
	{
	    G4String type = "SubtractSolid";
	    
		//From the string, be able to take the needed paramenters out as seperate variables
	    G4Tokenizer next(newValue);
    
        G4String Name = CheckName(next, command, newValue);
    
	    //Names of the two objects being used
	    G4String OuterObject = next();
	    G4String InnerObject= next();
    
	    //Find the positions of the inner object inside the string 
	    G4double x = ConvertToDouble(next, newValue, command);
	    G4double y = ConvertToDouble(next, newValue, command);
	    G4double z = ConvertToDouble(next, newValue, command);
	    G4double LengthUnit = CheckUnits(next, command, newValue, "Length");
	    x = x * LengthUnit; y = y * LengthUnit; z = z * LengthUnit;
	    G4ThreeVector innerPosition = G4ThreeVector(x, y, z); 

	    //Find the rotation of the inner object insde the string
	    G4double Rotx = ConvertToDouble(next, newValue, command); 
	    G4double Roty = ConvertToDouble(next, newValue, command);; 
	    G4double Rotz = ConvertToDouble(next, newValue, command);;
	    G4double AngleUnit = CheckUnits(next, command, newValue, "Angle");
	    Rotx = Rotx * AngleUnit; Roty = Roty * AngleUnit; Rotz = Rotz * AngleUnit;
	    G4ThreeVector innerRotation = G4ThreeVector(Rotx, Roty, Rotz);
	    
	    TC -> CreateObject(Name, OuterObject, InnerObject, type, innerPosition, innerRotation);
	}
	else if(command == UnionSolid_Cmd)
	{
	    G4String type = "UnionSolid";
	    
		//From the string, be able to take the needed paramenters out as seperate variables
	    G4Tokenizer next(newValue);
    
        G4String Name = CheckName(next, command, newValue);
    
	    //Names of the two objects being used
	    G4String OuterObject = next();
	    G4String InnerObject= next();
    
	    //Find the positions of the inner object inside the string 
	    G4double x = ConvertToDouble(next, newValue, command);
	    G4double y = ConvertToDouble(next, newValue, command);
	    G4double z = ConvertToDouble(next, newValue, command);
	    G4double LengthUnit = CheckUnits(next, command, newValue, "Length");
	    x = x * LengthUnit; y = y * LengthUnit; z = z * LengthUnit;
	    G4ThreeVector innerPosition = G4ThreeVector(x, y, z); 

	    //Find the rotation of the inner object insde the string
	    G4double Rotx = ConvertToDouble(next, newValue, command); 
	    G4double Roty = ConvertToDouble(next, newValue, command);; 
	    G4double Rotz = ConvertToDouble(next, newValue, command);;
	    G4double AngleUnit = CheckUnits(next, command, newValue, "Angle");
	    Rotx = Rotx * AngleUnit; Roty = Roty * AngleUnit; Rotz = Rotz * AngleUnit;
	    G4ThreeVector innerRotation = G4ThreeVector(Rotx, Roty, Rotz);
	    
	    TC -> CreateObject(Name, OuterObject, InnerObject, type, innerPosition, innerRotation);
	} 
	else if(command == TargetPosition_Cmd)
	{
		G4Tokenizer next(newValue);
		G4String Name = next();
		
		G4double x = ConvertToDouble(next, newValue, command); 
		G4double y = ConvertToDouble(next, newValue, command);
		G4double z = ConvertToDouble(next, newValue, command);
		G4double Unit = CheckUnits(next, command, newValue, "Length");
		
		x = x*Unit; y = y*Unit; z = z*Unit;
		
		TC -> AddPosition(Name, G4ThreeVector(x, y, z));
	}
	else if(command == TargetRotation_Cmd)
	{
		G4Tokenizer next(newValue);
		G4String Name = next();
		
		G4double x = ConvertToDouble(next, newValue, command);
		G4double y = ConvertToDouble(next, newValue, command);
		G4double z = ConvertToDouble(next, newValue, command);
		G4double Unit = CheckUnits(next, command, newValue, "Angle");
		
		x = x*Unit; y = y*Unit; z = z*Unit;
		TC -> AddRotation(Name, G4ThreeVector(x, y, z));
	}
	else if(command == TargetMaterial_Cmd )
	{
	    G4Tokenizer next(newValue);
	    
	    G4String Name = next();
	    G4String Material = next();
	    
	    TC -> AddMaterial(Name, Material);
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
	else
	{
	    G4cout << "\nError: command not found!" << G4endl;
	    exit(1);
	}
}

G4double TargetConstructionMessenger::ConvertToDouble(G4Tokenizer &next, G4String input, G4UIcommand* command)
{
    G4double output;
    
    try
    {   output = std::stod(next());} 
        
    catch (const std::invalid_argument& ia)
    {
        G4cout << "\nERROR: Invalid input for " << command -> GetCommandPath() << " " << input
               << "\nGuidance: " << command -> GetGuidanceLine(0) << G4endl;
        exit(1);
    }
    
    return output;
}

G4double TargetConstructionMessenger::CheckUnits(G4Tokenizer &next, G4UIcommand* command, G4String newValue, G4String TypeOfUnit)
{
    G4String UnitString = next();
    
    bool LengthUnit = MapLengthUnits.count(UnitString);
    bool AngleUnit = MapAngleUnits.count(UnitString); 
    
    if (LengthUnit == false && AngleUnit == false )
    {
        G4cout << "\nERROR: " << command -> GetCommandPath() << " " << newValue << " -> Invalid ";
        if (TypeOfUnit == "Length")
            G4cout << "length unit!\nGuidance: ";
        else if (TypeOfUnit == "Angle")
            G4cout << "angle unit!\nGuidance: ";
            
        G4cout << command -> GetGuidanceLine(0) << "\n\nAvailable units \nLength: ";
        
        for (std::map<std::string, double>::iterator it = MapLengthUnits.begin(); it != MapLengthUnits.end(); ++it)
             G4cout << it -> first << " ";
             
        G4cout << "\nAngle: ";
        
        for (std::map<std::string, double>::iterator it = MapAngleUnits.begin(); it != MapAngleUnits.end(); ++it)
             G4cout << it -> first << " ";
        
        G4cout << G4endl;
        
        exit(1);
    }
    
    if (LengthUnit == true)
        return MapLengthUnits[UnitString];

    else if (AngleUnit == true)
        return MapAngleUnits[UnitString];
}

G4String TargetConstructionMessenger::CheckName(G4Tokenizer &next, G4UIcommand* command, G4String newValue)
{
    G4String Name = next();
    std::vector<CustomObject> ObjectDatabase = TC -> GetObjectDatabase();
    
    for (int n = 0 ; n < ObjectDatabase.size() ; n++)
    {   if (ObjectDatabase[n].Name == Name)
        {
            G4cout << "\nERROR: " << command -> GetCommandPath() << " " << newValue << " -> Object name \"" << Name << "\" already exists. Please pick another. " << G4endl;
            exit(1);
        }
    }
    
    return Name;
}
