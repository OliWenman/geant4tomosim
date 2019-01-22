#include "DefineMaterials.hh"
#include "DefineMaterialsMessenger.hh"

#include "G4Material.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"

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

void DefineMaterials::DefineMolecule(G4String Name, G4double density, G4int n_components)
{
    G4Material* newMaterial = new G4Material(Name, density, n_components);
}

void DefineMaterials::AddElementToMolecule(G4String MoleculeName, G4String ElementName, G4int NumberOfAtoms)
{
    G4Material* Molecule = FindMaterial(MoleculeName);
    G4Element* Element = FindElement(ElementName);
    
    if (Element && Molecule)
        Molecule -> AddElement(Element, NumberOfAtoms);
    else
    {
        if(!Element)
            G4cout << "ERROR: Couldn't find element \"" << ElementName << "\" to add to molecule \"" << MoleculeName << "\"" << G4endl;
        if (!Molecule)
            G4cout << "ERROR: Couldn't find compound \"" << MoleculeName << "\" " << G4endl;
        
        exit(1);
    }
}

void DefineMaterials::DefineCompound(G4String Name, G4double density, G4int n_components)
{
    G4Material* newCompound = new G4Material(Name, density, n_components);
}

void DefineMaterials::AddElementToCompound(G4String CompoundName, G4String ElementName, G4double FractionalMass)
{
    G4Material* Compound = FindMaterial(CompoundName);
    G4Element* Element = FindElement(ElementName);
    
    if (Element && Compound)
        Compound -> AddElement(Element, FractionalMass);
    else
    {
        if(!Element)
            G4cout << "ERROR: Couldn't find element \"" << ElementName << "\" to add to element \"" << CompoundName << "\"" << G4endl;
        if (!Compound)
            G4cout << "ERROR: Couldn't find compound \"" << CompoundName << "\" " << G4endl;
        
        exit(1);
    }
}

G4Material* DefineMaterials::FindMaterial(G4String MaterialName)
{
	//Obtain pointer to NIST material manager to find the build materials 
	return G4NistManager::Instance() -> FindOrBuildMaterial(MaterialName);
}

G4Element* DefineMaterials::FindElement(G4String ElementName)
{
    //Obtain pointer to NIST material manager to find the element
	return G4NistManager::Instance() -> FindOrBuildElement(ElementName, false);
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
