#include "DefineMaterials.hh"
#include "DefineMaterialsMessenger.hh"

#include "G4Material.hh"
#include "G4SystemOfUnits.hh"

DefineMaterials::DefineMaterials()
{
	materialsMessenger = new DefineMaterialsMessenger(this);
}

DefineMaterials::~DefineMaterials()
{
	delete materialsMessenger;
}

void DefineMaterials::DefineElement(G4String name, G4int z, G4double a, G4double density)
{
	G4Material* newMaterial = new G4Material(name, z, a, density);
}

void DefineMaterials::DefineIsotope(G4String name, G4int z, G4int A, G4double atomicWeight)
{
	G4Isotope* newIsotope = new G4Isotope(name, z, A, atomicWeight);
}

void DefineMaterials::DefineMolecules()
{

}

void DefineMaterials::DefineCompounds()
{

}

//=======================================================================

	/*//Molecules
	G4String name = "Hydrogen";
	G4String symbol = "H";
	G4int z = 1;
	G4double a = 1.01*g/mole;	
	G4Element* hydrogen = new G4Element(name, symbol, z, a); 

	name = "Oxygen";
	symbol = "O";
	z = 8;
	a = 16.00*g/mole; 
	G4Element* oxygen = new G4Element(name, symbol, z, a);

	name = "Water";
	G4double density = 1.000*g/cm3;
	G4int n_components = 2;
	G4Material* H2O = new G4Material(name, density, n_components);

	H2O -> AddElement(hydrogen, 2);
	H2O -> AddElement(oxygen, 1);

	return H2O;

//=====================================================================

	//Isotopes
	G4int z = 92;
	G4int A = 235;
	G4double a = 235.044*g/mole;
	G4String name = "U235";
	G4Isotope* isoU235 = new G4Isotope(name, z, A, a);

	z = 92;
	A = 238;
	a = 238.051*g/mole;
	name = "U238";
	G4Isotope* isoU238 = new G4Isotope(name, z, A, a);

	name = "enr. U";
	G4String symbol = "U";
	G4int nComponents = 2;
	G4Element* enrichedU = new G4Element(name, symbol, nComponents);

	enrichedU -> AddIsotope(isoU235, 80.*perCent);
	enrichedU -> AddIsotope(isoU238, 20.*perCent);

	G4double density = 19.1*g/cm3;
	nComponents = 1;
	G4Material* mat_enrichedU = new G4Material("enr. U", density, nComponents);
	G4int fraction_mass = 1;
	mat_enrichedU -> AddElement(enrichedU, fraction_mass);
*/
