#ifndef InputMessenger_h
#define InputMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class Input;
class Data;

class G4UImessenger;

class G4UIdirectory;
class G4UIcmdWithoutParameters;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;

class InputMessenger: public G4UImessenger 
{ 
	public:
		InputMessenger(Input* input, Data* data);
		~InputMessenger(void);

		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:
		Input* input;
		Data* data;

		G4UIdirectory* InputDirectory;

    		G4UIcmdWithABool* TextFileCmd;
		G4UIcmdWithABool* HDF5FileCmd;
		G4UIcmdWithAnInteger *NoBins_Cmd;
		G4UIcmdWithAnInteger *NoImages_Cmd;
		G4UIcmdWithAString *NoPhotons_Cmd;
		G4UIcmdWithABool *Visualization_Cmd;
		G4UIcmdWithAnInteger *seedCmd;
		G4UIcmdWithABool *EnergyDataCmd;
};

#endif 
