#include "ParticleBeam.hh"

#include "G4Event.hh"

#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

ParticleBeam::ParticleBeam()
{
    fastGun = new G4ParticleGun(1);
    advaGun = new G4GeneralParticleSource();
    
    DefualtValues();
}

ParticleBeam::~ParticleBeam()
{
    delete fastGun;
    delete advaGun;
}

void ParticleBeam::DefualtValues()
{  
    //Defualt values
    usefastGun = true;
    autoSourcePlacement = true;
    randomPolization = true;
    
    G4ParticleDefinition* defualtParticle = G4ParticleTable::GetParticleTable() -> FindParticle("gamma"); 
    G4ThreeVector defaultMomentum(1., 0., 0.);
    double defaultEnergy = 50*keV;
    double defaultHalfX  = 1.05*cm;
    double defaultHalfY  = 1.25*cm;
    G4ThreeVector defaultiPosition(0, 0, 0);

    //Setup faster particle gun
    fastGun -> SetParticleDefinition(defualtParticle);
    fastGun -> SetParticleEnergy(defaultEnergy);
    fastGun -> SetParticleMomentumDirection(defaultMomentum);
    //fastGun -> SetParticlePolarization(G4ThreeVector(1,0,0));
    //advaGun -> SetParticlePolarization(G4ThreeVector(1,0,0));
    
    //Setup advanced particlegun
    advaGun -> GetCurrentSource() -> GetPosDist() -> SetCentreCoords(G4ThreeVector(0, 0, 0));
    advaGun -> GetCurrentSource() -> GetPosDist() -> SetHalfX(defaultHalfX);
    advaGun -> GetCurrentSource() -> GetPosDist() -> SetHalfY(defaultHalfY);
    //Automatically set the correct rotation
    advaGun -> GetCurrentSource() -> GetPosDist() -> SetPosRot1(G4ThreeVector(0, 0, 1));
    advaGun -> GetCurrentSource() -> GetPosDist() -> SetPosRot2(G4ThreeVector(0, 1, 0));
    //Automatically select the shape and type
    advaGun -> GetCurrentSource() -> GetPosDist() -> SetPosDisType("Plane");
    advaGun -> GetCurrentSource() -> GetPosDist() -> SetPosDisShape("Square");

    //Automatically set the gun momentum in the positive x direction
    advaGun -> GetCurrentSource() -> GetAngDist() -> SetParticleMomentumDirection(defaultMomentum);

    advaGun -> GetCurrentSource() -> GetEneDist() -> SetEnergyDisType("Mono");      
    advaGun -> GetCurrentSource() -> GetEneDist() -> SetMonoEnergy(defaultEnergy);
    advaGun -> SetParticleDefinition(defualtParticle);
    
    halfx = defaultHalfX;
    halfy = defaultHalfY;
    iposition = defaultiPosition;
    isource = defaultiPosition.x();
}

G4ThreeVector ParticleBeam::FireParticle(G4Event* event)
{
    G4ThreeVector position;
  
    if (usefastGun) 
    {
        double x = CalculatePositionX();
        double y = CalculatePositionY();
        
        if (randomPolization) {fastGun -> SetParticlePolarization(RandomPolarization());}
        
        fastGun -> SetParticlePosition(G4ThreeVector(isource, x, y));
        fastGun -> GeneratePrimaryVertex(event);
        
        position = G4ThreeVector(isource, x, y);
    }
    else      
    {
        if (randomPolization) {advaGun -> SetParticlePolarization(RandomPolarization());}
    
        advaGun->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(isource, 0, 0));
        advaGun -> GeneratePrimaryVertex(event);
        
        position = advaGun->GetParticlePosition();
    }
    return position;
}

void ParticleBeam::SetParticle(G4String value)
{
    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable() -> FindParticle(value);
	if (!particle){G4cout << "\nCannot find particle '" << value << "'. Aborting run..." << G4endl; exit(0);}
	
	fastGun -> SetParticleDefinition(particle);
	advaGun -> SetParticleDefinition(particle);
}

G4ThreeVector ParticleBeam::RandomPolarization()
{
    G4double angle = G4UniformRand() * 360.0*deg;

    G4ThreeVector normal (1., 0., 0.);
    
    G4ThreeVector kphoton;
    if (usefastGun){kphoton = fastGun->GetParticleMomentumDirection();}
    else           {kphoton = advaGun->GetParticleMomentumDirection();}
    
    G4ThreeVector product = normal.cross(kphoton);
    G4double modul2       = product*product;
 
    G4ThreeVector e_perpend (0., 0., 1.);
    if (modul2 > 0.) e_perpend = (1./std::sqrt(modul2))*product;
    G4ThreeVector e_paralle    = e_perpend.cross(kphoton);
    
    return std::cos(angle)*e_paralle + std::sin(angle)*e_perpend;
}

G4double ParticleBeam::GetEnergyOfEvent()
{
    if(usefastGun) {return fastGun->GetParticleEnergy();}
    else           {return advaGun->GetParticleEnergy();}
}

void ParticleBeam::SetBeamEnergy(G4double mono, G4double sigma)
{
    if(usefastGun) {fastGun->SetParticleEnergy(mono);}
    else           {advaGun->GetCurrentSource()->GetEneDist()->SetMonoEnergy(mono);
                    advaGun->GetCurrentSource()->GetEneDist()->SetBeamSigmaInE(sigma);}
}

