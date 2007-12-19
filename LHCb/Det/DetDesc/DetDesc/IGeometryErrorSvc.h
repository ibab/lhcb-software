// $Id: IGeometryErrorSvc.h,v 1.1 2007-12-19 09:42:39 ibelyaev Exp $
// ============================================================================
#ifndef DETDESC_IGEOMETRYERRORSVC_H 
#define DETDESC_IGEOMETRYERRORSVC_H 1
// ============================================================================
// Include files
// ============================================================================
// Gaudi Kernel
// ============================================================================
#include "GaudiKernel/IService.h"
// ============================================================================
// DetDesc 
// ============================================================================
#include "DetDesc/ITransportSvc.h"
// ============================================================================
namespace DetDesc 
{
  // ==========================================================================
  /** @class IGeometryErrorSvc DetDesc/IGeometryErrorSvc.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2007-12-14
   */
  class IGeometryErrorSvc : virtual public IService 
  {
  public:
    // ========================================================================
    /** set/reset the current "status" of geometry erorrs  
     *  @param sc the status code 
     *  @param volume the problematic logicla volume 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-12-14
     */
    virtual void setCode 
    ( const StatusCode& sc , const ILVolume*   volume ) = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    /** inspect the potential error in intersections 
     *  @param  volume   the problematic volume 
     *  @param  point    3D point 
     *  @param  vector   3D direction vector 
     *  @param  cnt  the problematic container of intersections  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-12-14
     */
    virtual void inspect
    ( const ILVolume*                volume , 
      const Gaudi::XYZPoint&         pnt    , 
      const Gaudi::XYZVector&        vect   , 
      const ILVolume::Intersections& cnt    ) = 0 ;
    // ========================================================================
    /** report the recovered action in intersections 
     *  @param  volume    the problematic volume 
     *  @param  material1 the affected material 
     *  @param  material2 the affected material 
     *  @param  delta     the problematic delta  (non-negative!)
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-12-14
     */
    virtual void recovered
    ( const ILVolume* volume    , 
      const Material* material1 ,
      const Material* material2 ,
      const double    delta     ) = 0 ;
    // ========================================================================
    /** report the skipped intersection 
     *  @param  volume   the problematic volume 
     *  @aram   material the affected material 
     *  @param  delta    the problematic delta  (non-negative!)
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-12-14
     */
    virtual void skip
    ( const ILVolume* volume   , 
      const Material* material ,
      const double    delta    ) = 0 ;
  public: 
    /// Retrieve the unique interface ID
    static const InterfaceID& interfaceID() ;
  protected:
    // virtual & protected desctructor 
    virtual ~IGeometryErrorSvc( ); ///< virtual & protected destructor
  };
  // ==========================================================================
} // end of namespace DetDesc 
// ============================================================================
// The END 
// ============================================================================
#endif // DETDESC_IGEOMETRYERRORSVC_H
// ============================================================================
