#include "SampleConstructionMessenger.hh"
#include "SampleConstruction.hh"

#include "G4SystemOfUnits.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

SampleConstructionMessenger::SampleConstructionMessenger(SampleConstruction* sc): G4UImessenger(), sampleconstruction(sc)
{	
//=================================================================================================
	//Directory
	directory = new G4UIdirectory("/Target/");
	directory -> SetGuidance("Commands to build your sample. ");

//=================================================================================================
//  DIFFERENT OBJECT SHAPES

	//Command to set the dimensions of a sphere
	build_sphere = new G4UIcmdWithAString("/Target/Sphere/dimensions", this);
	build_sphere -> SetGuidance("Set the dimensions of a sphere you would like: \n"
	                                    "name innerRadius outerRadius lengthUnit startPh endPhi startTheta endTheta angleUnit. ");

	//Command to set the dimensions of a sphere
	build_cylinder = new G4UIcmdWithAString("/Target/Cylinder/dimensions", this);
	build_cylinder -> SetGuidance("Set the dimensions of a cylinder that you would like: \n" 
	                                      "name innerRadius outerRadius height lengthUnit startPhi endPhi angleUnit");

	//Command to set the dimensions of a cube
	build_cube = new G4UIcmdWithAString("/Target/Cube/dimensions", this);
	build_cube -> SetGuidance("Set the dimensions of a cube: \n"
	                                  "name x y z lengthUnit");

	//Command to set the dimensions of a trapezoid
	build_trapezoid = new G4UIcmdWithAString("/Target/Trapezoid/dimensions", this);
	build_trapezoid -> SetGuidance("Set the dimensions of a trapezoid you would like: \n" 
	                                       "name dx1 dx2 dy1 dy2 dz lengthUnit");

	//Command to set the dimensions of a ellipsoid
	build_ellipsoid = new G4UIcmdWithAString("/Target/Ellipsoid/dimensions", this);
	build_ellipsoid -> SetGuidance("Set the dimensions of a ellipsoid you would like: \n"
	                                       "name pxSemiAxis pySemiAxis pzSemiAxis pzBottomCut pzTopCut lengthUnit");

//=================================================================================================
//  SUBTRACT SOLID COMMANDS

	//Command to set the dimensions of subtraction solid
	build_subtractsolid = new G4UIcmdWithAString("/Target/SubtractSolid", this);
	build_subtractsolid -> SetGuidance("Choose two solids to be subtracted to create a new shape: \n"
	                                    "name ComponenentName1 ComponenetName2 x y z lengthUnit xTheta yTheta zTheta angleUnit");
	                                    
    build_unionsolid = new G4UIcmdWithAString("/Target/UnionSolid", this);
    build_unionsolid -> SetGuidance("Choose two solids to be joinded together to create a new shape: \n"
	                              "name ComponenentName1 ComponenetName2 x y z lengthUnit xTheta yTheta zTheta angleUnit");

//=================================================================================================
//  POSITION, ROTATION AND MATERIAL COMMANDS	

	//Command to set the position of an object
	setsample_position = new G4UIcmdWithAString("/Target/Position", this);
	setsample_position -> SetGuidance("Set the position of a solid: \n"
	                                  "name x y z lengthUnit");

	//Command to set the rotation of an object
	setsample_rotation = new G4UIcmdWithAString("/Target/Rotation", this);
	setsample_rotation -> SetGuidance("Set the starting rotation of a solid: \n "
	                                  "name xTheta yTheta zTheta angleUnit");

	//Command to set the material of an object
	setsample_material = new G4UIcmdWithAString("/Target/Material", this);
	setsample_material -> SetGuidance("Set the material of a solid: \n" 
	                                  "name Material (Material must already exist in the database)");

//=================================================================================================
//  OVERLAP CHECKING COMMAND

    setsample_colour = new G4UIcmdWithAString("/Target/Colour", this);
    setsample_colour -> SetGuidance("Set the visualization attribute of the target (if graphics are turned on): \n"
                                       "name Colour"); 

	//Command to check for overlaps in the geometry
	checkforoverlaps = new G4UIcmdWithABool("/Target/CheckAllOverlaps", this);
	checkforoverlaps -> SetGuidance("Choose if you would like the volumes to check for overlaps");
	checkforoverlaps -> SetDefaultValue(false);

//=================================================================================================
//  OBJECT IMAGE AND ROTATION COMMANDS

	//Command to set the off set radius of an object when it rotates between projections
	setradiusoffset = new G4UIcmdWithADoubleAndUnit("/Target/OffSet/Radius", this);
	setradiusoffset -> SetGuidance("Set the off set of the poisition of the target with a radius");
	setradiusoffset -> SetUnitCategory("Length");
	setradiusoffset -> SetDefaultUnit("cm");
	setradiusoffset -> SetDefaultValue(0.5*cm);

//=================================================================================================
//  UNIT DICTIONARY

	//Create a dictionary for units
	length_units.insert(std::make_pair("m",m));
	length_units.insert(std::make_pair("cm",cm));
	length_units.insert(std::make_pair("mm",mm));
	length_units.insert(std::make_pair("um",um));
	length_units.insert(std::make_pair("nm",nm));
	
	angle_units.insert(std::make_pair("rad", rad));
	angle_units.insert(std::make_pair("deg", deg));
}

