#include "G4SteppingManager.hh"
#include "G4TrackVector.hh"
#include "SteppingAction.hh"
#include "G4ios.hh"
#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4TrackVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4UserEventAction.hh"
#include "G4TransportationManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "G4VPhysicalVolume.hh"
#include "G4RunManager.hh"

/////////////////////////////////////////////////////////////////////////////
SteppingAction::SteppingAction()
{
    
}

/////////////////////////////////////////////////////////////////////////////
SteppingAction::~SteppingAction()
{

}

/////////////////////////////////////////////////////////////////////////////
void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
    /*G4Track* theTrack = aStep->GetTrack();
    
    const G4ParticleDefinition *particle = theTrack -> GetDynamicParticle() -> GetParticleDefinition();
    
    //particle -> GetProcessManager() -> DumpInfo();
     
    G4StepPoint* PreStep = aStep->GetPreStepPoint();
    G4StepPoint* PostStep = aStep->GetPostStepPoint();
     
    G4TouchableHandle touchPreStep = PreStep->GetTouchableHandle();
    G4TouchableHandle touchPostStep = PostStep->GetTouchableHandle();
    
    G4VPhysicalVolume* volumePre = touchPreStep->GetVolume();
    G4VPhysicalVolume* volumePost = touchPostStep->GetVolume();
    
    G4int eventNum = G4RunManager::GetRunManager() -> GetCurrentEvent() -> GetEventID();
    
    const G4VProcess* process = theTrack -> GetCreatorProcess();
    
    G4String processName;
    if (process)
    {
        processName = process -> GetProcessName();
    }
     
    if (volumePre)
    { 
        G4cout << "\n" << eventNum <<") Currentvolume  = " << volumePre -> GetName() << " -> " << theTrack -> GetParticleDefinition() -> GetParticleName();
    }
    */
}
