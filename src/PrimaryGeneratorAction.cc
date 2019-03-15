//Own classes
#include "Data.hh"
#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorActionMessenger.hh"
#include "G4GeneralParticleSource.hh"
#include "G4VPrimaryGenerator.hh"

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

#include "PrintLines.hh"
#include "ProgressTracker.hh"

#include "ParticleBeam.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(Data* DataObject):G4VUserPrimaryGeneratorAction(), data(DataObject), progress()
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
	
	//Set the max energy value (in keV)
    eMax = 175;
    randPolization = true;
    
    beam = new ParticleBeam();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  	//delete ParticleGun;
	delete gunMessenger;
	
	if (ParticleGun){delete ParticleGun;}
	if (fastParticleGun){delete fastParticleGun;}
	
	delete beam;
}

void PrimaryGeneratorAction::SetParticleType(G4String type)
{
    particle = G4ParticleTable::GetParticleTable() -> FindParticle(type); 
	if (!particle){
	    G4cout << "\nINVALID PARTICLE: " << type << "\nSwitching to gamma..." << G4endl;
	    particle = G4ParticleTable::GetParticleTable() -> FindParticle("gamma"); 
	}
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
	/*if(CurrentEvent == 1)
	{   
	    //Cleans the beam energy data at the start of each run    
	    memset(&beamEnergy[0], 0, sizeof(beamEnergy[0]) * Bins);

        //Prints only at the start of the simulation	    
	    if (CurrentImage == 1)
	    {	
	        progress.Timer.Start();	
		    G4cout << "\n================================================================================"
		              "\n                            SIMULATION RUNNING..."
	                  "\n================================================================================\n\n\n\n";
	    }
    }
    
    G4ThreeVector polar;
    if (randPolization){polar = RandomPolarization();}
    else {polar = polization;}
    
    if (EnergyDistTypeCmd == "Mono")
    {             
        fastParticleGun -> SetParticlePolarization(polar);
      
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
        ParticleGun -> SetParticlePolarization(polar);
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
    
    if (ShowProgressBar == true && CurrentEvent > 1){progress.PrintProgress(CurrentEvent, CurrentImage);}*/
    
    if(CurrentEvent == 1)
	{   
	    //Cleans the beam energy data at the start of each run    
	    memset(&beamEnergy[0], 0, sizeof(beamEnergy[0]) * Bins);

        //Prints only at the start of the simulation	    
	    if (CurrentImage == 1)
	    {	
	        progress.Timer.Start();	
		    G4cout << "\n================================================================================"
		              "\n                            SIMULATION RUNNING..."
	                  "\n================================================================================\n\n\n\n";
	    }
    }
    
    G4ThreeVector particleposition = beam->FireParticle(event);
    
    //Save beam energy data    	
    int bin = floor(beam->GetEnergyOfEvent()*1000/(eMax/Bins)) -1;
		
    if (bin < 0)                      {bin = 0;}
    else if (bin > beamEnergy.size()) {bin = beamEnergy.size();}

    ++beamEnergy[bin];
    ++CurrentEvent;
    
    if (ShowProgressBar == true && CurrentEvent > 1) {progress.PrintProgress(CurrentEvent, CurrentImage);}
} 

void PrimaryGeneratorAction::ReadOutInfo(SettingsLog& log)
{
	G4ThreeVector BeamDimensions = G4ThreeVector(0, BeamWidthY_Cmd, BeamHeightZ_Cmd);

	PrintToEndOfTerminal(log, '-');
	log << "BEAM INFORMATION:"
		   "\n- Particles: " << particle -> GetParticleName()  
		<< "\n- Polization: ";
		   if (randPolization){G4cout << "random";}
		   else {G4cout << G4BestUnit(polization, "Angle");} 
           /*"\n- Beam type: " << EnergyDistTypeCmd;
           
    if (EnergyDistTypeCmd == "Gauss")
    {
        log << "\n- Mean energy: " << G4BestUnit(energyCmd, "Energy")
            << "\n- Sigma of the distrbution: " << G4BestUnit(EnergySigmaCmd, "Energy");
    }
    else
        log << "\n- Energy of the monochomatic beam: " << G4BestUnit(energyCmd, "Energy");
	*/       
	log << "\n- Half beam dimensions: " << G4BestUnit(BeamDimensions, "Length");
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
    /*std::vector<G4String> EDistTypeOptions = {"Mono", "Mono(GPS)", "Gauss"};
    
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
    
    data -> SetMaxEnergy(eMax);

    std::vector<int> ibeamEnergy(Bins, 0);
   	beamEnergy = ibeamEnergy;*/
}

G4ThreeVector PrimaryGeneratorAction::RandomPolarization()
{
    G4double angle = G4UniformRand() * 360.0*deg;

    G4ThreeVector normal (1., 0., 0.);
    
    G4ThreeVector kphoton;
    if(fastParticleGun){kphoton = fastParticleGun->GetParticleMomentumDirection();}
    if(ParticleGun){kphoton = ParticleGun->GetParticleMomentumDirection();}
    
    G4ThreeVector product = normal.cross(kphoton);
    G4double modul2       = product*product;
 
    G4ThreeVector e_perpend (0., 0., 1.);
    if (modul2 > 0.) e_perpend = (1./std::sqrt(modul2))*product;
    G4ThreeVector e_paralle    = e_perpend.cross(kphoton);
    
    return std::cos(angle)*e_paralle + std::sin(angle)*e_perpend;
}
