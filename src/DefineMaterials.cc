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

#include "Exceptions.hh"
#include "xraylib.h"

DefineMaterials::DefineMaterials()
{
	materialsMessenger = new DefineMaterialsMessenger(this);
}

DefineMaterials::~DefineMaterials()
{	
    //Free up memory of stored custom G4Elements and G4Isotopes
    for (int i =0; i< ElementList.size();i++) {delete (ElementList[i]);}
    for (int i =0; i< IsotopeList.size();i++) {delete (IsotopeList[i]);} 
    
    ElementList.clear();
    IsotopeList.clear();
    
    delete materialsMessenger;
}

void DefineMaterials::DefineElement(G4String name, G4int z, G4double a, G4double density)
{
    //Make sure all are positive numbers
    if (z <= 0. || a <= 0. || density <= 0.) { throw parameterIsNegative();}

    //Check if the element already exists
    G4Material* element = G4NistManager::Instance() -> FindOrBuildMaterial(name);
    if (!element) {G4Material* newElement = new G4Material(name, z, a, density);}
    else          {throw (materialAlreadyExists());}
}

void DefineMaterials::DefineIsotope(G4String name, G4int z, G4int a, G4double atomicWeight)
{
    //Make sure all are positive numbers
    if (z <= 0. || a <= 0. || atomicWeight <= 0.) { throw parameterIsNegative();}

    //Check if isotope already exists
    for (int i = 0 ; i < IsotopeList.size() ; i++)
    {
        if (IsotopeList[i]->GetName() == name) {throw (materialAlreadyExists());}
    } 
	G4Isotope* newIsotope = new G4Isotope(name, z, a, atomicWeight);
	IsotopeList.push_back(newIsotope);
}

void DefineMaterials::DefineIsotopeMix(G4String name, G4String symbol, G4int nComponents)
{
     //Make sure all are positive numbers
    if (nComponents <= 0.) { throw parameterIsNegative();}

    //Check if element already exists
    for (int i = 0 ; i < ElementList.size() ; i++)
    {
        if (ElementList[i]->GetName() == name) {throw (materialAlreadyExists());}
    } 

    G4Element* newIsotopeMix = new G4Element(name, symbol, nComponents);
    ElementList.push_back(newIsotopeMix);
}

void DefineMaterials::AddToIsoMix(G4String isotopemix, G4String isotope, G4double abundance)
{
    int isoNumber;
    int isomixNumber;
    bool isofound = false;
    bool isomixFound = false;
 
    if (abundance <= 0.) { throw parameterIsNegative();}
    
    for(int n = 0 ; n < IsotopeList.size() ; n++)
    {
        if (IsotopeList[n] -> GetName() == isotope)
        {    
            isoNumber = n;
            isofound = true;
            break;
        } 
    } 
    
    for(int n = 0 ; n < ElementList.size() ; n++)
    {
        if (ElementList[n] -> GetName() == isotopemix)
        {    
            isomixNumber = n;
            isomixFound = true;
            break;
        } 
    } 
    
    if(isomixFound && isofound)
    {
        ElementList[isomixNumber] -> AddIsotope(IsotopeList[isoNumber], abundance);
    }
    else
    {
        if(isomixFound == false)
            G4cout << "Warning: Couldn't find isotope mix \"" << isotopemix << "\" to add the isotope \"" << isotope << "\"" << G4endl;
        if (isofound == false)
            G4cout << "Warning: Couldn't find isotope \"" << isotope << "\" " << G4endl;
        
        throw (parameterNotFound());
    }   
}

void DefineMaterials::AddIsoMixDensity(G4String isotopemix, G4double density)
{
    int noAtoms = 1;
    int isomixNumber;
    bool isomixFound = false;
    
    if (density <= 0.) { throw parameterIsNegative();}
    
    for(int n = 0 ; n < ElementList.size() ; n++)
    {
        if (ElementList[n] -> GetName() == isotopemix)
        {    
            isomixNumber = n;
            isomixFound = true;
            break;
        } 
    } 
    
    if(isomixFound == true )
    {
        G4Material* newMaterial = new G4Material(isotopemix, density, noAtoms);
        newMaterial -> AddElement(ElementList[isomixNumber], noAtoms);
    }
    else
    {
        throw (parameterNotFound());
    }
}

void DefineMaterials::DefineMolecule(G4String molecule, G4double density, G4int n_components)
{
    if (n_components <= 0. || density <= 0.) { throw parameterIsNegative();}

    G4Material* material = G4NistManager::Instance() -> FindOrBuildMaterial(molecule);
    if (!material) {G4Material* newMaterial = new G4Material(molecule, density, n_components);}
    else           {throw materialAlreadyExists();}
}

