// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/DetectorElement.cpp,v 1.1 2001-01-22 09:55:41 ibelyaev Exp $
#include "GaudiKernel/Kernel.h"

#include "GaudiKernel/IGeometryInfo.h"
#include "GaudiKernel/IDataDirectory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"

#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/TransientStore.h" 
#include "GaudiKernel/ObjectFactory.h" 


#include "DetDesc/DetectorElement.h"
#include "DetDesc/GeometryInfo.h"



const CLID& CLID_DetectorElement = 1000;


DetectorElement::DetectorElement( const std::string&   name        ,
				  const ITime&         validSince  ,   
				  const ITime&         validTill   , 
				  IDataProviderSvc*    datasvc     ,
				  IMessageSvc*         messsvc     )
  : DataObject           (  name   )
  , m_de_iGeometry       (    0    ) 
  // 
  , m_de_iAlignment      (    0    ) 
  , m_de_iCalibration    (    0    )   
  , m_de_iReadOut        (    0    ) 
  , m_de_iSlowControl    (    0    ) 
  , m_de_iFastControl    (    0    ) 
  //
  , m_de_parentLoaded    (  false  ) 
  , m_de_parent          (    0    ) 
  // 
  , m_de_childrensLoaded (  false  )
  , m_de_childrens       (         ) 
  //
  , m_de_validSince      (    0    ) 
  , m_de_validTill       (    0    )
  //
  , m_de_dataSvc         ( datasvc )
  , m_de_messageSvc      ( messsvc ) 
{
  //
  m_de_validSince = new(std::nothrow) TimePoint( validSince ) ;
  m_de_validTill  = new(std::nothrow) TimePoint( validTill  ) ; 
  //
};

///
///
///

DetectorElement::DetectorElement( const std::string&   name        ,
				  IDataProviderSvc*    datasvc     ,
				  IMessageSvc*         messsvc     )
  : DataObject           (  name   )
  , m_de_iGeometry       (    0    ) 
  // 
  , m_de_iAlignment      (    0    ) 
  , m_de_iCalibration    (    0    )   
  , m_de_iReadOut        (    0    ) 
  , m_de_iSlowControl    (    0    ) 
  , m_de_iFastControl    (    0    ) 
  //
  , m_de_parentLoaded    (  false  ) 
  , m_de_parent          (    0    ) 
  // 
  , m_de_childrensLoaded (  false  )
  , m_de_childrens       (         ) 
  //
  , m_de_validSince      (    0    ) 
  , m_de_validTill       (    0    )
  //
  , m_de_dataSvc         ( datasvc )
  , m_de_messageSvc      ( messsvc ) 
{
  ///
  m_de_validSince = new(std::nothrow) TimePoint( time_absolutepast   ) ;
  m_de_validTill  = new(std::nothrow) TimePoint( time_absolutefuture ) ; 
  ///
};

///
///
///

DetectorElement::~DetectorElement()
{
  if ( 0 != m_de_iGeometry     ) { delete m_de_iGeometry     ;  m_de_iGeometry     = 0 ; } 
  //  if ( 0 != m_de_iAlignment    ) { delete m_de_iAlignment    ;  m_de_iAlignment    = 0 ; }
  //  if ( 0 != m_de_iCalibration  ) { delete m_de_iCalibration  ;  m_de_iCalibration  = 0 ; }
  //  if ( 0 != m_de_iReadOut      ) { delete m_de_iReadOut      ;  m_de_iReadOut      = 0 ; }
  //  if ( 0 != m_de_iSlowControl  ) { delete m_de_iSlowControl  ;  m_de_iSlowControl  = 0 ; }
  //  if ( 0 != m_de_iFastConrol   ) { delete m_de_iFastConstrol ;  m_de_iFastControl  = 0 ; }
  
  //  if ( 0 != m_de_validSince    ) { delete m_de_validSince    ;  m_de_validSince    = 0 ; }
  //  if ( 0 != m_de_validTill     ) { delete m_de_validTill     ;  m_de_validTill     = 0 ; }
};


///
///
///

bool DetectorElement::acceptInspector( IInspector* pInspector ) 
{
  ///
  if( 0 == pInspector ) { return false; } 
  ///
  pInspector->inspectByRef( m_de_iGeometry , this , "GeometryInfo" ); 
  ///
  return DataObject::acceptInspector( pInspector ) ;
  ///
};  

