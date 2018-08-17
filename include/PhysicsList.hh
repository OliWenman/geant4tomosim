#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VModularPhysicsList.hh"

/// Modular physics list
/// - G4EmStandardPhysics

class PhysicsList: public G4VModularPhysicsList
{
	public:
  		PhysicsList();
  		virtual ~PhysicsList();

  		virtual void SetCuts();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

