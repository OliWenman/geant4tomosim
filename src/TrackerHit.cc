#include "TrackerHit.hh"
#include "Analysis.hh"
//#include "SaveData.hh"

//#include "GlobalVariables.hh"

#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>
#include <fstream>

G4ThreadLocal G4Allocator<TrackerHit>* TrackerHitAllocator=0;

TrackerHit::TrackerHit():G4VHit(), fTrackID(-1), fChamberNb(-1), fEdep(0.), fPos(G4ThreeVector()), fParticle(0)
{
	//SaveDataClass = new SaveData();
}

TrackerHit::~TrackerHit() {}

TrackerHit::TrackerHit(const TrackerHit& right):G4VHit()
{
  	fTrackID   = right.fTrackID;
  	fChamberNb = right.fChamberNb;
  	fEdep      = right.fEdep;
  	fPos       = right.fPos;
	fParticle  = right.fParticle;
}

const TrackerHit& TrackerHit::operator=(const TrackerHit& right)
{
  	fTrackID   = right.fTrackID;
  	fChamberNb = right.fChamberNb;
  	fEdep      = right.fEdep;
  	fPos       = right.fPos;
	fParticle  = right.fParticle;


  	return *this;
}


//G4int TrackerHit::operator==(const TrackerHit& right) const
//{
//  	return ( this == &right ) ? 1 : 0;
//}


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
    		pVVisManager->Draw(circle);
  	}
}


void TrackerHit::Print()
{	
	G4double x = fPos.x();
	G4double y = fPos.y();
	G4double z = fPos.z();
	G4double weight = 1;


  	G4cout << "Detector " << fChamberNb << ": " << fParticle << G4endl;
     	       //<< "Edep: "
               //<< std::setw(7) << G4BestUnit(fEdep,"Energy")
               
               //<< G4endl;

	
  	// get analysis manager
  	auto analysisManager = G4AnalysisManager::Instance();

  	// fill histograms
  	analysisManager->FillH2(0, y, z, weight);

	/*std::ofstream myFile;
	myFile.open("output.out");
	if (myFile.is_open()) // Check that file is opened
	{		
		myFile << fChamberNb << ",\n";	
		myFile.close();
	}*/
}
