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

class PhysicsListMessenger: public G4UImessenger 
{ 
	public:
		PhysicsListMessenger(PhysicsList* Physics);
		~PhysicsListMessenger(void);

		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:
		PhysicsList* Physics;

		G4UIdirectory* physicsDirectory;
		
		G4UIcmdWithAString* PhysicsProcessCmd;
};

#endif 
