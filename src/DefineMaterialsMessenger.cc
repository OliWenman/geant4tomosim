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
	MaterialsDirectory = new G4UIdirectory("/materials/", this);
	MaterialsDirectory -> SetGuidance("Directory to define new materials to be used");

//=================================================================================================
    //DEFINING ELEMENTS/ISOTOPES
    
	define_element = new MultiParameters("/materials/define/element", this);
	define_element -> SetNumberOfParameters(6);
	define_element -> SetGuidance("Define an element\n"
	                          "name z atomicweight unit density unit");

	define_isotope = new MultiParameters("/materials/define/isotope", this);
	define_isotope -> SetNumberOfParameters(5);
	define_isotope -> SetGuidance("Define an isotope\n"
	                          "name z nucleonnumber atomicweight unit");
	                          
	define_isotopemix = new MultiParameters("/materials/define/isotopemix", this);
	define_isotopemix -> SetNumberOfParameters(3);
	define_isotopemix -> SetGuidance("Create an element with a certain mixture of isotopes\n"
	                          "name  symbol numberofisotopes");
	
	addto_isotopemix = new MultiParameters("/materials/addyo/isotope_mix", this);
	addto_isotopemix -> SetNumberOfParameters(3);
	addto_isotopemix -> SetGuidance("Add specific isotopes of an element to the isotope mixture\n"
	                                "name elementsymbol abundance unit"); 
	                                
	addto_isotopemix_density = new MultiParameters("/Materials/addto/isotope_mix/density", this);
	addto_isotopemix_density -> SetNumberOfParameters(3);
	addto_isotopemix_density -> SetGuidance("Set the density to the isotope mix\n"
	                                  "name density unit");

//=================================================================================================
    //DEFINING A NEW MOLECULE
	define_molecule = new MultiParameters("/materials/define/molecule", this);
	define_molecule -> SetNumberOfParameters(4);
	define_molecule -> SetGuidance("Define a new molecule\n"
	                           "name no.atoms density unit");
	
	addto_molecule_element = new MultiParameters("/materials/addto/molecule", this);
	addto_molecule_element -> SetNumberOfParameters(3);
    addto_molecule_element -> SetGuidance("Choose an element to be added to an existing molecule\n"
                                        "moleculename elemenetname no.atoms");
                                        
//=================================================================================================
    //DEFINING A NEW COMPOUND (PERCENTAGE OF DIFFERENT ELEMENTS, FOR EXAMPLE ALLOYS)                                       
    
    define_compound = new MultiParameters("/materials/define/compound", this);
    define_compound -> SetNumberOfParameters(4);
    define_compound -> SetGuidance("Create a compound of elements\n"
                               "compoundname no.elements density unit");
    
    addto_compound_element = new MultiParameters("/materials/addto/compound", this);
    addto_compound_element -> SetNumberOfParameters(4);
    addto_compound_element -> SetGuidance("Add elements to a custom compound\n"
                                        "compoundname elementname fractionalmass unit");
                                        
//=================================================================================================
    //DEFINING A NEW MIXTURE (MADE UP OF A PERCENTAGE OF DIFFERENT ELEMENTS OR MATERIALS) 
                                        
    define_mixture = new MultiParameters("/materials/define/mixture", this);
    define_mixture -> SetNumberOfParameters(4);
    define_mixture -> SetGuidance("Create a material made from a mixture of elements and materials\n"
                              "mixturename no.componenets density unit");
                              
    addto_mixture_material = new MultiParameters("materials/addto/mixture", this);
    addto_mixture_material -> SetNumberOfParameters(4);
    addto_mixture_material -> SetGuidance("Add materials/elements to a mixture\n"
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

    mpt_print = new G4UIcmdWithAString("/materials/mpt/print", this);

    mpt_xraylib_add_all_op = new MultiParameters("/materials/mpt/xraylib/add/allopticalproperties", this);
    mpt_xraylib_add_all_op -> SetNumberOfParameters(2);
    mpt_xraylib_add_all_op -> SetGuidance("Automatically set the optical properties of a material (compound or element) for refraction and absorption by specifying the material and an array of energy values.\n"
                                            "Example: material=name energy=(e1,e2,eN...)[units]\n"
                                            "Flags optional. For the array input you can specifiy to use the function linspace. energy=linspace(starte,ende,no.values)[units] ");
    
    mpt_xraylib_add_absorptionlen = new MultiParameters("/materials/mpt/xraylib/add/absorptionlength",this);
    mpt_xraylib_add_absorptionlen -> SetNumberOfParameters(2);
    mpt_xraylib_add_absorptionlen -> SetGuidance("Set the optical properties of a material (compound or element) for absorption length values automatically using the xraylib library. Specify the material, "
                                               "and array of energy values.\n"
                                               "Example: material=name energy=(e1,e2,eN...)[units]\n"
                                               "Flags optional. For the array input you can specifiy to use the function linspace. Example: energy=linspace(starte,ende,no.values)[units] ");
    
    mpt_xraylib_add_refraction = new MultiParameters("/materials/mpt/xraylib/add/refractive_index",this);
    mpt_xraylib_add_refraction -> SetNumberOfParameters(2);
    mpt_xraylib_add_refraction -> SetGuidance("Set the optical properties of a material (compound or element) for absorption length values automatically using the xraylib library. Specify the material, "
                                               "and array of energy values.\n"
                                               "Example: material=name energy=(e1,e2,eN...)[units]\n"
                                               "Flags optional. For the array input you can specifiy to use the function linspace. Example: energy=linspace(starte,ende,no.values)[units] ");
    
    mpt_xraylib_add_complexrefraction = new MultiParameters("/materials/mpt/xraylib/add/complexrefractive_index",this);
    mpt_xraylib_add_complexrefraction -> SetNumberOfParameters(2);
    mpt_xraylib_add_complexrefraction -> SetGuidance("Set the optical properties of a material (compound or element) for absorption length values automatically using the xraylib library. Specify the material, "
                                               "and array of energy values.\n"
                                               "Example: material=name energy=(e1,e2,eN...)[units]\n"
                                               "Flags optional. For the array input you can specifiy to use the function linspace. Example: energy=linspace(starte,ende,no.values)[units] ");
                                           
    mpt_add_refractiveindex = new MultiParameters("/materials/mpt/add/refractive_index", this);
    mpt_add_refractiveindex -> SetNumberOfParameters(3);
    mpt_add_refractiveindex -> SetGuidance("Set the optical properties of a material (compound or element) for refraction by specifying the material, "
                                      "an array of energy values and an array of corrosponding refraction values.\n"
                                      "Example: material=name energy=(e1,e2,eN...)[units] rindex=(r1,r2,rN...)\n"
                                      "Flags optional. For the array input you can specifiy to use the function linspace. Example: energy=linspace(starte,ende,no.values)[units] ");
    
    mpt_add_imrefractiveindex = new MultiParameters("/materials/mpt/add/complexrefractive_index", this);
    mpt_add_imrefractiveindex -> SetNumberOfParameters(3);
    mpt_add_imrefractiveindex -> SetGuidance("Set the optical properties of a material (compound or element) for complex refraction values by specifying the material, "
                                         "an array of energy values and an array of corrosponding refraction values.\n"
                                         "Example: material=name energy=(e1,e2,eN...)[units] rindex=(r1,r2,rN...)\n"
                                         "Flags optional. For the array input you can specifiy to use the function linspace. Example: energy=linspace(starte,ende,no.values)[units] ");
    
    mpt_add_absorptionlen = new MultiParameters("/materials/mpt/add/absorptionlength", this);
    mpt_add_absorptionlen -> SetNumberOfParameters(3);
    mpt_add_absorptionlen -> SetGuidance("Set the optical properties of a material (compound or element) for absorption length values by specifying the material, "
                                       "an array of energy values and an array of corrosponding absorption length values.\n"
                                       "Example: material=name energy=(e1,e2,eN...)[units] abslength=(a1,a2,aN...)\n"
                                       "Flags optional. For the array input you can specifiy to use the function linspace. Example: energy=linspace(starte,ende,no.values)[units] ");
}