void DefineMaterials::AddElementToMolecule(G4String moleculeName, G4String elementName, G4int nAtoms)
{
    if (nAtoms <= 0.) { throw parameterIsNegative();}

    G4Material* molecule = FindMaterial(moleculeName);
    G4Element* element = FindElement(elementName);
    
    if (element && molecule)
        molecule -> AddElement(element, nAtoms);
    else
    {
        if(!element)
            G4cout << "ERROR: Couldn't find element \"" << elementName << "\" to add to molecule \"" << moleculeName << "\"" << G4endl;
        if (!molecule)
            G4cout << "ERROR: Couldn't find compound \"" << moleculeName << "\" " << G4endl;
        
        throw (parameterNotFound());
    }
}

void DefineMaterials::DefineCompound(G4String compound, G4double density, G4int n_components)
{
    if (density <= 0. || n_components <= 0.) { throw parameterIsNegative();}

    G4Material* material = G4NistManager::Instance() -> FindOrBuildMaterial(compound);
    
    if (!material){G4Material* newCompound = new G4Material(compound, density, n_components);}
    else {throw (materialAlreadyExists());}
}

void DefineMaterials::AddElementToCompound(G4String compoundName, G4String elementName, G4double fractionalmass)
{
    if (fractionalmass <= 0.) { throw parameterIsNegative();}

    G4Material* compound = G4NistManager::Instance() -> FindOrBuildMaterial(compoundName);;
    G4Element* element = FindElement(elementName);
    
    if (element && compound) {compound -> AddElement(element, fractionalmass);}
    else
    {
        if(!element)
            G4cout << "ERROR: Couldn't find element \"" << elementName << "\" to add to element \"" << compoundName << "\"" << G4endl;
        if (!compound)
            G4cout << "ERROR: Couldn't find compound \"" << compoundName << "\" " << G4endl;
        
        throw (parameterNotFound());
    }
}

void DefineMaterials::DefineMixture(G4String compound, G4double density, G4int n_components)
{
    if (density <= 0. || n_components <= 0.) { throw parameterIsNegative();}

    G4Material* mixture = G4NistManager::Instance() -> FindOrBuildMaterial(compound);
    if (!mixture){G4Material* newMixture = new G4Material(compound, density, n_components);}
    else         {throw (materialAlreadyExists());}
    
}

void DefineMaterials::AddMaterialToMixture(G4String mixtureName, G4String materialName, G4double fractionalmass)
{
    if (fractionalmass <= 0.) { throw parameterIsNegative();}

    G4Material* mixture  = FindMaterial(mixtureName);
    G4Material* material = FindMaterial(materialName);
    G4Element*  element  = FindElement(materialName);
    
    if (mixture && material)
        mixture -> AddMaterial(material, fractionalmass);
        
    else if (mixture && element)
        mixture -> AddElement(element, fractionalmass);
        
    else
    {
        if(!mixture)
            G4cout << "ERROR: Couldn't find compound \"" << mixtureName << "\" " << G4endl;
        if (!element && !material)
             G4cout << "ERROR: Couldn't find \"" << materialName << "\" to add to mixture \"" << mixtureName << "\"" << G4endl;
        
        throw (parameterNotFound());
    }
}

