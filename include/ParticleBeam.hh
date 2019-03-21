#ifndef ParticleBeam_h 
#define ParticleBeam_h 1

#include "G4VPrimaryGenerator.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "Randomize.hh"

class G4Event;

class ParticleBeam
{
    public:
        ParticleBeam();
        ~ParticleBeam();        
        
        G4ThreeVector FireParticle(G4Event* event);
        double CalculatePositionX()              {return halfx * 2 * (G4UniformRand() - 0.5) + centre.y(); }
        double CalculatePositionY()              {return halfy * 2 * (G4UniformRand() - 0.5) + centre.z(); }
        inline double CalculatePositionX(double _halfx) {return _halfx * 2 * (G4UniformRand() - 0.5);}
        inline double CalculatePositionY(double _halfy) {return _halfy * 2 * (G4UniformRand() - 0.5);} 
        
        void DoAutoSourcePlacement(G4double value) {if (autoSourcePlacement) {centre = G4ThreeVector(value, centre.y(), centre.z());}}
        void SetAutoSourcePlacement(bool value)    {autoSourcePlacement = value;}
        //void SetSourcePlacement(G4double value)    {centre = G4ThreeVector(value, centre.y(), centre.z());}
        void SetCentreCoordinates(G4ThreeVector value) {centre = value;}
                
        //Get functions
        G4ParticleGun*           GetFastGun() {return fastGun;}
        G4GeneralParticleSource* GetAdvaGun() {return advaGun;}
        
        G4double GetEnergyOfEvent();
        G4String GetParticleName() {if (usefastGun) {return fastGun->GetParticleDefinition()->GetParticleName();}
                                    else            {return advaGun->GetParticleDefinition()->GetParticleName();}}
        bool PolarizationRandom() {return randomPolization;}
        G4ThreeVector GetPolarization() {if (usefastGun) {return fastGun->GetParticlePolarization();}
                                         else            {return advaGun->GetParticlePolarization();}}
                                         
        double GetHalfX(){return halfx;}
        double GetHalfY(){return halfy;}
        
        //Set fucntions
        void SetHalfX(double value) {advaGun->GetCurrentSource()->GetPosDist()->SetHalfX(value); halfx = value;}
        void SetHalfY(double value) {advaGun->GetCurrentSource()->GetPosDist()->SetHalfY(value); halfy = value;}
        //void SetSource(double value) {isource = value;}
        void SetPolization(G4ThreeVector value) {advaGun->SetParticlePolarization(value);
                                                 fastGun->SetParticlePolarization(value);
                                                 randomPolization = false;}
        
        void SetMomemtumDirection(G4ThreeVector value) {advaGun->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(value); 
                                                        fastGun->SetParticleMomentumDirection(value);}
        void SetBeamMonoEnergy(double value)           {advaGun->GetCurrentSource()->GetEneDist()->SetMonoEnergy(value);
                                                        fastGun->SetParticleEnergy(value);}                                                    
        void SetParticle(G4String particle);
        void SetBeamEnergy(G4double mono, G4double sigma);
        
        void UseFastGun(bool value) {usefastGun = value;}
        bool UseFastGun() {return usefastGun;}
        
    private:
        void DefualtValues();      
        G4ThreeVector RandomPolarization(); 
    
        G4ParticleGun*           fastGun;
        G4GeneralParticleSource* advaGun;
        
        bool usefastGun;
        bool autoSourcePlacement;
        bool randomPolization;
        double halfx;
        double halfy;
        G4ThreeVector centre;
};

#endif 
