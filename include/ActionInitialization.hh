#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class Data;
class DetectorConstruction;

// Action initialization class. 
class ActionInitialization : public G4VUserActionInitialization
{
  	public:
    		 
		ActionInitialization(Data* data, DetectorConstruction* DC);
    		virtual ~ActionInitialization();

    		virtual void BuildForMaster() const;
    		virtual void Build() const;

	private:
		Data* data;
		DetectorConstruction* DC;
};

#endif

    
