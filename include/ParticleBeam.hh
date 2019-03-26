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
        bool          CheckIfFan();
        inline double CalculatePositionX()              {return halfx * 2 * (G4UniformRand() - 0.5) + centre.y(); }
        inline double CalculatePositionY()              {return halfy * 2 * (G4UniformRand() - 0.5) + centre.z(); }
        inline double CalculatePositionX(double _halfx) {return _halfx * 2 * (G4UniformRand() - 0.5) + centre.y();}
        inline double CalculatePositionY(double _halfy) {return _halfy * 2 * (G4UniformRand() - 0.5) + centre.z();} 
        
        void DoAutoSourcePlacement(G4double value) {if (autoSourcePlacement) {centre = G4ThreeVector(value, centre.y(), centre.z());}}
                
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
        
        G4ThreeVector GetMomentum(){if (usefastGun) {return fastGun->GetParticleMomentumDirection();}
                                    else            {return advaGun->GetParticleMomentumDirection();}}
        
        G4ThreeVector GetCentreCoordinates(){return centre;}
        
        //Set fucntions
        void SetHalfX(double value) {advaGun->GetCurrentSource()->GetPosDist()->SetHalfX(value); halfx = value;}
        void SetHalfY(double value) {advaGun->GetCurrentSource()->GetPosDist()->SetHalfY(value); halfy = value;}
        void SetPolization(G4ThreeVector value) {advaGun->SetParticlePolarization(value);
                                                 fastGun->SetParticlePolarization(value);
                                                 randomPolization = false;}
        
        void SetMomentumDirection(G4ThreeVector value) {advaGun->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(value); 
                                                        fastGun->SetParticleMomentumDirection(value);} 
        void SetAutoSourcePlacement(bool value)        {autoSourcePlacement = value;}
        void SetCentreCoordinates(G4ThreeVector value) {centre = value;}                                                 
                                                                                                    
        void SetParticle(G4String particle);
        void SetBeamEnergy(G4double mono, G4double sigma);
        void SetBeamMonoEnergy(double value)           {advaGun->GetCurrentSource()->GetEneDist()->SetMonoEnergy(value);
                                                        fastGun->SetParticleEnergy(value);}       
        
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
