#ifndef TrackerHit_h
#define TrackerHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"
#include "G4ParticleDefinition.hh"

//Tracker hit class.
//It defines data members to store the trackID, chamberNb, energy deposit,
//and position of charged particles in a selected volume:
//- fTrackID, fChamberNB, fEdep, fPos

class TrackerHit : public G4VHit
{
  	public:
    		TrackerHit();
    		TrackerHit(const TrackerHit&);
    		virtual ~TrackerHit();

    		// operators
    		const TrackerHit& operator = (const TrackerHit&);
    		G4int operator == (const TrackerHit&) const;

    		inline void* operator new(size_t);
    		inline void  operator delete(void*);

    		// methods from base class
    		virtual void Draw();
    		virtual void Print();

    		// Set methods
    		inline void SetTrackID(G4int value){fTrackID = value;};
    		inline void SetChamberNb(G4int value){fChamberNb = value;};
    		inline void SetEdep(G4double value){fEdep = value;};
    		inline void SetPos(G4ThreeVector xyz){fPos = xyz;};
		inline void SetParticle(G4ParticleDefinition* particle){fParticle = particle;}

		inline void SetDraw(G4bool value){DrawOption = value;}

    		// Get methods
    		G4int GetTrackID() const {return fTrackID;};
    		G4int GetChamberNb() const {return fChamberNb;};
    		G4double GetEdep() const {return fEdep;};
    		G4ThreeVector GetPos() const {return fPos;};
		inline G4ParticleDefinition* GetParticle(){return fParticle;};
		G4bool GetDraw() const {return DrawOption;}

  	private:
      		G4int         fTrackID;
      		G4int         fChamberNb;
      		G4double      fEdep;
      		G4ThreeVector fPos;
		G4ParticleDefinition* fParticle;

		G4bool DrawOption;
};

typedef G4THitsCollection<TrackerHit> TrackerHitsCollection;

extern G4ThreadLocal G4Allocator<TrackerHit>* TrackerHitAllocator;

inline void* TrackerHit::operator new(size_t)
{
  	if(!TrackerHitAllocator)
      		TrackerHitAllocator = new G4Allocator<TrackerHit>;
	return (void *) TrackerHitAllocator->MallocSingle();
}


inline void TrackerHit::operator delete(void *hit)
{
  	TrackerHitAllocator->FreeSingle((TrackerHit*) hit);
}


#endif
