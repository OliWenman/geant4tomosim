#include "DefineMaterials.hh"
#include "DefineMaterialsMessenger.hh"

#include "G4Material.hh"
#include "G4Isotope.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4NistManager.hh"
#include "G4MaterialPropertiesTable.hh"
//#include "G4UnitDefinition.hh"

#include "xraylib.h"

DefineMaterials::DefineMaterials()
{
	materialsMessenger = new DefineMaterialsMessenger(this);
}

DefineMaterials::~DefineMaterials()
{	
    //Free up memory of stored custom G4Elements and G4Isotopes
    for (int i =0; i< ElementList.size();i++)
        delete (ElementList[i]);
   
    for (int i =0; i< IsotopeList.size();i++)
        delete (IsotopeList[i]); 
    
    ElementList.clear();
    IsotopeList.clear();
    
    delete materialsMessenger;
}

void DefineMaterials::DefineElement(G4String name, G4int z, G4double a, G4double density)
{
	G4Material* newElement = new G4Material(name, z, a, density);
}

void DefineMaterials::DefineIsotope(G4String name, G4int z, G4int A, G4double atomicWeight)
{
	G4Isotope* newIsotope = new G4Isotope(name, z, A, atomicWeight);
	IsotopeList.push_back(newIsotope);
}

void DefineMaterials::DefineIsotopeMix(G4String name, G4String symbol, G4int nComponents)
{
    G4Element* newIsotopeMix = new G4Element(name, symbol, nComponents);
    ElementList.push_back(newIsotopeMix);
}

void DefineMaterials::AddToIsoMix(G4String IsotopeMixName, G4String IsotopeName, G4double Abundance)
{
    int IsoNumber;
    int IsoMixNumber;
    bool IsoFound = false;
    bool IsoMixFound = false;
    
    for(int n = 0 ; n < IsotopeList.size() ; n++)
    {
        if (IsotopeList[n] -> GetName() == IsotopeName)
        {    
            IsoNumber = n;
            IsoFound = true;
            break;
        } 
    } 
    
    for(int n = 0 ; n < ElementList.size() ; n++)
    {
        if (ElementList[n] -> GetName() == IsotopeMixName)
        {    
            IsoMixNumber = n;
            IsoMixFound = true;
            G4cout << "\nIsotopeMix nComp = " << ElementList[n] -> GetNumberOfIsotopes() << G4endl;
            break;
        } 
    } 
    
    if(IsoMixFound == true && IsoFound == true)
    {
        ElementList[IsoMixNumber] -> AddIsotope(IsotopeList[IsoNumber], Abundance);
    }
    else
    {
        if(IsoMixFound == false)
            G4cout << "ERROR: Couldn't find isotope mix \"" << IsotopeMixName << "\" to add the isotope \"" << IsotopeName << "\"" << G4endl;
        if (IsoFound == false)
            G4cout << "ERROR: Couldn't find isotope \"" << IsotopeName << "\" " << G4endl;
        
        exit(1);
    }   
}

void DefineMaterials::AddIsoMixDensity(G4String IsotopeMixName, G4double Density)
{
    int IsoMixNumber;
    bool IsoMixFound = false;
    
    for(int n = 0 ; n < ElementList.size() ; n++)
    {
        if (ElementList[n] -> GetName() == IsotopeMixName)
        {    
            IsoMixNumber = n;
            IsoMixFound = true;
            break;
        } 
    } 
    
    if(IsoMixFound == true )
    {
        G4Material* newMaterial = new G4Material(IsotopeMixName, Density, 1);
        newMaterial -> AddElement(ElementList[IsoMixNumber], 1);
    }
    else
    {
        G4cout << "ERROR: Couldn't find isotope mix \"" << IsotopeMixName << "\" to add the density \"" << G4BestUnit(Density, "Density") << "\"" << G4endl;
    }
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

void DefineMaterials::DefineMixture(G4String Name, G4double density, G4int n_components)
{
    G4Material* newMaterial = new G4Material(Name, density, n_components);
}

void DefineMaterials::AddMaterialToMixture(G4String MixtureName, G4String MaterialName, G4double FractionalMass)
{
    G4Material* Mixture = FindMaterial(MixtureName);
    G4Material* AddMaterial = FindMaterial(MaterialName);
    G4Element* AddElement = FindElement(MaterialName);
    
    if (Mixture && AddMaterial)
        Mixture -> AddMaterial(AddMaterial, FractionalMass);
        
    else if (Mixture && AddElement)
        Mixture -> AddElement(AddElement, FractionalMass);
        
    else
    {
        if(!Mixture)
            G4cout << "ERROR: Couldn't find compound \"" << MixtureName << "\" " << G4endl;
        if (!AddElement && !AddMaterial)
             G4cout << "ERROR: Couldn't find \"" << MaterialName << "\" to add to mixture \"" << MixtureName << "\"" << G4endl;
        
        exit(1);
    }
}

void DefineMaterials::AddRefractiveIndex(std::string MaterialsName, G4double density, double energyValues[], int nSize)
{   
    G4Material* Material = FindMaterial(MaterialsName);
    
    double refractiveIndexes [nSize];
    const char* materialNamec = MaterialsName.c_str();
    
    for (int i = 0 ; i < nSize ; i++){refractiveIndexes[i] = 0.8/*Refractive_Index_Re(materialNamec, energyValues[i], density)*/; energyValues[i] = energyValues[i]*keV;}
    
    G4MaterialPropertiesTable* MPT = new G4MaterialPropertiesTable();   
    MPT -> AddProperty("RINDEX", energyValues, refractiveIndexes, nSize); 
    
    G4cout << "\n" << MaterialsName << G4endl;
    MPT -> DumpTable();
   
    Material -> SetMaterialPropertiesTable(MPT); 
}

G4Material* DefineMaterials::FindMaterial(G4String MaterialName)
{
	//Obtain pointer to NIST material manager to find the build materials 
	G4Material* material = G4NistManager::Instance() -> FindOrBuildMaterial(MaterialName);
	
	if (!material){material = G4NistManager::Instance() -> FindOrBuildMaterial("G4_" + MaterialName);}
	if (!material){G4cout << "\nERROR: material " << MaterialName << " not found " << G4endl;}
	
	return material;
}

G4Element* DefineMaterials::FindElement(G4String ElementName)
{
    //Obtain pointer to NIST material manager to find the element
	G4Element* element = G4NistManager::Instance() -> FindOrBuildElement(ElementName, false);
	
	//If can't find element in NIST database, try the custom ones
	if (!element)
	{
	    for(int n = 0 ; n < ElementList.size() ; n++)
        {
            if (ElementList[n] -> GetName() == ElementName)
            {    
                element = ElementList[n];
                break;
            } 
        } 
	}
	
	return element;
	
}

/*void AddRefractiveIndex(G4String MaterialName, G4double refractiveIndex, G4double energy)
{

}*/
