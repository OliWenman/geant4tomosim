#include "StackingAction.hh"

#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

StackingAction::StackingAction(): G4UserStackingAction(), fParticle(0) 
{
	G4cout << G4endl << "StackingAction has been created" << G4endl;

  	fKillSecondary  = true;
  	fParticle       = 0; 
}

StackingAction::~StackingAction(){G4cout << G4endl << "StackingAction has been deleted " << G4endl;}

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
  	G4ClassificationOfNewTrack status = fUrgent;

  	const G4ParticleDefinition* part = aTrack->GetDefinition();

  	//stack or delete secondaries
  	if(aTrack->GetTrackID() > 1) 
	{  
    		if (fKillSecondary || fParticle == part) 
			{ status = fKill; }
  	}
  	return status;
}


void StackingAction::SetKillStatus(G4bool value)    
{ 
  	fKillSecondary = value;
}


void StackingAction::SetKill(const G4String& name)  
{ 
  	fParticle = G4ParticleTable::GetParticleTable()->FindParticle(name);
}
