#ifndef GammaOpticalAbsorption_h
#define GammaOpticalAbsorption_h 1

#include "G4OpAbsorption.hh"
#include "G4Gamma.hh"

class GammaOpticalAbsorption : public G4OpAbsorption
{
    public:
        //Overrides the function for G4OpsAbsorption to make the physics process avaliable to be used by gamma particles 
         virtual G4bool inline IsApplicable (const G4ParticleDefinition &aParticleType)
         {
            return ( &aParticleType == G4Gamma::Gamma() || &aParticleType == G4OpticalPhoton::OpticalPhoton());
         }
};

#endif
