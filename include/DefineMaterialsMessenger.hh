#ifndef DefineMaterialsMessenger_h
#define DefineMaterialsMessenger_h 1

#include "globals.hh"
#include <map>
#include "G4UImessenger.hh"

class DefineMaterials;

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


class DefineMaterialsMessenger: public G4UImessenger
{
	public:
		DefineMaterialsMessenger(DefineMaterials* materials);
		~DefineMaterialsMessenger();
	
		//Methods
		void SetNewValue(G4UIcommand* command, G4String newValue);
	private:
		DefineMaterials *materials;		

		G4UIdirectory *MaterialsDirectory;

		G4UIcmdWithAString *DefElement;
		G4UIcmdWithAString *DefIsotope;

		std::map<std::string, double> UnitDict;
};
#endif
