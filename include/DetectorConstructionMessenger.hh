#ifndef DetectorConstructionMessenger_h
#define DetectorConstructionMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class DetectorConstruction;
class Input;

class G4UImessenger;

class G4UIdirectory;
class G4UIcmdWithoutParameters;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithABool;

class DetectorConstructionMessenger: public G4UImessenger 
{ 
	public:
		DetectorConstructionMessenger(DetectorConstruction *ConstructDet, Input* input);
		~DetectorConstructionMessenger(void);

		//Methods
		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:		
		//Pointer to the DetectorConstruction class
		DetectorConstruction *ConstructDet;
		Input* input;

		//Pointer to the G4UIdirectory
		G4UIdirectory *WorldDirectory;
		G4UIdirectory *DetectorDirectory;
		G4UIdirectory *DataDirectory;
		
		//Pointers for different data members
		//WORLD VARIABLES
		G4UIcmdWith3VectorAndUnit *WorldSize_Cmd;
		G4UIcmdWithABool *Visualization_Cmd;

		//DETECTOR VARIABLES
		G4UIcmdWithAnInteger *NoDetectorsY_Cmd;
		G4UIcmdWithAnInteger *NoDetectorsZ_Cmd;
		G4UIcmdWith3VectorAndUnit *DetectorSize_Cmd;
		G4UIcmdWithAString *DetectorMaterial_Cmd;
		G4UIcmdWithABool *DetectorEfficiency_Cmd;

};

#endif 
