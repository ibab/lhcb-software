//-----------------------------------------------------------------------------
// Implementation file for class : TrackerHit
//
// 16/05/2002 : Witold POKORSKI
// 25/07/2006 : Gloria CORTI
//-----------------------------------------------------------------------------

// Include files 
// from G4
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

// local
#include "TrackerHit.h"

G4Allocator<TrackerHit> TrackerHitAllocator;

//=============================================================================
// Default Constructor
//=============================================================================
TrackerHit::TrackerHit() { }

//=============================================================================
// Destructor
//=============================================================================
TrackerHit::~TrackerHit() {}

//=============================================================================
// Constructor
//=============================================================================
TrackerHit::TrackerHit(const TrackerHit &right)
  : GaussHitBase(right)
{
  m_edep = right.m_edep;
  m_entryPos = right.m_entryPos;
  m_exitPos = right.m_exitPos;
  m_timeOfFlight = right.m_timeOfFlight;
  m_momentum = right.m_momentum;
}

//=============================================================================
// new operator (fast G4 allocator)
//=============================================================================
void* TrackerHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) TrackerHitAllocator.MallocSingle();
  return aHit;
}

//=============================================================================
// delete operator (fast G4 allocator)
//=============================================================================
void TrackerHit::operator delete(void *aHit)
{
  TrackerHitAllocator.FreeSingle((TrackerHit*) aHit);
}

//=============================================================================
// assignmnet operator
//=============================================================================
const TrackerHit& TrackerHit::operator=(const TrackerHit &right)
{
  m_edep = right.m_edep;
  m_entryPos = right.m_entryPos;
  m_exitPos = right.m_exitPos;
  m_timeOfFlight = right.m_timeOfFlight;
  m_momentum = right.m_momentum;
  return *this;
}

//=============================================================================
// equality operator
//=============================================================================
int TrackerHit::operator==(const TrackerHit& /*right*/ ) const
{
  return 0;
}

//=============================================================================
// Accessors for energy deposition
//=============================================================================
void TrackerHit::SetEdep(G4double de) { 
  m_edep = de; 
}

G4double TrackerHit::GetEdep() { 
  return m_edep; 
}

//=============================================================================
// Accessors for entry point
//=============================================================================
void TrackerHit::SetEntryPos(G4ThreeVector xyz) { 
  m_entryPos = xyz; 
}

G4ThreeVector TrackerHit::GetEntryPos() {
  return m_entryPos; 
}

//=============================================================================
// Accessors for exit point
//=============================================================================
void TrackerHit::SetExitPos(G4ThreeVector xyz) { 
  m_exitPos = xyz; 
}

G4ThreeVector TrackerHit::GetExitPos() { 
  return m_exitPos; 
}

//=============================================================================
// Accessors for time of flight at entry point
//=============================================================================
void TrackerHit::SetTimeOfFlight(G4double tof) { 
  m_timeOfFlight = tof; 
}

G4double TrackerHit::GetTimeOfFlight() { 
  return m_timeOfFlight;
}

//=============================================================================
// Accessors for momentum at entry point
//=============================================================================
void TrackerHit::SetMomentum(G4ThreeVector p) {
  m_momentum = p; 
}
 
G4ThreeVector TrackerHit::GetMomentum() {
  return m_momentum; 
} 

//=============================================================================
// Draw 
//=============================================================================
void TrackerHit::Draw() {

  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if ( pVVisManager ) {
    G4Circle circle( (m_exitPos + m_entryPos)*0.5 );
    circle.SetScreenSize( 5. );
    circle.SetFillStyle( G4Circle::filled );
    G4Colour colour( 1., 0., 0.5 );
    G4VisAttributes attribs( colour );
    circle.SetVisAttributes( attribs );    

    pVVisManager->Draw( circle );
  }
}

//=============================================================================
// Print 
//=============================================================================
void TrackerHit::Print() {
  std::cout << std::endl;
  std::cout << "Printing TrackerHit:"
            << " entryPos=(" << this->m_entryPos.x() << "," 
            << this->m_entryPos.y() << "," << this->m_entryPos.z() << ")" 
            << " displacement=(" << this->m_exitPos.x() << ","
            << this->m_exitPos.y() << "," << this->m_exitPos.z() << ")"
            << " tof=" << this->m_timeOfFlight
            << " edep=" << this->m_edep 
            << " momentum=(" << this->m_momentum.x() << ","
            << this->m_momentum.y() << "," << this->m_momentum.z() << ")"
            << std::endl;
}

//=============================================================================
