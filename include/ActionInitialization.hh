#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class Data;

// Action initialization class. 
class ActionInitialization : public G4VUserActionInitialization
{
  	public:
    		 
		ActionInitialization(Data* data);
    		virtual ~ActionInitialization();

    		virtual void BuildForMaster() const;
    		virtual void Build() const;

	private:
		Data* data;
};

#endif

    
