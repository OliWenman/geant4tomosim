#include "DefineMaterialsMessenger.hh"
#include "DefineMaterials.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

#include "G4TwoVector.hh"
#include "G4Tokenizer.hh"

DefineMaterialsMessenger::DefineMaterialsMessenger(DefineMaterials* DefMaterials): materials(DefMaterials)
{
//=================================================================================================
	MaterialsDirectory = new G4UIdirectory("/Materials/", this);
	MaterialsDirectory -> SetGuidance("Directory to define new materials to be used");

//=================================================================================================
    //DEFINING ELEMENTS/ISOTOPES
    
	DefElement = new G4UIcmdWithAString("/Materials/Define/Element", this);
	DefElement -> SetGuidance("Define an element"
	                          "Element_name Z_number Atomic_weight Unit Density Unit");

	DefIsotope = new G4UIcmdWithAString("/Materials/Define/Isotope", this);
	DefIsotope -> SetGuidance("Define an isotope\n"
	                          "Isotope_name, Z_number, Nucleon_number Atomic_weight Unit");
	                          
	IsotopeMix = new G4UIcmdWithAString("/Materials/Define/IsotopeMix", this);
	IsotopeMix -> SetGuidance("Create an element with a certain mixture of isotopes\n"
	                          "Isotope_mix_Name  Symbol Number_of_isotopes");
	
	AddTo_IsotopeMix = new G4UIcmdWithAString("/Materials/AddTo/IsotopeMix", this);
	AddTo_IsotopeMix -> SetGuidance("Add specific isotopes of an element to the isotope mixture\n"
	                                "Isotope_mixture_name Density Element_symbol Abundance Unit"); 
	                                
	Density_IsotopeMix = new G4UIcmdWithAString("/Materials/AddDensity/IsotopeMix", this);
	Density_IsotopeMix -> SetGuidance("Add the density to the isotope mix\n"
	                                  "Isotope_mixture_name Density Unit");

//=================================================================================================
    //DEFINING A NEW MOLECULE
	DefMolecule = new G4UIcmdWithAString("/Materials/Define/Molecule", this);
	DefMolecule -> SetGuidance("Define a new molecule\n"
	                           "Molecule_name Number_of_atoms Density Unit");
	
	AddElementToMolecule = new G4UIcmdWithAString("/Materials/AddTo/Molecule", this);
    AddElementToMolecule -> SetGuidance("Choose an element to be added to an existing molecule\n"
                                        "Molecule_name Elemenet_name Number_of_atoms");
                                        
//=================================================================================================
    //DEFINING A NEW COMPOUND (PERCENTAGE OF DIFFERENT ELEMENTS, FOR EXAMPLE ALLOYS)                                       
    
    DefCompound = new G4UIcmdWithAString("/Materials/Define/Compound", this);
    DefCompound -> SetGuidance("Create a compound of elements\n"
                               "Compound_name Number_of_elements Density Unit");
    
    AddElementToCompound = new G4UIcmdWithAString("/Materials/AddTo/Compound", this);
    AddElementToCompound -> SetGuidance("Add elements to a custom compound\n"
                                        "Compound_name Element_name Fractional_mass Unit");
                                        
//=================================================================================================
    //DEFINING A NEW MIXTURE (MADE UP OF A PERCENTAGE OF DIFFERENT ELEMENTS OR MATERIALS) 
                                        
    DefMixture = new G4UIcmdWithAString("/Materials/Define/Mixture", this);
    DefMixture -> SetGuidance("Create a material made from a mixture of elements and materials\n"
                              "Mixture_name Number_of_componenets Density Unit");
                              
    AddMaterialToMixture = new G4UIcmdWithAString("Materials/AddTo/Mixture", this);
    AddMaterialToMixture -> SetGuidance("Add materials/elements to a mixture\n"
                                        "Mixture_name Material/Element_name Fractional_mass Unit");                          
                                        
//=================================================================================================                                        
    //DEFINING UNITS THAT A USER CAN USE WHEN USING THESE COMMANDS
                                        
	densityUnits.insert(std::make_pair("g/cm3", g/cm3));
	densityUnits.insert(std::make_pair("kg/cm3", kg/cm3));
	densityUnits.insert(std::make_pair("kg/m3", kg/m3));

    atomicWeightUnits.insert(std::make_pair("g/mole", g/mole));
	atomicWeightUnits.insert(std::make_pair("kg/mole", kg/mole));
	
	percentageUnit.insert(std::make_pair("%", perCent));
	
//=================================================================================================
}

