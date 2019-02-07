#ifndef SimulationMessenger_h
#define SimulationMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class Simulation;

class G4UImessenger;

class G4UIdirectory;
class G4UIcmdWithoutParameters;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;

class SimulationMessenger: public G4UImessenger 
{ 
	public:
		SimulationMessenger(Simulation* sim);
		~SimulationMessenger(void);

		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:
		//Pointer to Simulation class
		Simulation* sim;

		//Pointers to different commands
		G4UIdirectory* SimulationDirectory;
    	G4UIcmdWithABool* WriteToTextCmd;
		G4UIcmdWithAnInteger *NoImages_Cmd;
		G4UIcmdWithAString *NoPhotons_Cmd;
		G4UIcmdWithAnInteger *seedCmd;
		G4UIcmdWithAnInteger *verboseCmd;
};


#endif 
