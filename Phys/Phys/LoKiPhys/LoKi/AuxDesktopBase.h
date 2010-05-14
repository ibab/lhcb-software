// $Id: AuxDesktopBase.h,v 1.4 2010-05-14 15:28:33 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_AUXDESKTOPBASE_H 
#define LOKI_AUXDESKTOPBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IDVAlgorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AuxFunBase.h"
#include "LoKi/Interface.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class AuxDesktopBase LoKi/AuxDesktopBase.h
   *  The helper class to implement many "context-dependent" functors   
   *  @author Vanya BELYAEV Ivan.Belyaev@physics.syr.edu
   *  @date   2008-01-16
   */  
  class AuxDesktopBase : public virtual LoKi::AuxFunBase 
  { 
  public:
    // ========================================================================
    /// default constructor (invalid desktop!)
    AuxDesktopBase ( ) ;
    /// constructor form the desktop 
    AuxDesktopBase ( const IDVAlgorithm* desktop  ) ;
    /// constructor from the desktop
    AuxDesktopBase ( const LoKi::Interface<IDVAlgorithm>& desktop ) ;
    /// copy constrictor 
    AuxDesktopBase ( const AuxDesktopBase& right ) ;
    /// destructor
    virtual ~AuxDesktopBase() ;
    // ========================================================================
  public:
    // ========================================================================
    /// get the desktop 
    const LoKi::Interface<IDVAlgorithm>& desktop () const 
    {
      if ( !validDesktop() ) { loadDesktop()  ; }
      return m_desktop ;  
    }
    /// get the desktop
    IDVAlgorithm* getDesktop() const {  return desktop() ; }
    // ========================================================================
  public:
    // ========================================================================
    /// set the desktop
    void setDesktop ( const IDVAlgorithm*                  d ) { m_desktop = d ; }
    /// set the desktop
    void setDesktop ( const LoKi::Interface<IDVAlgorithm>& d ) { m_desktop = d ; }
    // ========================================================================
  public:
    // ========================================================================
    /// check the desktop
    bool validDesktop() const { return m_desktop.validPointer() ; }
    // ========================================================================
  public:
    // ========================================================================
    /// load the desktop
    void loadDesktop() const ;
    // ========================================================================
  public:
    // ========================================================================
    // cast to desktop 
    operator const LoKi::Interface<IDVAlgorithm>& () const { return desktop() ; }
    // ========================================================================
  public:
    // ========================================================================
    /// get "the best related vertex" 
    const LHCb::VertexBase* bestVertex ( const LHCb::Particle* p ) const ;
    /// get all primary vertices 
    LHCb::RecVertex::Range  primaryVertices() const ;
    // ========================================================================
  private:
    // ========================================================================
    /// the desktop itself 
    mutable LoKi::Interface<IDVAlgorithm>    m_desktop ; // the desktop itself 
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_AUXDESKTOPBASE_H
// ============================================================================