SampleConstructionMessenger::~SampleConstructionMessenger()
{
	delete directory;

	delete build_cube;
	delete build_sphere;
	delete build_cylinder;
	delete build_trapezoid;
	delete build_ellipsoid;
	delete build_subtractsolid;
	delete build_unionsolid;

	delete setsample_position;
	delete setsample_rotation;

    delete setsample_colour;
	delete setsample_material;
	delete checkforoverlaps;

	delete setradiusoffset;
}

#include "SampleDescription.hh"
#include "Boolean_Sample.hh"
#include "G4Tokenizer.hh"
#include "G4UnitsTable.hh"

#include "MyVectors.hh"
#include "G4ThreeVector.hh"

void SampleConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command == build_sphere)
	{
		//Allows for multiple inputs for complex commands, seperates the variables out
		G4Tokenizer next(newValue);
		G4String name = next();
		
		//Define each variable with in the string for length
		G4double innerRadius = ConvertToDouble(next, newValue, command);
		G4double outerRadius = ConvertToDouble(next, newValue, command);
		G4double radiusUnit  = CheckUnits(next, command, newValue, "Length");

		//Define each variable with in the string for angle
		G4double startingPhi   = ConvertToDouble(next, newValue, command);
		G4double endPhi        = ConvertToDouble(next, newValue, command);
		G4double startingTheta = ConvertToDouble(next, newValue, command);
		G4double endTheta      = ConvertToDouble(next, newValue, command);
		G4double angleUnit     = CheckUnits(next, command, newValue, "Length");

		//Multiply the values by the correct unit
		innerRadius   = innerRadius   * radiusUnit;
		outerRadius   = outerRadius   * radiusUnit;
		startingPhi   = startingPhi   * angleUnit;
		endPhi        = endPhi        * angleUnit;
		startingTheta = startingTheta * angleUnit;
		endTheta      = endTheta      * angleUnit;

		//Turn the variables into an array and append to the dimensions vector
		double_vector1D dimensions = {innerRadius, outerRadius, startingPhi, endPhi, startingTheta, endTheta};
		
		std::string type = "sphere";
		
		sampleconstruction->AddToSampleList(name, type, dimensions);
	}
	else if(command == build_cylinder)
	{
		//Allows for multiple inputs for complex commands, seperates the variables out
		G4Tokenizer next(newValue);
		G4String name = next();
		
		//Define each variable with in the string for length
		G4double innerRadius = ConvertToDouble(next, newValue, command);
		G4double outerRadius = ConvertToDouble(next, newValue, command);
		G4double length      = ConvertToDouble(next, newValue, command);
		G4double radiusUnit  = CheckUnits(next, command, newValue, "Length");
		
		//Define each variable with in the string for angle
		G4double startingPhi = ConvertToDouble(next, newValue, command);
		G4double endPhi      = ConvertToDouble(next, newValue, command);
		G4double angleUnit   = CheckUnits(next, command, newValue, "Angle");
		
		//Multiply the values by the correct unit
		innerRadius = innerRadius * radiusUnit;
		outerRadius = outerRadius * radiusUnit;
		length      = length      * radiusUnit;
		startingPhi = startingPhi * angleUnit;
		endPhi      = endPhi      * angleUnit;
		
		//Turn the variables into an array and append to the dimensions vector
		double_vector1D dimensions = {innerRadius, outerRadius, length, startingPhi, endPhi};
		
		std::string type = "cylinder";
		
		sampleconstruction->AddToSampleList(name, type, dimensions);
	}
	else if(command == build_cube)
	{	
		G4Tokenizer next(newValue);
		G4String name = next();
		
		G4double x    = ConvertToDouble(next, newValue, command);
		G4double y    = ConvertToDouble(next, newValue, command);
		G4double z    = ConvertToDouble(next, newValue, command);
		G4double unit = CheckUnits(next, command, newValue, "Length");
		
		x = x * unit; 
		y = y * unit; 
		z = z * unit;
		
		//Turn the variables into an array and append to the dimensions vector
		double_vector1D dimensions = {x, y, z};
		std::string type = "box";
		
		sampleconstruction->AddToSampleList(name, type, dimensions);
	}
	else if(command == build_trapezoid)
	{   
		G4Tokenizer next(newValue);
		G4String name = next();
		
		G4double dx1  = ConvertToDouble(next, newValue, command);
		G4double dx2  = ConvertToDouble(next, newValue, command);
		G4double dy1  = ConvertToDouble(next, newValue, command);
		G4double dy2  = ConvertToDouble(next, newValue, command);
		G4double dz   = ConvertToDouble(next, newValue, command);
		G4double unit = CheckUnits(next, command, newValue, "Length");

		dx1 = dx1 * unit;
		dx2 = dx2 * unit;
		dy1 = dy1 * unit;
		dy2 = dy2 * unit;
		dz  = dz  * unit;
		
		double_vector1D dimensions = {dx1, dx2, dy1, dy2, dz};
        std::string type = "trapezoid";
		
		sampleconstruction->AddToSampleList(name, type, dimensions);
	}
	else if(command ==  build_ellipsoid)
	{
		G4Tokenizer next(newValue);

        G4String name = next();

		G4double pxSemiAxis  = ConvertToDouble(next, newValue, command);
        G4double pySemiAxis  = ConvertToDouble(next, newValue, command);
        G4double pzSemiAxis  = ConvertToDouble(next, newValue, command);
        G4double pzBottomCut = ConvertToDouble(next, newValue, command);
        G4double pzTopCut    = ConvertToDouble(next, newValue, command);
		G4double Unit        = CheckUnits(next, command, newValue, "Length");

		pxSemiAxis  = pxSemiAxis  * Unit;
		pySemiAxis  = pySemiAxis  * Unit;
		pzSemiAxis  = pzSemiAxis  * Unit;
		pzBottomCut = pzBottomCut * Unit;
		pzTopCut    = pzTopCut    * Unit;

		double_vector1D dimensions = {pxSemiAxis, pySemiAxis, pzSemiAxis, pzBottomCut, pzTopCut};
		std::string type = "ellipsoid";
		
		sampleconstruction->AddToSampleList(name, type, dimensions);
	}

	else if(command == build_subtractsolid)
	{
		//From the string, be able to take the needed paramenters out as seperate variables
	    G4Tokenizer next(newValue);
    
        G4String name = next();
    
	    //Names of the two objects being used
	    G4String outerObject = next();
	    G4String innerObject= next();
    
	    //Find the positions of the inner object inside the string 
	    G4double x          = ConvertToDouble(next, newValue, command);
	    G4double y          = ConvertToDouble(next, newValue, command);
	    G4double z          = ConvertToDouble(next, newValue, command);
	    G4double lengthUnit = CheckUnits(next, command, newValue, "Length");

	    //Find the rotation of the inner object insde the string
	    G4double rotx      = ConvertToDouble(next, newValue, command); 
	    G4double roty      = ConvertToDouble(next, newValue, command); 
	    G4double rotz      = ConvertToDouble(next, newValue, command);
	    G4double angleUnit = CheckUnits(next, command, newValue, "Angle");
	    
	    x    = x    * lengthUnit; 
	    y    = y    * lengthUnit; 
	    z    = z    * lengthUnit;
	    rotx = rotx * angleUnit; 
	    roty = roty * angleUnit; 
	    rotz = rotz * angleUnit;
	    
	    G4ThreeVector innerPosition = G4ThreeVector(x, y, z); 
	    G4ThreeVector innerRotation = G4ThreeVector(rotx, roty, rotz);
	    
	    std::string type = "subtract";
	    double_vector1D dimensions = {0};
	    sampleconstruction->AddToSampleList(name, type, dimensions);
		
		G4cout << "\n About to get pointer... " << G4endl;
		Boolean_Sample* boolsample = sampleconstruction->FindSample_Boolean(name);//dynamic_cast<Boolean_Sample*> (sampleconstruction->FindSample(name));
		G4cout << "\n done... " << G4endl;
		
		boolsample->SetComponent1(outerObject);
		boolsample->SetComponent2(innerObject);
		boolsample->SetInsideRotation(innerRotation);
		boolsample->SetInsidePosition(innerPosition);
	}
	else if(command == build_unionsolid)
	{
		//From the string, be able to take the needed paramenters out as seperate variables
	    G4Tokenizer next(newValue);
    
        G4String name = next();
    
	    //Names of the two objects being used
	    G4String outerObject = next();
	    G4String innerObject= next();
    
	    //Find the positions of the inner object inside the string 
	    G4double x          = ConvertToDouble(next, newValue, command);
	    G4double y          = ConvertToDouble(next, newValue, command);
	    G4double z          = ConvertToDouble(next, newValue, command);
	    G4double lengthUnit = CheckUnits(next, command, newValue, "Length");

	    //Find the rotation of the inner object insde the string
	    G4double rotx      = ConvertToDouble(next, newValue, command); 
	    G4double roty      = ConvertToDouble(next, newValue, command); 
	    G4double rotz      = ConvertToDouble(next, newValue, command);
	    G4double angleUnit = CheckUnits(next, command, newValue, "Angle");
	    
	    x    = x    * lengthUnit; 
	    y    = y    * lengthUnit; 
	    z    = z    * lengthUnit;
	    rotx = rotx * angleUnit; 
	    roty = roty * angleUnit; 
	    rotz = rotz * angleUnit;
	    
	    G4ThreeVector innerPosition = G4ThreeVector(x, y, z); 
	    G4ThreeVector innerRotation = G4ThreeVector(rotx, roty, rotz);
	    
	    std::string type = "union";
	    double_vector1D dimensions = {0};
		
		sampleconstruction->AddToSampleList(name, type, dimensions);
		G4cout << "\n About to get pointer... " << G4endl;
		Boolean_Sample* boolsample = sampleconstruction->FindSample_Boolean(name);//dynamic_cast<Boolean_Sample*> (sampleconstruction->FindSample(name));
		G4cout << "\n done... " << G4endl;
		
		boolsample->SetComponent1(outerObject);
		boolsample->SetComponent2(innerObject);
		boolsample->SetInsideRotation(innerRotation);
		boolsample->SetInsidePosition(innerPosition);
	} 
	else if(command == setsample_position)
	{
		G4Tokenizer next(newValue);
		G4String name = next();
		
		G4double x    = ConvertToDouble(next, newValue, command); 
		G4double y    = ConvertToDouble(next, newValue, command);
		G4double z    = ConvertToDouble(next, newValue, command);
		G4double unit = CheckUnits(next, command, newValue, "Length");
		
		x = x * unit; 
		y = y * unit; 
		z = z * unit;
		
		sampleconstruction->FindSample(name)->SetPosition(G4ThreeVector(x, y, z));
	}
	else if(command == setsample_rotation)
	{
		G4Tokenizer next(newValue);
		G4String name = next();
		
		G4double x    = ConvertToDouble(next, newValue, command);
		G4double y    = ConvertToDouble(next, newValue, command);
		G4double z    = ConvertToDouble(next, newValue, command);
		G4double unit = CheckUnits(next, command, newValue, "Angle");
		
		x = x * unit; 
		y = y * unit; 
		z = z * unit;
		
		sampleconstruction->FindSample(name)->SetRotation(G4ThreeVector(x, y, z));
	}
	else if(command == setsample_material )
	{
	    G4Tokenizer next(newValue);
	    
	    G4String name     = next();
	    G4String material = next();
	    
	    sampleconstruction->FindSample(name)->SetMaterial(material);	
	}
	else if(command == setsample_colour)
	{
	    G4Tokenizer next(newValue);
	    
	    G4String name   = next();
	    G4String colour = next();
	    
	    sampleconstruction->FindSample(name)->SetColour(colour);
	}
	else if(command == checkforoverlaps)
	{
		//sampleconstruction -> SetOverlapCheck(checkforoverlaps -> GetNewBoolValue(newValue));
	}
	else if(command == setradiusoffset )
	{
	    setradiusoffset -> GetNewUnitValue(newValue);
		sampleconstruction -> SetRadiusOffSet(setradiusoffset -> GetNewDoubleValue(newValue));	
	}
}

