#ifndef FluorescenceDetectorMessenger_h
#define FluorescenceDetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class FluorescenceDetector;

class G4UImessenger;

class G4UIdirectory;
class G4UIcmdWithoutParameters;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWith2Vector;
class G4UIcmdWithABool;

class FluorescenceDetectorMessenger: public G4UImessenger 
{ 
	public:
		FluorescenceDetectorMessenger(FluorescenceDetector *FD);
		~FluorescenceDetectorMessenger(void);

		//Methods
		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:		
		//Pointer to the DetectorConstruction class
		FluorescenceDetector *detectorbuilder;

		//Pointer to the G4UIdirectory
		G4UIdirectory *directory;

		//DETECTOR VARIABLES
        G4UIcmdWith3VectorAndUnit *setdimensions;
        G4UIcmdWithABool *setfullmapping;
        G4UIcmdWithABool *setfullfield;
        
        G4UIcmdWithAnInteger* setbins;
        G4UIcmdWithADoubleAndUnit* setmaxenergy;
};

#endif 
