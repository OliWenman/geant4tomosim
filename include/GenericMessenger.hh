#ifndef GenericMessenger_h
#define GenericMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"
#include <iostream>

#include "G4SystemOfUnits.hh"

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

class G4UImessenger;

class G4UIdirectory;
class G4UIcmdWithoutParameters;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWith3VectorAndUnit;

class GenericMessenger: public G4UImessenger 
{ 
	public:
		GenericMessenger()
		{	
			//DETECTOR
			//Directory
			DetectorDirectory = new G4UIdirectory("/detectorTest/");
			DetectorDirectory -> SetGuidance("Commands to control the detector variables. ");

			//Number in y direction command
			NoDetectorsY_Cmd = new G4UIcmdWithAnInteger("/detectorTest/numberY", this);
			NoDetectorsY_Cmd -> SetDefaultValue(100);
			NoDetectorsY_Cmd -> SetGuidance("Pick the number of detectors you would like to have.  ");

			//Number in z direction command
			NoDetectorsZ_Cmd = new G4UIcmdWithAnInteger("/detectorTest/numberZ", this);
			NoDetectorsZ_Cmd -> SetDefaultValue(100);
			NoDetectorsZ_Cmd -> SetGuidance("Pick the number of detectors you would like to have.  ");
		}
		
		~GenericMessenger()
		{
			delete DetectorDirectory;
			delete NoDetectorsY_Cmd;
			delete NoDetectorsZ_Cmd;
		};

		//Get method
		G4int GetNoDetectorsY(){return NoDetectorsY;}
		G4int GetNoDetectorsZ(){return NoDetectorsZ;}

		//Set method
		void SetNoDetectorsY(G4int value){NoDetectorsY = value;}
		void SetNoDetectorsZ(G4int value){NoDetectorsZ = value;}

		//Methods
		void SetNewValue(G4UIcommand* command, G4String newValue)
		{
			if( command == NoDetectorsY_Cmd )
  			{ 			
				SetNoDetectorsY(NoDetectorsY_Cmd -> GetNewIntValue(newValue));
			}
			else if ( command == NoDetectorsZ_Cmd )
			{
				SetNoDetectorsZ(NoDetectorsZ_Cmd -> GetNewIntValue(newValue));
			}
		}
		
	private:		
		G4UIdirectory *DetectorDirectory;

		//DETECTOR VARIABLES
		G4UIcmdWithAnInteger *NoDetectorsY_Cmd;
		G4UIcmdWithAnInteger *NoDetectorsZ_Cmd;

		G4int NoDetectorsY;
		G4int NoDetectorsZ;
		
};

#endif 
