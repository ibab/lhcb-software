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
class ISvcLocator          ;
class IToolSvc             ;
class IAlgContextSvc       ;
class IIncidentSvc         ;
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
  // ==========================================================================
  /** @class ILoKi ILoKiSvc.h LoKi/ILoKiSvc.h
   *  The major LOKi object
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-01-16
   */
  class ILoKiSvc : virtual public IService         
  {
  public:    
    // ========================================================================
    /** get "good" error reporter
     *  @return pointer to Good error reporter
     *  @see LoKi::IReporter
     */
    virtual LoKi::IReporter*     reporter    () const = 0 ;    
    // ========================================================================
    /** The the sequential event number 
     *  (needed for the proper synchronizations checks) 
     *  @return the sequential event number 
     */
    virtual unsigned long long   event       () const = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    /** Retrieve interface ID
     *  mandatory method from IInterface 
     *  @see IInterface 
     *  @see InterfaceID 
     *  @return unique interface identifier 
     */
    static const InterfaceID& interfaceID() ;
    // ========================================================================
  protected:
    // ========================================================================
    // virtual protected destructor 
    virtual ~ILoKiSvc() ;
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
//  The END 
// ============================================================================
#endif // LOKI_ILOKISVC_H
// ============================================================================
