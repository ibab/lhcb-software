// $Id: Services.h,v 1.9 2008-12-04 14:37:31 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_SERVICES_H 
#define LOKI_SERVICES_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// forward declarations 
// ============================================================================
namespace LHCb { class IParticlePropertySvc ; }
class IAlgContextSvc       ;
class IHistogramSvc        ;
class IDataProviderSvc     ;
class IRndmGenSvc          ;
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
  /** @class Services Services.h LoKi/Services.h
   *  
   *  Static accessor to some Gaudi Services.
   *  Nice ideas from Sebastien Ponce are used. 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-05-02
   */
  class Services 
  {
  public:    
    // ========================================================================
    /// get the instance of the class 
    static Services& instance();
    /// release all services 
    StatusCode releaseAll() ;
    // set new LoKi service 
    LoKi::ILoKiSvc*       setLoKi( LoKi::ILoKiSvc* loki ) ;
    /// accesor to LoKi service 
    LoKi::ILoKiSvc*       lokiSvc     () const ;
    /// accessor to particle properties service
    LHCb::IParticlePropertySvc* ppSvc () const ;
    /// accessor to algorithm context service
    IAlgContextSvc*       contextSvc  () const ;
    /// accessor to histogram service
    IHistogramSvc*        histoSvc    () const ;
    /// accessor to Event Data Service 
    IDataProviderSvc*     evtSvc      () const ;
    /// accessor to Random Number Service 
    IRndmGenSvc*          randSvc     () const ;
    /// destructor (virtual) 
    virtual ~Services() ;
    // ========================================================================
  protected:
    // ========================================================================
    /** Print the error  message, return status code
     *  @param msg    error message 
     *  @param st     status code 
     *  @param mx     the maximal number of prints 
     *  @return       status code 
     */
    StatusCode Error     
    ( const std::string& msg                       , 
      const StatusCode   st  = StatusCode::FAILURE , 
      const size_t       mx  = 10                  ) const  ;
    /** Print the warning  message, return status code 
     *  @param msg    warning message 
     *  @param st     status code  
     *  @param mx     the maximal number of prints 
     *  @return       status code 
     */
    StatusCode Warning   
    ( const std::string& msg                       , 
      const StatusCode   st  = StatusCode::FAILURE , 
      const size_t       mx  = 10                  ) const  ;
    // ========================================================================
  protected:
    // ========================================================================
    /// Standard (default) constructor
    Services() ;
    // ========================================================================
  private:
    // ========================================================================
    // copy constructor     is disabled 
    Services           ( const Services& ) ;
    // assignement operator is disabled 
    Services& operator=( const Services& ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// The major service : LoKi service 
    LoKi::ILoKiSvc*        m_lokiSvc    ; // the major service for LoKi 
    // The source of particle properties 
    mutable LHCb::IParticlePropertySvc* m_ppSvc ; // particle properties 
    // The algorithm context service: needed for "context" services
    mutable IAlgContextSvc* m_contextSvc ; // algorithm context 
    // The source of random numbers 
    mutable IRndmGenSvc*   m_randSvc    ; // the source of random numbers 
    // The histogram storage 
    mutable IHistogramSvc* m_histoSvc   ; // the histogram storage 
    // The eevtn data service 
    mutable IDataProviderSvc* m_evtSvc  ; // the evetn data service 
    // =======================================================================
  };
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_SERVICES_H
// ============================================================================
