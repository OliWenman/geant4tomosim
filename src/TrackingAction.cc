#include "TrackingAction.hh"

#include "G4TrackingManager.hh"
#include "G4Track.hh"
  
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"


void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
 {
   //if(aTrack->GetParentID()!=0)
   //{
	   fpTrackingManager->SetStoreTrajectory(false); 
   //}
}


TrackingAction::~TrackingAction(void)
{
}

TrackingAction::TrackingAction(void)
{
}
