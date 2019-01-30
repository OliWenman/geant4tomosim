#ifndef StackingAction_h
#define StackingAction_h 1

#include "G4UserStackingAction.hh"
#include "globals.hh"

class G4Track;
class G4Step;
class G4ParticleDefinition;

class StackingAction : public G4UserStackingAction
{
	public:
  		StackingAction();
  		~StackingAction();
  		
  		void SetKillStatus(G4bool value);
  		void SetKill(const G4String& name);
  		G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);
  		
  		void SetKillElectrons(bool value){killElectrons = value;}
    
	private:
  		G4bool fKillSecondary;
 		const G4ParticleDefinition* fParticle;
 		
 		bool killElectrons;
};

#endif

