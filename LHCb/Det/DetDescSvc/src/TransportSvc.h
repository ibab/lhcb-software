// $Id: TransportSvc.h,v 1.3 2006-07-26 09:46:58 cattanem Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef     _DETDESCSVC_TRANSPORTSVC_H
#define     _DETDESCSVC_TRANSPORTSVC_H

// Include Files
#include "GaudiKernel/Kernel.h" 
#include "GaudiKernel/Service.h" 
#include "GaudiKernel/StatusCode.h"

#include "DetDesc/ITransportSvc.h" 
#include "DetDesc/TransportSvcException.h" 

#include <iostream> 
#include <string> 

class IDataProviderSvc;
class IMagneticFieldSvc;
class IMessageSvc;
class IChronoStatSvc; 
class IDetectorElement; 
class ISvcLocator;
class GaudiException; 

/** @class TransportSvc TransportSvc.h DetDescSvc/TransportSvc.h
 *
 *  Implementation of abstract interface ITransportSvc 
 *
 *  @author Vanya Belyaev
 */

class TransportSvc: public         Service        , 
                    virtual public ITransportSvc
{  
  /// typedefs: (internal only!) 
  typedef std::vector<IGeometryInfo*>     GeoContainer;
  typedef GeoContainer::reverse_iterator  rGeoIt      ; 
  ///  
 public:
  /// constructor 
  TransportSvc( const std::string& name, ISvcLocator* ServiceLocator );
  /// destructor 
  virtual ~TransportSvc(); 
  ///
 public:
  /// Methods from Service:
  ///     Initialise the service.
  virtual StatusCode initialize    ();
  ///     Finalise the service.
  virtual StatusCode finalize      ();
  ///     Query the interfaces.
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  ///     Service type.
  virtual const InterfaceID& type  () const { return IID_ITransportSvc ; } ; 
  ///  
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
  virtual double distanceInRadUnits
  ( const Gaudi::XYZPoint& Point1                    , 
    const Gaudi::XYZPoint& Point2                    , 
    double            Threshold                 = 0  , 
    IGeometryInfo*    AlternativeGeometry       = 0  , 
    IGeometryInfo*    GeometryGuess             = 0  ) ;
  
  /** general method ( returns the "full history" of the volume 
   *  boundary intersections 
   * with different material properties between 2 points )
   *  @see ITransportSvc
   *  @see IGeometryInfo
   *  @see ILVolume
   *  @param Point               initial point on the line  
   *  @param Vector              direction vector of the line 
   *  @param TickMin             minimal value of line paramater
   *  @param TickMax             maximal value of line parameter 
   *  @param Intersept           (output) container of intersections
   *  @param Threshold           threshold value 
   *  @param AlternativeGeometry source of alternative geometry information 
   *  @param GeometryGuess       a guess for navigation 
   */
  virtual unsigned long intersections
  ( const Gaudi::XYZPoint&   Point                   ,   
    const Gaudi::XYZVector&  Vector                  ,    
    const ISolid::Tick&      TickMin                 ,    
    const ISolid::Tick&      TickMax                 ,    
    ILVolume::Intersections& Intersept               ,    
    double                   Threshold           = 0 ,    
    IGeometryInfo*           AlternativeGeometry = 0 ,    
    IGeometryInfo*           GeometryGuess       = 0 )  ; 
  ///  
private:
  /// own private methods 
  /// access to ChronoStat service   
  inline IChronoStatSvc*          chronoSvc           () const ;
  /// access to Magnetic Field service 
  inline IMagneticFieldSvc*       magFieldSvc         () const ; 
  /// access to Detector Data Service
  inline IDataProviderSvc*        detSvc              () const ; 
  /**  source of "standard" geometry information - 
   *  "top of Detector Description tree"
   */
  inline IGeometryInfo*           standardGeometry    () const ; 
  ///  previous geometry information 
  inline IGeometryInfo*           previousGeometry    () const ; 
  inline IGeometryInfo*           setPreviousGeometry 
  ( IGeometryInfo* previous )  ; 
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
private:
  ///  Own private data members:
  /// names of used services:
  std::string                    m_chronoStatSvc_name    ; 
  std::string                    m_magFieldSvc_name      ;   
  std::string                    m_detDataSvc_name       ;
  ///  Chrono & Stat service 
  mutable  IChronoStatSvc*       m_chronoStatSvc         ;
  ///  Magnetic Field Service 
  mutable IMagneticFieldSvc*     m_magFieldSvc           ; 
  ///  Detector Data Service 
  mutable IDataProviderSvc*      m_detDataSvc            ;
  /**  Name (address in Transient Store) for the top element 
   * of "standard" geometry source 
   */
  std::string                    m_standardGeometry_address ; 
  mutable  IGeometryInfo*        m_standardGeometry         ; 
  /// 
  mutable IGeometryInfo*         m_previousGeometry ;
  /// previous parameters 
  mutable Gaudi::XYZPoint          m_prevPoint1          ; 
  mutable Gaudi::XYZPoint          m_prevPoint2          ; 
  mutable double                   m_previousThreshold   ;
  mutable IGeometryInfo*           m_previousGuess       ; 
  mutable IGeometryInfo*           m_previousAlternative ; 
  mutable ILVolume::Intersections  m_localIntersections  ; 
  /// some cache:
  mutable ILVolume::Intersections  m_local_intersects    ; 
  mutable GeoContainer             m_vGi1                ; 
  mutable GeoContainer             m_vGi2                ; 
  mutable GeoContainer             m_vGi                 ; 
  mutable ISolid::Ticks            m_local_ticks         ;
  ///
};
///

/// access to Chrono & Stat Service 
inline IChronoStatSvc*      TransportSvc::chronoSvc            () const 
{ return m_chronoStatSvc    ; } ; 

/// access to Magnetic Field  Service 
inline IMagneticFieldSvc*   TransportSvc::magFieldSvc          () const 
{ return m_magFieldSvc      ; } ;

/// access to Detector Data  Service 
inline IDataProviderSvc*    TransportSvc::detSvc               () const 
{ return m_detDataSvc       ; } ;


/// access to the top of standard detector geometry information 
inline IGeometryInfo*       TransportSvc::standardGeometry     () const 
{ 
  return ( 0 != m_standardGeometry ) ? 
    m_standardGeometry : m_standardGeometry = 
    findGeometry( m_standardGeometry_address ) ; 
};

/// access to the top of standard detector geometry information 
inline IGeometryInfo*       TransportSvc::previousGeometry     () const 
{ return m_previousGeometry ; }

///
inline IGeometryInfo*       TransportSvc::setPreviousGeometry  
( IGeometryInfo* previous ) 
{ m_previousGeometry = previous ; return previousGeometry() ; }

/// Assertion !!!
inline void TransportSvc::Assert
( bool                assertion  , 
  const std::string&  Message    , 
  const StatusCode&   statusCode ) const 
{ if( !assertion ) { throw TransportSvcException( Message , statusCode ); } };

/// Assertion !!!
inline void TransportSvc::Assert
( bool                  assertion  , 
  const std::string&    Message    ,
  const GaudiException& Exception  ,  
  const StatusCode&     statusCode ) const 
{ if( !assertion ) 
  { throw TransportSvcException( Message , Exception , statusCode ); } };

// ============================================================================
#endif  //   DETDESCSVC__TRANSPORTSVC_H 
// ============================================================================
