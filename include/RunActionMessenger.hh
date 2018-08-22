#ifndef RunActionMessenger_h
#define RunActionMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class RunAction;

class G4UImessenger;

class G4UIdirectory;
class G4UIcmdWithoutParameters;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;

class RunActionMessenger: public G4UImessenger 
{ 
	public:
		RunActionMessenger(RunAction *RAction);
		~RunActionMessenger(void);

		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:
		RunAction *RAction;

		G4UIdirectory 		    *runDirectory;
		
		G4UIcmdWithAnInteger *seedCmd;
};

#endif 
