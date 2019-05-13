/*
Class to control the particle beam. 
Two different methods viabale. Using the G4GeneralParticleSource (GPS) 
has its own built in messenger class and is very flexible on building the 
beam such as custom energy spectrum, position biasing, beam shape etc. 
Can also switch to using the G4ParticleGun, more basic but slightly 
faster than the GPS method (around 5 - 20%).

Author: Oliver Jack Wenman

*/

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
        
        G4double GetMonoEnergy()   {if (usefastGun) {return fastGun->GetParticleEnergy();}
                                    else            {return advaGun->GetCurrentSource()->GetEneDist()->GetMonoEnergy();}}
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
        void SetMonoEnergy(double value)           {advaGun->GetCurrentSource()->GetEneDist()->SetMonoEnergy(value);
                                                    fastGun->SetParticleEnergy(value);}       
        
        void UseFastGun(bool value) {usefastGun = value;}
        bool UseFastGun()           {return usefastGun;}
        void UseGPSgun(bool value)  {usefastGun = !value;} 
        bool GPSgunActive()         {if (usefastGun) {return false;} else {return true;}}
        
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
