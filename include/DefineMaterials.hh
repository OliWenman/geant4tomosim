#ifndef DefineMaterials_h
#define DefineMaterials_h 1

#include "globals.hh"

class DefineMaterialsMessenger;

class G4Material;
class G4Element;

class DefineMaterials
{
	public:
		DefineMaterials();
		~DefineMaterials();

		void DefineElement(G4String name, G4int z, G4double a, G4double density);
		void DefineIsotope(G4String name, G4int z, G4int A, G4double atomicWeight);
		
		void DefineMolecule(G4String Name, G4double density, G4int n_components);
		void AddElementToMolecule(G4String MoleculeName, G4String ElementName, G4int NumberOfAtoms);
		
		void DefineCompound(G4String Name, G4double density, G4int n_components);
		void AddElementToCompound(G4String CompoundName, G4String ElementName, G4double FractionalMass);

	private:
	
	    G4Material* FindMaterial(G4String MaterialName);
		G4Element* FindElement(G4String ElementName);
	
		DefineMaterialsMessenger* materialsMessenger;

};
#endif
