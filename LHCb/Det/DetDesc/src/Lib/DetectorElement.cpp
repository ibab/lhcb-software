// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/DetectorElement.cpp,v 1.5 2001-03-07 13:06:13 ibelyaev Exp $
#include "GaudiKernel/Kernel.h"

#include "GaudiKernel/IGeometryInfo.h"
#include "GaudiKernel/IDataDirectory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"

#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/TransientStore.h" 
#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/Bootstrap.h"


#include "DetDesc/DetectorElement.h"


/// local !!!
#include "GeometryInfo.h"




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DetectorElement::DetectorElement( const std::string&   name        ,
                                  const ITime&         validSince  ,   
                                  const ITime&         validTill   )
  : DataObject           (  name   )
  , m_de_iGeometry       (    0    ) 
  // 
  , m_de_iAlignment      (    0    ) 
  , m_de_iCalibration    (    0    )   
  , m_de_iReadOut        (    0    ) 
  , m_de_iSlowControl    (    0    ) 
  , m_de_iFastControl    (    0    ) 
  //
  , m_de_childrensLoaded (  false  )
  , m_de_childrens       (         ) 
  //
  , m_de_validSince      (    0    ) 
  , m_de_validTill       (    0    )
  //
  , m_de_dataSvc         (    0    )
  , m_de_msgSvc          (    0    ) 
  , m_de_svcLoc          (    0    ) 
{
  ///
  m_de_validSince = new(std::nothrow) TimePoint( validSince ) ;
  m_de_validTill  = new(std::nothrow) TimePoint( validTill  ) ; 
  ///
  m_de_svcLoc = Gaudi::svcLocator(); 
  if( 0 == svcLoc() ) { throw DetectorElementException("DetectorElement(1), ISvcLocator* points to NULL!"); }
  svcLoc()->addRef();
  ///
  {  
    const std::string tmp("DetectorDataSvc") ;
    StatusCode sc = svcLoc()->service( tmp , m_de_dataSvc );
    if( sc.isFailure() ) { throw DetectorElementException("DetectorElement(1), could no tload IDataProviderSvc="+tmp  ); }
    if( 0 == dataSvc() ) { throw DetectorElementException("DetectorElement(1), IDataProviderSvc* points to NULL, "+tmp ); }
    dataSvc()->addRef();
  }
  ///
  {  
    const std::string tmp("MessageSvc") ;
    StatusCode sc = svcLoc()->service( tmp , m_de_msgSvc );
    if( sc.isFailure() ) { throw DetectorElementException("DetectorElement(1), could no load IMessageSvc="+tmp  ); }
    if( 0 == msgSvc () ) { throw DetectorElementException("DetectorElement(1), IMessageSvc* points to NULL, "+tmp ); }
    msgSvc()->addRef();
  }
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DetectorElement::DetectorElement( const std::string&   name   )
  : DataObject           (  name   )
  , m_de_iGeometry       (    0    ) 
  // 
  , m_de_iAlignment      (    0    ) 
  , m_de_iCalibration    (    0    )   
  , m_de_iReadOut        (    0    ) 
  , m_de_iSlowControl    (    0    ) 
  , m_de_iFastControl    (    0    ) 
  //
  , m_de_childrensLoaded (  false  )
  , m_de_childrens       (         ) 
  //
  , m_de_validSince      (    0    ) 
  , m_de_validTill       (    0    )
  //
  , m_de_dataSvc         (    0    )
  , m_de_msgSvc          (    0    ) 
  , m_de_svcLoc          (    0    ) 
{
  ///
  m_de_validSince = new(std::nothrow) TimePoint( time_absolutepast   ) ;
  m_de_validTill  = new(std::nothrow) TimePoint( time_absolutefuture ) ; 
  ///
  m_de_svcLoc = Gaudi::svcLocator(); 
  if( 0 == svcLoc() ) { throw DetectorElementException("DetectorElement(2), ISvcLocator* points to NULL!"); }
  svcLoc()->addRef();
  ///
  {  
    const std::string tmp("DetectorDataSvc") ;
    StatusCode sc = svcLoc()->service( tmp , m_de_dataSvc );
    if( sc.isFailure() ) { throw DetectorElementException("DetectorElement(2), could no tload IDataProviderSvc="+tmp  ); }
    if( 0 == dataSvc() ) { throw DetectorElementException("DetectorElement(2), IDataProviderSvc* points to NULL, "+tmp ); }
    dataSvc()->addRef();
  }
  ///
  {  
    const std::string tmp("MessageSvc") ;
    StatusCode sc = svcLoc()->service( tmp , m_de_msgSvc );
    if( sc.isFailure() ) { throw DetectorElementException("DetectorElement(2), could no load IMessageSvc="+tmp  ); }
    if( 0 == msgSvc () ) { throw DetectorElementException("DetectorElement(2), IMessageSvc* points to NULL, "+tmp ); }
    msgSvc()->addRef();
  }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DetectorElement::~DetectorElement()
{
  if( 0 != msgSvc  () ) { msgSvc  ()->release() ; m_de_msgSvc  = 0 ; }  
  if( 0 != dataSvc () ) { dataSvc ()->release() ; m_de_dataSvc = 0 ; }
  if( 0 != svcLoc  () ) { svcLoc  ()->release() ; m_de_svcLoc  = 0 ; }
  ///
  if ( 0 != m_de_iGeometry     ) { delete m_de_iGeometry     ;  m_de_iGeometry     = 0 ; } 
  ///

  //  if ( 0 != m_de_iAlignment    ) { delete m_de_iAlignment    ;  m_de_iAlignment    = 0 ; }
  //  if ( 0 != m_de_iCalibration  ) { delete m_de_iCalibration  ;  m_de_iCalibration  = 0 ; }
  //  if ( 0 != m_de_iReadOut      ) { delete m_de_iReadOut      ;  m_de_iReadOut      = 0 ; }
  //  if ( 0 != m_de_iSlowControl  ) { delete m_de_iSlowControl  ;  m_de_iSlowControl  = 0 ; }
  //  if ( 0 != m_de_iFastConrol   ) { delete m_de_iFastConstrol ;  m_de_iFastControl  = 0 ; }
  
  //  if ( 0 != m_de_validSince    ) { delete m_de_validSince    ;  m_de_validSince    = 0 ; }
  //  if ( 0 != m_de_validTill     ) { delete m_de_validTill     ;  m_de_validTill     = 0 ; }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DetectorElement::acceptInspector( IInspector* pInspector ) 
{
  if( 0 == pInspector ) { return false; } 
  pInspector->inspectByRef( m_de_iGeometry , this , "GeometryInfo" ); 
  return DataObject::acceptInspector( pInspector ) ;
};  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DetectorElement::acceptInspector( IInspector* pInspector ) const  
{
  if( 0 == pInspector ) { return false; } 
  pInspector->inspectByRef( m_de_iGeometry , this , "GeometryInfo" ); 
  return DataObject::acceptInspector( pInspector ) ;
};  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StreamBuffer& DetectorElement::serialize( StreamBuffer& sb ) const 
{
  DataObject::serialize( sb ) ; 
  sb << *m_de_iGeometry ;
  return sb;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StreamBuffer& DetectorElement::serialize( StreamBuffer& sb ) 
{
  reset() ; 
  DataObject::serialize( sb ) ; 
  if( 0 == m_de_iGeometry ) 
    { m_de_iGeometry = new GeometryInfo( this ) ; } 
  sb >> *m_de_iGeometry ;
  return sb;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::ostream& DetectorElement::printOut( std::ostream& os ) const
{ 
  os << "DetectorElement::"  << fullpath(); 
  return ( 0 == geometry() ? os : (os << "GeometryInfo::" << geometry()) ); 
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MsgStream& DetectorElement::printOut( MsgStream& os ) const
{ 
  os << "DetectorElement::"  << fullpath(); 
  return ( 0 == geometry() ? os : (os << "GeometryInfo::" << geometry() ) );
};
/// reset to the initial state/////////////////////////////////////////////////////////////////////////////////////////
IDetectorElement* DetectorElement::reset() 
{
  /// reset geometry
  if( 0 != geometry() ) { geometry()->reset() ;} 
  if( m_de_childrensLoaded ) { std::for_each( childBegin() , childEnd() , std::mem_fun(&IDetectorElement::reset) );} 
  m_de_childrensLoaded = false ; 
  m_de_childrens.clear()       ; 
  return this;  
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const IGeometryInfo* DetectorElement::createGeometryInfo()
{
  Assert( 0 == geometry() , "Could not create GHOST: Geometry already exist!" );
  m_de_iGeometry = new GeometryInfo( this );
  return geometry();
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const IGeometryInfo* DetectorElement::createGeometryInfo( const std::string& LogVol )
{
  Assert( 0 == geometry() , "Could not create ORPHAN: Geometry already exist!" );
  m_de_iGeometry = new GeometryInfo( this , LogVol );
  return geometry();
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const IGeometryInfo* DetectorElement::createGeometryInfo( const std::string& LogVol   , 
                                                          const std::string& Support  ,
                                                          const std::string& NamePath )
{
  Assert( 0 == geometry() , "Could not create REGULAR(1): Geometry already exist!" );
  m_de_iGeometry = new GeometryInfo( this , LogVol , Support , NamePath );
  return geometry();
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const IGeometryInfo* DetectorElement::createGeometryInfo( const std::string           & LogVol   , 
                                                          const std::string           & Support  ,
                                                          const ILVolume::ReplicaPath & rPath    ) 
{
  Assert( 0 == geometry() , "Could not create REGULAR(2): Geometry already exist!" );
  m_de_iGeometry = new GeometryInfo( this , LogVol , Support , rPath );
  return geometry();
};
/// functions from IValidity ///////////////////////////////////////////////////////////////////////////////////////////
const ITime&  DetectorElement::validSince ()
{
  if ( 0 != m_de_validSince ){  setValiditySince( time_absolutepast ); }
  return *m_de_validSince; 
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const ITime&  DetectorElement::validTill  () 
{
  if ( 0 != m_de_validTill ) { setValidityTill( time_absolutefuture ); }
  return *m_de_validTill; 
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void          DetectorElement::setValidity       ( const ITime& Since , 
                                                                const ITime& Till )
{
  setValiditySince( Since );
  setValidityTill ( Till  );
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void          DetectorElement::setValiditySince  ( const ITime& Since ) 
{
  if( 0 != m_de_validSince ) { delete m_de_validSince; m_de_validSince = 0 ;} 
  m_de_validSince = new(std::nothrow)  TimePoint( Since );   
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void          DetectorElement::setValidityTill  ( const ITime& Till ) 
{
  if( 0 != m_de_validTill ){ delete m_de_validTill; m_de_validTill = 0 ;} 
  m_de_validTill = new(std::nothrow)  TimePoint( Till );   
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode    DetectorElement::updateValidity    ()
{
  validSince();
  validTill ();
  //
  //  not yet implemented in a proper way 
  //
  return StatusCode::SUCCESS;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////









