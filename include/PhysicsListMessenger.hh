/*
Class to use PhysicsList functions via commands.

Author: Oliver Jack Wenman
*/

#ifndef PhysicsListMessenger_h
#define PhysicsListMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class PhysicsList;

class G4UImessenger;

class G4UIdirectory;
class G4UIcmdWithoutParameters;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;

class PhysicsListMessenger: public G4UImessenger 
{ 
	public:
		PhysicsListMessenger(PhysicsList* physics);
		~PhysicsListMessenger(void);

		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:
		PhysicsList* physics;

		G4UIdirectory* physicsDirectory;
		
		G4UIcmdWithABool* gamma_liv_photoelectricCmd;
		G4UIcmdWithABool* gamma_liv_comptonscatteringCmd;
		G4UIcmdWithABool* gamma_liv_rayleighscatteringCmd;
		G4UIcmdWithABool* gamma_fluorescenceCmd;
		G4UIcmdWithABool* gamma_refractionCmd;
		
		G4UIcmdWithABool* optical_absorptionCmd;
        G4UIcmdWithABool* optical_refractionCmd;		

        G4UIcmdWithADoubleAndUnit* gammacuts;
        G4UIcmdWithADoubleAndUnit* opticalcuts;
        G4UIcmdWithADoubleAndUnit* electroncuts;
        
};

#endif 
