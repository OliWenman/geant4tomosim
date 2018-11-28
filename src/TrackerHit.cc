#include "TrackerHit.hh"

#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4ThreadLocal G4Allocator<TrackerHit>* TrackerHitAllocator = 0;

TrackerHit::TrackerHit():G4VHit(), fTrackID(-1), fChamberNb(-1), fEdep(0.), fPos(G4ThreeVector()), fParticle(0)
{}

TrackerHit::~TrackerHit(){}

TrackerHit::TrackerHit(const TrackerHit& right):G4VHit()
{
  	fTrackID   = right.fTrackID;
  	fChamberNb = right.fChamberNb;
  	fEdep      = right.fEdep;
  	fPos       = right.fPos;
	fParticle  = right.fParticle;
}

const TrackerHit& TrackerHit::operator = (const TrackerHit& right)
{
  	fTrackID   = right.fTrackID;
  	fChamberNb = right.fChamberNb;
  	fEdep      = right.fEdep;
  	fPos       = right.fPos;
	fParticle  = right.fParticle;

  	return *this;
}

G4int TrackerHit::operator==(const TrackerHit& right) const
{
  	return ( this == &right ) ? 1 : 0;
}

void TrackerHit::Draw()
{
  	G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  	if(pVVisManager)
  	{
    		G4Circle circle(fPos);
    		circle.SetScreenSize(4.);
    		circle.SetFillStyle(G4Circle::filled);
    		G4Colour colour(1.,0.,0.);
    		G4VisAttributes attribs(colour);
    		circle.SetVisAttributes(attribs);
    		pVVisManager -> Draw(circle);
	}
}

void TrackerHit::PrintEnergy()
	{G4cout << " Detector " << fChamberNb << " hit -> Energy deposit = " << G4BestUnit(fEdep,"Energy");}

void TrackerHit::PrintHit()
	{G4cout << " Detector " << fChamberNb << " hit. "; }

