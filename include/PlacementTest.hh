#ifndef PlacementTest_h
#define PlacementTest_h 1

#include "G4PVPlacement.hh"

class PlacementTest : public G4PVPlacement
{
    public:
    PlacementTest(G4RotationMatrix *pRot, const G4ThreeVector &tlate, G4LogicalVolume *pCurrentLogical, const G4String &pName, G4LogicalVolume *pMotherLogical, G4bool pMany, G4int pCopyNo, G4bool pSurfChk) :G4PVPlacement(pRot, tlate, pCurrentLogical, pName, pMotherLogical, pMany, pCopyNo, pSurfChk=false){}
    ~PlacementTest(){G4cout << "\nI'm being deleted " << G4endl; G4cout << "\nDeleted" << G4endl;}
};
#endif
