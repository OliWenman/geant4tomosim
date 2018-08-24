#include "ActionInitialization.hh"

#include "DetectorConstruction.hh"
#include "Data.hh"

#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "StackingAction.hh"
					     
ActionInitialization::ActionInitialization(Data* DataObject, DetectorConstruction* DC_Object):G4VUserActionInitialization(), data(DataObject), DC(DC_Object)
{ 
	G4cout << G4endl << "ActionInitialization has been created " << G4endl;
}

ActionInitialization::~ActionInitialization(){ }

void ActionInitialization::BuildForMaster() const
{
  	SetUserAction(new RunAction(data));
}

void ActionInitialization::Build() const
{
  	SetUserAction(new RunAction(data));	
	SetUserAction(new StackingAction());
  	SetUserAction(new EventAction());
	SetUserAction(new PrimaryGeneratorAction(DC));
}  


