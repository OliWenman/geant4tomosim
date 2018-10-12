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
  		virtual ~StackingAction();
  		void SetKillStatus(G4bool value);
  		void SetKill(const G4String& name);
  		virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);
    
	private:
  		G4bool fKillSecondary;
 		const G4ParticleDefinition* fParticle;

};

#endif

