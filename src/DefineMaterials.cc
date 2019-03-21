#include "DefineMaterials.hh"
#include "DefineMaterialsMessenger.hh"

#include "G4Material.hh"
#include "G4Isotope.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4NistManager.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"
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
    G4Material* element = G4NistManager::Instance() -> FindOrBuildMaterial(name);
    if (!element) {G4Material* newElement = new G4Material(name, z, a, density);}
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
    G4Material* material = G4NistManager::Instance() -> FindOrBuildMaterial(Name);
    if (!material) {G4Material* newMaterial = new G4Material(Name, density, n_components);}
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
    G4Material* compound = G4NistManager::Instance() -> FindOrBuildMaterial(Name);
    
    if (!compound){G4Material* newCompound = new G4Material(Name, density, n_components);}
}

void DefineMaterials::AddElementToCompound(G4String CompoundName, G4String ElementName, G4double FractionalMass)
{
    G4Material* Compound = G4NistManager::Instance() -> FindOrBuildMaterial(CompoundName);;
    G4Element* Element = FindElement(ElementName);
    
    if (Element && Compound) {Compound -> AddElement(Element, FractionalMass);}
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

void DefineMaterials::FillOpticalProperties_xraylib(std::string MaterialsName, double energyValues[], int nSize)
{   
    G4Material* MaterialFound = FindMaterial(MaterialsName);
    
    double refractiveIndexes_Re [nSize];
    double refractiveIndexes_Im [nSize];
    double absorption_length [nSize];
    double efficiency[nSize];
    
    std::vector<Ele_FracMass> elementTable;
    
    //If material is found, find it's element components
    if (MaterialFound)
    {
        const G4ElementVector* Material_elements = MaterialFound -> GetElementVector();
        const G4double* Material_fractional_mass = MaterialFound -> GetFractionVector(); 
        const int nElements = Material_elements -> size();
        
        std::vector<Ele_FracMass> elementTableTemp (nElements);
        
        //G4cout << "\nMaterial: " << MaterialFound -> GetName()
        //       << "\nMade up of the following elements\n" << std::flush;
        
        //Find the elements name, fractional mass, Z and density 
        for (int i = 0; i < nElements; i++)
        {
            G4Element const* element = Material_elements[0][i];
            G4String const eleName = element -> GetName();
            elementTableTemp[i].name = eleName;
            elementTableTemp[i].frac_mass = Material_fractional_mass[i];
            elementTableTemp[i].Z = element -> GetZ();
            elementTableTemp[i].density = ElementDensity(elementTableTemp[i].Z);
            //G4cout << "Element: " << elementTableTemp[i].name
            //       << " -> mass: " << elementTableTemp[i].frac_mass*100 << "%\ \n" << std::flush;
        }
        
        elementTable = elementTableTemp;
    }
    else
    {
        G4cout << "\nWARNING: couldn't find material " << MaterialFound -> GetName() << G4endl;
    }
    
    //TotalValues
    double total_RI_Re = 0;
    double total_RI_Im = 0;
    double total_abs_len = 0;
    
    //Loop through the energy values
    for (int energyValue = 0 ; energyValue < nSize ; energyValue++)
    {
        //Loop through the elements, find its refractive index and absoprtion length
        for (int ele = 0; ele < elementTable.size() ; ele++)
        { 
            //Add to the total values the fractional values
            total_RI_Re = total_RI_Re + (elementTable[ele].frac_mass * Refractive_Index_Re(elementTable[ele].name, energyValues[energyValue], elementTable[ele].density)); 
            total_RI_Im = total_RI_Im + (elementTable[ele].frac_mass * Refractive_Index_Im(elementTable[ele].name, energyValues[energyValue], elementTable[ele].density)); 
            double mass_absorption_coefficient = CS_Total(elementTable[ele].Z, energyValues[energyValue]); //[cm2/g]
            total_abs_len = total_abs_len + (elementTable[ele].frac_mass *(1./(mass_absorption_coefficient*elementTable[ele].density))*cm); //[cm-1]=[cm2/g]*[g/cm3] => [cm]=1/[cm-1] 
        }
        
        //Save the values in the arrays
        double RI_Re = total_RI_Re;
        double RI_Im = total_RI_Im;
        double abs_len = total_abs_len;
        
        refractiveIndexes_Re[energyValue] = RI_Re; 
        refractiveIndexes_Im[energyValue] = RI_Im; 
        absorption_length[energyValue] = abs_len;
        efficiency[energyValue] = 1;
        
        //Convert the units of keV to Geant4 keV
        energyValues[energyValue] = energyValues[energyValue]*keV;
        
        total_RI_Re = 0;
        total_RI_Im = 0;
        total_abs_len = 0;
    }
    
    //Add the arrays to the properties table
    G4MaterialPropertiesTable* MPT = new G4MaterialPropertiesTable();   
    MPT -> AddProperty("RINDEX", energyValues, refractiveIndexes_Re, nSize); 
    MPT -> AddProperty("REALRINDEX", energyValues, refractiveIndexes_Re, nSize); 
    MPT -> AddProperty("IMAGINARYRINDEX", energyValues, refractiveIndexes_Im, nSize); 
    MPT -> AddProperty("ABSLENGTH", energyValues, absorption_length, nSize);  
    //MPT -> AddProperty("EFFICIENCY", energyValues, efficiency, nSize);
   
    MaterialFound -> SetMaterialPropertiesTable(MPT); 
}

void AddOpticalProperty_xraylib(std::string MaterialsName, const char *key, double energyValues[], int NumElements)
{
    
}

void DefineMaterials::AddOpticalProperty(std::string MaterialsName, const char *key, double energyValues[], double opticalProperty[], int NumElements)
{
    G4Material* MaterialFound = FindMaterial(MaterialsName);
    
    if(MaterialFound)
    {
        G4MaterialPropertiesTable* MPT = MaterialFound -> GetMaterialPropertiesTable();
        
        if(!MPT)
        {
            MPT = new G4MaterialPropertiesTable();
        }
        
        MPT -> AddProperty(key, energyValues, opticalProperty, NumElements);
        MaterialFound -> SetMaterialPropertiesTable(MPT); 
    }
}

void DefineMaterials::PrintMPT(std::string Material)
{
    G4Material* MaterialFound = FindMaterial(Material);
    
    if(MaterialFound)
    {
        G4MaterialPropertiesTable* MPT_Found = MaterialFound -> GetMaterialPropertiesTable();
        
        if (MPT_Found)
        {
            G4cout << "\nMaterialPropertiesTable for " << Material << G4endl;
            MPT_Found -> DumpTable();
        }
        else
        {
            G4cout << "\nMaterial '" << Material << "' doesn't have a MaterialsPropertiesTable" << G4endl;
        }
    }
    else
    {
        G4cout << "Couldn't find material '" << Material << "'" << G4endl;
    }
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
