// $Id: GaussHitBase.h,v 1.2 2002-12-04 14:42:55 ibelyaev Exp $
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
  
  /// Accessor in "Gaudi-style"  
  inline G4int trackID    () const           { return m_trackID      ; }
  /// Accessor in "Gaudi-style"  
  inline void  setTrackID ( const G4int id ) {        m_trackID = id ; }
  
private:
  G4int m_trackID;
};

#include "GaussTools/GaussHitBase.icpp"  

#endif // GAUSSTOOLS_GAUSSHITBASE_H
