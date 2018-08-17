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

class PrimaryGeneratorActionMessenger: public G4UImessenger 
{ 
	public:
		PrimaryGeneratorActionMessenger(PrimaryGeneratorAction *Action);
		~PrimaryGeneratorActionMessenger(void);

		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:
		PrimaryGeneratorAction *Action;

		G4UIdirectory *gunDirectory;
		//Pointer to the G4UIdirectory
		G4UIdirectory *ParticleDirectory;

    		G4UIcmdWithADoubleAndUnit *energyCmd;  
		G4UIcmdWithADoubleAndUnit *BeamWidthY_Cmd;
		G4UIcmdWithADoubleAndUnit *BeamHeightZ_Cmd;

		//Pointers for different data members
		//WORLD VARIABLES
		G4UIcmdWithADoubleAndUnit *ParticleX_Cmd;
};

#endif 
