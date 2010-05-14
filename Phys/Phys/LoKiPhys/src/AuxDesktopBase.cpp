// $Id: AuxDesktopBase.cpp,v 1.3 2010-05-14 07:48:30 jpalac Exp $
// ============================================================================
// Include files 
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IPhysDesktop.h"
#include "Kernel/Particle2Vertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GetPhysDesktop.h"
#include "LoKi/AuxDesktopBase.h"
// ============================================================================
/** @file
 *  The Implementation file for class LoKi::AuxDesktopBase
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2008-01-16
 */
// ============================================================================
// the default constructor (invalid desktop!)
// ============================================================================
LoKi::AuxDesktopBase::AuxDesktopBase ()
  : LoKi::AuxFunBase () 
  , m_desktop ( (IPhysDesktop*) 0 ) 
{}
// ============================================================================
// constructor from the desktop 
// ============================================================================
LoKi::AuxDesktopBase::AuxDesktopBase 
( const IPhysDesktop* desktop ) 
  : LoKi::AuxFunBase () 
  , m_desktop ( desktop ) 
{}
// ============================================================================
// constructor from the desktop
// ============================================================================
LoKi::AuxDesktopBase::AuxDesktopBase 
( const LoKi::Interface<IPhysDesktop>& desktop ) 
  : LoKi::AuxFunBase () 
  , m_desktop ( desktop ) 
{}
// =============================================================================
// copy constrictor 
// =============================================================================
LoKi::AuxDesktopBase::AuxDesktopBase 
( const LoKi::AuxDesktopBase& right ) 
  : LoKi::AuxFunBase ( right ) 
  , m_desktop        ( right.m_desktop ) 
{}
// =============================================================================
// destructor
// =============================================================================
LoKi::AuxDesktopBase::~AuxDesktopBase() {}
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
const LHCb::VertexBase* LoKi::AuxDesktopBase::relatedVertex 
( const LHCb::Particle* p ) const 
{
  if ( 0 == p ) 
  {
    Warning ( "relatedVertex: LHCb::Particle* points to NULL, return NULL") ;
    return 0 ;
  }
  //
  if ( !validDesktop() ) { loadDesktop() ; }
  Assert ( validDesktop() , "Unable to retrieve PhysDekstop!" ) ;
  //
  const LHCb::VertexBase* vertex = desktop()->relatedVertex( p ) ;
  if ( 0 == vertex ) 
  { Warning ( "relatedVertex: desktop()->relatedVertex() returns NULL!") ; }
  //
  return vertex ;
}
// =============================================================================
// get all primary vertices 
// =============================================================================
const LHCb::RecVertex::Range LoKi::AuxDesktopBase::primaryVertices() const 
{
  //
  if ( !validDesktop() ) { loadDesktop() ; }
  Assert ( validDesktop() , "Unable to retrieve PhysDekstop!" ) ;
  //
  const LHCb::RecVertex::Range pvs = m_desktop->primaryVertices() ;
  //  if ( 0 == pvs ) 
  //  { Warning ( "primaryVertices: desktop()->primaryVertices() returns NULL!") ; }
  //
  return pvs ;
}


 
// ============================================================================
// The END 
// ============================================================================
