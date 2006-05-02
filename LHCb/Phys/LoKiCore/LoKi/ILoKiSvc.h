// $Id: ILoKiSvc.h,v 1.3 2006-05-02 14:29:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_ILOKISVC_H 
#define LOKI_ILOKISVC_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IIncidentListener.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/IReporter.h"
// ============================================================================
// forward declarations 
// ============================================================================
class IParticlePropertySvc ;
class ISvcLocator          ;
class IToolSvc             ;
// ============================================================================

// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi
{
  
  /** @class ILoKi ILoKiSvc.h LoKi/ILoKiSvc.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-01-16
   */
  class ILoKiSvc : 
    virtual public IInterface       , 
    virtual public IService         , 
    virtual public IIncidentListener 
  {
  public:
    
    /** get the pointer to service locator 
     *  @return ponter to Service Locator 
     *  @see ISvcLocator 
     */
    virtual ISvcLocator*          svcLoc  () const = 0 ;
    
    /** get the pointer to Particle Property Service 
     *  @return pointer to Particle Property Service 
     *  @see IParticlePropertySvc 
     */
    virtual IParticlePropertySvc* ppSvc   () const = 0 ;
    
    /** get the pointer to Tool Service 
     *  @return pointer to Tool Service 
     *  @see IToolSvc 
     */
    virtual IToolSvc*             toolSvc () const = 0 ;
   
    /** get "good" error reporter
     *  @return pointer to Good error reporter
     *  @see LoKi::IReporter
     */
    virtual LoKi::IReporter*     reporter() const = 0 ;
    
  public:
    
    /** Retrieve interface ID
     *  mandatory method from IInterface 
     *  @see IInterface 
     *  @see InterfaceID 
     *  @return unique interface identifier 
     */
    static const InterfaceID& interfaceID() ; 
    
  protected:
    
    // virtual protected destructor 
    virtual ~ILoKiSvc() ;
    
  };
  
}; // end of namespace LoKi 

// ============================================================================
//  The END 
// ============================================================================
#endif // LOKI_ILOKISVC_H
// ============================================================================
