/*
A messenger class for AbsorptionDetector, interfaces commands to control variables
with in that class.

Author: Oliver Jack Wenman
*/

#ifndef AbsorptionDetectorMessenger_h
#define AbsorptionDetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class AbsorptionDetector;;

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

class AbsorptionDetectorMessenger: public G4UImessenger 
{ 
	public:
		AbsorptionDetectorMessenger(AbsorptionDetector *AD);
		~AbsorptionDetectorMessenger(void);

		//Methods
		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:		
		//Pointer to the DetectorConstruction class
		AbsorptionDetector *AD;

		//Pointer to the G4UIdirectory
		G4UIdirectory *Directory;

		//DETECTOR VARIABLES
		G4UIcmdWithAnInteger *SetRows;
   		G4UIcmdWithAnInteger *SetColumns;
        G4UIcmdWith3VectorAndUnit *SetDimensions;

};

#endif 
