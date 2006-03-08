#ifndef TrackerHit_h
#define TrackerHit_h 1

/** @class TrackerHit TrackerHit.h component/TrackerHit.h
 *  
 *
 *  @author Witold Pokorski
 *  @date   16/05/2002
 */


#include "GaussTools/GaussHitBase.h"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class TrackerHit : public GaussHitBase
{
public:

  TrackerHit();
  virtual ~TrackerHit();
  TrackerHit(const TrackerHit &right);
  const TrackerHit& operator=(const TrackerHit &right);
  int operator==(const TrackerHit &right) const;

  void *operator new(size_t);
  void operator delete(void *aHit);

  void Draw();
  void Print();

  private:
  G4double edep;
  G4ThreeVector entryPos;
  G4ThreeVector exitPos;
  G4double timeOfFlight;
  G4double momentum;

public:

  void SetEdep(G4double de);
  G4double GetEdep();
  void SetEntryPos(G4ThreeVector xyz);
  G4ThreeVector GetEntryPos(); 
  void SetExitPos(G4ThreeVector xyz);
  G4ThreeVector GetExitPos();
  void SetTimeOfFlight(G4double tof);
  G4double GetTimeOfFlight();
  void SetMomentum(G4double momentum);
  G4double GetMomentum();
};

typedef G4THitsCollection<TrackerHit> TrackerHitsCollection;

extern G4Allocator<TrackerHit> TrackerHitAllocator;

// ============================================================================
/** @fn  trackerHits
 *  Fast cast of G4VHitsCollection interface to concrete Gauss implementation
 *  @param  g4   pointer to G4VHitsCollection interface 
 *  @return cast (dynamic or static) to TrackerHitsColelction*
 */
// ============================================================================
inline TrackerHitsCollection* trackerHits( G4VHitsCollection* g4 )
{
  GiGaUtil::FastCast<G4VHitsCollection, TrackerHitsCollection> cast ;
  return cast( g4 );  
};

inline TrackerHit* trackerHit( G4VHit* g4 )
{
  GiGaUtil::FastCast<G4VHit,TrackerHit> cast ;
  return cast( g4 );  
};
// ============================================================================



#endif