DefineMaterialsMessenger::~DefineMaterialsMessenger()
{
	delete MaterialsDirectory;

	delete DefElement;
	delete DefIsotope;
	delete IsotopeMix;
	delete AddTo_IsotopeMix;
	delete Density_IsotopeMix;
	
	delete DefMolecule;
    delete AddElementToMolecule;
    
    delete DefCompound;
    delete AddElementToCompound;
    
    delete DefMixture;
    delete AddMaterialToMixture;
}

void DefineMaterialsMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if (command == DefElement)
	{
		G4Tokenizer next(newValue);
		G4String name = next();
		
		G4int z = ConvertToNumber<int> (next, newValue, command);
		G4double a = ConvertToNumber<int> (next, newValue, command);
		G4double awUnit = CheckUnits(next, command, newValue, "AtomicWeight");
		

		G4double density = ConvertToNumber<double> (next, newValue, command);
		G4double denUnit = CheckUnits(next, command, newValue, "Density");

		materials -> DefineElement(name, z, a*awUnit, density*denUnit);
	}
	else if(command == DefIsotope)
	{
		G4Tokenizer next(newValue);
		G4String name = next();
	
		G4int z = ConvertToNumber<int> (next, newValue, command);
		G4int A = ConvertToNumber<int> (next, newValue, command);

		G4double atomicWeight = ConvertToNumber<double> (next, newValue, command);
		G4double unit = CheckUnits(next, command, newValue, "AtomicWeight");

		materials -> DefineIsotope(name, z, A, atomicWeight*unit);
    }
    else if(command == IsotopeMix)
    {
        G4Tokenizer next(newValue);
		G4String Name = next();
		G4String Symbol = next();
		
		G4int nComponents = ConvertToNumber<int> (next, newValue, command);

        materials -> DefineIsotopeMix(Name, Symbol, nComponents);
    }
    else if(command == AddTo_IsotopeMix)
    {
        G4Tokenizer next(newValue);
		G4String IsotopeMixName = next();
		G4String IsotopeName = next();
		
		G4int Abundance = ConvertToNumber<int> (next, newValue, command);
		G4double AbUnit = CheckUnits(next, command, newValue, "Percentage");
		
		materials -> AddToIsoMix(IsotopeMixName, IsotopeName, Abundance*AbUnit);
    }
    else if(command == Density_IsotopeMix)
    {
        G4Tokenizer next(newValue);
		G4String IsotopeMixName = next();
		
		G4double Density = ConvertToNumber<double> (next, newValue, command);
		G4double denUnit = CheckUnits(next, command, newValue, "Density");
		
		materials -> AddIsoMixDensity(IsotopeMixName, Density*denUnit);
    }
    else if (command == DefMolecule)
    {
        G4Tokenizer next(newValue);     
        G4String Name = next();
        
        G4int nComponents = std::stoi(next());
        
        G4double density = ConvertToNumber<double> (next, newValue, command);
        G4double unit = CheckUnits(next, command, newValue, "Density");
        
        materials -> DefineMolecule(Name, density*unit, nComponents);
    }
    else if (command == AddElementToMolecule)
    {
        G4Tokenizer next(newValue);
        G4String NameMolecule = next();
        G4String NameElement = next();
        
        G4int nComponents = ConvertToNumber<int> (next, newValue, command);
        
        materials -> AddElementToMolecule(NameMolecule, NameElement, nComponents);
    }
    else if (command == DefCompound)
    {
        G4Tokenizer next(newValue);
        G4String NameCompound = next();
        
        G4int nComponents = ConvertToNumber<int> (next, newValue, command);
        
        G4double density = ConvertToNumber<double> (next, newValue, command);
        G4double unit = CheckUnits(next, command, newValue, "Density");
        
        materials -> DefineCompound(NameCompound, density*unit, nComponents);
    }
    else if (command == AddElementToCompound)
    {
        G4Tokenizer next(newValue);
        G4String NameCompound = next();
        G4String NameElement = next();
        
        G4double fractionalMass = ConvertToNumber<double> (next, newValue, command);
        G4double unit = CheckUnits(next, command, newValue, "Percentage");
        
        materials -> AddElementToCompound(NameCompound, NameElement, fractionalMass*unit);
    }
	else if (command == DefMixture)
	{
	    G4Tokenizer next(newValue);
        G4String NameMixture = next();
        
        G4int nComponents = ConvertToNumber<int> (next, newValue, command);
        
        G4double density = ConvertToNumber<double> (next, newValue, command);
        G4double unit = CheckUnits(next, command, newValue, "Density");
        
        materials -> DefineMixture(NameMixture, density*unit, nComponents);
	}
	else if (command == AddMaterialToMixture)
	{
	    G4Tokenizer next(newValue);
        G4String NameMixture = next();
        G4String NameMaterial = next();
        
        G4double fractionalMass = ConvertToNumber<double> (next, newValue, command);
        G4double unit = CheckUnits(next, command, newValue, "Percentage");
        
        materials -> AddMaterialToMixture(NameMixture, NameMaterial, fractionalMass*unit);
	}
}

