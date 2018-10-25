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

	//Directory
	TargetDirectory = new G4UIdirectory("/Target/");
	TargetDirectory -> SetGuidance("Commands to control the detector variables. ");

	//Command to set the dimensions of a sphere
	SphereDimensions_Cmd = new G4UIcmdWithAString("/Target/Sphere/Dimensions", this);
	SphereDimensions_Cmd -> SetGuidance("Set the dimensions of a sphere you would like, Inner radius, Outer radius, Starting Phi angle, Delta Phi angle, Starting Theta angle , Delta Theta angle");

	//Command to set the dimensions of a sphere
	CylinderDimensions_Cmd = new G4UIcmdWithAString("/Target/Cylinder/Dimensions", this);
	CylinderDimensions_Cmd -> SetGuidance("Set the dimensions of a cylinder that you would like, Inner radius, Outer radius, Length, Delta Phi angle and ending Phi angle");

	//Command to set the dimensions of a cube
	CubeDimensions_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/Cube/Dimensions", this);
	CubeDimensions_Cmd -> SetGuidance("Set the dimensions of a cube for x, y and z. ");
	CubeDimensions_Cmd -> SetUnitCandidates("mm cm m ");
	CubeDimensions_Cmd -> SetDefaultUnit("mm");
	CubeDimensions_Cmd -> SetDefaultValue(G4ThreeVector(0.3*mm, 0.3*mm, 0.0*mm));
	
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

	delete TargetPosition_Cmd;
	delete TargetRotation_Cmd;
	delete TargetMaterial_Cmd;

	delete OffSetRadius_Cmd;
	delete Centre_Cmd;

	G4cout << G4endl << "TargetConstructionMessenger has been deleted ";
}

void TargetConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command == SphereDimensions_Cmd)
	{
		G4Tokenizer next(newValue);
		
		G4double innerRadius = std::stod(next());
		G4double outerRadius = std::stod(next());
		G4String RadiusUnit = next();

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

		G4double StartingPhi = std::stod(next());
		G4double EndPhi = std::stod(next());
		G4double StartingTheta = std::stod(next());
		G4double EndTheta = std::stod(next());	
		G4String AngleUnit = next();

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

		std::vector<G4double> Array = {innerRadius, outerRadius, StartingPhi, EndPhi, StartingTheta, EndTheta};
		TC -> AddDimensions(Array);
		TC -> AddTypeOfObjects("Sphere");
	}
	else if(command == CylinderDimensions_Cmd)
	{
		G4Tokenizer next(newValue);
		
		G4double innerRadius = std::stod(next());
		G4double outerRadius = std::stod(next());
		G4double length = std::stod(next())/2;
		G4String RadiusUnit = next();

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

		G4double StartingPhi = std::stod(next());
		G4double EndPhi = std::stod(next());
		G4String AngleUnit = next();

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

		std::vector<G4double> Array = {innerRadius, outerRadius, length, StartingPhi, EndPhi};
		TC -> AddDimensions(Array);
		TC -> AddTypeOfObjects("Cylinder");
	}
	else if(command == CubeDimensions_Cmd)
	{
		G4ThreeVector Dimensions = CubeDimensions_Cmd -> GetNew3VectorValue(newValue);
		std::vector<G4double> Array = {Dimensions.x(), Dimensions.y(), Dimensions.z()};
		TC -> AddDimensions(Array);
		TC -> AddTypeOfObjects("Cube");	
		G4cout << "TargetConstruction -> AddCubeDimensions command detected "<< G4endl;
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
