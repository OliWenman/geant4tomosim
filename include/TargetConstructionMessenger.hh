#ifndef TargetConstructionMessenger_h
#define TargetConstructionMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

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

class TargetConstructionMessenger: public G4UImessenger 
{ 
	public:
		TargetConstructionMessenger(TargetConstruction* TC);
		~TargetConstructionMessenger(void);

		//Base class method
		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:		
		//Pointer to the TargetConstruction class
		TargetConstruction *TC;

		//Pointer to the G4UIdirectory
		G4UIdirectory *TargetDirectory;

		//Pointers to different variables
		G4UIcmdWith3VectorAndUnit *CubeDimensions_Cmd;
		G4UIcmdWithAString *HollowCubeDimensions_Cmd;
		G4UIcmdWithAString *SphereDimensions_Cmd;
		G4UIcmdWithAString *CylinderDimensions_Cmd;

		G4UIcmdWith3VectorAndUnit *TargetPosition_Cmd;
		G4UIcmdWith3VectorAndUnit *TargetRotation_Cmd;
		G4UIcmdWithAString *TargetMaterial_Cmd;
		G4UIcmdWithABool *BooleanOp_Cmd;
		G4UIcmdWithABool *OverlapCheck_Cmd;

		G4UIcmdWithADoubleAndUnit *OffSetRadius_Cmd;
		G4UIcmdWith3VectorAndUnit *Centre_Cmd;		
};
#endif
