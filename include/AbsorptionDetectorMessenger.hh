/*
A messenger class for AbsorptionDetector, interfaces commands to control variables
with in that class via macrofiles.

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
		AbsorptionDetectorMessenger(AbsorptionDetector *absorptiondetector);
		~AbsorptionDetectorMessenger(void);

		//Inherited method from G4UImessenger 
		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:		
		//Pointer to the DetectorConstruction class
		AbsorptionDetector *absorptiondetector;

		//Pointer to the G4UIdirectory
		G4UIdirectory *directory;

		//DETECTOR VARIABLES
		G4UIcmdWithAnInteger      *xpixels;
   		G4UIcmdWithAnInteger      *ypixels;
        G4UIcmdWith3VectorAndUnit *halfdimensions;

};

#endif 
