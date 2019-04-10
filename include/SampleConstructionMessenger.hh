#ifndef SampleConstructionMessenger_h
#define SampleConstructionMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"
#include <map>

class SampleConstruction;

class G4UImessenger;

class G4UIdirectory;
class G4UIcmdWithoutParameters;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithABool;

class G4Tokenizer;

class SampleConstructionMessenger : public G4UImessenger 
{ 
	public:
		SampleConstructionMessenger(SampleConstruction* sc);
		~SampleConstructionMessenger(void);

		//Base class method
		void SetNewValue(G4UIcommand* command, G4String newValue);

		//Own methods
		//double GetLengthUnit(G4String Unit){return MapLengthUnits[Unit];}
		//double GetAngleUnit(G4String Unit){return MapAngleUnits[Unit];}
		
	private:	
	
	    G4double ConvertToDouble(G4Tokenizer &next, G4String theCommand, G4UIcommand* Command);
	    G4double CheckUnits(G4Tokenizer &next, G4UIcommand* command, G4String newValue, G4String TypeOfUnit);
	
		//Pointer to the SampleConstruction class
		SampleConstruction *sampleconstruction;

		G4UIdirectory      *directory;
		G4UIcmdWithAString *build_cube;
		G4UIcmdWithAString *build_sphere;
		G4UIcmdWithAString *build_cylinder;
		G4UIcmdWithAString *build_trapezoid;
		G4UIcmdWithAString *build_ellipsoid;
		
		G4UIcmdWithAString *build_subtractsolid;
		G4UIcmdWithAString *build_unionsolid;	

		//Set sample attributes
		G4UIcmdWithAString *setsample_position;
		G4UIcmdWithAString *setsample_rotation;
		G4UIcmdWithAString *setsample_material;
		G4UIcmdWithAString *setsample_colour;
		G4UIcmdWithABool   *checkforoverlaps;

		//Rotation radius
		G4UIcmdWithADoubleAndUnit *setradiusoffset;

		//Dictionary for units
		std::map<std::string, double> length_units;		
		std::map<std::string, double> angle_units;		
};
#endif

