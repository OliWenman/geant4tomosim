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
    
    G4ParticleDefinition* defaultParticle = G4ParticleTable::GetParticleTable() -> FindParticle("gamma"); 
    G4ThreeVector defaultMomentum(1., 0., 0.);
    double defaultEnergy = 50*keV;
    double defaultHalfX  = 1.05*cm;
    double defaultHalfY  = 1.25*cm;
    G4ThreeVector defaultcentre(0, 0, 0);

    //Setup faster particle gun
    fastGun -> SetParticleDefinition(defaultParticle);
    fastGun -> SetParticleEnergy(defaultEnergy);
    fastGun -> SetParticleMomentumDirection(defaultMomentum);
    
    //Default variables for the advanced particle gun GPS
    G4ThreeVector defaultRotation1 (0, 0, 1);
    G4ThreeVector defualtRotation2 (0, 1, 0);
    
    G4String beamType = "Plane";
    G4String beamShape = "Rectangle";
    
    advaGun -> SetParticleDefinition(defaultParticle);
    
    //Setup advanced particlegun
    advaGun -> GetCurrentSource() -> GetPosDist() -> SetCentreCoords(G4ThreeVector(0, 0, 0));
    advaGun -> GetCurrentSource() -> GetPosDist() -> SetHalfX(defaultHalfX);
    advaGun -> GetCurrentSource() -> GetPosDist() -> SetHalfY(defaultHalfY);
    //Automatically set the correct rotation
    advaGun -> GetCurrentSource() -> GetPosDist() -> SetPosRot1(defaultRotation1);
    advaGun -> GetCurrentSource() -> GetPosDist() -> SetPosRot2(defualtRotation2);
    //Automatically select the shape and type
    advaGun -> GetCurrentSource() -> GetPosDist() -> SetPosDisType(beamType);
    advaGun -> GetCurrentSource() -> GetPosDist() -> SetPosDisShape(beamShape);

    //Automatically set the gun momentum in the positive x direction
    advaGun -> GetCurrentSource() -> GetAngDist() -> SetParticleMomentumDirection(defaultMomentum);
    advaGun -> GetCurrentSource() -> GetAngDist() -> DefineAngRefAxes("angref1", defaultRotation1);
    advaGun -> GetCurrentSource() -> GetAngDist() -> DefineAngRefAxes("angref2", defualtRotation2);

    advaGun -> GetCurrentSource() -> GetEneDist() -> SetEnergyDisType("Mono");      
    advaGun -> GetCurrentSource() -> GetEneDist() -> SetMonoEnergy(defaultEnergy);
    
    halfx = defaultHalfX;
    halfy = defaultHalfY;
    centre = defaultcentre;
    isource = defaultcentre.x();
}

G4ThreeVector ParticleBeam::FireParticle(G4Event* event)
{
    G4ThreeVector position;
  
    if (usefastGun) 
    {
        double x = CalculatePositionX();
        double y = CalculatePositionY();
        
        if (randomPolization) {fastGun -> SetParticlePolarization(RandomPolarization());}
        
        fastGun -> SetParticlePosition(G4ThreeVector(centre.x(), y, x));
        fastGun -> GeneratePrimaryVertex(event);
        
        position = G4ThreeVector(isource, x, y);
    }
    else      
    {
        if (randomPolization) {advaGun -> SetParticlePolarization(RandomPolarization());}
    
        advaGun->GetCurrentSource()->GetPosDist()->SetCentreCoords(centre);
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
    
    if (sigma == 0 && !usefastGun)
    {
        advaGun -> GetCurrentSource() -> GetEneDist() -> SetEnergyDisType("Mono");      
    }
    else if (sigma > 0 && !usefastGun)
    {
        advaGun -> GetCurrentSource() -> GetEneDist() -> SetEnergyDisType("Gauss");      
    }
    
}

