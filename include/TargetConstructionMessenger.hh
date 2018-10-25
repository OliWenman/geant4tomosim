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

class TargetConstructionMessenger: public G4UImessenger 
{ 
	public:
		TargetConstructionMessenger(TargetConstruction* TC);
		~TargetConstructionMessenger(void);

		//Methods
		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:		
		//Pointer to the DetectorConstruction class
		TargetConstruction *TC;

		//Pointer to the G4UIdirectory
		G4UIdirectory *TargetDirectory;

		//TARGET VARIABLES
		G4UIcmdWithAnInteger *NumberOfObjects_Cmd;
		G4UIcmdWithAString *TypeOfObjects_Cmd;
		G4UIcmdWith3VectorAndUnit *CubeDimensions_Cmd;
		G4UIcmdWith3VectorAndUnit *TargetPosition_Cmd;
		G4UIcmdWith3VectorAndUnit *TargetRotation_Cmd;
		G4UIcmdWithAString *TargetMaterial_Cmd;
		G4UIcmdWithADoubleAndUnit *OffSetRadius_Cmd;
		G4UIcmdWith3VectorAndUnit *Centre_Cmd;		
};
#endif
