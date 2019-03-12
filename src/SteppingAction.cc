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
    n = 0;
}

/////////////////////////////////////////////////////////////////////////////
SteppingAction::~SteppingAction()
{

}

/////////////////////////////////////////////////////////////////////////////
void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
    /*G4String processname = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    G4ThreeVector momentumDirection = aStep->GetPostStepPoint()->GetMomentumDirection();
    
    if (processname == "Rayl") {G4cout << n << ")" << processname << "new momentum = " << momentumDirection << G4endl; n++;}*/

    //G4int eventNum = G4RunManager::GetRunManager() -> GetCurrentEvent() -> GetEventID();

    //G4Track* theTrack = aStep->GetTrack();
     
    //G4StepPoint* PreStep = aStep->GetPreStepPoint();
    //G4StepPoint* PostStep = aStep->GetPostStepPoint();
     
    //G4TouchableHandle touchPreStep = PreStep->GetTouchableHandle();
    //G4TouchableHandle touchPostStep = PostStep->GetTouchableHandle();
    
    //G4VPhysicalVolume* volumePre = touchPreStep->GetVolume();
    //G4VPhysicalVolume* volumePost = touchPostStep->GetVolume();
    /*if (volumePost)
    {
        G4cout << eventNum << ") volumePre = " << volumePre -> GetName() << ", volumePost = " << volumePost -> GetName() << G4endl;
    }*/
    
}
