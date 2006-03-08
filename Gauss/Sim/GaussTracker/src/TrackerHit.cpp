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
#include "TrackerHit.h"

G4Allocator<TrackerHit> TrackerHitAllocator;

TrackerHit::TrackerHit()
{;}

TrackerHit::~TrackerHit()
{;}

TrackerHit::TrackerHit(const TrackerHit &right):GaussHitBase(right)
{
  edep = right.edep;
  entryPos = right.entryPos;
  exitPos = right.exitPos;
  timeOfFlight = right.timeOfFlight;
  momentum = right.momentum;
}

void* TrackerHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) TrackerHitAllocator.MallocSingle();
  return aHit;
}

void TrackerHit::operator delete(void *aHit)
{
  TrackerHitAllocator.FreeSingle((TrackerHit*) aHit);
}

const TrackerHit& TrackerHit::operator=(const TrackerHit &right)
{
  edep = right.edep;
  entryPos = right.entryPos;
  exitPos = right.exitPos;
  timeOfFlight = right.timeOfFlight;
  momentum = right.momentum;
  return *this;
}

int TrackerHit::operator==(const TrackerHit &right) const
{
  return 0;
}
  void TrackerHit::SetEdep(G4double de)
  { edep = de; }

  G4double TrackerHit::GetEdep()
  { return edep; }

  void TrackerHit::SetEntryPos(G4ThreeVector xyz)
  { entryPos = xyz; }

  G4ThreeVector TrackerHit::GetEntryPos()
  { return entryPos; }

  void TrackerHit::SetExitPos(G4ThreeVector xyz)
  { exitPos = xyz; }

  G4ThreeVector TrackerHit::GetExitPos()
  { return exitPos; }

  void TrackerHit::SetTimeOfFlight(G4double tof)
  { timeOfFlight = tof; }

  G4double TrackerHit::GetTimeOfFlight()
  { return timeOfFlight; }

  void TrackerHit::SetMomentum(G4double p)
  { momentum = p; }
 
  G4double TrackerHit::GetMomentum()
  { return momentum; } 


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

