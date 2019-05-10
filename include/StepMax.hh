#ifndef StepMax_h
#define StepMax_h 1

#include "globals.hh"
#include "G4VDiscreteProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4Step.hh"

class StepMaxMessenger;

class StepMax : public G4VDiscreteProcess
{
    public:

        StepMax(const G4String& processName = "UserStepMax");
        ~StepMax();

        G4bool   IsApplicable(const G4ParticleDefinition&);
        void     SetStepMax(G4double);
        G4double GetStepMax() { return fStepMax;};
        G4double PostStepGetPhysicalInteractionLength( const G4Track& track,
                                                       G4double previousStepSize,
                                                       G4ForceCondition* condition);

        G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);
        G4double GetMeanFreePath(const G4Track&, G4double,G4ForceCondition*) {return DBL_MAX;};    

    private:

        G4double fStepMax;
        StepMaxMessenger* messenger;
};

inline G4double StepMax::PostStepGetPhysicalInteractionLength(const G4Track& aTrack, 
                                                              G4double, 
                                                              G4ForceCondition* condition)
{
    // condition is set to "Not Forced"
    *condition = Forced;
  
    G4double limit = 10000; 
    
    return limit;
}

inline G4VParticleChange* StepMax::PostStepDoIt(const G4Track& aTrack, const G4Step&)
{
    // do nothing
    aParticleChange.Initialize(aTrack);
    return &aParticleChange;
}

#endif
