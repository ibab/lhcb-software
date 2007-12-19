// $Id: IntersectionErrors.h,v 1.1 2007-12-19 09:42:39 ibelyaev Exp $
// ============================================================================
#ifndef DETDESC_INTERSECTIONERRORS_H 
#define DETDESC_INTERSECTIONERRORS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <map>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// DetDesc 
// ============================================================================
#include "DetDesc/ILVolume.h"
// ============================================================================
namespace DetDesc
{
  // forward declarations 
  class IGeometryErrorSvc ;
  // ==========================================================================
  /** @class IntersectionErrors DetDesc/IntersectionErrors.h
   *  
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2007-12-12
   */
  class IntersectionErrors 
  {
  public:
    /// get the total number of uncatched errors 
    static unsigned long errors () ;
  public:
    /// set the static error code 
    static void              setCode 
    ( const StatusCode& sc , const ILVolume*   volume ) ;
    /// get the static error code 
    static const StatusCode& code    ()  ;
  public:
    /// set the static pointer to Geoemtry Error Service 
    static void setService ( DetDesc::IGeometryErrorSvc* svc) ;
    /// get the static pointer to Geoemtry Error Service 
    static DetDesc::IGeometryErrorSvc* service() ;
  public:
    /// set the static flag for recovery
    static void setRecovery ( const bool value ) ;
    /// get teh static flag for recovery
    static bool    recovery () ;
  public:
    // ========================================================================
    /** report the recovered action in intersections 
     *  @param  volume    the problematic volume 
     *  @aram   material1 the affected material 
     *  @aram   material2 the affected material 
     *  @param  delta    the problematic delta  (non-negative!)
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-12-14
     */
    static void recovered
    ( const ILVolume* volume    , 
      const Material* material1 ,
      const Material* material2 ,
      const double    delta     ) ;
    // ========================================================================
    /** report the skipped intersection 
     *  @param  volume   the problematic volume 
     *  @aram   material the affected material 
     *  @param  delta    the problematic delta  (non-negative!)
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-12-14
     */
    static void skip
    ( const ILVolume* volume   , 
      const Material* material ,
      const double    delta    ) ;
    // ========================================================================
  public:
    /// inspect the potential error in intersections
    static void  inspect
    ( const ILVolume*                volume , 
      const Gaudi::XYZPoint&         pnt    , 
      const Gaudi::XYZVector&        vect   , 
      const ILVolume::Intersections& cnt    ) ;
  private:  
    // static code 
    static StatusCode                  s_code     ; /// the static error code 
    // static pointer to IGeometryErrorSvc 
    static DetDesc::IGeometryErrorSvc* s_service  ; /// pointer to IGeometryErrorSvc 
    // static counter of unreported errors 
    static unsigned long               s_errors   ;
    // static flag to perform the error ecovery 
    static bool                        s_recovery ;
  } ;
} // end of namespace DetDesc
// ============================================================================ 
// The END 
// ============================================================================ 
#endif // DETDESC_INTERSECTIONERRORS_H
// ============================================================================ 
