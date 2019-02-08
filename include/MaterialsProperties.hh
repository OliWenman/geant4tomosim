#ifndef MaterialsProperties_h
#define MaterialsProperties_h 1

#include <vector>
#include "G4MaterialPropertiesTable.hh"

struct MaterialsProperties
{
    G4String Name;
    G4double refractiveIndex;
    G4double energy;
};

#endif
