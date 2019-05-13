/*
A way to allow the particle G4Gamma to have access to refraction physics as
Geant4 has a seperate particle, G4OpticalPhoton, for this physics process.
Overriding the IsApplicable method to accpect the G4Gamma particle instead.
NOTES: This is experimental and may have unintended side effects. One
of these is that fluorescence will no longer occur at all with this process
on for some reason.
*/

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
           return ( &aParticleType == G4Gamma::Gamma());//|| &aParticleType == G4OpticalPhoton::OpticalPhoton());
        }
};

#endif