DefineMaterialsMessenger::~DefineMaterialsMessenger()
{
	delete MaterialsDirectory;

	delete define_element;
	delete define_isotope;
	delete define_isotopemix;
	delete addto_isotopemix;
	delete addto_isotopemix_density;
	
	delete define_molecule;
    delete addto_molecule_element;
    
    delete define_compound;
    delete addto_compound_element;
    
    delete define_mixture;
    delete addto_mixture_material;
    
    
    delete mpt_print;
    
    delete mpt_xraylib_add_all_op;
    delete mpt_xraylib_add_absorptionlen;
    delete mpt_xraylib_add_refraction;
    delete mpt_xraylib_add_complexrefraction;
    
    delete mpt_add_refractiveindex;
    delete mpt_add_imrefractiveindex;
    delete mpt_add_absorptionlen;
}

#include "CommandStatus.hh"

int DefineMaterialsMessenger::ApplyCommand(G4UIcommand* command, G4String newValue)
{
    if (command == define_element)
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
	else if(command == define_isotope)
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
    else if(command == define_isotopemix)
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
    else if(command == addto_isotopemix)
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
    else if(command == addto_isotopemix_density)
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
    else if (command == define_molecule)
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
    else if (command == addto_molecule_element)
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
    else if (command == define_compound)
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
    else if (command == addto_compound_element)
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
	else if (command == define_mixture)
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
	else if (command == addto_mixture_material)
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
	else if (command == mpt_xraylib_add_all_op)
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
	else if (command == mpt_xraylib_add_absorptionlen)
	{
	    Tokenizer token(newValue, command);
	
	    Flag namef(0, "material");
	    Flag energyf(1, "energy", energyUnits);
	    
	    namef.parameter = 0;
	    energyf.parameter = 1;
	    
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
	         
	    materials -> AddOpticalProperty_xraylib(material, "ABSLENGTH", energy.values, energy.size);
	}
	else if (command == mpt_xraylib_add_refraction)
	{
	    G4cout << "\nhere" << G4endl;
	
	    Tokenizer token(newValue, command);
	
	    Flag namef(0, "material");
	    Flag energyf(1, "energy", energyUnits);
	    
	    std::string material;
	    doubleArray energy;
	    doubleArray rindex_array;
	    
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
	         
	    materials -> AddOpticalProperty_xraylib(material, "RINDEX", energy.values, energy.size);
	}
	else if (command == mpt_xraylib_add_complexrefraction)
	{
	    Tokenizer token(newValue, command);
	
	    Flag namef(0, "material");
	    Flag energyf(1, "energy", energyUnits);
	    
	    std::string material;
	    doubleArray energy;
	    doubleArray rindex_array;
	    
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
	         
	    materials -> AddOpticalProperty_xraylib(material, "IMAGINARYRINDEX", energy.values, energy.size);
	}
	else if (command == mpt_add_refractiveindex)
	{
	    G4cout << "\nDoing" << G4endl;
	
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
	else if (command == mpt_add_imrefractiveindex)
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
	else if (command == mpt_add_absorptionlen)
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
	
	return 0;
}

void DefineMaterialsMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if (command == mpt_print)
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


