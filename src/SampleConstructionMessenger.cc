#include "SampleConstructionMessenger.hh"
#include "SampleConstruction.hh"

#include "G4SystemOfUnits.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

#include "MultiParameters.hh"
#include "MyG4UImessenger.hh"

SampleConstructionMessenger::SampleConstructionMessenger(SampleConstruction* sc): MyG4UImessenger(), sampleconstruction(sc)
{	
//=================================================================================================
	//Directory
	directory = new G4UIdirectory("/sample/");
	directory -> SetGuidance("Commands to build your sample. ");

//=================================================================================================
//  G4VSolids

	//Command to set the dimensions of a sphere
	build_sphere = new MultiParameters("/sample/G4VSolid/sphere", this);
	build_sphere->SetNumberOfParameters(9);
	build_sphere->SetGuidance("Create an instance of G4Sphere with parameters\n"
	                          "name innerRadius outerRadius lengthUnit startPh endPhi startTheta endTheta angleUnit. ");

	//Command to set the dimensions of a sphere
	build_cylinder = new MultiParameters("/sample/G4VSolid/cylinder", this);
	build_cylinder->SetNumberOfParameters(8);
	build_cylinder->SetGuidance("Create an instance of G4Tubs with parameters\n" 
	                            "name innerRadius outerRadius height lengthUnit startPhi endPhi angleUnit");

	//Command to set the dimensions of a cube
	build_cube = new MultiParameters("/sample/G4VSolid/box", this);
	build_cube->SetNumberOfParameters(5);
	build_cube->SetGuidance("Create an instance of a G4Box with parameters: \n"
	                        "name xlen ylen zlen lengthUnit");

	//Command to set the dimensions of a trapezoid
	build_trapezoid = new MultiParameters("/sample/G4VSolid/trapezoid", this);
	build_trapezoid->SetNumberOfParameters(7);
	build_trapezoid->SetGuidance("Create an instance of G4Trd with parameters: \n" 
	                             "name dx1 dx2 dy1 dy2 dz lengthUnit");

	//Command to set the dimensions of a ellipsoid
	build_ellipsoid = new MultiParameters("/sample/G4VSolid/ellipsoid", this);
	build_ellipsoid->SetNumberOfParameters(7);
	build_ellipsoid -> SetGuidance("Create an instance of G4Ellipsoid with parameters: \n"
	                               "name pxSemiAxis pySemiAxis pzSemiAxis pzBottomCut pzTopCut lengthUnit");

	//Command to set the dimensions of subtraction solid
	build_subtractsolid = new MultiParameters("/sample/G4VSolid/subtract", this);
	build_subtractsolid->SetNumberOfParameters(3);
	build_subtractsolid->SetGuidance("Create an instance of G4SubtractSolid from two existing G4VSolids with parameters: \n"
	                                 "name componenentName1 componenetName2 x y z lengthUnit xTheta yTheta zTheta angleUnit");
	                                    
    build_unionsolid = new MultiParameters("/sample/G4VSolid/union", this);
    build_unionsolid->SetNumberOfParameters(3);
    build_unionsolid->SetGuidance("Create an instance of G4UnionSolid from two existing G4VSolids with parameters: \n"
	                              "name componenentName1 componenetName2 x y z lengthUnit xTheta yTheta zTheta angleUnit");
	
	setsample_innerPosition = new MultiParameters("/sample/G4VSolid/insideposition",this);
	setsample_innerPosition->SetNumberOfParameters(5);
    setsample_innerPosition->SetGuidance("For a G4SubtractSolid or G4UnionSolid, choose the position for the second G4VSolid component relative to the first G4VSolid:"
                                         "\nname xpos ypos zpos unit"
                                         "\nExample: /sample/G4VSolid/subtract/insideposition solid1 0.00 0.50 0.00 cm");
    
    setsample_innerRotation = new MultiParameters("/sample/G4VSolid/insiderotation",this);
    setsample_innerRotation->SetNumberOfParameters(5);
    setsample_innerRotation->SetGuidance("For a G4SubtractSolid or G4UnionSolid, choose the rotation for the second G4VSolid component relative to the first G4VSolid:"
                                         "\nname xrot yrot zrot unit"
                                         "\nExample: /sample/G4VSolid/subtract/insiderotation solid1 00.0 90.0 00.0 rad");

//=================================================================================================
// G4LogicalVolumes

    setsample_colour = new MultiParameters("/sample/G4LogicalVolume/colour", this);
    setsample_colour->SetNumberOfParameters(2);
    setsample_colour->SetGuidance("Set the visualization attribute of the target (if graphics are turned on): \n"
                                  "name Colour"); 
    
    //Command to set the material of an object
	setsample_material = new MultiParameters("/sample/G4LogicalVolume/material", this);
	setsample_material->SetNumberOfParameters(2);
	setsample_material->SetGuidance("Set the material of a solid: \n" 
	                                "name material \n"
	                                "(Material must already exist in the database)");

//=================================================================================================
//  G4PVPlacements

	//Command to set the position of an object
	setsample_position = new MultiParameters("/sample/G4VPhysicalVolume/position", this);
	setsample_position->SetNumberOfParameters(5);
	setsample_position->SetGuidance("Set the position of a solid: \n"
	                                "name x y z lengthUnit");

	//Command to set the rotation of an object
	setsample_rotation = new MultiParameters("/sample/G4VPhysicalVolume/rotation", this);
	setsample_rotation->SetNumberOfParameters(5);
	setsample_rotation->SetGuidance("Set the starting rotation of a solid: \n "
	                                "name xTheta yTheta zTheta angleUnit");

    //Command to check for overlaps in the geometry
	checkforoverlaps = new MultiParameters("/sample/G4VPhysicalVolume/checkoverlaps", this);
	checkforoverlaps->SetNumberOfParameters(2);
	checkforoverlaps->SetGuidance("Choose if you would like the volumes to check for overlaps");


//=================================================================================================
//  OBJECT IMAGE AND ROTATION COMMANDS

	//Command to set the off set radius of an object when it rotates between projections
	setradiusoffset = new G4UIcmdWithADoubleAndUnit("/sample/rotation/radius", this);
	setradiusoffset -> SetGuidance("Set the off set of the poisition of the target with a radius");
	setradiusoffset -> SetUnitCategory("Length");
	setradiusoffset -> SetDefaultUnit("cm");
	setradiusoffset -> SetDefaultValue(0.5*cm);
	
	set_xtiltangle = new G4UIcmdWithADoubleAndUnit("/sample/tilt/anglex", this);
	set_xtiltangle -> SetGuidance("Set the x angle if the whole sample needs to be tilted");
	set_xtiltangle -> SetUnitCategory("Angle");
	
    set_ytiltangle = new G4UIcmdWithADoubleAndUnit("/sample/tilt/angley", this);
	set_ytiltangle -> SetGuidance("Set the y angle if the whole sample needs to be tilted");
	set_ytiltangle -> SetUnitCategory("Angle");
	
	set_tiltcentre = new G4UIcmdWith3VectorAndUnit("/sample/tilt/centre", this);
	set_tiltcentre -> SetGuidance("Set centre of where all the samples will be rotate relative to");
	set_tiltcentre -> SetUnitCategory("Length");
	
	checkforalloverlaps = new G4UIcmdWithABool("/sample/checkforalloverlaps", this);
	checkforalloverlaps -> SetGuidance("Check for overlaps for all the samples placed");
	
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
	delete set_xtiltangle;
	delete set_ytiltangle;
	delete set_tiltcentre;
	
	delete checkforalloverlaps;
}

