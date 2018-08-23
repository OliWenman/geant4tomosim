#include "ActionInitialization.hh"

#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "StackingAction.hh"
					     
ActionInitialization::ActionInitialization():G4VUserActionInitialization()
{ 
	G4cout << G4endl << "ActionInitialization has been created " << G4endl;
}

ActionInitialization::~ActionInitialization(){ }

void ActionInitialization::BuildForMaster() const
{
  	SetUserAction(new RunAction);
}

void ActionInitialization::Build() const
{
  	SetUserAction(new RunAction);	
	SetUserAction(new StackingAction);
  	SetUserAction(new EventAction);
	SetUserAction(new PrimaryGeneratorAction);
}  


