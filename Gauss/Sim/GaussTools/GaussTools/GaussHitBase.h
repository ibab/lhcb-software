// $Id: GaussHitBase.h,v 1.3 2002-12-07 21:19:13 ibelyaev Exp $
#ifndef GAUSSTOOLS_GAUSSHITBASE_H 
#define GAUSSTOOLS_GAUSSHITBASE_H 1
// GiGa 
#include "GiGa/GiGaUtil.h"
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

// ============================================================================
/** @fn  gaussHit
 *  Fast cast of G4VHit interface to concrete Gauss implementation
 *  @param  g4   pointer to G4VHit interface 
 *  @return cast (dynamic or static) to GaussHitBase
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline GaussHitBase* gaussHit( G4VHit* g4 )
{
  GiGaUtil::FastCast<G4VHit,GaussHitBase> cast ;
  return cast( g4 );  
}
// ============================================================================



#endif // GAUSSTOOLS_GAUSSHITBASE_H
