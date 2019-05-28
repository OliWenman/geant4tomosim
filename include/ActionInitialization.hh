#ifndef ACTION_INITIALIZATION_H
#define ACTION_INITIALIZATION_H

#include "G4VUserActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "StackingAction.hh"
#include "SteppingAction.hh"

class ActionInitialization : public G4VUserActionInitialization {
public:
  ActionInitialization();
  ~ActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;
};

#endif