G4double SampleConstructionMessenger::ConvertToDouble(G4Tokenizer &next, G4String input, G4UIcommand* command)
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

G4double SampleConstructionMessenger::CheckUnits(G4Tokenizer &next, G4UIcommand* command, G4String newValue, G4String TypeOfUnit)
{
    /*G4String UnitString = next();
    
    bool LengthUnit = length_units.count(UnitString);
    bool AngleUnit = angle_units.count(UnitString); 
    
    if (LengthUnit == false && AngleUnit == false )
    {
        G4cout << "\nERROR: " << command -> GetCommandPath() << " " << newValue << " -> Invalid ";
        if (TypeOfUnit == "Length")
            G4cout << "length unit!\nGuidance: ";
        else if (TypeOfUnit == "Angle")
            G4cout << "angle unit!\nGuidance: ";
            
        G4cout << command -> GetGuidanceLine(0) << "\n\nAvailable units \nLength: ";
        
        for (std::map<std::string, double>::iterator it = length_units.begin(); it != length_units.end(); ++it)
             G4cout << it -> first << " ";
             
        G4cout << "\nAngle: ";
        
        for (std::map<std::string, double>::iterator it = angle_units.begin(); it != angle_units.end(); ++it)
             G4cout << it -> first << " ";
        
        G4cout << G4endl;
        
        exit(1);
    }
    
    if (LengthUnit == true)
        return length_units[UnitString];

    else if (AngleUnit == true)
        return angle_units[UnitString];*/
        
    G4String UnitString = next();
    G4bool Success;
    
    if (TypeOfUnit == "Length")
    {
        bool LenUnit = length_units.count(UnitString);
        
        if (LenUnit == false)
            Success = false;
        else 
        {
            Success = true;
            return length_units[UnitString];    
        }
    }
    else if (TypeOfUnit == "Angle")
    {
        bool AngleUnit = angle_units.count(UnitString);
       
        if (AngleUnit == false)
            Success = false;
        else 
        {
            Success = true;
            return angle_units[UnitString];   
        }
    }
    
    if (Success == false)
    {
        G4cout << "\nERROR: " << command -> GetCommandPath() << " " << newValue << " -> Invalid ";
        
        if (TypeOfUnit == "Length")
        {
            G4cout << "length unit!\nGuidance: " << command -> GetGuidanceLine(0) << "\n\nAvailable units \n";   
            for (std::map<std::string, double>::iterator it = length_units.begin(); it != length_units.end(); ++it)
                G4cout << it -> first << " ";
        }
        else if (TypeOfUnit == "Angle")
        {
            G4cout << "angle unit!\nGuidance: " << command -> GetGuidanceLine(0) << "\n\nAvailable units \n";
            for (std::map<std::string, double>::iterator it = angle_units.begin(); it != angle_units.end(); ++it)
                G4cout << it -> first << " ";
        }         
        G4cout << G4endl; 
        exit(0);
    }
}
