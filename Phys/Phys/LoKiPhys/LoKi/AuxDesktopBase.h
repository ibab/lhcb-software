// $Id: AuxDesktopBase.h,v 1.1 2008-01-25 14:42:22 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_AUXDESKTOPBASE_H 
#define LOKI_AUXDESKTOPBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IPhysDesktop.h"
#include "Kernel/IGeomDispCalculator.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AuxFunBase.h"
#include "LoKi/Interface.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class AuxDesktopBase AuxDesktopBase.h LoKi/AuxDesktopBase.h
   *  The helper class to implement many "context-dependent" functors   
   *  @author Vanya BELYAEV Ivan.Belyaev@physics.syr.edu
   *  @date   2008-01-16
   */  
  class AuxDesktopBase : public virtual LoKi::AuxFunBase 
  { 
  public:
    /// default constructor (invalid desktop!)
    AuxDesktopBase ( ) ;
    /// constructor form the desktop 
    AuxDesktopBase ( const IPhysDesktop* desktop  ) ;
    /// constructor from the desktop
    AuxDesktopBase ( const LoKi::Interface<IPhysDesktop>& desktop ) ;
    /// copy constrictor 
    AuxDesktopBase ( const AuxDesktopBase& right ) ;
    /// destructor
    virtual ~AuxDesktopBase() ;
  public:
    /// get the desktop 
    const LoKi::Interface<IPhysDesktop>& desktop () const 
    {
      if ( !validDesktop() ) { loadDesktop()  ; }
      return m_desktop ;  
    }
    /// get the desktop
    IPhysDesktop* getDesktop() const {  return desktop() ; }
  public:
    /// set the desktop
    void setDesktop ( const IPhysDesktop*                  d ) { m_desktop = d ; }
    /// set the desktop
    void setDesktop ( const LoKi::Interface<IPhysDesktop>& d ) { m_desktop = d ; }
  public:
    /// check the desktop
    bool validDesktop() const { return m_desktop.validPointer() ; }
  public:
    /// load the desktop
    void loadDesktop() const ;
  public:
    // cast to desktop 
    operator const LoKi::Interface<IPhysDesktop>& () const { return desktop() ; }
  private:
    // the desktop itself 
    mutable LoKi::Interface<IPhysDesktop>    m_desktop ; ///< the desktop itself 
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_AUXDESKTOPBASE_H
// ============================================================================
