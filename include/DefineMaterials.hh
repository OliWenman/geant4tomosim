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
		void DefineMolecules();
		void DefineCompounds();

	private:
		DefineMaterialsMessenger* materialsMessenger;

};

#endif
