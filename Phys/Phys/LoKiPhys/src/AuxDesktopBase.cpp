// $Id: AuxDesktopBase.cpp,v 1.1 2008-01-25 14:42:22 ibelyaev Exp $
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



// ============================================================================
// The END 
// ============================================================================
