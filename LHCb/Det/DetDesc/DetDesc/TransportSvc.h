// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/TransportSvc.h,v 1.4 2001-03-13 21:57:32 mato Exp $
#ifndef     _DETDESC_TRANSPORTSVC_H
#define     _DETDESC_TRANSPORTSVC_H

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

/** @class TransportSvc TransportSvc.h DetDesc/TransportSvc.h

    Implementation of abstract interface ITransportSvc 

    @author Vanya Belyaev
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
  virtual StatusCode queryInterface( const IID& riid, void** ppvInterface );
  ///     Service type.
  virtual const IID& type          () const { return IID_ITransportSvc ; } ; 
  ///  
  /// Methods from ITransportSvc:
  ///     Estimate the distance between 2 points in units of radiation length units 
  virtual double distanceInRadUnits( const HepPoint3D& Point1                         ,    /// first  point 
                                     const HepPoint3D& Point2                         ,    /// second point 
                                     double            Threshold                 = 0  ,    /// threshold value 
                                     IGeometryInfo*    AlternativeGeometry       = 0  ,    /// source of alternative geometry information 
                                     IGeometryInfo*    GeometryGuess             = 0  )  ; /// a guess for navigation
  ///     general method ( returns the "full history" of the volume boundary intersections 
  ///     with different material properties between 2 points ) 
  virtual unsigned long intersections( const HepPoint3D&        Point                   ,    /// initial point on the line  
                                       const HepVector3D&       Vector                  ,    /// direction vector of the line 
                                       const ISolid::Tick&      TickMin                 ,    /// minimal value of line paramater
                                       const ISolid::Tick&      TickMax                 ,    /// maximal value of line parameter 
                                       ILVolume::Intersections& Intersept               ,    /// (output) container of intersections
                                       double                   Threshold           = 0 ,    /// threshold value 
                                       IGeometryInfo*           AlternativeGeometry = 0 ,    /// source of alternative geometry information 
                                       IGeometryInfo*           GeometryGuess       = 0 )  ; /// a guess for navigation 
  ///  
 private:
  /// own private methods 
  /// access to ChronoStat service   
  inline IChronoStatSvc*          chronoSvc           () const ;
  /// access to Magnetic Field service 
  inline IMagneticFieldSvc*       magFieldSvc         () const ; 
  /// access to Detector Data Service
  inline IDataProviderSvc*        detSvc              () const ; 
  ///  source of "standard" geometry information - "top of Detector Description tree"
  inline IGeometryInfo*           standardGeometry    () const ; 
  ///  previous geometry information 
  inline IGeometryInfo*           previousGeometry    () const ; 
  inline IGeometryInfo*           setPreviousGeometry ( IGeometryInfo* previous )  ; 
  /// assertion!
  inline void Assert( bool                  assertion                        , 
                      const std::string&    message                          ,
                      const StatusCode&     statusCode = StatusCode::FAILURE ) const;
  /// 
  inline void Assert( bool                  assertion                        , 
                      const std::string&    message                          ,
                      const GaudiException& Exception                        ,
                      const StatusCode&     statusCode = StatusCode::FAILURE ) const; 
  /// get the geometry info by name
  IGeometryInfo* findGeometry( const std::string& address ) const ; 
  /// check for "good" geometry info 
  bool           goodLocalGI( const HepPoint3D& point1 , 
                              const HepPoint3D& point2 , 
                              IGeometryInfo*    gi     ) const; 
  ///  find good local geometry element 
  IGeometryInfo* findLocalGI( const HepPoint3D& point1 , 
                              const HepPoint3D& point2 , 
                              IGeometryInfo*    gi     ) const;   
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
  ///  Name (address in Transient Store) for the top element of "standard" geometry source 
  std::string                    m_standardGeometry_address ; 
  mutable  IGeometryInfo*        m_standardGeometry         ; 
  /// 
  mutable IGeometryInfo*         m_previousGeometry ;
  /// previous parameters 
  mutable HepPoint3D               m_prevPoint1          ; 
  mutable HepPoint3D               m_prevPoint2          ; 
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
#include "DetDesc/TransportSvc.icpp"
///


#endif  //   DETDESC__TRANSPORTSVC_H 






