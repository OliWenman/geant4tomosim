/*
A class that can define new G4Element's, G4Isotope's and G4material's to be used.

Author: Oliver Jack Wenman

*/

#ifndef DefineMaterials_h
#define DefineMaterials_h 1

#include "globals.hh"
#include <vector>
#include <string>

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
		void AddToIsoMix(G4String isotopemix, G4String isotopename, G4double abundance);
		void AddIsoMixDensity(G4String isotopemix, G4double denisty);
		
		void DefineMolecule(G4String name, G4double density, G4int n_components);
		void AddElementToMolecule(G4String molecule, G4String element, G4int nAtoms);
		
		void DefineCompound(G4String name, G4double density, G4int n_components);
		void AddElementToCompound(G4String compound, G4String element, G4double fractionalmass);
		
        void DefineMixture(G4String name, G4double density, G4int n_components);
	    void AddMaterialToMixture(G4String mixture, G4String material, G4double fractionalmass);
	    
	    void FillOpticalProperties_xraylib(std::string material, double energyValues[], int nElements);
	    void AddOpticalProperty(std::string material, const char *key, double energyValues[], double opticalProperty[], int nElements);
	    void AddOpticalProperty_xraylib(std::string material, const char *key, double energyValues[], int nElements);
	    void PrintMPT(std::string material);
	    
	private:
	
	    G4Material* FindMaterial(G4String material);
		G4Element*  FindElement (G4String element);
		G4Isotope*  FindIsotope (G4String isotope);
	
		DefineMaterialsMessenger* materialsMessenger;

        std::vector<G4Isotope*> isotopelist;
        std::vector<G4Element*> elementlist;
};

struct Ele_FracMass
{
    G4String name;
    G4double frac_mass;
    G4int Z;
    G4double density;
};

#endif
