//-----------------------------------------------------------------------------
// Implementation file for class : TrackerHit
//
// 16/05/2002 : Witold Pokorski
//-----------------------------------------------------------------------------

//G4
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

// local
#include "GiGa/TrackerHit.h"

G4Allocator<TrackerHit> TrackerHitAllocator;

TrackerHit::TrackerHit()
{;}

TrackerHit::~TrackerHit()
{;}

TrackerHit::TrackerHit(const TrackerHit &right):G4VHit(right)
{
  edep = right.edep;
  entryPos = right.entryPos;
  exitPos = right.exitPos;
  timeOfFlight = right.timeOfFlight;
}

const TrackerHit& TrackerHit::operator=(const TrackerHit &right)
{
  edep = right.edep;
  entryPos = right.entryPos;
  exitPos = right.exitPos;
  timeOfFlight = right.timeOfFlight;
  return *this;
}

int TrackerHit::operator==(const TrackerHit &right) const
{
  return 0;
}

void TrackerHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle((exitPos+entryPos)*0.5);
    circle.SetScreenSize(5.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.5);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);    

    pVVisManager->Draw(circle);
  }
}

void TrackerHit::Print()
{
  std::cout << std::endl;
  std::cout << "Printing TrackerHit:"
      << " entryPos=(" << this->entryPos.x() << "," 
            << this->entryPos.y() << "," << this->entryPos.z() << ")" 
      << " exitPos=(" << this->exitPos.x() << "," << this->exitPos.y() 
            << "," << this->exitPos.z() << ")" 
            << " edep="  << this->edep << std::endl;
}

