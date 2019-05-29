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

#include "MultiParameters.hh"


#include "G4TwoVector.hh"
#include "G4Tokenizer.hh"
#include "Tokenizer.hh"

DefineMaterialsMessenger::DefineMaterialsMessenger(DefineMaterials* DefMaterials): MyG4UImessenger(), materials(DefMaterials)
{
//=================================================================================================
	MaterialsDirectory = new G4UIdirectory("/Materials/", this);
	MaterialsDirectory -> SetGuidance("Directory to define new materials to be used");

//=================================================================================================
    //DEFINING ELEMENTS/ISOTOPES
    
	DefElement = new MultiParameters("/Materials/Define/Element", this);
	DefElement -> SetNumberOfParameters(6);
	DefElement -> SetGuidance("Define an element\n"
	                          "name z atomicweight unit density unit");

	DefIsotope = new MultiParameters("/Materials/Define/Isotope", this);
	DefIsotope -> SetNumberOfParameters(5);
	DefIsotope -> SetGuidance("Define an isotope\n"
	                          "name z nucleonnumber atomicweight unit");
	                          
	IsotopeMix = new MultiParameters("/Materials/Define/IsotopeMix", this);
	IsotopeMix -> SetNumberOfParameters(3);
	IsotopeMix -> SetGuidance("Create an element with a certain mixture of isotopes\n"
	                          "name  symbol numberofisotopes");
	
	AddTo_IsotopeMix = new MultiParameters("/Materials/AddTo/IsotopeMix", this);
	AddTo_IsotopeMix -> SetNumberOfParameters(3);
	AddTo_IsotopeMix -> SetGuidance("Add specific isotopes of an element to the isotope mixture\n"
	                                "name elementsymbol abundance unit"); 
	                                
	Density_IsotopeMix = new MultiParameters("/Materials/AddDensity/IsotopeMix", this);
	Density_IsotopeMix -> SetNumberOfParameters(3);
	Density_IsotopeMix -> SetGuidance("Set the density to the isotope mix\n"
	                                  "name density unit");

//=================================================================================================
    //DEFINING A NEW MOLECULE
	DefMolecule = new MultiParameters("/Materials/Define/Molecule", this);
	DefMolecule -> SetNumberOfParameters(4);
	DefMolecule -> SetGuidance("Define a new molecule\n"
	                           "name no.atoms density unit");
	
	AddElementToMolecule = new MultiParameters("/Materials/AddTo/Molecule", this);
	AddElementToMolecule -> SetNumberOfParameters(3);
    AddElementToMolecule -> SetGuidance("Choose an element to be added to an existing molecule\n"
                                        "moleculename elemenetname no.atoms");
                                        
//=================================================================================================
    //DEFINING A NEW COMPOUND (PERCENTAGE OF DIFFERENT ELEMENTS, FOR EXAMPLE ALLOYS)                                       
    
    DefCompound = new MultiParameters("/Materials/Define/Compound", this);
    DefCompound -> SetNumberOfParameters(4);
    DefCompound -> SetGuidance("Create a compound of elements\n"
                               "compoundname no.elements density unit");
    
    AddElementToCompound = new MultiParameters("/Materials/AddTo/Compound", this);
    AddElementToCompound -> SetNumberOfParameters(4);
    AddElementToCompound -> SetGuidance("Add elements to a custom compound\n"
                                        "compoundname elementname fractionalmass unit");
                                        
//=================================================================================================
    //DEFINING A NEW MIXTURE (MADE UP OF A PERCENTAGE OF DIFFERENT ELEMENTS OR MATERIALS) 
                                        
    DefMixture = new MultiParameters("/Materials/Define/Mixture", this);
    DefMixture -> SetNumberOfParameters(4);
    DefMixture -> SetGuidance("Create a material made from a mixture of elements and materials\n"
                              "mixturename no.componenets density unit");
                              
    AddMaterialToMixture = new MultiParameters("Materials/AddTo/Mixture", this);
    AddMaterialToMixture -> SetNumberOfParameters(4);
    AddMaterialToMixture -> SetGuidance("Add materials/elements to a mixture\n"
                                        "mixturename materialname fractionalmass unit");                          
                                        
//=================================================================================================                                        
    //DEFINING UNITS THAT A USER CAN USE WHEN USING THESE COMMANDS
                                        
	densityUnits.insert(std::make_pair("g/cm3", g/cm3));
	densityUnits.insert(std::make_pair("kg/cm3", kg/cm3));
	densityUnits.insert(std::make_pair("kg/m3", kg/m3));

    atomicWeightUnits.insert(std::make_pair("g/mole", g/mole));
	atomicWeightUnits.insert(std::make_pair("kg/mole", kg/mole));
	
	percentageUnit.insert(std::make_pair("%", perCent));
	
	energyUnits.insert(std::make_pair("MeV", MeV));
	energyUnits.insert(std::make_pair("keV", keV));
	energyUnits.insert(std::make_pair("eV" , eV));
	
	lengthUnits.insert(std::make_pair("km", km));
	lengthUnits.insert(std::make_pair("m" , m));
	lengthUnits.insert(std::make_pair("cm", cm));
	lengthUnits.insert(std::make_pair("mm", mm));
	lengthUnits.insert(std::make_pair("um", um));
	lengthUnits.insert(std::make_pair("nm", nm));
	
	xraylibDensity.insert(std::make_pair("g/cm3" , 1./1.));
	xraylibDensity.insert(std::make_pair("kg/m3" , 1000./(100.*100.*100.)));
	xraylibDensity.insert(std::make_pair("kg/cm3", 1000./(1.*1.*1.)));
	
	xraylibEnergy.insert(std::make_pair("keV", 1.));
	xraylibEnergy.insert(std::make_pair("eV" , 1./1000.));
	xraylibEnergy.insert(std::make_pair("MeV", 1.e3));
//=================================================================================================

    AutoOpticalProperties_El = new MultiParameters("/Materials/AutoOpticalProperties", this);
    AutoOpticalProperties_El -> SetNumberOfParameters(2);
    AutoOpticalProperties_El -> SetGuidance("Automatically set the optical properties of a material (compound or element) for refraction and absorption by specifying the material and an array of energy values.\n"
                                            "Example: material=name energy=(e1,e2,eN...)[units]\n"
                                            "Flags optional. For the array input you can specifiy to use the function linspace. energy=linspace(starte,ende,no.values)[units] ");
                                            
    AddRefractiveIndex = new MultiParameters("/Materials/AddRefractiveIndex", this);
    AddRefractiveIndex -> SetNumberOfParameters(3);
    AddRefractiveIndex -> SetGuidance("Set the optical properties of a material (compound or element) for refraction by specifying the material, "
                                      "an array of energy values and an array of corrosponding refraction values.\n"
                                      "Example: material=name energy=(e1,e2,eN...)[units] rindex=(r1,r2,rN...)\n"
                                      "Flags optional. For the array input you can specifiy to use the function linspace. Example: energy=linspace(starte,ende,no.values)[units] ");
    
    AddRefracticeIndex_Im = new MultiParameters("/Materials/AddRefractiveIndex_Im", this);
    AddRefracticeIndex_Im -> SetNumberOfParameters(3);
    AddRefracticeIndex_Im -> SetGuidance("Set the optical properties of a material (compound or element) for complex refraction values by specifying the material, "
                                         "an array of energy values and an array of corrosponding refraction values.\n"
                                         "Example: material=name energy=(e1,e2,eN...)[units] rindex=(r1,r2,rN...)\n"
                                         "Flags optional. For the array input you can specifiy to use the function linspace. Example: energy=linspace(starte,ende,no.values)[units] ");
    
    AddAbsorptionLength = new MultiParameters("/Materials/AddAbsorptionLength", this);
    AddAbsorptionLength -> SetNumberOfParameters(2);
    AddAbsorptionLength -> SetGuidance("Set the optical properties of a material (compound or element) for absorption length values by specifying the material, "
                                       "an array of energy values and an array of corrosponding absorption length values.\n"
                                       "Example: material=name energy=(e1,e2,eN...)[units] abslength=(a1,a2,aN...)\n"
                                       "Flags optional. For the array input you can specifiy to use the function linspace. Example: energy=linspace(starte,ende,no.values)[units] ");
    
    AddAbsorptionLength_xraylib = new MultiParameters("/Materials/xraylib/AddAbsorptionLength",this);
    AddAbsorptionLength_xraylib -> SetNumberOfParameters(2);
    AddAbsorptionLength_xraylib -> SetGuidance("Set the optical properties of a material (compound or element) for absorption length values automatically using the xraylib library. Specify the material, "
                                               "and array of energy values.\n"
                                               "Example: material=name energy=(e1,e2,eN...)[units]\n"
                                               "Flags optional. For the array input you can specifiy to use the function linspace. Example: energy=linspace(starte,ende,no.values)[units] ");
    
    AddEfficiency = new MultiParameters("/Materials/AddEfficiency", this);
    AddEfficiency -> SetNumberOfParameters(2);
    
    PrintMPT = new G4UIcmdWithAString("/Materials/PrintMaterialPropertiesTable", this);
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
    
    delete AutoOpticalProperties_El;
    delete AddRefractiveIndex;
    delete AddRefracticeIndex_Im;
    delete AddAbsorptionLength;
    delete AddAbsorptionLength_xraylib;
    delete AddEfficiency;
    delete PrintMPT;
}

