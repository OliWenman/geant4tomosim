/*
Messenger class for DefineMaterials. Interfaces its functions to be able
to work via commands.

Author: Oliver Jack Wenman
*/

#ifndef DefineMaterialsMessenger_h
#define DefineMaterialsMessenger_h 1

#include "globals.hh"
#include <map>
#include "MyG4UImessenger.hh"
#include "G4Tokenizer.hh"
#include "G4UIcommand.hh"

class DefineMaterials;

class MyG4UImessenger;

class G4UIdirectory;
class G4UIcmdWithoutParameters;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWith2Vector;
class G4UIcmdWithABool;

class MultiParameters;

class DefineMaterialsMessenger: public MyG4UImessenger
{
	public:
		DefineMaterialsMessenger(DefineMaterials* materials);
		~DefineMaterialsMessenger();
	
		//Methods
		void SetNewValue (G4UIcommand* command, G4String newValue);
		int  ApplyCommand(G4UIcommand* command, G4String newValue);
	private:
	    G4double CheckUnits(G4Tokenizer &next, G4UIcommand* command, G4String newValue, G4String TypeOfUnit);
	    
	    int CheckUnits(std::map<std::string, double> correctunits, 
                       std::string                   unit_str,
                       double                        &unit);
	    
	    template <typename T> T ConvertToNumber(G4Tokenizer &next, G4String input, G4UIcommand* command);
	
		DefineMaterials *materials;		

		G4UIdirectory *MaterialsDirectory;

		MultiParameters* DefElement;
		MultiParameters* DefIsotope;
		MultiParameters* IsotopeMix;
		MultiParameters* AddTo_IsotopeMix;
		MultiParameters* Density_IsotopeMix;
		
		MultiParameters* DefMolecule;
		MultiParameters* AddElementToMolecule;
		
		MultiParameters* DefCompound;
	    MultiParameters* AddElementToCompound;	
	    
	    MultiParameters* DefMixture;
	    MultiParameters* AddMaterialToMixture;
	    
	    MultiParameters* AutoOpticalProperties_El;
	    MultiParameters* AddRefractiveIndex;
	    MultiParameters* AddRefracticeIndex_Im;
	    MultiParameters* AddAbsorptionLength;
	    MultiParameters* AddAbsorptionLength_xraylib;
	    MultiParameters* AddEfficiency;
	    G4UIcmdWithAString* PrintMPT;

		std::map<std::string, double> densityUnits;
		std::map<std::string, double> atomicWeightUnits;
	    std::map<std::string, double> percentageUnit;
	    std::map<std::string, double> energyUnits;	
	    std::map<std::string, double> lengthUnits;
	    std::map<std::string, double> xraylibDensity;
	    std::map<std::string, double> xraylibEnergy;
};

#endif
