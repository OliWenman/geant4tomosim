#ifndef GammaOpticalRefraction_h
#define GammaOpticalRefraction_h 1

#include "G4OpBoundaryProcess.hh"
#include "G4Gamma.hh"

class GammaOpticalRefraction : public G4OpBoundaryProcess
{
    public:
        //Overrides the function for G4OpBoundaryProcess to make the physics process avaliable to be used by gamma particles
        virtual G4bool inline IsApplicable (const G4ParticleDefinition &aParticleType)
        {
           return ( &aParticleType == G4Gamma::Gamma() || &aParticleType == G4OpticalPhoton::OpticalPhoton());
        }
};

#endif
