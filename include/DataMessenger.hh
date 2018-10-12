#ifndef DataMessenger_h
#define DataMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class Data;

class G4UImessenger;

class G4UIdirectory;
class G4UIcmdWithoutParameters;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;

class DataMessenger: public G4UImessenger 
{ 
	public:
		DataMessenger(Data* data);
		~DataMessenger(void);

		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:
		Data* data;

		G4UIdirectory* DataDirectory;

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
