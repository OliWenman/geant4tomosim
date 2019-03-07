#ifndef DataMessenger_h
#define DataMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class Data;;

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

class DataMessenger: public G4UImessenger 
{ 
	public:
		DataMessenger(Data* data);
		~DataMessenger(void);

		//Methods
		void SetNewValue(G4UIcommand* command, G4String newValue);
		
	private:		
		//Pointer to the DetectorConstruction class
		Data *data;

		//Pointer to the G4UIdirectory
		G4UIdirectory *Directory;

		//DETECTOR VARIABLES
        G4UIcmdWithABool *RecordFullMapping;
        G4UIcmdWithABool *RecordFullField;
        G4UIcmdWithAnInteger *SetBins;
};

#endif 
