#include "ActionInitialization.hh"

#include "DetectorConstruction.hh"
#include "Data.hh"
#include "Input.hh"

#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "StackingAction.hh"
					     
ActionInitialization::ActionInitialization(Input* InputObject, DetectorConstruction* DC_Object, Data* DataObject):G4VUserActionInitialization() ,input(InputObject), DC(DC_Object), data(DataObject)
{ }

ActionInitialization::~ActionInitialization(){ }

void ActionInitialization::BuildForMaster() const
{
  	SetUserAction(new RunAction(input));
}

void ActionInitialization::Build() const
{
  	//SetUserAction(new RunAction(input));	
	SetUserAction(new StackingAction());
  	//SetUserAction(new EventAction());
	SetUserAction(new PrimaryGeneratorAction(DC, input, data));
}  


