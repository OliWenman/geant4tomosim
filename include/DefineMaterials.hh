#ifndef DefineMaterials_h
#define DefineMaterials_h 1

#include "globals.hh"
#include <vector>

class DefineMaterialsMessenger;

class G4Material;
class G4Element;
class G4Isotope;

class DefineMaterials
{
	public:
		DefineMaterials();
		~DefineMaterials();

		void DefineElement(G4String name, G4int z, G4double a, G4double density);
		void DefineIsotope(G4String name, G4int z, G4int A, G4double atomicWeight);
		void DefineIsotopeMix(G4String name, G4String symbol, G4int nComponents);
		void AddToIsoMix(G4String IsotopeMixName, G4String IsotopeName, G4double Abundance);
		void AddIsoMixDensity(G4String IsotopeMixName, G4double Density);
		
		void DefineMolecule(G4String Name, G4double density, G4int n_components);
		void AddElementToMolecule(G4String MoleculeName, G4String ElementName, G4int NumberOfAtoms);
		
		void DefineCompound(G4String Name, G4double density, G4int n_components);
		void AddElementToCompound(G4String CompoundName, G4String ElementName, G4double FractionalMass);
		
        void DefineMixture(G4String Name, G4double density, G4int n_components);
	    void AddMaterialToMixture(G4String MixtureName, G4String MaterialName, G4double FractionMass);
	    
	private:
	
	    G4Material* FindMaterial(G4String MaterialName);
		G4Element* FindElement(G4String ElementName);
		G4Isotope* FindIsotope(G4String IsoName);
	
		DefineMaterialsMessenger* materialsMessenger;

        std::vector<G4Isotope*> IsotopeList;
        std::vector<G4Element*> ElementList;
};
#endif
