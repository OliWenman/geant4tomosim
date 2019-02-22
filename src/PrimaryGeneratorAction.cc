//Own classes
#include "Data.hh"
#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorActionMessenger.hh"
#include "G4GeneralParticleSource.hh"
//Output to console/write to file
#include "SettingsLog.hh"

//Used to create an event to fire the particles
#include "G4Event.hh"
#include "G4ParticleGun.hh"

//Used to define particles
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

//Geant4 Units
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4TwoVector.hh"

//Randomize position of the beam
#include "Randomize.hh"

//To access GPS varabiles
#include "G4SPSPosDistribution.hh"
#include "G4SPSAngDistribution.hh"
#include "G4SPSEneDistribution.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(Data* DataObject):G4VUserPrimaryGeneratorAction(), data(DataObject), Timer()//, ProgressTracker()
{
	//Create a messenger for this class
  	gunMessenger = new PrimaryGeneratorActionMessenger(this);

	gunExists = false;
	
	ShowProgressBar = true;
	
	//Defualt values
	EnergyDistTypeCmd = "Mono";
	energyCmd = 30*keV;
	EnergySigmaCmd = 0*keV;
	SetParticleType("gamma");
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  	//delete ParticleGun;
	delete gunMessenger;
	
	if (ParticleGun){delete ParticleGun;}
	if (fastParticleGun){delete fastParticleGun;}
}

void PrimaryGeneratorAction::SetParticleType(G4String type)
{
    particle = G4ParticleTable::GetParticleTable() -> FindParticle(type); 
	if (!particle){
	    G4cout << "\nINVALID PARTICLE: " << type << "\nSwitching to gamma." << G4endl;
	    particle = G4ParticleTable::GetParticleTable() -> FindParticle("gamma"); 
	}
	G4cout << "\nThe beam is made up of " << particle -> GetParticleName() << G4endl;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	if(CurrentEvent == 1)
	{   
	    //Cleans the beam energy data at the start of each run    
	    memset(&beamEnergy[0], 0, sizeof(beamEnergy[0]) * Bins);

        //Prints only at the start of the simulation	    
	    if (CurrentImage == 1)
	    {	
	        Timer.Start();	
		    G4cout << "\n================================================================================"
		              "\n                            SIMULATION RUNNING..."
	                  "\n================================================================================\n\n\n\n";
	    }
    }
    
    if (EnergyDistTypeCmd == "Mono")
    { 
        if (particle -> GetParticleName() == "opticalphoton")
        {
            //G4cout << "\nBEING USED " << G4endl;
            fastParticleGun -> SetParticlePolarization(G4ThreeVector(1,0,1));
        }
    
        //Allow the particles to be fired randomly within the beam width
      	G4double y0 = BeamHeightZ_Cmd*2 * (G4UniformRand()-0.5);
      	G4double z0 = BeamWidthY_Cmd*2 * (G4UniformRand()-0.5);

        //Set the ParticleGun conditions
        fastParticleGun -> SetParticlePosition(G4ThreeVector(StartingPosition, y0, z0));
        	
        //Generate the particle in the event
      	fastParticleGun -> GeneratePrimaryVertex(anEvent);
      	
      	if (FluoreFM == true){
        		data -> SetParticlePosition(G4ThreeVector(StartingPosition, y0, z0));}
    }
	else
    {
        //Generate the particle in the event
        ParticleGun -> GeneratePrimaryVertex(anEvent);
         
        if (FluoreFM == true){
       		data -> SetParticlePosition(ParticleGun -> GetParticlePosition());}
    }
	
    //Save beam energy data
    int bin;
        	
    if (EnergyDistTypeCmd == "Mono"){
        bin = floor(fastParticleGun -> GetParticleEnergy()*1000/(eMax/Bins)) -1;}
    else {
        bin = floor(ParticleGun -> GetParticleEnergy()*1000/(eMax/Bins)) - 1;}
		
    if (bin < 0){
       	bin = 0;}
    else if (bin > beamEnergy.size()){
        bin = beamEnergy.size();}

    ++beamEnergy[bin];
    
    ++CurrentEvent;
    
    if (ShowProgressBar == true && CurrentEvent > 1){
        PrintProgress();}
} 

void PrimaryGeneratorAction::ReadOutInfo(SettingsLog& log)
{
	G4ThreeVector BeamDimensions = G4ThreeVector(0, BeamWidthY_Cmd, BeamHeightZ_Cmd);

	log << "\n--------------------------------------------------------------------"
	       "\nBEAM INFORMATION: \n"
		      
           "\n- Beam type: " << EnergyDistTypeCmd;
    if (EnergyDistTypeCmd == "Gauss")
    {
        log << "\n- Mean energy: " << G4BestUnit(energyCmd, "Energy")
            << "\n- Sigma of the distrbution: " << G4BestUnit(EnergySigmaCmd, "Energy");
    }
    else
        log << "\n- Energy of the monochomatic beam: " << G4BestUnit(energyCmd, "Energy");
	       
	log << "\n- Half beam dimensions: " << G4BestUnit(BeamDimensions, "Length") << G4endl;
}

