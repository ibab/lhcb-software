// $Id: CaloHit.cpp,v 1.1 2002-12-07 14:41:44 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
};
// ============================================================================

// ============================================================================
/** standard  constructor 
 *  @param cell ID of calorimeter cell 
 */
// ============================================================================
CaloHit::CaloHit( const CaloCellID& cell ) 
  : m_cellID ( cell ) 
  , m_map    (      ) 
{
#ifdef GIGA_DEBUG
  CaloHitLocal::s_Counter.increment () ;
#endif  
} ; 
// ============================================================================

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
};
// ============================================================================

// ============================================================================
/// clone menthod  
// ============================================================================
CaloHit* CaloHit::clone() const { return new CaloHit( *this ); };
// ============================================================================

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
};
// ============================================================================

// ============================================================================
/// overloaded 'new' oerator 
// ============================================================================
void* CaloHit::operator new(size_t)
{
  void *hit  ;
  hit = (void *) CaloHitLocal::s_Allocator.MallocSingle () ;
  return hit ;
};
// ============================================================================

// ============================================================================
/// overloaded 'delete' oerator 
// ============================================================================
void CaloHit::operator delete( void *hit )
{ CaloHitLocal::s_Allocator.FreeSingle( (CaloHit*) hit ); };
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
