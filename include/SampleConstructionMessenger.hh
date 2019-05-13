#ifndef SampleConstructionMessenger_h
#define SampleConstructionMessenger_h 1

#include "MyG4UImessenger.hh"
#include "globals.hh"
#include <map>

class SampleConstruction;

class G4UIdirectory;
class G4UIcmdWithoutParameters;
class MultiParameters;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithABool;

class MultiParameters;

class G4Tokenizer;

class SampleConstructionMessenger : public MyG4UImessenger 
{ 
	public:
		SampleConstructionMessenger(SampleConstruction* sc);
		~SampleConstructionMessenger();

		//Base class method
		void SetNewValue(G4UIcommand* command, G4String newValue);
		int  ApplyCommand(G4UIcommand* command, G4String newValue);
		
	private:	
	
		//Pointer to the SampleConstruction class
		SampleConstruction *sampleconstruction;
        
        G4UIdirectory   *directory;
        
        //My custom commands
        //G4VSolid types
		MultiParameters *build_cube;
		MultiParameters *build_sphere;
		MultiParameters *build_cylinder;
		MultiParameters *build_trapezoid;
		MultiParameters *build_ellipsoid;

		//Set sample attributes
		MultiParameters  *setsample_position;
		MultiParameters  *setsample_rotation;
		MultiParameters  *setsample_material;
		MultiParameters  *setsample_colour;
		MultiParameters  *checkforoverlaps;
		
		//Boolean solids
		MultiParameters *build_subtractsolid;
		MultiParameters *build_unionsolid;	
		MultiParameters *setsample_innerPosition;
		MultiParameters *setsample_innerRotation;
		
		MultiParameters *groupsamples;

        //Geant4 commands
		//Adjusted positioning and rotation of the sample
		G4UIcmdWithADoubleAndUnit  *setradiusoffset;
		G4UIcmdWithADoubleAndUnit  *set_xtiltangle;
		G4UIcmdWithADoubleAndUnit  *set_ytiltangle;
		G4UIcmdWith3VectorAndUnit *set_tiltcentre;
		
		G4UIcmdWithABool *checkforalloverlaps;

		//Dictionary for units
		std::map<std::string, double> length_units;		
		std::map<std::string, double> angle_units;		
};
#endif