void PrimaryGeneratorAction::SetValues(int nBins, double Position)
{
    StartingPosition = -Position;   
    Bins = nBins;
}

void PrimaryGeneratorAction::SetupFastParticleGun(G4double monoEnergy)
{
     fastParticleGun = new G4ParticleGun(1);
        
     fastParticleGun -> SetParticleDefinition(particle);
        
     fastParticleGun -> SetParticleMomentumDirection(G4ThreeVector(1, 0, 0));
        
     fastParticleGun -> SetParticleEnergy(monoEnergy);
        
     EnergySigmaCmd = 0*keV;
     energyCmd = monoEnergy;
}

void PrimaryGeneratorAction::SetupParticleGun(G4String GunType, G4double monoEnergy, G4double sigmaEnergy)
{

    ParticleGun = new G4GeneralParticleSource();
    ParticleGun -> SetParticleDefinition(particle);

    //Automatically set the gun position to the end of the world volume
    ParticleGun -> GetCurrentSource() -> GetPosDist() -> SetCentreCoords(G4ThreeVector(StartingPosition, 0, 0));
    ParticleGun -> GetCurrentSource() -> GetPosDist() -> SetHalfX(BeamWidthY_Cmd);
    ParticleGun -> GetCurrentSource() -> GetPosDist() -> SetHalfY(BeamHeightZ_Cmd);
    //Automatically set the correct rotation
    ParticleGun -> GetCurrentSource() -> GetPosDist() -> SetPosRot1(G4ThreeVector(0, 0, 1));
    ParticleGun -> GetCurrentSource() -> GetPosDist() -> SetPosRot2(G4ThreeVector(0, 1, 0));
    //Automatically select the shape and type
    ParticleGun -> GetCurrentSource() -> GetPosDist() -> SetPosDisType("Plane");
    ParticleGun -> GetCurrentSource() -> GetPosDist() -> SetPosDisShape("Square");

    //Automatically set the gun momentum in the positive x direction
    ParticleGun -> GetCurrentSource() -> GetAngDist() -> SetParticleMomentumDirection(G4ThreeVector(1, 0, 0));

    if (GunType == "Mono(GPS)")
    {
        ParticleGun -> GetCurrentSource() -> GetEneDist() -> SetEnergyDisType("Mono");
        sigmaEnergy = 0*keV;
    }
    else {
        ParticleGun -> GetCurrentSource() -> GetEneDist() -> SetEnergyDisType("Gauss");}
        
    ParticleGun -> GetCurrentSource() -> GetEneDist() -> SetMonoEnergy(monoEnergy);
    ParticleGun -> GetCurrentSource() -> GetEneDist() -> SetBeamSigmaInE(sigmaEnergy);
        
    energyCmd = monoEnergy;
    EnergySigmaCmd = sigmaEnergy; 
    EnergyDistTypeCmd = GunType;
}


void PrimaryGeneratorAction::SetupGun(G4String GunType, G4double monoEnergy, G4double sigmaEnergy)
{  
    //A list of available options for a user to pick from
    std::vector<G4String> EDistTypeOptions = {"Mono", "Mono(GPS)", "Gauss"};
    
    //Checks to see if a gun exists first
    if (!gunExists)
    {
        //If option picked is "Mono"
        if (GunType == EDistTypeOptions[0]){
            SetupFastParticleGun(monoEnergy);
            gunExists = true;
            EnergyDistTypeCmd = GunType;
        }
        //If option picked is "Mono(GPS)" or "Gauss"
        else if (GunType == EDistTypeOptions[1] || GunType == EDistTypeOptions[2] ){
            SetupParticleGun(GunType, monoEnergy, sigmaEnergy);
            gunExists = true;
            EnergyDistTypeCmd = GunType;
        }
        else
        {
            G4cout << "\nYou have not selected an available option for the command \"/beam/energy/type\". You can pick from: \n";
            for (int ele = 0; ele < EDistTypeOptions.size(); ele++){
                G4cout << EDistTypeOptions[ele] << " ";}
            G4cout << G4endl;
            exit(0);
        }
    }
    //If a ParticleGun already exists, modify the energy values or switch particle gun between images
    if (gunExists)
    {
        if (ParticleGun)
        {
            if (GunType == EDistTypeOptions[1]) {   
                ParticleGun -> GetCurrentSource() -> GetEneDist() -> SetMonoEnergy(monoEnergy);
            }
            else if (GunType == EDistTypeOptions[2]){              
                ParticleGun -> GetCurrentSource() -> GetEneDist() -> SetMonoEnergy(monoEnergy);
                ParticleGun -> GetCurrentSource() -> GetEneDist() -> SetBeamSigmaInE(sigmaEnergy);
            }
            //If the particle gun needs to change to a fast one, delete the current one and setup new one
            else if (GunType == EDistTypeOptions[0]){
                delete ParticleGun;
                SetupFastParticleGun(monoEnergy);
                EnergyDistTypeCmd = GunType;
            }
        }       
        //If fastParticleGun exists, modiy it's energy or switch particleGun
        else if (fastParticleGun)
        {
            if (GunType == EDistTypeOptions[0]){
                fastParticleGun -> SetParticleEnergy(monoEnergy);
            } 
            //Switch to the other particleGun and delete the current one if that option is selected
            else if (GunType == EDistTypeOptions[1] || EDistTypeOptions[2]){
                
                delete fastParticleGun;
                SetupParticleGun(GunType, monoEnergy, sigmaEnergy);
                EnergyDistTypeCmd = GunType;
            }
        }  
    }
    
    //Set the max energy value (in keV)
    eMax = 175;
    
    data -> SetMaxEnergy(eMax);

    std::vector<int> ibeamEnergy(Bins, 0);
   	beamEnergy = ibeamEnergy;
}

