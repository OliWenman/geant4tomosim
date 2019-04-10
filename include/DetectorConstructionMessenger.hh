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
class G4UIcmdWith2Vector;
class G4UIcmdWithABool;

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
		
		G4UIcmdWith3VectorAndUnit *setworld_dimensions;
		G4UIcmdWithAString *setworld_material;
		G4UIcmdWithABool *Visualization_Cmd;

};

#endif 
