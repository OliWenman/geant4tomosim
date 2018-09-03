#ifndef MainMessenger_h 
#define MainMessenger_h 1

#include "Main.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithAnInteger.hh"

class G4UIcmdWithADouble;
class G4UIcmdWithABool;
class G4UIcmdWithAString;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3Vector;
class G4UIcmdWithAInteger;

class MainMessenger : public G4UImessenger 
{
	public:
		MainMessenger();
		~MainMessenger(void);
		void SetNewValue(G4UIcommand* command, G4String newValue);
	private:

		G4UIcmdWithABool *bSavePhaseSpace, *bStopAtPhaseSpace, *bSaveROG, *bForcePhaseSpaceBeforeJaws;
};

#endif
