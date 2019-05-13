/*
Class to control PrimaryGeneratorAction class via commands.
*/

#ifndef PrimaryGeneratorActionMessenger_h
#define PrimaryGeneratorActionMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class PrimaryGeneratorAction;

class G4UImessenger;

class G4UIdirectory;
class G4UIcmdWithoutParameters;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWith3Vector;
class G4UIcmdWithABool;

class PrimaryGeneratorActionMessenger: public G4UImessenger 
{ 
	public:
		PrimaryGeneratorActionMessenger(PrimaryGeneratorAction *Action);
		~PrimaryGeneratorActionMessenger(void);

		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:
		PrimaryGeneratorAction* PGAction;


		G4UIdirectory *BeamDirectory;

        //Energy and particle
        G4UIcmdWithABool          *usefastGunCmd;
        G4UIcmdWithAString        *particleCmd;
    	G4UIcmdWithADoubleAndUnit *monoEnergyCmd;  		
		G4UIcmdWith3VectorAndUnit *polizationCmd;
		
		//Position commands
		G4UIcmdWithADoubleAndUnit *beamHalfXCmd;
		G4UIcmdWithADoubleAndUnit *beamHalfYCmd;
		G4UIcmdWith3VectorAndUnit *centreCoordinates;
		G4UIcmdWithABool          *auto_setupbeam;
		
		//Angle commands
		G4UIcmdWith3Vector *momentumDirection;
		
		
		G4UIcmdWithADoubleAndUnit *maxEnergyBinCmd;
		G4UIcmdWithAnInteger      *numberofbins;
};

#endif 
