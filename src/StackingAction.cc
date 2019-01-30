#include "StackingAction.hh"

#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

StackingAction::StackingAction(): G4UserStackingAction(), fParticle(0) 
{
  	fKillSecondary  = true;
  	fParticle = 0; 
}

StackingAction::~StackingAction(){ }

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
  	G4ClassificationOfNewTrack status = fUrgent;
  	const G4ParticleDefinition* particle = aTrack->GetDefinition();
  	
  	//Kill secondaries
  	if(fKillSecondary && aTrack->GetTrackID() > 1) 
  	{
  	    //Kill all secondary electrons
  	    if (killElectrons && particle -> GetParticleName() == "e-" )
  	        {status = fKill;}
  	        
  	    //Kill all photons that aren't going to hit the fluorescence detector in the positive y direction    
  	    if (aTrack -> GetMomentumDirection().y() < 0)
  	        {status = fKill;}    
  	}

  	return status;
}

void StackingAction::SetKillStatus(G4bool value)    
	{fKillSecondary = value;}


void StackingAction::SetKill(const G4String& name)  
	{fParticle = G4ParticleTable::GetParticleTable()->FindParticle(name);}
