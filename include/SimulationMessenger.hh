/*
Control general settings for the simulation via commands.

Author: Oliver Jack Wenman
*/

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
class G4UIcmdWithoutParameter;

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
		G4UIdirectory*           directory;
		G4UIcmdWithAString*      applymacrofile;
		G4UIcmdWithAnInteger*    setseed;
		G4UIcmdWithAString*      setgraphics;
		G4UIcmdWithoutParameter* progressSingleline;
};


#endif 
