// $Id: MuonHit.h,v 1.2 2002-10-09 16:03:09 witoldp Exp $
#ifndef TRACKING_MuonHit_H 
#define TRACKING_MuonHit_H 1

// Include files
#include "TrackerHit.h"

/** @class MuonHit MuonHit.h Tracking/MuonHit.h
 *  
 *
 *  @author Witold Pokorski
 *  @date   2002-08-16
 */
class MuonHit : public TrackerHit {
public:
  /// Standard constructor
  MuonHit();
  
  ~MuonHit( ); ///< Destructor

  void *operator new(size_t);
  void operator delete(void *aHit);

  /// Retrieve Chamber Number
  G4int ChamberID() const; 

  /// Retrieve Gap Number
  G4int GapID() const; 

  /// Update Chamber Number
  void SetChamberID(G4int value);

  /// Update Gap Number
  void SetGapID(G4int value);


protected:

private:

  G4int m_chamberID; /// chamberID 
  G4int m_gapID;
  
};

inline G4int MuonHit::ChamberID() const 
{
  return m_chamberID;
}

inline void MuonHit::SetChamberID(G4int value)
{
  m_chamberID = value; 
}

inline G4int MuonHit::GapID() const 
{
  return m_gapID;
}

inline void MuonHit::SetGapID(G4int value)
{
  m_gapID = value; 
}

typedef G4THitsCollection<MuonHit> MuonHitsCollection;


#endif // TRACKING_MuonHit_H