///
///
///

bool DetectorElement::acceptInspector( IInspector* pInspector ) const  
{
  ///
  if( 0 == pInspector ) { return false; } 
  ///
  pInspector->inspectByRef( m_de_iGeometry , this , "GeometryInfo" ); 
  ///
  return DataObject::acceptInspector( pInspector ) ;
  ///
};  

StreamBuffer& DetectorElement::serialize( StreamBuffer& sb ) const 
{
  ///
  DataObject::serialize( sb ) ; 
  ///
  sb << *m_de_iGeometry ;
  ///
  return sb;
  ///
};


///
///
///
StreamBuffer& DetectorElement::serialize( StreamBuffer& sb ) 
{
  ///
  reset() ; 
  ///
  DataObject::serialize( sb ) ; 
  ///
  if( 0 == m_de_iGeometry ) 
    { m_de_iGeometry = new GeometryInfo( this , dataSvc() , msgSvc() ) ; } 
  ///
  sb >> *m_de_iGeometry ;
  ///
  return sb;
  ///
};

///
///
///

std::ostream& DetectorElement::printOut( std::ostream& os ) const
{ 
  os << "DetectorElement::"  << fullpath(); 
  return ( 0 == geometry() ? os : (os << "GeometryInfo::" << geometry()) ); 
};

//
//
//
MsgStream& DetectorElement::printOut( MsgStream& os ) const
{ 
  os << "DetectorElement::"  << fullpath(); 
  return ( 0 == geometry() ? os : (os << "GeometryInfo::" << geometry() ) );
};


///
///
/// reset to the initial state
IDetectorElement* DetectorElement::reset() 
{
  /// reset geometry
  if( 0 != geometry() ) { geometry()->reset() ;} 
  ///  
  if( m_de_childrensLoaded ) { std::for_each( childBegin() , childEnd() , std::mem_fun(&IDetectorElement::reset) );} 
  ///
  m_de_childrensLoaded = false ; 
  m_de_childrens.clear()       ; 
  /// reset parent 
  m_de_parentLoaded = false ; 
  m_de_parent       = 0     ;
  ///
  return this;  
};
///
///
///

/*
  //
  //
  // create "ghost" 
  const IGeometryInfo* DetectorElement::createGeometryInfo( IDataProviderSvc*            datasvc,
  IMessageSvc*                 messsvc )
  {
  if( 0 != geometry() ) 
  {
  MsgStream log( msgSvc() , "DetectorElement::createGeometryInfo1");
  log << MSG::ERROR << "GeometryInfo is already created, skip! " << endreq; 
  return geometry();
  }
  //
  IGeometryInfo* gi = 0;
  try 
  { 
  gi = new GeometryInfo( this , 
  (0 == datasvc) ? dataSvc() : datasvc , 
  (0 == messsvc) ? msgSvc () : messsvc ); 
  }
  catch( const DetectorElement::DetectorElementException& dee ) { dee.print() ; gi = 0 ; }
  catch( const GeometryInfo::GeometryInfoException&       gie ) { gie.print() ; gi = 0 ; }
  catch(...)                                                    {               gi = 0 ; }
  
  if( 0 == gi )
  {
  MsgStream log( msgSvc() , "DetectorElement::createGeometryInfo1");
  log << MSG::ERROR << "Could not create GeometryInfo object " << endreq; 
  }
  
  de_igeo = gi;
  return geometry();
  };
  
  //
  //
  //
  const IGeometryInfo* DetectorElement::createGeometryInfo( const std::string&           LogVol      ,
  IDataProviderSvc*            datasvc ,
  IMessageSvc*                 messsvc )
  {
  if( 0 != geometry() ) 
  {
  MsgStream log( msgSvc() , "DetectorElement::createGeometryInfo2");
  log << MSG::ERROR << "GeometryInfo is already created, skip! " << endreq; 
  return geometry();
  }
  //
  IGeometryInfo* gi = 0;
  try 
  { 
  gi = new GeometryInfo( this                                 , 
  LogVol                               ,
  (0 == datasvc) ? dataSvc() : datasvc , 
  (0 == messsvc) ? msgSvc () : messsvc ); 
  }
  catch( const DetectorElement::DetectorElementException& dee ) { dee.print() ; gi = 0 ; }
  catch( const GeometryInfo::GeometryInfoException&       gie ) { gie.print() ; gi = 0 ; }
  catch(...)                                                    {               gi = 0 ; }
  
  if( 0 == gi )
  {
  MsgStream log( msgSvc() , "DetectorElement::createGeometryInfo2");
  log << MSG::ERROR << "Could not create GeometryInfo object " << endreq; 
  }
  
  de_igeo = gi;
  return geometry();
  };
  
  //
  //
  const IGeometryInfo* DetectorElement::createGeometryInfo( const std::string&           LogVol      , 
  const std::string&           Support     ,
  const ILVolume::ReplicaPath& replicaPath ,
  IDataProviderSvc*            datasvc ,
  IMessageSvc*                 messsvc )
  {  
  if( 0 != geometry() ) 
  {
  MsgStream log( msgSvc() , "DetectorElement::createGeometryInfo3");
  log << MSG::ERROR << "GeometryInfo is already created, skip! " << endreq; 
  return geometry();
  }
  //
  IGeometryInfo* gi = 0;
  try 
  { 
  gi = new GeometryInfo( this                                 , 
  LogVol                               ,
  Support                              ,
  replicaPath                          ,
  (0 == datasvc) ? dataSvc() : datasvc , 
  (0 == messsvc) ? msgSvc () : messsvc ); 
  }
  catch( const DetectorElement::DetectorElementException& dee ) { dee.print() ; gi = 0 ; }
  catch( const GeometryInfo::GeometryInfoException&       gie ) { gie.print() ; gi = 0 ; }
  catch(...)                                                    {               gi = 0 ; }
  
  if( 0 == gi )
  {
  MsgStream log( msgSvc() , "DetectorElement::createGeometryInfo3");
  log << MSG::ERROR << "Could not create GeometryInfo object " << endreq; 
  }
  
  de_igeo = gi;
  return de_igeo;
  };
  */
