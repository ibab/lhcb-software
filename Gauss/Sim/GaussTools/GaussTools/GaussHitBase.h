// $Id: GaussHitBase.h,v 1.1 2002-10-29 17:34:57 witoldp Exp $
#ifndef GAUSSTOOLS_GAUSSHITBASE_H 
#define GAUSSTOOLS_GAUSSHITBASE_H 1

// Include files
#include "G4VHit.hh"

/** @class GaussHitBase GaussHitBase.h GaussTools/GaussHitBase.h
 *  
 *
 *  @author Witold Pokorski
 *  @date   2002-10-29
 */

class GaussHitBase : public G4VHit 
{
public:
  /// Standard constructor
  inline GaussHitBase( ); 

  virtual inline ~GaussHitBase( ); ///< Destructor

  inline void SetTrackID(G4int tid);
  inline G4int GetTrackID();

private:
  G4int m_trackID;
};

#include "GaussTools/GaussHitBase.icpp"  

#endif // GAUSSTOOLS_GAUSSHITBASE_H
