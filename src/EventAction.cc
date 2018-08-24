#include "EventAction.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"

EventAction::EventAction():G4UserEventAction(){}

EventAction::~EventAction(){}

void EventAction::BeginOfEventAction(const G4Event*){}

void EventAction::EndOfEventAction(const G4Event* event)
{
  	/*// get number of stored trajectories
  	G4TrajectoryContainer* trajectoryContainer = event->GetTrajectoryContainer();
  	G4int n_trajectories = 0;
  	
	if (trajectoryContainer) 
		n_trajectories = trajectoryContainer->entries();*/

  	// periodic printing

	/*if ( verboseLevel > 0 ) 
	{ 
  		G4int eventID = event->GetEventID();
    		G4cout << G4endl <<"Event: " << eventID  << G4endl ;
	}*/

	//G4Step* aStep;
	//G4StepPoint* preStepPoint = aStep -> GetPreStepPoint();
	//G4TouchableHistory* theTouchable = (G4TouchableHistory*)(preStepPoint -> GetTouchable());
	//G4int copyNo = theTouchable -> GetVolume() -> GetCopyNo();

    	/*if ( trajectoryContainer ) 
	{
      		G4cout << G4endl
		       << "==============================================================="
		       << G4endl
		       << n_trajectories
             	       << " trajectories stored in this event in detector " <<  G4endl;
    	}
    		
	G4VHitsCollection* hc = event->GetHCofThisEvent()->GetHC(0);
    		
	G4cout << "    "  
               << hc->GetSize() << " hits stored in this event" << G4endl;
  	*/
}  