#include "SampleDescription.hh"
#include "Boolean_Sample.hh"
#include "G4Tokenizer.hh"
#include "G4UnitsTable.hh"

#include "MyVectors.hh"
#include "G4ThreeVector.hh"
#include "CommandStatus.hh"

//Default method for commands that use Geant4 default type commands 

void SampleConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{  
    if (command == setradiusoffset)
	{
	    setradiusoffset -> GetNewUnitValue(newValue);
		sampleconstruction -> SetRadiusOffSet(setradiusoffset -> GetNewDoubleValue(newValue));	
	}
    else if (command == set_xtiltangle)
    {
        set_xtiltangle->GetNewUnitValue(newValue);
        sampleconstruction->SetTiltAngleX(set_xtiltangle->GetNewDoubleValue(newValue));
    }
    else if (command == set_ytiltangle)
    {
        set_ytiltangle->GetNewUnitValue(newValue);
        sampleconstruction->SetTiltAngleY(set_ytiltangle->GetNewDoubleValue(newValue));
    }
    else if (command == set_tiltcentre)
    {
        sampleconstruction->SetTiltCentre(set_tiltcentre->GetNew3VectorValue(newValue));
    }
    else if(command == checkforoverlaps)
	{
		//sampleconstruction -> SetOverlapCheck(checkforoverlaps -> GetNewBoolValue(newValue));
	}
	else if (command == checkforalloverlaps)
	{
	    sampleconstruction->SetCheckForAllOverLaps(checkforalloverlaps->GetNewBoolValue(newValue));
	}
}

