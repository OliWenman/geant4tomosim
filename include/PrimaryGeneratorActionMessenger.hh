#ifndef PrimaryGeneratorActionMessenger_h
#define PrimaryGeneratorActionMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class PrimaryGeneratorAction;

class G4UImessenger;

class G4UIdirectory;
class G4UIcmdWithoutParameters;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithABool;

class PrimaryGeneratorActionMessenger: public G4UImessenger 
{ 
	public:
		PrimaryGeneratorActionMessenger(PrimaryGeneratorAction *Action);
		~PrimaryGeneratorActionMessenger(void);

		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:
		PrimaryGeneratorAction* PGAction;

		G4UIdirectory *BeamDirectory;

    	G4UIcmdWithADoubleAndUnit *EnergyCmd;  
		G4UIcmdWithADoubleAndUnit *BeamHalfXCmd;
		G4UIcmdWithADoubleAndUnit *BeamHalfYCmd;
		
		G4UIcmdWithAString *EnergyDisTypeCmd;
		G4UIcmdWithADoubleAndUnit *EnergySigmaCmd;
		
		G4UIcmdWithAString *particleCmd;
		G4UIcmdWith3VectorAndUnit *SetPolizationCmd;
		
		G4UIcmdWithADoubleAndUnit *MaxEnergyBinCmd;
		
		G4UIcmdWithABool *autoPlacement;
};

#endif 