G4double DefineMaterialsMessenger::CheckUnits(G4Tokenizer &next, G4UIcommand* command, G4String newValue, G4String TypeOfUnit)
{
    G4String UnitString = next();
    G4bool Success;
    
    //Checks to see if unit exists depending on which type of unit it is. If it does, it will return it
    if (TypeOfUnit == "Density")
    {
        if (densityUnits.count(UnitString) == false)
            Success = false;
        else 
        {
            Success = true;
            return densityUnits[UnitString];    
        }
    }
    else if (TypeOfUnit == "AtomicWeight")
    {    
        if (atomicWeightUnits.count(UnitString) == false)
            Success = false;
        else 
        {
            Success = true;
            return atomicWeightUnits[UnitString];   
        }
    }
    else if (TypeOfUnit == "Percentage")
    {  
        if (percentageUnit.count(UnitString) == false)
            Success == true;
        else
        {
            Success = true;
            return percentageUnit[UnitString];   
        }
    }
    
    //If Success is false, then output reason for error and stop the programm
    if (Success == false)
    {
        G4cout << "\nERROR: " << command -> GetCommandPath() << " " << newValue << " -> Invalid ";
        
        if (TypeOfUnit == "Density")
        {
            G4cout << "denisty unit!\nGuidance: " << command -> GetGuidanceLine(0) << "\n\nAvailable units \n";   
            for (std::map<std::string, double>::iterator it = densityUnits.begin(); it != densityUnits.end(); ++it)
                G4cout << it -> first << " ";
        }
        else if (TypeOfUnit == "AtomicWeight")
        {
            G4cout << "atomic weight unit!\nGuidance: " << command -> GetGuidanceLine(0) << "\n\nAvailable units \n";
            for (std::map<std::string, double>::iterator it = atomicWeightUnits.begin(); it != atomicWeightUnits.end(); ++it)
                G4cout << it -> first << " ";
        }
        else if (TypeOfUnit == "Percentage")
        {
            G4cout << "percentage unit!\nGuidance: " << command -> GetGuidanceLine(0) << "\n\nAvailable units \n";
            for (std::map<std::string, double>::iterator it = percentageUnit.begin(); it != percentageUnit.end(); ++it)
                G4cout << it -> first << " ";
        }
          
        G4cout << G4endl; 
        exit(0);
    }
}
		
//Function to convert string input to the type of number you need	
template <typename T> T DefineMaterialsMessenger::ConvertToNumber(G4Tokenizer &next, G4String input, G4UIcommand* command)
{
    try
    {   return static_cast<T> (std::stod(next()));} 
        
    catch (const std::invalid_argument& ia)
    {
        G4cout << "\nERROR: Invalid input for " << command -> GetCommandPath() << " " << input
               << "\nGuidance: " << command -> GetGuidanceLine(0) << G4endl;
        exit(1);
    }
}