void DefineMaterials::FillOpticalProperties_xraylib(std::string materialsName, double energyValues[], int nSize)
{   
    G4Material* material = FindMaterial(materialsName);
    
    double refractiveIndexes_Re [nSize];
    double refractiveIndexes_Im [nSize];
    double absorption_length [nSize];
    double efficiency[nSize];
    
    std::vector<Ele_FracMass> elementTable;
    
    //If material is found, find it's element components
    if (material)
    {
        const G4ElementVector* material_elements = material -> GetElementVector();
        const G4double* material_fractionalmass = material -> GetFractionVector(); 
        const int nElements = material_elements -> size();
        
        std::vector<Ele_FracMass> elementTableTemp (nElements);
        
        //Find the elements name, fractional mass, Z and density 
        for (int i = 0; i < nElements; i++)
        {
            G4Element const* element = material_elements[0][i];
            G4String const eleName = element -> GetName();
            elementTableTemp[i].name = eleName;
            elementTableTemp[i].frac_mass = material_fractionalmass[i];
            elementTableTemp[i].Z = element -> GetZ();
            elementTableTemp[i].density = ElementDensity(elementTableTemp[i].Z);
        }
        
        elementTable = elementTableTemp;
    }
    else
    {
        throw (parameterNotFound());
    }
    
    //TotalValues
    double total_ri_RE = 0;
    double total_ri_Im = 0;
    double total_abs_len = 0;
    
    //Loop through the energy values
    for (int energyValue = 0 ; energyValue < nSize ; energyValue++)
    {
        //Loop through the elements, find its refractive index and absoprtion length
        for (int ele = 0; ele < elementTable.size() ; ele++)
        { 
            if (energyValues[energyValue] <= 0) { throw (parameterIsNegative()); } 
        
            //Add to the total values the fractional values
            total_ri_RE = total_ri_RE + (elementTable[ele].frac_mass * Refractive_Index_Re(elementTable[ele].name, energyValues[energyValue], elementTable[ele].density)); 
            total_ri_Im = total_ri_Im + (elementTable[ele].frac_mass * Refractive_Index_Im(elementTable[ele].name, energyValues[energyValue], elementTable[ele].density)); 
            double mass_absorption_coefficient = CS_Total(elementTable[ele].Z, energyValues[energyValue]); //[cm2/g]
            total_abs_len = total_abs_len + (elementTable[ele].frac_mass *(1./(mass_absorption_coefficient*elementTable[ele].density))*cm); //[cm-1]=[cm2/g]*[g/cm3] => [cm]=1/[cm-1] 
        }
        
        //Save the values in the arrays
        double ri_RE = total_ri_RE;
        double ri_Im = total_ri_Im;
        double abs_len = total_abs_len;
        
        refractiveIndexes_Re[energyValue] = ri_RE; 
        refractiveIndexes_Im[energyValue] = ri_Im; 
        absorption_length[energyValue] = abs_len;
        efficiency[energyValue] = 1;
        
        //Convert the units of keV to Geant4 keV
        energyValues[energyValue] = energyValues[energyValue]*keV;
        
        total_ri_RE = 0;
        total_ri_Im = 0;
        total_abs_len = 0;
    }
    
    //Add the arrays to the properties table
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();   
    mpt -> AddProperty("RINDEX", energyValues, refractiveIndexes_Re, nSize); 
    mpt -> AddProperty("REALRINDEX", energyValues, refractiveIndexes_Re, nSize); 
    mpt -> AddProperty("IMAGINARYRINDEX", energyValues, refractiveIndexes_Im, nSize); 
    mpt -> AddProperty("ABSLENGTH", energyValues, absorption_length, nSize);  
    //mpt -> AddProperty("EFFICIENCY", energyValues, efficiency, nSize);
   
    material -> SetMaterialPropertiesTable(mpt); 
}

void AddOpticalProperty_xraylib(std::string materialsName, const char *key, double energyValues[], int nelements)
{
    
}

void DefineMaterials::AddOpticalProperty(std::string materialsName, const char *key, double energyValues[], double opticalProperty[], int nelements)
{
    G4Material* material = FindMaterial(materialsName);
    
    if(material)
    {
        G4MaterialPropertiesTable* mpt = material -> GetMaterialPropertiesTable();
        
        if(!mpt)
        {
            mpt = new G4MaterialPropertiesTable();
        }
        
        for (int i = 0; i < nelements ; i++)
        {
            if (energyValues[i] <= 0.) { throw (parameterIsNegative()); } 
        }
        
        mpt -> AddProperty(key, energyValues, opticalProperty, nelements);
        material -> SetMaterialPropertiesTable(mpt); 
    }
    else
    {
        throw parameterNotFound();
    }
}

void DefineMaterials::PrintMPT(std::string materialName)
{
    G4Material* material = FindMaterial(materialName);
    
    if(material)
    {
        G4MaterialPropertiesTable* mpt_Found = material -> GetMaterialPropertiesTable();
        
        if (mpt_Found)
        {
            G4cout << "\nMaterialPropertiesTable for " << materialName << G4endl;
            mpt_Found -> DumpTable();
        }
        else
        {
            G4cout << "\nMaterial '" << materialName << "' doesn't have a MaterialsPropertiesTable" << G4endl;
        }
    }
    else
    {
        G4cout << "Couldn't find material '" << materialName << "'" << G4endl;
    }
}

G4Material* DefineMaterials::FindMaterial(G4String materialName)
{
	//Obtain pointer to NIST material manager to find the build materials 
	G4Material* material = G4NistManager::Instance() -> FindOrBuildMaterial(materialName);
	
	if (!material){material = G4NistManager::Instance() -> FindOrBuildMaterial("G4_" + materialName);}
	
	if (!material) {throw (parameterNotFound());}
	
	return material;
}

G4Element* DefineMaterials::FindElement(G4String elementName)
{
    //Obtain pointer to NIST material manager to find the element
	G4Element* element = G4NistManager::Instance() -> FindOrBuildElement(elementName, false);
	
	//If can't find element in NIST database, try the custom ones
	if (!element)
	{
	    for(int n = 0 ; n < ElementList.size() ; n++)
        {
            if (ElementList[n] -> GetName() == elementName)
            {    
                element = ElementList[n];
                break;
            } 
        } 
	}
	
	if (!element) {throw (parameterNotFound());}
	
	return element;
	
}

/*void AddRefractiveIndex(G4String materialName, G4double refractiveIndex, G4double energy)
{

}*/
