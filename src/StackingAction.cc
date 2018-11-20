#include "StackingAction.hh"

#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4TransportationManager.hh"

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

  	//stack or delete secondaries
  	if(aTrack->GetTrackID() > 1) 
	{  
    		if (fKillSecondary || fParticle == particle) 
			{status = fKill; }
  	}

	//G4Step* theStep;
	//G4String thePostPVName = theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName(); 

	//if (thePostPVname == "LogicDetector")
	//	{status = fKill; }

  	return status;
}


void StackingAction::SetKillStatus(G4bool value)    
	{fKillSecondary = value;}


void StackingAction::SetKill(const G4String& name)  
	{fParticle = G4ParticleTable::GetParticleTable()->FindParticle(name);}
