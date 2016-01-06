#ifndef     _DETDESCSVC_TRANSPORTSVC_H
#define     _DETDESCSVC_TRANSPORTSVC_H
// ============================================================================
// Include Files
// ============================================================================
// STD & STL
// ============================================================================
#include <iostream>
#include <string>
#include <map>
// ============================================================================
// GaudiKErnel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StatEntity.h"
// ============================================================================
// DetDesc
// ============================================================================
#include "DetDesc/ITransportSvc.h"
#include "DetDesc/IGeometryErrorSvc.h"
#include "DetDesc/TransportSvcException.h"
// ============================================================================
// forward decalrations
// ============================================================================
class IDataProviderSvc;
class IMessageSvc;
class IDetectorElement;
class ISvcLocator;
class GaudiException;
// ============================================================================
/** @class TransportSvc TransportSvc.h DetDescSvc/TransportSvc.h
 *
 *  Implementation of abstract interface ITransportSvc
 *  and abstract interface DetDesc::IGometryErrorSvc
 *
 *  @author Vanya Belyaev ibelyaev@physics.syr.edu
 */
class TransportSvc : public extends< Service, ITransportSvc, DetDesc::IGeometryErrorSvc>
{
  /// typedefs: (internal only!)
  typedef std::vector<IGeometryInfo*>     GeoContainer;
  typedef GeoContainer::reverse_iterator  rGeoIt      ;
  ///
public:
  /// constructor
  TransportSvc
  ( const std::string& name, ISvcLocator* ServiceLocator );
  /// destructor
  ~TransportSvc() override;
  ///
public:
  // ==========================================================================
  // Methods from (I)Service/IInterface
  // ==========================================================================
  ///     Initialise the service.
  StatusCode initialize    () override;
  ///     Finalise the service.
  StatusCode finalize      () override;
  // ==========================================================================
public:
  // ==========================================================================
  // Methods from ITransportSvc
  // ==========================================================================
  /** Estimate the distance between 2 points in
   *  units of radiation length units
   *  @see ITransportSvc
   *  @param Point1 first point
   *  @param Point2 second point
   *  @param Threshold threshold value
   *  @param AlternativeGeometry  source ot alternative geomtry
   *  @param GeometryGuess guess foe geometry
   *  @return distance in rad length units
   */
  double distanceInRadUnits
  ( const Gaudi::XYZPoint& Point1         ,
    const Gaudi::XYZPoint& Point2         ,
    double            Threshold           ,
    IGeometryInfo*    AlternativeGeometry ,
    IGeometryInfo*    GeometryGuess       ) const override;
  // ==========================================================================
  /** general method ( returns the "full history" of the volume
   *  boundary intersections
   * with different material properties between 2 points )
   *  @see ITransportSvc
   *  @see IGeometryInfo
   *  @see ILVolume
   *  @param point               initial point on the line
   *  @param vect                direction vector of the line
   *  @param tickMin             minimal value of line paramater
   *  @param tickMax             maximal value of line parameter
   *  @param intersept           (output) container of intersections
   *  @param threshold           threshold value
   *  @param alternativeGeometry source of alternative geometry information
   *  @param geometryGuess       a guess for navigation
   */
  unsigned long intersections
  ( const Gaudi::XYZPoint&   point               ,
    const Gaudi::XYZVector&  vect                ,
    const ISolid::Tick&      tickMin             ,
    const ISolid::Tick&      tickMax             ,
    ILVolume::Intersections& intersept           ,
    double                   threshold           ,
    IGeometryInfo*           alternativeGeometry ,
    IGeometryInfo*           geometryGuess       )  const override;
  // ==========================================================================
public:
  // ==========================================================================
  // The methods from DetDesc::IGeometryErrorSvc
  // ==========================================================================
  /** set/reset the current "status" of geometry erorrs
   *  @param sc the status code
   *  @param volume the volume
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-12-14
   */
  virtual void setCode
  ( const StatusCode& sc     ,
    const ILVolume*   volume ) ;
  // ==========================================================================
  /** inspect the potential error in intersections
   *  @param  volume   the problematic volume
   *  @param  pnt      3D point
   *  @param  vect     3D direction vector
   *  @param  cnt  the problematic container of intersections
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-12-14
   */
  virtual void inspect
  ( const ILVolume*                volume ,
    const Gaudi::XYZPoint&         pnt    ,
    const Gaudi::XYZVector&        vect   ,
    const ILVolume::Intersections& cnt    ) ;
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
    const double    delta     ) ;
  // ========================================================================
  /** report the skipped intersection
   *  @param  volume   the problematic volume
   *  @param  material the affected material
   *  @param  delta    the problematic delta  (non-negative!)
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-12-14
   */
  virtual void skip
  ( const ILVolume* volume   ,
    const Material* material ,
    const double    delta    ) ;
  // ========================================================================
