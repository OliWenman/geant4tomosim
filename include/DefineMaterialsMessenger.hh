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

		MultiParameters* define_element;
		MultiParameters* define_isotope;
		MultiParameters* define_isotopemix;
		MultiParameters* addto_isotopemix;
		MultiParameters* addto_isotopemix_density;
		
		MultiParameters* define_molecule;
		MultiParameters* addto_molecule_element;
		
		MultiParameters* define_compound;
	    MultiParameters* addto_compound_element;	
	    
	    MultiParameters* define_mixture;
	    MultiParameters* addto_mixture_material;
	    
	    //MaterialsPropertyTable commands
	    G4UIcmdWithAString* mpt_print;
	    
	    MultiParameters* mpt_xraylib_add_all_op;
	    MultiParameters* mpt_xraylib_add_absorptionlen;
	    MultiParameters* mpt_xraylib_add_refraction;
	    MultiParameters* mpt_xraylib_add_complexrefraction;
	    
	    MultiParameters* mpt_add_refractiveindex;
	    MultiParameters* mpt_add_imrefractiveindex;
	    MultiParameters* mpt_add_absorptionlen;

		std::map<std::string, double> densityUnits;
		std::map<std::string, double> atomicWeightUnits;
	    std::map<std::string, double> percentageUnit;
	    std::map<std::string, double> energyUnits;	
	    std::map<std::string, double> lengthUnits;
	    std::map<std::string, double> xraylibDensity;
	    std::map<std::string, double> xraylibEnergy;
};

#endif
