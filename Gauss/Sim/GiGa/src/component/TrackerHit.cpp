//-----------------------------------------------------------------------------
// Implementation file for class : TrackerHit
//
// 16/05/2002 : Witold Pokorski
//-----------------------------------------------------------------------------

#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4Text.hh"

// local
#include "TrackerHit.h"

G4Allocator<TrackerHit> TrackerHitAllocator;

TrackerHit::TrackerHit()
{;}

TrackerHit::~TrackerHit()
{;}

TrackerHit::TrackerHit(const TrackerHit &right)
{
  edep = right.edep;
  pos = right.pos;
}

const TrackerHit& TrackerHit::operator=(const TrackerHit &right)
{
  edep = right.edep;
  pos = right.pos;
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
    G4Circle circle(pos);
    circle.SetScreenSize(4.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.5);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);    

    pVVisManager->Draw(circle);
  }
}

void TrackerHit::Print()
{std::cout << "Printing TrackerHit " << std::endl;}