//New custom method to apply custom commands that require more error checking. Has a return type which can be used to identify the issue with the command.

int SampleConstructionMessenger::ApplyCommand(G4UIcommand* command, G4String newValue)
{
//-------------------------------------------------------------------------------------------------------------------------------
	if(command == build_sphere)
	{
		//Seperate out the strings parameters into seperate tokens
		G4Tokenizer next(newValue);
		G4String name = next();
		
		//Declare the parameters the command expects to have
		double innerRadius, outerRadius, radiusUnit, startingPhi, endPhi, startingTheta, endTheta, angleUnit;
		
		//Try to convert string to doubles
		try
        {   
            innerRadius = std::stod(next());
		    outerRadius = std::stod(next());
        }    
        //Catch errors, return error code 
        catch (const std::invalid_argument& ia)
        {
            return fParameterNotADouble;
        }
         
        //Get the unit used
        G4String radiusUnitstr = next();
        
        //Check if its an appropiate unit, if it isn't return error code
		bool correctunit1 = length_units.count(radiusUnitstr);
		if (correctunit1) { radiusUnit = length_units[radiusUnitstr];}
		else              { return fIncorrectUnit;}
		
		//Try to convert string to doubles
		try
		{
		    startingPhi   = std::stod(next());
		    endPhi         = std::stod(next());
            startingTheta = std::stod(next());
            endPhi         = std::stod(next());
		}
		//Catch errors, return error code
		catch (const std::invalid_argument& ia)
		{
		    return fParameterNotADouble;
		}
		
		//Check if its an appropiate unit, if it isn't return error code
		G4String angleUnitstr = next();
		bool correctunit2 = angle_units.count(radiusUnitstr);
		if (correctunit2) { angleUnit = angle_units[radiusUnitstr];}
		else              { return fIncorrectUnit;}

		//Multiply the values by the correct unit
		innerRadius   = innerRadius   * radiusUnit;
		outerRadius   = outerRadius   * radiusUnit;
		startingPhi   = startingPhi   * angleUnit;
		endPhi        = endPhi        * angleUnit;
		startingTheta = startingTheta * angleUnit;
		endTheta      = endTheta      * angleUnit;

		//Turn the variables into an array and append to the dimensions vector
		double_vector1D dimensions = {innerRadius, outerRadius, startingPhi, endPhi, startingTheta, endTheta};
		
		//Pass the values to the sampleconstrucion class to build later
		std::string type = "sphere";
		return sampleconstruction->AddToSampleList(name, type, dimensions);
	}
//-------------------------------------------------------------------------------------------------------------------------------
	else if(command == build_cylinder)
	{
		//Seperate out the strings parameters into seperate tokens
		G4Tokenizer next(newValue);
		G4String name = next();
		
		//Declare the parameters the command expects to have
		double innerRadius, outerRadius, length, radiusUnit, startingPhi, endPhi, angleUnit;
		
		//Try to convert string to doubles
		try
        {   
            innerRadius = std::stod(next());
		    outerRadius = std::stod(next());
		    length      = std::stod(next());
        }   
        //Catch errors, return error code   
        catch (const std::invalid_argument& ia)
        {
            return fParameterNotADouble;
        }
        
        //Get the unit used
        G4String radiusUnitstr = next();
        
        //Check if its an appropiate unit, if it isn't return error code
		bool correctunit1 = length_units.count(radiusUnitstr);
		if (correctunit1) { radiusUnit = length_units[radiusUnitstr];}
		else              { return fIncorrectUnit;}
		
		//Try to convert string to doubles
		try
		{
		    startingPhi   = std::stod(next());
		    endPhi         = std::stod(next());
		}
		//Catch errors, return error code   
		catch (const std::invalid_argument& ia)
		{
		    return fParameterNotADouble;
		}
		
		//Get the unit used
		G4String angleUnitstr = next();
		
		//Check if its an appropiate unit, if it isn't return error code
		bool correctunit2 = angle_units.count(angleUnitstr);
		if (correctunit2) { angleUnit = angle_units[angleUnitstr];}
		else              { return fIncorrectUnit;}
		
		//Multiply the values by the correct unit
		innerRadius = innerRadius * radiusUnit;
		outerRadius = outerRadius * radiusUnit;
		length      = length      * radiusUnit;
		startingPhi = startingPhi * angleUnit;
		endPhi      = endPhi      * angleUnit;
		
		//Turn the variables into an array and append to the dimensions vector
		double_vector1D dimensions = {innerRadius, outerRadius, length, startingPhi, endPhi};
		
		//Pass the values to the sampleconstrucion class to build later
		std::string type = "cylinder";		
		return sampleconstruction->AddToSampleList(name, type, dimensions);
	}
//-------------------------------------------------------------------------------------------------------------------------------
	else if(command == build_cube)
	{
	    //Seperate out the strings parameters into seperate tokens
		G4Tokenizer next(newValue);
		G4String name = next();
		
		//Declare the parameters the command expects to have
		double x, y, z, unit;
		
		//Try to convert string to doubles
		try
        {   
            x = std::stod(next());
		    y = std::stod(next());
		    z = std::stod(next());
        }      
        //Catch errors, return error code   
        catch (const std::invalid_argument& ia)
        {
            return fParameterNotADouble;
        }
        
        //Get the unit used
        G4String unitstr = next();
        
        //Check if its an appropiate unit, if it isn't return error code
		bool correctunit = length_units.count(unitstr);
		if (correctunit) { unit = length_units[unitstr];}
		else             { return fIncorrectUnit;}
		
		//Multiply the values by the correct unit
		x = x * unit; 
		y = y * unit; 
		z = z * unit;
		
		//Turn the variables into an array and append to the dimensions vector
		double_vector1D dimensions = {x, y, z};
		
		//Pass the values to the sampleconstrucion class to build later
		std::string type = "box";
		return sampleconstruction->AddToSampleList(name, type, dimensions);
	}
//-------------------------------------------------------------------------------------------------------------------------------
	else if(command == build_trapezoid)
	{   
		G4Tokenizer next(newValue);
		G4String name = next();
		
		double dx1, dx2, dy1, dy2, dz, unit;		
		try
        {   
            dx1 = std::stod(next());
		    dx2 = std::stod(next());
		    dy1 = std::stod(next());
            dy2 = std::stod(next());
            dz  = std::stod(next());
        }      
        catch (const std::invalid_argument& ia)
        {
            return fParameterNotADouble;
        }
         
        G4String unitstr = next();
        
		bool correctunit = length_units.count(unitstr);
		if (correctunit) { unit = length_units[unitstr];}
		else             { return fIncorrectUnit;}

		dx1 = dx1 * unit;
		dx2 = dx2 * unit;
		dy1 = dy1 * unit;
		dy2 = dy2 * unit;
		dz  = dz  * unit;
		
		double_vector1D dimensions = {dx1, dx2, dy1, dy2, dz};
		
        std::string type = "trapezoid";
		return sampleconstruction->AddToSampleList(name, type, dimensions);
	}
//-------------------------------------------------------------------------------------------------------------------------------
	else if(command ==  build_ellipsoid)
	{
		G4Tokenizer next(newValue);

        G4String name = next();
        
        double pxSemiAxis, pySemiAxis, pzSemiAxis, pzBottomCut, pzTopCut, unit;
        
        try
        {   
            pxSemiAxis  = std::stod(next());
		    pySemiAxis  = std::stod(next());
		    pzSemiAxis  = std::stod(next());
            pzBottomCut = std::stod(next());
            pzTopCut    = std::stod(next());
        }      
        catch (const std::invalid_argument& ia)
        {
            return fParameterNotADouble;
        }

        G4String unitstr = next();
        
		bool correctunit = length_units.count(unitstr);
		if (correctunit) { unit = length_units[unitstr];}
		else             { return fIncorrectUnit;}

		pxSemiAxis  = pxSemiAxis  * unit;
		pySemiAxis  = pySemiAxis  * unit;
		pzSemiAxis  = pzSemiAxis  * unit;
		pzBottomCut = pzBottomCut * unit;
		pzTopCut    = pzTopCut    * unit;

		double_vector1D dimensions = {pxSemiAxis, pySemiAxis, pzSemiAxis, pzBottomCut, pzTopCut};
		std::string type = "ellipsoid";
		
		return sampleconstruction->AddToSampleList(name, type, dimensions);
	}
//-------------------------------------------------------------------------------------------------------------------------------
	else if(command == build_subtractsolid)
	{
		//From the string, be able to take the needed paramenters out as seperate variables
	    G4Tokenizer next(newValue);
    
        G4String name = next();
    
	    //Names of the two objects being used
	    G4String outerObject = next();
	    G4String innerObject = next();
	    
	    double_vector1D dimensions = {0};
	    std::string     type       = "subtract";
	    
	    int status = sampleconstruction->AddToSampleList(name, type, dimensions);
	    
	    if (status != 0) {return status;}
	    
		Boolean_Sample* boolsample = sampleconstruction->FindSample_Boolean(name);
		
		boolsample->SetComponent1(outerObject);
		boolsample->SetComponent2(innerObject);
	}
//-------------------------------------------------------------------------------------------------------------------------------
	else if(command == build_unionsolid)
	{
		//From the string, be able to take the needed paramenters out as seperate variables
	    G4Tokenizer next(newValue);
    
        G4String name = next();
    
	    //Names of the two objects being used
	    G4String outerObject = next();
	    G4String innerObject= next();
		
		double_vector1D dimensions = {0};
		std::string     type       = "union";
		
		int status = sampleconstruction->AddToSampleList(name, type, dimensions);
		if (status != 0) {return status;}
		
		Boolean_Sample* boolsample = sampleconstruction->FindSample_Boolean(name);
		
		boolsample->SetComponent1(outerObject);
		boolsample->SetComponent2(innerObject);
	} 
//-------------------------------------------------------------------------------------------------------------------------------
	else if(command == setsample_position)
	{
		G4Tokenizer next(newValue);
		G4String name = next();
	
	    //Declare the parameters the command expects to have
		double xpos, ypos, zpos, unit;
		
		//Try to convert string to doubles
		try
        {   
            xpos = std::stod(next());
		    ypos = std::stod(next());
		    zpos = std::stod(next());
        }      
        //Catch errors, return error code   
        catch (const std::invalid_argument& ia)
        {
            return fParameterNotADouble;
        }
        
        //Get the unit used
        G4String unitstr = next();
        
        //Check if its an appropiate unit, if it isn't return error code
		bool correctunit = length_units.count(unitstr);
		if (correctunit) { unit = length_units[unitstr];}
		else             { return fIncorrectUnit;}
		
		xpos = xpos * unit; 
		ypos = ypos * unit; 
		zpos = zpos * unit;
		
		SampleDescription* sample = sampleconstruction->FindSample(name);	
		if (!sample) {return fParameterNotFound;}
		
		sample->SetPosition(G4ThreeVector(xpos, ypos, zpos));
	}
//-------------------------------------------------------------------------------------------------------------------------------
	else if(command == setsample_rotation)
	{
		G4Tokenizer next(newValue);
		G4String name = next();
		
		//Declare the parameters the command expects to have
		double xrot, yrot, zrot, unit;
		
		//Try to convert string to doubles
		try
        {   
            xrot = std::stod(next());
		    yrot = std::stod(next());
		    zrot = std::stod(next());
        }      
        //Catch errors, return error code   
        catch (const std::invalid_argument& ia)
        {
            return fParameterNotADouble;
        }
        
        //Get the unit used
        G4String unitstr = next();
        
        //Check if its an appropiate unit, if it isn't return error code
		bool correctunit = angle_units.count(unitstr);
		if (correctunit) { unit = angle_units[unitstr];}
		else             { return fIncorrectUnit;}
		
		xrot = xrot * unit; 
		yrot = yrot * unit; 
		zrot = zrot * unit;
		
		SampleDescription* sample = sampleconstruction->FindSample(name);	
		if (!sample) {return fParameterNotFound;}
		
		sample->SetRotation(G4ThreeVector(xrot, yrot, zrot));
	}
//-------------------------------------------------------------------------------------------------------------------------------
	else if (command == setsample_innerPosition)
	{
	    G4Tokenizer next(newValue);
		G4String name = next();
	
	    //Declare the parameters the command expects to have
		double xpos, ypos, zpos, unit;
		
		//Try to convert string to doubles
		try
        {   
            xpos = std::stod(next());
		    ypos = std::stod(next());
		    zpos = std::stod(next());
        }      
        //Catch errors, return error code   
        catch (const std::invalid_argument& ia)
        {
            return fParameterNotADouble;
        }
        
        //Get the unit used
        G4String unitstr = next();
        
        //Check if its an appropiate unit, if it isn't return error code
		bool correctunit = length_units.count(unitstr);
		if (correctunit) { unit = length_units[unitstr];}
		else             { return fIncorrectUnit;}
		
		xpos = xpos * unit; 
		ypos = ypos * unit; 
		zpos = zpos * unit;
		
		Boolean_Sample* boolsample = sampleconstruction->FindSample_Boolean(name);
		if (!boolsample) {return fParameterNotFound;}
		
		boolsample->SetInsidePosition(G4ThreeVector(xpos, ypos, zpos));
	}
//-------------------------------------------------------------------------------------------------------------------------------
	else if (command == setsample_innerRotation)
	{
	    G4Tokenizer next(newValue);
		G4String name = next();
		
		//Declare the parameters the command expects to have
		double xrot, yrot, zrot, unit;
		
		//Try to convert string to doubles
		try
        {   
            xrot = std::stod(next());
		    yrot = std::stod(next());
		    zrot = std::stod(next());
        }      
        //Catch errors, return error code   
        catch (const std::invalid_argument& ia)
        {
            return fParameterNotADouble;
        }
        
        //Get the unit used
        G4String unitstr = next();
        
        //Check if its an appropiate unit, if it isn't return error code
		bool correctunit = angle_units.count(unitstr);
		if (correctunit) { unit = angle_units[unitstr];}
		else             { return fIncorrectUnit;}
		
		xrot = xrot * unit; 
		yrot = yrot * unit; 
		zrot = zrot * unit;
		
		Boolean_Sample* boolsample = sampleconstruction->FindSample_Boolean(name);
		if (!boolsample) {return fParameterNotFound;}
		
		boolsample->SetInsideRotation(G4ThreeVector(xrot, yrot, zrot));	
	}
//-------------------------------------------------------------------------------------------------------------------------------
	else if(command == setsample_material )
	{
	    G4Tokenizer next(newValue);
	    
	    G4String name     = next();
	    G4String material = next();
	    
	    SampleDescription* sample = sampleconstruction->FindSample(name);
		if (!sample) {return fParameterNotFound;}
		
	    sample->SetMaterial(material);	
	}
//-------------------------------------------------------------------------------------------------------------------------------
	else if(command == setsample_colour)
	{
	    G4Tokenizer next(newValue);
	    
	    G4String name   = next();
	    G4String colour = next();
	    
	    SampleDescription* sample = sampleconstruction->FindSample(name);
		if (!sample) {return fParameterNotFound;}
	    
	    sample->SetColour(colour);
	}
//-------------------------------------------------------------------------------------------------------------------------------

    //If command was successful, return 0
	return 0;
}

