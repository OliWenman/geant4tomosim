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
class G4UIcmdWith3Vector;
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

        //Energy and particle
    	G4UIcmdWithADoubleAndUnit *EnergyCmd;  		
		G4UIcmdWithAString *EnergyDisTypeCmd;
		G4UIcmdWithADoubleAndUnit *EnergySigmaCmd;
		G4UIcmdWithAString *particleCmd;
		G4UIcmdWith3VectorAndUnit *SetPolizationCmd;
		
		//Position commands
		G4UIcmdWithABool *autoPlacement;
		G4UIcmdWithADoubleAndUnit *BeamHalfXCmd;
		G4UIcmdWithADoubleAndUnit *BeamHalfYCmd;
		G4UIcmdWith3VectorAndUnit *centreCoordinates;
		
		//Angle commands
		G4UIcmdWith3Vector *momentumDirection;
		
		
		G4UIcmdWithADoubleAndUnit *MaxEnergyBinCmd;
};

#endif 
