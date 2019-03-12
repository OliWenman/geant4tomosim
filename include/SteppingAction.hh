#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "globals.hh"

class SteppingAction : public G4UserSteppingAction
{
    public:
        SteppingAction();
        ~SteppingAction();
  
        void UserSteppingAction(const G4Step*);

    private:
        int n;
  
};
#endif
