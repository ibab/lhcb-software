// $Id$
// ============================================================================
// Include files 
// ============================================================================
// DaVinciInterface 
// ============================================================================
#include "Kernel/IDVAlgorithm.h"
#include "Kernel/GetIDVAlgorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AuxDesktopBase.h"
#include "LoKi/GetPhysDesktop.h"
// ============================================================================
/** @file
 *  The Implementation file for class LoKi::AuxDesktopBase
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2008-01-16
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /// null pointer 
  const IDVAlgorithm* const s_IDVAlgorithm = 0 ;
  // ==========================================================================
}
// ============================================================================
// the default constructor (invalid desktop!)
// ============================================================================
LoKi::AuxDesktopBase::AuxDesktopBase ()
  : LoKi::AuxFunBase (                ) 
  , m_desktop        ( s_IDVAlgorithm ) 
{
  if ( !m_desktop && gaudi() ) {  loadDesktop() ; }
}
// ============================================================================
// constructor from the desktop 
// ============================================================================
LoKi::AuxDesktopBase::AuxDesktopBase 
( const IDVAlgorithm* desktop ) 
  : LoKi::AuxFunBase () 
  , m_desktop ( desktop ) 
{
  if ( !m_desktop && gaudi() ) { loadDesktop() ; }
}
// ============================================================================
// constructor from the desktop
// ============================================================================
LoKi::AuxDesktopBase::AuxDesktopBase 
( const LoKi::Interface<IDVAlgorithm>& desktop ) 
  : LoKi::AuxFunBase (         ) 
  , m_desktop        ( desktop )
{
  if (  !m_desktop && gaudi() ) { loadDesktop() ; }
}
// =============================================================================
// copy constructor 
// =============================================================================
LoKi::AuxDesktopBase::AuxDesktopBase 
( const LoKi::AuxDesktopBase& right ) 
  : LoKi::AuxFunBase ( right ) 
  , m_desktop        (       ) // ATTENTION - it is not a right copy!!! 
{
  if (  !m_desktop && gaudi() ) { loadDesktop() ; }
}
// =============================================================================
// destructor
// =============================================================================
LoKi::AuxDesktopBase::~AuxDesktopBase () 
{
  if ( m_desktop && !gaudi() ) 
  {
    // Warning("IPhysDesktop:        manual reset") ;
    m_desktop.reset() ; 
  }
}
// =============================================================================
// load the desktop
// =============================================================================
void LoKi::AuxDesktopBase::loadDesktop() const 
{ 
  m_desktop = LoKi::getPhysDesktop ( lokiSvc() ) ;
  if ( !validDesktop() ) 
  { Error ( "loadDesktop(): unable to load IPhysDesktop!" ) ; }    
}
// =============================================================================
// get "the best related vertex" 
// =============================================================================
const LHCb::VertexBase* LoKi::AuxDesktopBase::bestVertex 
( const LHCb::Particle* p ) const 
{
  if ( 0 == p ) 
  {
    Warning ( "relatedVertex: LHCb::Particle* points to NULL, return NULL") ;
    return 0 ;
  }
  //
  if ( !validDesktop() ) 
  { 
    Warning ( "Late acquire of IDVAlgorithm" ) ;
    loadDesktop() ; 
  }
  Assert ( validDesktop() , "Unable to retrieve IDVAlgorithm" ) ;
  //
  const LHCb::VertexBase* vertex = desktop()->bestVertex( p ) ;
  if ( 0 == vertex ) 
  { Warning ( "bestVertex: IDVAlgorithm::bestVertex() returns NULL!") ; }
  //
  return vertex ;
}
// =============================================================================
// get all primary vertices 
// =============================================================================
LHCb::RecVertex::Range LoKi::AuxDesktopBase::primaryVertices() const 
{
  //
  if ( !validDesktop() ) 
  { 
    Warning ( "Late acquire of IDVAlgorithm" ) ;
    loadDesktop() ; 
  }
  Assert ( validDesktop() , "Unable to retrieve PhysDekstop!" ) ;
  //
  LHCb::RecVertex::Range pvs = m_desktop->primaryVertices() ;
  if ( pvs.empty() ) 
  { Warning ( "primaryVertices: IDVAlgorithm::primaryVertices() empty ") ; }
  //
  return pvs ;
}


 
// ============================================================================
// The END 
// ============================================================================
