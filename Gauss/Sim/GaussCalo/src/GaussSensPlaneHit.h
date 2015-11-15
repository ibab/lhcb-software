// $Id: GaussSensPlaneHit.h,v 1.3 2006-01-17 15:52:57 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/01/14 13:38:10  ranjard
// v6r0 - fix to be used with Gaudi v14r0
//
// Revision 1.1  2003/07/07 16:09:39  ibelyaev
//  add Calorimeter Sensitive Palne hits and the converter
//
// ============================================================================
#ifndef GAUSSTOOLS_GAUSSSENSITIVEPLANEHIT_H
#define GAUSSTOOLS_GAUSSSENSITIVEPLANEHIT_H 1
// ============================================================================
// Include files
// ============================================================================
// CLHEP
#include "CLHEP/Vector/LorentzVector.h"
// Event
#include "Kernel/ParticleID.h"
// GaussTools
#include "GaussTools/GaussHitBase.h"
// Geant4
#include "G4THitsCollection.hh"
// ============================================================================


/** @class GaussSensPlaneHit GaussSensPlaneHit.h
 *
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-07-07
 */
class GaussSensPlaneHit : public GaussHitBase
{
public :
  /// the actual type of TrackID
  typedef int TrackID ;
public:

  /// overloaded 'new'    operator
  void* operator  new    ( size_t    ) ;

  /// overloaded 'delete' operator
  void  operator  delete ( void *hit ) ;

  /** Standard constructor
   *  @param track   trackID of th eparticle (or its parent particle!)
   *  @param pid     the actual pid of the particle (can be different!)
   *  @param position position(3D+time) of the hit
   *  @param momentum 4-momentum of the particle
   */
  GaussSensPlaneHit
  ( const TrackID&          track    = 0                   ,
    const LHCb::ParticleID&  pid      = LHCb::ParticleID       () ,
    const CLHEP::HepLorentzVector& position = CLHEP::HepLorentzVector () ,
    const CLHEP::HepLorentzVector& momentum = CLHEP::HepLorentzVector () ) ;

  /** copy constructor
   *  @param hit hit to be copied
   */
  GaussSensPlaneHit
  ( const GaussSensPlaneHit& hit ) ;

  /// destructor
  virtual ~GaussSensPlaneHit() ;

  /// access to actual particle ID
  inline const LHCb::ParticleID&       pid         () const { return m_pid      ; }
  // set new value for particle ID
  inline void                    setPID      ( const LHCb::ParticleID& val        )
  { m_pid      = val ; }

  /// accessor to the hit position         (const version)
  inline const CLHEP::HepLorentzVector& position    () const { return m_position ; }
  /// accessor to the hit position     (non-const version)
  inline       CLHEP::HepLorentzVector& position    ()       { return m_position ; }
  /// set the new value for hist position
  inline void                    setPosition ( const CLHEP::HepLorentzVector& val  )
  { m_position = val ; }

  /// accessor to the particle momentum     (const version)
  inline const CLHEP::HepLorentzVector& momentum    () const { return m_momentum ; }
  /// accessor to the particle momentum (non-const version)
  inline       CLHEP::HepLorentzVector& momentum    ()       { return m_momentum ; }
  /// set the new value for particle momentum
  inline void                    setMomentum ( const CLHEP::HepLorentzVector& val  )
  { m_momentum = val ; }


private:

  // the actual particle tye (can be different from track PID)
  LHCb::ParticleID        m_pid      ;
  // the actual hit position (3D + global time)
  CLHEP::HepLorentzVector  m_position ;
  // the particle 4-momentum at hit position
  CLHEP::HepLorentzVector  m_momentum ;
};


/// type for the hit collection
typedef G4THitsCollection<GaussSensPlaneHit>
GaussSensPlaneHitsCollection ;

// ============================================================================
/** @fn  gaussSensPlaneHit
 *  Fast cast of G4VHit interface to concrete Gauss implementation
 *  @param  g4   pointer to G4VHit interface
 *  @return cast (dynamic or static) to GaussSensitivelaneHit
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline GaussSensPlaneHit*
gaussSensPlaneHitHit ( G4VHit* g4 )
{
  GiGaUtil::FastCast<G4VHit,GaussSensPlaneHit> cast ;
  return cast( g4 );
}


// ============================================================================
/** @fn  gaussSensPlaneHit
 *  Fast cast of GaussHitBaseto concrete Gauss implementation
 *  @param  g4   pointer to GaussHitBase interface
 *  @return cast (dynamic or static) to GaussSensPlaneHit
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline GaussSensPlaneHit*
gaussSensPlaneHitHit ( GaussHitBase* g4 )
{
  GiGaUtil::FastCast<GaussHitBase,GaussSensPlaneHit> cast ;
  return cast( g4 );
}


// ============================================================================
/** @fn  caloHits
 *  Fast cast of G4VHitsCollection interface to concrete Gauss implementation
 *  @param  g4   pointer to G4VHitsCollection interface
 *  @return cast (dynamic or static) to CaloHitsColelction*
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline GaussSensPlaneHitsCollection*
gaussSensPlaneHits ( G4VHitsCollection* g4 )
{
  GiGaUtil::FastCast<G4VHitsCollection,GaussSensPlaneHitsCollection> cast ;
  return cast( g4 );
}


// ============================================================================
// The END
// ============================================================================
#endif // GAUSSTOOLS_GAUSSSENSITIVEPLANEHIT_H
// ============================================================================
