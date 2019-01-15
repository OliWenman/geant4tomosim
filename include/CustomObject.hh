#ifndef CustomObject_h 
#define CustomObject_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "vector.hh"

struct CustomObject
{
    G4String Name;
    G4String Type;
    std::vector<double> Dimensions;
    G4String Material;
    G4ThreeVector Position;
    G4ThreeVector Rotation;
};
#endif
