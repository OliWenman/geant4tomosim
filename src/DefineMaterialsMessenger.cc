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
	MaterialsDirectory = new G4UIdirectory("/Materials/", this);
	MaterialsDirectory -> SetGuidance("Directory to define new materials to be used");

	DefElement = new G4UIcmdWithAString("/Materials/Define/Element", this);
	DefElement -> SetGuidance("Define an element with input of it's name, z number, atomicweight, unit, density, unit");

	DefIsotope = new G4UIcmdWithAString("/Materials/Define/Isotope", this);
	DefIsotope -> SetGuidance("Define an isotope with input of its name, z number, nucleon number, and atomic weight");
	
	DefMolecule = new G4UIcmdWithAString("/Materials/Define/Molecule", this);
	DefMolecule -> SetGuidance("Define a new molecule\n");
	
	AddElementToMolecule = new G4UIcmdWithAString("/Materials/AddTo/Molecule", this);
    AddElementToMolecule -> SetGuidance("Choose an element to be added to an existing molecule");
    
    DefCompound = new G4UIcmdWithAString("/Materials/Define/Compound", this);
    DefCompound -> SetGuidance("Create a compound of elements\n"
                               "CompoundName NumberOfElements density unit");
    
    AddElementToCompound = new G4UIcmdWithAString("/Materials/AddTo/Compound", this);
    AddElementToCompound -> SetGuidance("Add elements to a custom compound\n"
                                        "CompoundName ElementName FractionalMass Unit");
	
	densityUnits.insert(std::make_pair("g/cm3", g/cm3));
	densityUnits.insert(std::make_pair("kg/cm3", kg/cm3));
	densityUnits.insert(std::make_pair("kg/m3", kg/m3));

    atomicWeightUnits.insert(std::make_pair("g/mole", g/mole));
	atomicWeightUnits.insert(std::make_pair("kg/mole", kg/mole));
	
	percentageUnit.insert(std::make_pair("%", perCent));
}

DefineMaterialsMessenger::~DefineMaterialsMessenger()
{
	delete MaterialsDirectory;

	delete DefElement;
	delete DefIsotope;
	
	delete DefMolecule;
    delete AddElementToMolecule;
    
    delete DefCompound;
    delete AddElementToCompound;
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
    else if (command == DefMolecule)
    {
        G4Tokenizer next(newValue);     
        G4String Name = next();
        
        G4double density = ConvertToNumber<double> (next, newValue, command);
        G4double unit = CheckUnits(next, command, newValue, "Density");
        
        G4int nComponents = std::stoi(next());
        
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
		
		/*//Isotopes
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
	mat_enrichedU -> AddElement(enrichedU, fraction_mass);*/
}

G4double DefineMaterialsMessenger::CheckUnits(G4Tokenizer &next, G4UIcommand* command, G4String newValue, G4String TypeOfUnit)
{
    G4String UnitString = next();
    
    if (TypeOfUnit == "Density")
    {
        bool denUnit = densityUnits.count(UnitString);
        
        if (denUnit == false)
        {
            G4cout << "\nERROR: " << command -> GetCommandPath() << " " << newValue << " -> Invalid denisty unit!\nGuidance: "
                   << command -> GetGuidanceLine(0) << "\n\nAvailable units \n";
                   
            for (std::map<std::string, double>::iterator it = densityUnits.begin(); it != densityUnits.end(); ++it)
                G4cout << it -> first << " ";
                
            G4cout << G4endl;
            
            exit(0);
        }
        else 
            return densityUnits[UnitString];    
    }
    if (TypeOfUnit == "AtomicWeight")
    {
        bool awUnit = atomicWeightUnits.count(UnitString);
        if (awUnit == false)
        {
            G4cout << "\nERROR: " << command -> GetCommandPath() << " " << newValue << " -> Invalid atomic weight unit!\nGuidance: "
                   << command -> GetGuidanceLine(0) << "\n\nAvailable units \n";
                   
            for (std::map<std::string, double>::iterator it = atomicWeightUnits.begin(); it != atomicWeightUnits.end(); ++it)
                G4cout << it -> first << " ";
        
            G4cout << G4endl;
            
            exit(0);
        }
        else
            return atomicWeightUnits[UnitString];   
    }
    if (TypeOfUnit == "Percentage")
    {
        bool perUnit = percentageUnit.count(UnitString);
        if (perUnit == false)
        {
            G4cout << "\nERROR: " << command -> GetCommandPath() << " " << newValue << " -> Invalid %\ unit!\nGuidance: "
                   << command -> GetGuidanceLine(0) << "\n\nAvailable units \n";
                   
            for (std::map<std::string, double>::iterator it = percentageUnit.begin(); it != percentageUnit.end(); ++it)
                G4cout << it -> first << " ";
        
            G4cout << G4endl;
            
            exit(0);
        }
        else
            return percentageUnit[UnitString];   
    }
}
		
		
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