#include "CommandStatus.hh"

int DefineMaterialsMessenger::ApplyCommand(G4UIcommand* command, G4String newValue)
{
    if (command == DefElement)
	{
	    std::string name;
		int    z;
		double a;
		double awUnit;
	    double density;
	    double denUnit;
	
		G4Tokenizer next(newValue);
        name = next();
		
		//convert to numbers and units
		try { z = stoi(next());}
		catch (std::invalid_argument& ia) {return fParameterNotAInteger;}
		try { a = stod(next());}
		catch (std::invalid_argument& ia) {return fParameterNotADouble;}
		
		int status = CheckUnits(atomicWeightUnits, next(), awUnit);
		if (status != 0) {return status;}
		
		try { density = stod(next()); }
		catch (std::invalid_argument& ia) {return fParameterNotADouble;}
		
		status = CheckUnits(densityUnits, next(), denUnit);
		if (status != 0) {return status;}

		try {materials -> DefineElement(name, z, a*awUnit, density*denUnit);}
		catch (materialAlreadyExists& mae) {return fParameterAlreadyExists;}
		catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
	}
	else if(command == DefIsotope)
	{	
		std::string name;
		int    z;
		int    A;
		double atomicWeight;
		double unit;
		
		G4Tokenizer next(newValue);
		name = next();

		//convert to numbers and units
		try { z = stoi(next());}
		catch (std::invalid_argument& ia) {return fParameterNotAInteger;}
		try { A = stoi(next());}
		catch (std::invalid_argument& ia) {return fParameterNotAInteger;}
		
		int status = CheckUnits(atomicWeightUnits, next(), unit);
		if (status != 0) {return status;}
		
		try {materials -> DefineIsotope(name, z, A, atomicWeight*unit);}
		catch (materialAlreadyExists& mae) {return fParameterAlreadyExists;}
		catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
    }
    else if(command == IsotopeMix)
    {
        G4Tokenizer next(newValue);
		G4String name = next();
		G4String symbol = next();		
		int nComponents;
		
		try   {nComponents = std::stoi(next());}
		catch (std::invalid_argument& ia) {return fParameterNotAInteger;}

        try {materials -> DefineIsotopeMix(name, symbol, nComponents);}
        catch (parameterNotFound& pnf) {return fParameterNotFound;}
        catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
    }
    else if(command == AddTo_IsotopeMix)
    {
        G4Tokenizer next(newValue);
		G4String isotopeMixName = next();
		G4String isotopeName = next();
		double abundance;
		double unit;
		
		try {abundance = std::stod(next());}
		catch (std::invalid_argument& ia) {return fParameterNotADouble;}
		
		int status = CheckUnits(percentageUnit, next(), unit);
		if (status != 0) {return status;}
		
		try {materials -> AddToIsoMix(isotopeMixName, isotopeName, abundance*unit);}
		catch (parameterNotFound& pnf) {return fParameterNotFound;}
		catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
    }
    else if(command == Density_IsotopeMix)
    {
        G4Tokenizer next(newValue);
		G4String isotopeMixName = next();
        double density;
        double unit;
        
		try {density = std::stod(next());}
		catch (std::invalid_argument& ia) {return fParameterNotADouble;}
		
		int status = CheckUnits(densityUnits, next(), unit);
		if (status != 0) {return status;}
		
		try {materials -> AddIsoMixDensity(isotopeMixName, density*unit);}
		catch (parameterNotFound& pnf) {return fParameterNotFound;}
		catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
    }
    else if (command == DefMolecule)
    {
        G4Tokenizer next(newValue);     
        G4String name = next();
        int nComponents;
        double density;
        double unit;
        try {nComponents = std::stoi(next());}
        catch (std::invalid_argument& ia) {return fParameterNotAInteger;}
        
        try {density = std::stod(next());}
        catch (std::invalid_argument& ia) {return fParameterNotADouble;}
        
        int status = CheckUnits(densityUnits, next(), unit);
        if (status != 0) {return status;}
        
        try { materials -> DefineMolecule(name, density*unit, nComponents);}
        catch (materialAlreadyExists& mae) {return fParameterAlreadyExists;}
        catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
    }
    else if (command == AddElementToMolecule)
    {
        G4Tokenizer next(newValue);
        G4String nameMolecule = next();
        G4String nameElement = next();
        int ncomponents;
        
        try {ncomponents = std::stoi(next());}
        catch (std::invalid_argument& ia) {return fParameterNotAInteger;}      
        
        try {materials -> AddElementToMolecule(nameMolecule, nameElement, ncomponents);}
        catch (parameterNotFound& pnf) {return fParameterNotFound;}
        catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
    }
    else if (command == DefCompound)
    {
        G4Tokenizer next(newValue);
        G4String nameCompound = next();
        int nComponents;
        double density;
        double unit;
        
        try {nComponents = std::stoi(next());}
        catch (std::invalid_argument& ia) {return fParameterNotAInteger;}
        
        try {density = std::stod(next());}
        catch (std::invalid_argument& ia) {return fParameterNotADouble;}
        
        int status = CheckUnits(densityUnits, next(), unit);
        if (status != 0) {return status;}
        
        try {materials -> DefineCompound(nameCompound, density*unit, nComponents);}
        catch (materialAlreadyExists& mae) {return fParameterAlreadyExists;}
        catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
    }
    else if (command == AddElementToCompound)
    {
        G4Tokenizer next(newValue);
        G4String nameCompound = next();
        G4String nameElement = next();
        double fractionalMass;
        double unit;
        
        try {fractionalMass = std::stod(next());}
        catch (std::invalid_argument& ia) {return fParameterNotADouble;}
        
        int status = CheckUnits(percentageUnit, next(), unit);
        if (status != 0) {return status;}
        
        try {materials -> AddElementToCompound(nameCompound, nameElement, fractionalMass*unit);}
        catch (parameterNotFound& pnf) {return fParameterNotFound;}
        catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
    }
	else if (command == DefMixture)
	{
	    G4Tokenizer next(newValue);
        G4String nameMixture = next();
        int    nComponents;
        double density;
        double unit;
        
        try {nComponents = std::stoi(next());}
        catch (std::invalid_argument& ia) { return fParameterNotAInteger;}
        
        try {density = std::stod(next());}
        catch (std::invalid_argument& ia) { return fParameterNotADouble;}
        
        int status = CheckUnits(densityUnits, next(), unit);
        if (status != 0) {return status;}
        
        try {materials -> DefineMixture(nameMixture, density*unit, nComponents);}
        catch (materialAlreadyExists& mae) {return fParameterAlreadyExists;}
        catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
	}
	else if (command == AddMaterialToMixture)
	{
	    G4Tokenizer next(newValue);
        G4String nameMixture = next();
        G4String nameMaterial = next();
        double fractionalMass;
        double unit;
        
        try {fractionalMass = std::stod(next());}
        catch (std::invalid_argument& ia) {return fParameterNotADouble;}
        
        int status = CheckUnits(percentageUnit, next(), unit);
        if (status !=0 ) {return status;}
        
        try {materials -> AddMaterialToMixture(nameMixture, nameMaterial, fractionalMass*unit);}
        catch (materialAlreadyExists& mae) {return fParameterAlreadyExists;}
        catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
	}
	else if (command == AutoOpticalProperties_El)
	{    
	    Tokenizer token(newValue, command);
	    
	    Flag namef(0, "material");
	    Flag arrayf(1, "energy", xraylibEnergy);
	    //arrayf.
	    namef.parameter = 0;
	    arrayf.parameter = 1;
	    
	    std::string material;
	    doubleArray energy;
	    try 
	    {
	        material = token.FindString(namef);
	        energy   = token.FindArray(arrayf);
	    }
	    catch (std::invalid_argument& ia) {return fParameterNotADouble;}
	    catch (InvalidUnits& iu)          {return fIncorrectUnit;}
	    catch (InvalidArray& ia)          {return fParameterNotADouble;}
	    catch (InvalidLinspace& ils)      {return fFlagNotFound;}
	    catch (InvalidFlag&     iflag)    {return fFlagNotFound;}
	    
	    try {materials -> FillOpticalProperties_xraylib(material, energy.values, energy.size);}
	    catch (parameterNotFound& pnf) {return fParameterNotFound;}
	    catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
	}
	else if (command == AddRefractiveIndex)
	{
	    Tokenizer token(newValue, command);
	    
	    Flag namef(0, "material");
	    Flag energyf(1, "energy", energyUnits);
	    Flag refractiveIndexf(2, "rindex");
	    
	    std::string material;
	    doubleArray energy;
	    doubleArray rIndex;
	    
	    try
	    {
	        material = token.FindString(namef);
	        energy = token.FindArray(energyf);
	        rIndex = token.FindArray(refractiveIndexf);
	    }
	    catch (std::invalid_argument& ia) {return fParameterNotADouble;}
	    catch (InvalidUnits& iu)          {return fIncorrectUnit;}
	    catch (InvalidArray& ia)          {return fParameterNotADouble;}
	    catch (InvalidLinspace& ils)      {return fFlagNotFound;}
	    catch (InvalidFlag&     iflag)    {return fFlagNotFound;}
	    
	    try {materials -> AddOpticalProperty(material, "RINDEX", energy.values, rIndex.values, energy.size);}
	    catch (parameterNotFound& pnf) {return fParameterNotFound;}
	    catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
	}
	else if (command == AddRefracticeIndex_Im)
	{
	    Tokenizer token(newValue, command);
	    
	    Flag namef(0, "material");
	    Flag energyf(1, "energy", energyUnits);
	    Flag refractiveIndexf(2, "rindex");
	    
	    std::string material;
	    doubleArray energy;
	    doubleArray rIndex;
	    
	    try 
	    {
	        material = token.FindString(namef);
	        energy = token.FindArray(energyf);
	        rIndex = token.FindArray(refractiveIndexf);
	    }
	    catch (std::invalid_argument& ia) {return fParameterNotADouble;}
	    catch (InvalidUnits& iu)          {return fIncorrectUnit;}
	    catch (InvalidArray& ia)          {return fParameterNotADouble;}
	    catch (InvalidLinspace& ils)      {return fFlagNotFound;}
	    catch (InvalidFlag&     iflag)    {return fFlagNotFound;}
	    
	    try {materials -> AddOpticalProperty(material, "IMAGINARYRINDEX", energy.values, rIndex.values, energy.size);}
	    catch (parameterNotFound& pnf) {return fParameterNotFound;}
	    catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
	}
	else if (command == AddAbsorptionLength)
	{
	    Tokenizer token(newValue, command);
	
	    Flag namef(0, "material");
	    Flag energyf(1, "energy", energyUnits);
	    Flag absLengthf(2, "abslength", lengthUnits);
	    
	    std::string material;
	    doubleArray energy;
	    doubleArray absLength;
	    
	    try
	    {
	        material = token.FindString(namef);
	        energy = token.FindArray(energyf);
	        absLength = token.FindArray(absLengthf);
	    }
	    catch (std::invalid_argument& ia) {return fParameterNotADouble;}
	    catch (InvalidUnits& iu)          {return fIncorrectUnit;}
	    catch (InvalidArray& ia)          {return fParameterNotADouble;}
	    catch (InvalidLinspace& ils)      {return fFlagNotFound;}
	    catch (InvalidFlag&     iflag)    {return fFlagNotFound;}
	    
	    try {materials -> AddOpticalProperty(material, "ABSLENGTH", energy.values, absLength.values, energy.size);}
	    catch (parameterNotFound& pnf) {return fParameterNotFound;}
	    catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
	}
	else if (command == AddAbsorptionLength_xraylib)
	{
	    Tokenizer token(newValue, command);
	
	    Flag namef(0, "material");
	    Flag energyf(1, "energy", energyUnits);
	    
	    std::string material;
	    doubleArray energy;
	    doubleArray absLength;
	    
	    try
	    {
	        material  = token.FindString(namef);
	        energy    = token.FindArray(energyf);
	    }
	    catch (std::invalid_argument& ia) {return fParameterNotADouble;}
	    catch (InvalidUnits& iu)          {return fIncorrectUnit;}
	    catch (InvalidArray& ia)          {return fParameterNotADouble;}
	    catch (InvalidLinspace& ils)      {return fFlagNotFound;}
	    catch (InvalidFlag&     iflag)    {return fFlagNotFound;}
	         
	    //materials -> AddOpticalProperty(material, "ABSLENGTH", energy.values, absLength.values, energy.size);
	}
	else if (command == AddEfficiency)
	{
	    Tokenizer token(newValue, command);
	
	    Flag namef(0, "material");
	    Flag energyf(1, "energy", energyUnits);
	    Flag efficiencyf(2, "efficiency");
	    
	    std::string material;
	    doubleArray energy;
	    doubleArray efficiency;
	    
	    try
	    {
	        material = token.FindString(namef);
	        energy = token.FindArray(energyf);
	        efficiency = token.FindArray(efficiencyf);
	    }
	    catch (std::invalid_argument& ia) {return fParameterNotADouble;}
	    catch (InvalidUnits& iu)          {return fIncorrectUnit;}
	    catch (InvalidArray& ia)          {return fParameterNotADouble;}
	    catch (InvalidLinspace& ils)      {return fFlagNotFound;}
	    catch (InvalidFlag&     iflag)    {return fFlagNotFound;}
	    
	    try {materials -> AddOpticalProperty(material, "EFFICIENCY", energy.values, efficiency.values, energy.size);}
	    catch (parameterNotFound& pnf) {return fParameterNotFound;}
	    catch (parameterIsNegative& pin) {return fParameterOutOfRange;}
	}
	
	return 0;
}

void DefineMaterialsMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if (command == PrintMPT)
	{
	    materials -> PrintMPT(newValue);
	}
}

int DefineMaterialsMessenger::CheckUnits(std::map<std::string, double> correctunits, 
                                         std::string                   unit_str,
                                         double                        &unit)
{
	bool unit_iscorrect = correctunits.count(unit_str);
	if (unit_iscorrect) { unit = correctunits[unit_str]; return 0;}
	else                { return fIncorrectUnit;}
}


