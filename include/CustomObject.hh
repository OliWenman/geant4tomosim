#ifndef CustomObject_h 
#define CustomObject_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include <vector>

//A structure that contains all needed information about an object
struct CustomObject
{
    G4String Name;
    G4String Type;
    int Number;
   
    G4String Material;
    bool WorldPlacement;
    
    std::vector<double> Dimensions;
    
    G4ThreeVector StartingPosition;
    G4ThreeVector Position;
    
    G4ThreeVector StartingRotation;
    G4ThreeVector Rotation;
};

struct ComplexObject : CustomObject
{
    G4String ComponentName1;
    G4String ComponentName2;
    
    G4ThreeVector InsideRotation;
    G4ThreeVector InsidePosition; 
};
#endif
