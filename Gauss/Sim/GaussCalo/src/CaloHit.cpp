// $Id: CaloHit.cpp,v 1.4 2006-01-17 15:52:57 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/01/14 13:38:10  ranjard
// v6r0 - fix to be used with Gaudi v14r0
//
// Revision 1.2  2002/12/13 16:52:56  ibelyaev
//  put updated versions of the packages
//
// Revision 1.1  2002/12/07 14:41:44  ibelyaev
//  add new Calo stuff
// 
// ============================================================================
// Include files
// GiGa
#include "GiGa/GiGaUtil.h"
// G4 
#include "G4Allocator.hh"
// local
#include "CaloHit.h"

// ============================================================================
/** @file 
 *  Implementation file for class  CaloHit
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-03
 */
// ============================================================================

namespace CaloHitLocal
{
  // ==========================================================================
  /** @var  s_Allocator
   *  allocator to make more efficient creation delete of CaloHits objects  
   */
  // ==========================================================================
  G4Allocator<CaloHit>            s_Allocator ;
  // ==========================================================================
  
  // ==========================================================================
  /** @var s_Counter 
   *  statsic instace counter for all functions 
   */
  // ==========================================================================
#ifdef GIGA_DEBUG
  static GiGaUtil::InstanceCounter<CaloHit> s_Counter   ;
#endif 
  // ==========================================================================
}


// ============================================================================
/** standard  constructor 
 *  @param cell ID of calorimeter cell 
 */
// ============================================================================
CaloHit::CaloHit( const LHCb::CaloCellID& cell ) 
  : m_cellID ( cell ) 
  , m_map    (      ) 
{
#ifdef GIGA_DEBUG
  CaloHitLocal::s_Counter.increment () ;
#endif  
}


// ============================================================================
/** copy constructor 
 *  @param right object to be copied 
 */
// ============================================================================
CaloHit::CaloHit( const CaloHit& right )
  : G4VHit   ( right          ) 
  , m_cellID ( right.cellID() )
  , m_map    (                )
{
  for( CaloHit::iterator hit = right.begin() ; right.end() != hit ; ++hit ) 
    { m_map( hit->first ) = hit->second->clone() ; }
#ifdef GIGA_DEBUG
  CaloHitLocal::s_Counter.increment () ;
#endif  
}


// ============================================================================
/// clone menthod  
// ============================================================================
CaloHit* CaloHit::clone() const 
{ 
  return new CaloHit( *this ); 
}


// ============================================================================
/// destructor
// ============================================================================
CaloHit::~CaloHit() 
{
  /// delete all sub hits 
  for( TheMap::iterator ihit = m_map.begin() ; m_map.end() != ihit ; ++ihit ) 
    {
      CaloSubHit* hit = ihit->second ;
      if( 0 != hit ) { delete hit ; ihit->second = 0 ; }
    }
  // clear the map 
  m_map.clear();
  ///
#ifdef GIGA_DEBUG
  CaloHitLocal::s_Counter.decrement () ;
#endif
}


// ============================================================================
/// overloaded 'new' oerator 
// ============================================================================
void* CaloHit::operator new(size_t)
{
  void *hit  ;
  hit = (void *) CaloHitLocal::s_Allocator.MallocSingle () ;
  return hit ;
}


// ============================================================================
/// overloaded 'delete' oerator 
// ============================================================================
void CaloHit::operator delete( void *hit )
{ 
  CaloHitLocal::s_Allocator.FreeSingle( (CaloHit*) hit ); 
}


// ============================================================================
// The End 
// ============================================================================
