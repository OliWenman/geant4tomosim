#include "StepMax.hh"
//#include "StepMaxMessenger.hh"

StepMax::StepMax(const G4String& processName): G4VDiscreteProcess(processName), messenger(0) 
{
    fStepMax = DBL_MAX;
    //fMess = new StepMaxMessenger(this);
}

StepMax::~StepMax() { delete messenger; }

G4bool StepMax::IsApplicable(const G4ParticleDefinition& particle)
{
    return true;
}

void StepMax::SetStepMax(G4double step) {fStepMax = step;}