private:
  // ==========================================================================
  // own private methods
  // ==========================================================================
  /// access to Detector Data Service
  inline IDataProviderSvc*        detSvc              () const ;
  /**  source of "standard" geometry information -
   *  "top of Detector Description tree"
   */
  inline IGeometryInfo*           standardGeometry    () const ;
  ///  previous geometry information
  inline IGeometryInfo*           previousGeometry    () const ;
  inline IGeometryInfo*           setPreviousGeometry
  ( IGeometryInfo* previous )  const;
  /// assertion!
  inline void Assert
  ( bool                  assertion                        ,
    const std::string&    message                          ,
    const StatusCode&     statusCode = StatusCode::FAILURE ) const;
  ///
  inline void Assert
  ( bool                  assertion                        ,
    const std::string&    message                          ,
    const GaudiException& Exception                        ,
    const StatusCode&     statusCode = StatusCode::FAILURE ) const;
  /// get the geometry info by name
  IGeometryInfo* findGeometry( const std::string& address ) const ;
  /// check for "good" geometry info
  bool           goodLocalGI
  ( const Gaudi::XYZPoint& point1,
    const Gaudi::XYZPoint& point2,
    IGeometryInfo*    gi     ) const;
  ///  find good local geometry element
  IGeometryInfo* findLocalGI
  ( const Gaudi::XYZPoint& point1,
    const Gaudi::XYZPoint& point2,
    IGeometryInfo*    gi     ,
    IGeometryInfo*    topGi  ) const;
  // ==========================================================================
private:
  // ==========================================================================
  /// Own private data members:
  /// names of used services:
  std::string                    m_detDataSvc_name       ;
  ///  Detector Data Service
  mutable SmartIF<IDataProviderSvc> m_detDataSvc            ;
  /**  Name (address in Transient Store) for the top element
   * of "standard" geometry source
   */
  std::string                    m_standardGeometry_address ;
  mutable  IGeometryInfo*        m_standardGeometry = nullptr;
  ///
  mutable IGeometryInfo*           m_previousGeometry = nullptr;
  /// previous parameters
  mutable Gaudi::XYZPoint          m_prevPoint1          ;
  mutable Gaudi::XYZPoint          m_prevPoint2          ;
  mutable double                   m_previousThreshold   ;
  mutable IGeometryInfo*           m_previousGuess = nullptr;
  mutable IGeometryInfo*           m_previousTopGeometry = nullptr;
  mutable ILVolume::Intersections  m_localIntersections  ;
  /// some cache:
  mutable ILVolume::Intersections  m_local_intersects    ;
  mutable GeoContainer             m_vGi1                ;
  mutable GeoContainer             m_vGi2                ;
  mutable GeoContainer             m_vGi                 ;
  mutable ISolid::Ticks            m_local_ticks         ;
  ///
private:
  /// the actual type of the Map
  typedef std::map<std::string, std::pair<StatEntity,StatEntity> > Map ;
  typedef std::map<std::string,unsigned long>                      Map1 ;
  Map  m_skip        ; /// the map of the skip-intervals
  Map  m_recover     ; /// the map of the recovered-intervals
  Map1 m_codes       ; /// the map of various error-codes
  /// property to allow the recovery
  bool m_recovery = true ;
  /// property to allow the protocol
  bool m_protocol = true ;

};
// ============================================================================
/// access to Detector Data  Service
inline IDataProviderSvc*    TransportSvc::detSvc               () const
{ return m_detDataSvc       ; }
// ============================================================================
/// access to the top of standard detector geometry information
inline IGeometryInfo*       TransportSvc::standardGeometry     () const
{
  return ( 0 != m_standardGeometry ) ?
    m_standardGeometry : m_standardGeometry =
    findGeometry( m_standardGeometry_address ) ;
}
// ============================================================================
/// access to the top of standard detector geometry information
inline IGeometryInfo*       TransportSvc::previousGeometry     () const
{ return m_previousGeometry ; }
// ============================================================================
///
inline IGeometryInfo*       TransportSvc::setPreviousGeometry
( IGeometryInfo* previous ) const
{ m_previousGeometry = previous ; return previousGeometry() ; }
// ============================================================================
/// Assertion !!!
inline void TransportSvc::Assert
( bool                assertion  ,
  const std::string&  Message    ,
  const StatusCode&   statusCode ) const
{ if( !assertion ) { throw TransportSvcException( Message , statusCode ); } }
// ============================================================================
/// Assertion !!!
inline void TransportSvc::Assert
( bool                  assertion  ,
  const std::string&    Message    ,
  const GaudiException& Exception  ,
  const StatusCode&     statusCode ) const
{ if( !assertion )
 { throw TransportSvcException( Message , Exception , statusCode ); } }
// ============================================================================
// The END
// ============================================================================
#endif  //   DETDESCSVC__TRANSPORTSVC_H
// ============================================================================