//
//
//

IDataProviderSvc* DetectorElement::dataSvc() 
{
  if( 0 != m_de_dataSvc ) { return m_de_dataSvc; }
  //
  DataObject*       Obj   = dynamic_cast<DataObject*>(this); 
  Assert( 0 != Obj   , "DetectorElement::dataSvc() Object    points to NULL " );
  IDataDirectory*   dir   = Obj->directory(); 
  Assert( 0 != dir   , "DetectorElement::dataSvc() Directory points to NULL " );
  TransientStore*   store = dir->store(); 
  Assert( 0 != store , "DetectorElement::dataSvc() Store     points to NULL " );
  IDataProviderSvc* svc   = store->dataSvc(); 
  Assert( 0 != svc   , "DetectorElement::dataSvc() Service   points to NULL " );
  m_de_dataSvc = svc;
  return m_de_dataSvc;
};




//
//
// functions from IValidity
//
const ITime&  DetectorElement::validSince ()
{
  if ( 0 != m_de_validSince ){  setValiditySince( time_absolutepast ); }
  return *m_de_validSince; 
};

//
//
//
const ITime&  DetectorElement::validTill  () 
{
  if ( 0 != m_de_validTill ) { setValidityTill( time_absolutefuture ); }
  return *m_de_validTill; 
};

//
//
//
void          DetectorElement::setValidity       ( const ITime& Since , 
								const ITime& Till )
{
  setValiditySince( Since );
  setValidityTill ( Till  );
};

//
//
//
void          DetectorElement::setValiditySince  ( const ITime& Since ) 
{
  if( 0 != m_de_validSince ) { delete m_de_validSince; m_de_validSince = 0 ;} 
  m_de_validSince = new(std::nothrow)  TimePoint( Since );   
};

//
//
//
void          DetectorElement::setValidityTill  ( const ITime& Till ) 
{
  if( 0 != m_de_validTill ){ delete m_de_validTill; m_de_validTill = 0 ;} 
  m_de_validTill = new(std::nothrow)  TimePoint( Till );   
};

//
//
//
StatusCode    DetectorElement::updateValidity    ()
{
  validSince();
  validTill ();
  //
  //  not yet implemented in a proper way 
  //
  return StatusCode::SUCCESS;
};










