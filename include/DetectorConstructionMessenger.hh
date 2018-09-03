#ifndef DetectorConstructionMessenger_h
#define DetectorConstructionMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class DetectorConstruction;

class G4UImessenger;

class G4UIdirectory;
class G4UIcmdWithoutParameters;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWith3VectorAndUnit;

class DetectorConstructionMessenger: public G4UImessenger 
{ 
	public:
		DetectorConstructionMessenger(DetectorConstruction *ConstructDet);
		~DetectorConstructionMessenger(void);

		//Methods
		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:		
		//Pointer to the DetectorConstruction class
		DetectorConstruction *ConstructDet;

		//Pointer to the G4UIdirectory
		G4UIdirectory *WorldDirectory;
		G4UIdirectory *DetectorDirectory;
		G4UIdirectory *TargetDirectory;
		G4UIdirectory *DataDirectory;
		
		//Pointers for different data members
		//WORLD VARIABLES
		G4UIcmdWith3VectorAndUnit *WorldSize_Cmd;

		//DETECTOR VARIABLES
		G4UIcmdWithAnInteger *NoDetectorsY_Cmd;
		G4UIcmdWithAnInteger *NoDetectorsZ_Cmd;
		G4UIcmdWith3VectorAndUnit *DetectorSize_Cmd;
		G4UIcmdWithAString *DetectorMaterial_Cmd;

		//TARGET VARIABLES
		G4UIcmdWith3VectorAndUnit *TargetPosition_Cmd;
		G4UIcmdWithAString *TargetMaterial_Cmd;

		//DATA MANIPULATION VARIABLES
		G4UIcmdWithAnInteger *NoBins_Cmd;
		G4UIcmdWithAnInteger *NoImages_Cmd;
		G4UIcmdWithAnInteger *NoPhotons_Cmd;

		G4UIcmdWithAString *Visualization_Cmd;

		
};

#endif 
