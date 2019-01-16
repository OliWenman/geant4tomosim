#ifndef TargetConstructionMessenger_h
#define TargetConstructionMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"
#include <map>

class TargetConstruction;

class G4UImessenger;

class G4UIdirectory;
class G4UIcmdWithoutParameters;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithABool;

class G4Tokenizer;

class TargetConstructionMessenger: public G4UImessenger 
{ 
	public:
		TargetConstructionMessenger(TargetConstruction* TC);
		~TargetConstructionMessenger(void);

		//Base class method
		void SetNewValue(G4UIcommand* command, G4String newValue);

		//Own methods
		double GetLengthUnit(G4String Unit){return MapLengthUnits[Unit];}
		double GetAngleUnit(G4String Unit){return MapAngleUnits[Unit];}
		
		G4double CheckUnits(G4Tokenizer &next, G4UIcommand* command, G4String newValue, G4String TypeOfUnit);
		
	private:		
	    G4double InputNumberCheck(G4Tokenizer &next, G4String theCommand, G4UIcommand* Command);
	
		//Pointer to the TargetConstruction class
		TargetConstruction *TC;

		//Pointer to the G4UIdirectory
		G4UIdirectory *TargetDirectory;

		//Different shapes
		G4UIcmdWithAString *CubeDimensions_Cmd;
		G4UIcmdWithAString *SphereDimensions_Cmd;
		G4UIcmdWithAString *CylinderDimensions_Cmd;
		G4UIcmdWithAString *TrapezoidDimensions_Cmd;
		G4UIcmdWithAString *EllipsoidDimensions_Cmd;
		
		G4UIcmdWithAString *SubtractionSolid_Cmd;

		//Object attributes
		G4UIcmdWithAString *TargetPosition_Cmd;
		G4UIcmdWithAString *TargetRotation_Cmd;
		G4UIcmdWithAString *TargetMaterial_Cmd;
		G4UIcmdWithABool *BooleanOp_Cmd;
		G4UIcmdWithABool *OverlapCheck_Cmd;

		//Image settings
		G4UIcmdWithADoubleAndUnit *FullRotationAngle_Cmd;
		G4UIcmdWithADoubleAndUnit *OffSetRadius_Cmd;
		G4UIcmdWith3VectorAndUnit *Centre_Cmd;	
		G4UIcmdWithAnInteger *CalibrationImages_Cmd;

		//Dictionary for units
		std::map<std::string, double> MapLengthUnits;		
		std::map<std::string, double> MapAngleUnits;		
};
#endif
