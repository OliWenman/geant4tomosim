#include "StackingAction.hh"

#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4VProcess.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4VPhysicalVolume.hh"
#include "G4AssemblyVolume.hh"

StackingAction::StackingAction(): G4UserStackingAction(), fParticle(0) 
{
  	fKillSecondary  = true;
  	fParticle = 0; 
}

StackingAction::~StackingAction(){ }

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
  	//G4ClassificationOfNewTrack status = fUrgent;
  	//const G4ParticleDefinition* particle = aTrack->GetDefinition();
  	
  	//Kill secondaries
  	//if(fKillSecondary )//&& aTrack->GetTrackID() > 1) 
  	//{
  	
  	//G4VPhysicalVolume* nextVolume = aTrack->GetNextVolume();
  	//if (nextVolume){G4cout << "next volume = " << nextVolume -> GetName()<< G4endl;}
  	//else {G4cout << "no NextVolume. Location = " << aTrack -> GetPosition() << " track ID = " << aTrack-> GetTrackID() << G4endl;}
  	    //Kill all secondary electrons
  	    if (aTrack->GetDefinition() == G4Electron::Electron()){return fKill;}//status = fKill;}
  	    //else if (aTrack->GetTrackID() > 3 &&  {return fUrgent;}    
  	    else {return fUrgent;}
  	    //Kill all photons that aren't going to hit the fluorescence detector located in the positive y direction    
  	    //if (aTrack -> GetMomentumDirection().y() < 0 && particle -> GetParticleName() == "gamma" ){status = fKill;}    
  	        
  	//}
  	

  	//return status;
}

void StackingAction::SetKillStatus(G4bool value)    
	{fKillSecondary = value;}


void StackingAction::SetKill(const G4String& name)  
	{fParticle = G4ParticleTable::GetParticleTable()->FindParticle(name);}
