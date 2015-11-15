// $Id: GaussSensPlaneHit.cpp,v 1.4 2006-01-17 15:52:57 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/12/14 14:53:18  gcorti
// fix for compilation
//
// Revision 1.2  2004/01/14 13:38:10  ranjard
// v6r0 - fix to be used with Gaudi v14r0
//
// Revision 1.1  2003/07/07 16:09:39  ibelyaev
//  add Calorimeter Sensitive Palne hits and the converter
//
// ============================================================================
// Include files
// ============================================================================
// from GiGa
#include "GiGa/GiGaUtil.h"
// Geant4
#include "G4Allocator.hh"
// local
#include "GaussSensPlaneHit.h"
// ============================================================================

using CLHEP::HepLorentzVector;

// ============================================================================
/** @file
 *
 *  Implementation file for class : GaussSensPlaneHit
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-07-07
 */
// ============================================================================

namespace GaussSensPlaneHitLocal
{
  // ==========================================================================
  /** @var  s_Allocator
   *  allocator to make more efficient creation delete
   *  of GaussSensPlaneHits objects
   */
  // ==========================================================================
  G4Allocator<GaussSensPlaneHit>            s_Allocator ;
  // ==========================================================================

  // ==========================================================================
  /** @var s_Counter
   *  statsic instace counter for all functions
   */
  // ==========================================================================
#ifdef GIGA_DEBUG
  static GiGaUtil::InstanceCounter<GaussSensPlaneHit> s_Counter   ;
#endif
  // ==========================================================================
}


// ============================================================================
/** Standard constructor
 *  @param track   trackID of th eparticle (or its parent particle!)
 *  @param position position(3D+time) of the hit
 *  @param momentum 4-momentum of the particle
 */
// ============================================================================
GaussSensPlaneHit::GaussSensPlaneHit
( const TrackID&          track    ,
  const LHCb::ParticleID&       pid      ,
  const HepLorentzVector& position ,
  const HepLorentzVector& momentum )
  : GaussHitBase ()
  , m_pid        ( pid      )
  , m_position   ( position )
  , m_momentum   ( momentum )
{
  setTrackID ( track ) ;
  // ==========================================================================
#ifdef GIGA_DEBUG
  GaussSensPlaneHitLocal::s_Counter.increment () ;
#endif
  // ==========================================================================
}

// ============================================================================
/// destructor
// ============================================================================
GaussSensPlaneHit::~GaussSensPlaneHit()
{
  // ==========================================================================
#ifdef GIGA_DEBUG
  GaussSensPlaneHitLocal::s_Counter.decrement () ;
#endif
  // ==========================================================================
}

// ============================================================================
/** copy constructor
 *  @param hit hit to be copied
 */
// ============================================================================
GaussSensPlaneHit::GaussSensPlaneHit
( const GaussSensPlaneHit& hit )
  : GaussHitBase ( hit             )
  , m_pid        ( hit.pid      () )
  , m_position   ( hit.position () )
  , m_momentum   ( hit.momentum () )
{
  // ==========================================================================
#ifdef GIGA_DEBUG
  GaussSensPlaneHitLocal::s_Counter.increment () ;
#endif
  // ==========================================================================
}


// ============================================================================
/// overloaded 'new' oerator
// ============================================================================
void* GaussSensPlaneHit::operator new(size_t)
{
  void *hit  ;
  hit = (void *) GaussSensPlaneHitLocal::s_Allocator.MallocSingle () ;
  return hit ;
}


// ============================================================================
/// overloaded 'delete' oerator
// ============================================================================
void GaussSensPlaneHit::operator delete( void *hit )
{ GaussSensPlaneHitLocal::s_Allocator.FreeSingle( (GaussSensPlaneHit*) hit ); }


// ============================================================================
// The END
// ============================================================================

