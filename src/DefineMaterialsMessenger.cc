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
	DefIsotope -> SetGuidance("Define an isotope ");

	UnitDict.insert(std::make_pair("g/mole", g/mole));
	UnitDict.insert(std::make_pair("kg/mole", kg/mole));
	UnitDict.insert(std::make_pair("g/cm3", g/cm3));
	UnitDict.insert(std::make_pair("kg/cm3", kg/cm3));
	UnitDict.insert(std::make_pair("kg/m3", kg/m3));
	UnitDict.insert(std::make_pair("%", perCent));

}

DefineMaterialsMessenger::~DefineMaterialsMessenger()
{
	delete MaterialsDirectory;

	delete DefElement;
	delete DefIsotope;
}

void DefineMaterialsMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if (command == DefElement)
	{
		G4Tokenizer next(newValue);

		G4String name = next();
		
		G4int z = std::stoi(next());
	
		G4double a = std::stoi(next());
		a = a * UnitDict[next()];

		G4double density = std::stod(next());
		density = density * UnitDict[next()];

		materials -> DefineElement(name, z, a, density);
	}
	else if(command == DefIsotope)
	{
		G4Tokenizer next(newValue);

		G4String name = next();
	
		G4int z = std::stoi(next());
	
		G4int A = std::stoi(next());

		G4double atomicWeight = std::stod(next());

		materials -> DefineIsotope(name, z, A, atomicWeight*UnitDict[next()]);
		
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
}
		