//========================================================================================================================
//Functions to do with printing the progress of the simulation

void PrimaryGeneratorAction::PrintProgress()
{
	//Works out the percentage of how many events it has completed
	int ImageProgress = (double(CurrentEvent)/NumberOfEvents)*100;

	//Only prints the percentage if the image number has changed
	if (ImageProgress != ImageProgressCheck)
	{	
		//Calculates the total progress of the simulationEnergySigmaCmd
		int FullProgress = double(CurrentImage - 1)/NumberOfRuns*100;
		TotalProgress = FullProgress + (double(ImageProgress)/100 * (1./NumberOfRuns)*100);
		
		ImageProgressCheck = ImageProgress;

		//Prints above one line and over rides it
		G4cout << "\033[2A" "\033[K" "\r" "Projection " << CurrentImage << " of " << NumberOfRuns << ": " << std::setw(3) << ImageProgress << "%\ complete ";
		if (ImageProgress == 100)
		    G4cout << "- Saving data...\n";
		else if (ImageProgress == 0)
		    G4cout << "                \n";
		else 
		    G4cout << "\n";

		if(TotalProgress != TotalProgressCheck)
		{	G4cout << "\033[K" "\rTotal progress: " << std::setw(3) << TotalProgress << "\%"; ProgressBar(TotalProgress); G4cout << "\n" "\033[40C";
			EstimatedTime(TotalProgress);
			TotalProgressCheck = TotalProgress;
		}
		else {
		    G4cout << "\n" "\033[40C";}

		G4cout << std::flush;
	}

	//Corrects the end perecentage to 100% once simulation is complete and outputs a space
	if(CurrentEvent == NumberOfEvents && CurrentImage == NumberOfRuns)
	{	
        G4cout << "\033[2A" "\033[K" "\r" "All " << NumberOfRuns << " projections are complete!              \n"
        "\rTotal progress: 100\%"; ProgressBar(100); G4cout << "\n";
		EstimatedTime(100);
		G4cout << G4endl;
		Timer.Stop();
	}
}

void PrimaryGeneratorAction::ProgressBar(int Percent)
{
	int intervals = 50;
	int dProgress = 100/intervals;

	G4cout << " (";
	for (int nbar = 0; nbar < intervals ; ++nbar)
	{
		if (nbar*dProgress < Percent)
			G4cout << "|";
		else
			G4cout << " ";
	}
	G4cout << ") ";
}

void PrimaryGeneratorAction::EstimatedTime(int Percent)
{
    if (Percent != 0)
    {
		Timer.Stop();	
		
		float currentTime = Timer.GetRealElapsed();
		
		remainingTime = ((currentTime*(100./Percent)) - currentTime) + (SavingTime * (NumberOfRuns - CurrentImage));
	
	    G4cout <<  "\rEstimated time remaining: ";
		PrintTime(remainingTime);

	}
	else
	{
		G4cout << "\rEstimated time remaining: calculating... ";
	}
}

void PrimaryGeneratorAction::PrintTime(double time)
{
	//Prints out the sustiable units for the estimated time 
	if (time > 60)
	{
		if(time > 60*60){
			G4cout << std::setw(4) << std::setprecision(3) << time/(60*60) << " hours    ";}
		else{
			G4cout << std::setw(4) << std::setprecision(2) << time/60 << " minutes  ";}
	}
	else {
        G4cout << std::setw(4) << std::setprecision(2) << int(time) << " s        ";}
}
