/*
Messenger class for DefineMaterials. Interfaces its functions to be able
to work via commands.

Author: Oliver Jack Wenman
*/

#ifndef DefineMaterialsMessenger_h
#define DefineMaterialsMessenger_h 1

#include "globals.hh"
#include <map>
#include "G4UImessenger.hh"
#include "G4Tokenizer.hh"
#include "G4UIcommand.hh"

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
	    G4double CheckUnits(G4Tokenizer &next, G4UIcommand* command, G4String newValue, G4String TypeOfUnit);
	    template <typename T> T ConvertToNumber(G4Tokenizer &next, G4String input, G4UIcommand* command);
	
		DefineMaterials *materials;		

		G4UIdirectory *MaterialsDirectory;

		G4UIcmdWithAString *DefElement;
		G4UIcmdWithAString *DefIsotope;
		G4UIcmdWithAString *IsotopeMix;
		G4UIcmdWithAString *AddTo_IsotopeMix;
		G4UIcmdWithAString *Density_IsotopeMix;
		
		G4UIcmdWithAString *DefMolecule;
		G4UIcmdWithAString *AddElementToMolecule;
		
		G4UIcmdWithAString *DefCompound;
	    G4UIcmdWithAString *AddElementToCompound;	
	    
	    G4UIcmdWithAString *DefMixture;
	    G4UIcmdWithAString *AddMaterialToMixture;
	    
	    G4UIcmdWithAString *AutoOpticalProperties_El;
	    G4UIcmdWithAString *AddRefractiveIndex;
	    G4UIcmdWithAString *AddRefracticeIndex_Im;
	    G4UIcmdWithAString *AddAbsorptionLength;
	    G4UIcmdWithAString *AddAbsorptionLength_xraylib;
	    G4UIcmdWithAString *AddEfficiency;
	    G4UIcmdWithAString *PrintMPT;

		std::map<std::string, double> densityUnits;
		std::map<std::string, double> atomicWeightUnits;
	    std::map<std::string, double> percentageUnit;
	    std::map<std::string, double> energyUnits;	
	    std::map<std::string, double> lengthUnits;
	    std::map<std::string, double> xraylibDensity;
	    std::map<std::string, double> xraylibEnergy;
};
#endif
