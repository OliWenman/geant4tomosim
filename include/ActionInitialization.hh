#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class Input;
class Data;
class DetectorConstruction;

// Action initialization class. 
class ActionInitialization : public G4VUserActionInitialization
{
  	public:
    		 
		ActionInitialization(Input* input, DetectorConstruction* DC, Data* data);
    		virtual ~ActionInitialization();

    		virtual void BuildForMaster() const;
    		virtual void Build() const;

	private:
		Input* input;
		DetectorConstruction* DC;
		Data* data;
};

#endif

    
