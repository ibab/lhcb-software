#ifndef TrackerHit_h
#define TrackerHit_h 1

/** @class TrackerHit TrackerHit.h component/TrackerHit.h
 *  
 *
 *  @author Witold Pokorski
 *  @date   16/05/2002
 */


#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class TrackerHit : public G4VHit
{
  public:

      TrackerHit();
      ~TrackerHit();
      TrackerHit(const TrackerHit &right);
      const TrackerHit& operator=(const TrackerHit &right);
      int operator==(const TrackerHit &right) const;

        inline void *operator new(size_t);
        inline void operator delete(void *aHit);

      void Draw();
      void Print();

  private:
      G4double edep;
      G4ThreeVector pos;

  public:
      inline void SetEdep(G4double de)
      { edep = de; }
      inline G4double GetEdep()
      { return edep; }
      inline void SetPos(G4ThreeVector xyz)
      { pos = xyz; }
      inline G4ThreeVector GetPos()
      { return pos; }

};

typedef G4THitsCollection<TrackerHit> TrackerHitsCollection;

extern G4Allocator<TrackerHit> TrackerHitAllocator;

inline void* TrackerHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) TrackerHitAllocator.MallocSingle();
  return aHit;
}

inline void TrackerHit::operator delete(void *aHit)
{
  TrackerHitAllocator.FreeSingle((TrackerHit*) aHit);
}

#endif


