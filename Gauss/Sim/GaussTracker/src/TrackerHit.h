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

public:

  void SetEdep(G4double de);
  G4double GetEdep();
  void SetEntryPos(G4ThreeVector xyz);
  G4ThreeVector GetEntryPos(); 
  void SetExitPos(G4ThreeVector xyz);
  G4ThreeVector GetExitPos();
  void SetTimeOfFlight(G4double tof);
  G4double GetTimeOfFlight();
};

typedef G4THitsCollection<TrackerHit> TrackerHitsCollection;

extern G4Allocator<TrackerHit> TrackerHitAllocator;


#endif


