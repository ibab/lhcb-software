// $Id: CaloSubHit.cpp,v 1.4 2006-01-17 15:52:57 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/01/14 13:38:10  ranjard
// v6r0 - fix to be used with Gaudi v14r0
//
// Revision 1.2  2002/12/13 16:52:57  ibelyaev
//  put updated versions of the packages
//
// Revision 1.1  2002/12/07 14:41:44  ibelyaev
//  add new Calo stuff
//
// ============================================================================
// Include files
// GiGa
#include "GiGa/GiGaUtil.h"
// Geant4 
#include "G4Allocator.hh"
#include "G4VHit.hh"
// G4 
// local
#include "CaloSubHit.h"

// ============================================================================
/** @file 
 *  Implementation file for all non-inlined methods of class CaloSubHit
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-03
 */
// ============================================================================

namespace CaloSubHitLocal
{
  // ==========================================================================
  /** @var  s_Allocator
   *  allocator to make more efficient creation delete of CaloSubHits objects  
   */
  // ==========================================================================
  G4Allocator<CaloSubHit>            s_Allocator ;
  // ==========================================================================
  
  // ==========================================================================
  /** @var s_Counter 
   *  statsic instace counter for all functions 
   */
  // ==========================================================================
#ifdef GIGA_DEBUG
  static GiGaUtil::InstanceCounter<CaloSubHit> s_Counter   ;
#endif 
  // ==========================================================================
}



// ============================================================================
/** Standard constructor
 *  @param cellID  cellID  of the detector cell  
 *  @param trackID trackID of the particle
 */
// ============================================================================
CaloSubHit::CaloSubHit( const LHCb::CaloCellID& cellID  , 
                        const int         trackID )
  : GaussHitBase (        )  
  , m_cellID     ( cellID ) 
  , m_map        (        ) 
{ 
  setTrackID( trackID ) ;
#ifdef GIGA_DEBUG
  CaloSubHitLocal::s_Counter.increment () ;
#endif 
}


// ============================================================================
/** copy constructor 
 *  @param right object to be copied 
 */
// ============================================================================
CaloSubHit::CaloSubHit( const CaloSubHit& right ) 
  : GaussHitBase( right    ) 
  , m_cellID ( right.cellID() ) 
  , m_map    ( right.m_map    ) 
{
#ifdef GIGA_DEBUG
  CaloSubHitLocal::s_Counter.increment () ;
#endif 
}


/// destructor 
// ============================================================================
CaloSubHit::~CaloSubHit() 
{ 
  m_map.clear() ; 
#ifdef GIGA_DEBUG
  CaloSubHitLocal::s_Counter.decrement () ;
#endif 
}


// ============================================================================
/// Clone method 
// ============================================================================
CaloSubHit* CaloSubHit::clone() const 
{ 
  return new CaloSubHit( *this ); 
}


// ============================================================================
/// overloaded 'new' operator 
// ============================================================================
void* CaloSubHit::operator new(size_t)
{
  void *hit  ;
  hit = (void *) CaloSubHitLocal::s_Allocator.MallocSingle () ;
  return hit ;
}


// ============================================================================
/// overloaded 'delete' operator 
// ============================================================================
void CaloSubHit::operator delete( void *hit )
{ 
  CaloSubHitLocal::s_Allocator.FreeSingle( (CaloSubHit*) hit ); 
}


// ============================================================================
// The END 
// ============================================================================
