#include "ActionInitialization.hh"

#include "Randomize.hh"
#include "time.h"

#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "StackingAction.hh"
					     
ActionInitialization::ActionInitialization():G4VUserActionInitialization(){ }

ActionInitialization::~ActionInitialization(){ }

void ActionInitialization::BuildForMaster() const
{
  	SetUserAction(new RunAction);
}

void ActionInitialization::Build() const
{
	G4int seedCmd = 1;
	if (seedCmd == 1)	//Keeps the seed
	{
		G4cout << G4endl << "The seed is constant" << G4endl; 
		//choose the Random engine
		CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
		
	}
	else if (seedCmd == 0)	//Random seed
	{
		G4cout << G4endl << "The seed is random" << G4endl; 
		//choose the Random engine
		CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());

		///set random seed with system time
		G4long Setting = time(NULL);
		CLHEP::HepRandom::setTheSeed(Setting);	
	}

  	SetUserAction(new RunAction);	
	SetUserAction(new StackingAction);
  	SetUserAction(new EventAction);
	SetUserAction(new PrimaryGeneratorAction);
}  


