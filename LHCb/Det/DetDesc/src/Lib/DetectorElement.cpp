/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.9  2001/08/10 14:59:02  ibelyaev
/// modifications in IGeometryInfo and related classes
/// 
/// ===========================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataDirectory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/TransientStore.h" 
#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/Bootstrap.h"
///
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/DetDesc.h"
/// local !!!
#include "GeoInfo.h"

/** @file DetectorElement.cpp
 *  
 * Implementation of class DetectorElement
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @date xx/xx/xxxx
 */

unsigned long DetectorElement::s_count = 0 ;

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
{
  ///
  m_de_validSince = new(std::nothrow) TimePoint( validSince ) ;
  m_de_validTill  = new(std::nothrow) TimePoint( validTill  ) ; 
  ///
  addRef();
};
////
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
{
  ///
  m_de_validSince = new(std::nothrow) TimePoint( time_absolutepast   ) ;
  m_de_validTill  = new(std::nothrow) TimePoint( time_absolutefuture ) ; 
  ///
  addRef();
};
////
DetectorElement::~DetectorElement()
{
  reset();
  ///
  if ( 0 != m_de_iGeometry     ) 
    { delete m_de_iGeometry     ;  m_de_iGeometry     = 0 ; } 
  ///release
  release();  
};
///
IDataProviderSvc*  DetectorElement::dataSvc () 
{ return DetDesc::detSvc(); }
///
IMessageSvc*  DetectorElement::msgSvc () 
{
  /// static pointer to message service 
  static IMessageSvc* s_msgSvc = 0 ;
  if( 0 == s_msgSvc ) 
    {
      ISvcLocator* svcLoc = Gaudi::svcLocator();
      if( 0 == svcLoc ) 
        { throw DetectorElementException("ISvcLocator* points to NULL!"); }
      StatusCode sc = 
        svcLoc->service( "MessageSvc" , s_msgSvc );
      if( sc.isFailure() ) 
        { throw DetectorElementException("Could not locate 'MessageSvc'!");}
      if( 0 == s_msgSvc ) 
        { throw DetectorElementException("IMessageSvc* points to NULL!");}
      /// add the reference 
      s_msgSvc->addRef();
    }
  ///
  return s_msgSvc; 
};

///
StatusCode 
DetectorElement::queryInterface( const InterfaceID& ID , void** ppI )
{
  if ( 0 == ppI ) { return StatusCode::FAILURE; }
  *ppI = 0 ;
  if      ( IDetectorElement::interfaceID()  == ID ) 
    { *ppI = static_cast<IDetectorElement*> ( this ) ; } 
  else if ( ISerialize::       interfaceID() == ID )
    { *ppI = static_cast<ISerialize*>       ( this ) ; } 
  else if ( IInterface::       interfaceID() == ID ) 
    { *ppI = static_cast<IInterface*>       ( this ) ; } 
  else                                                  
    { return StatusCode::FAILURE                     ; }
  /// add the reference 
  addRef();
  ///
  return StatusCode::SUCCESS;
};
///
unsigned long DetectorElement::addRef  () { return ++s_count ; }
unsigned long DetectorElement::release ()
{ return 0 < s_count ? --s_count : 0 ; }

///
bool DetectorElement::acceptInspector( IInspector* pInspector ) 
{
  if( 0 == pInspector ) { return false; } 
  pInspector->inspectByRef( m_de_iGeometry , this , "GeometryInfo" ); 
  return DataObject::acceptInspector( pInspector ) ;
};  
//
bool DetectorElement::acceptInspector( IInspector* pInspector ) const  
{
  if( 0 == pInspector ) { return false; } 
  pInspector->inspectByRef( m_de_iGeometry , this , "GeometryInfo" ); 
  return DataObject::acceptInspector( pInspector ) ;
};  
//
StreamBuffer& DetectorElement::serialize( StreamBuffer& sb ) const 
{
  DataObject::serialize( sb ) ; 
  sb << *m_de_iGeometry ;
  return sb;
};
//
StreamBuffer& DetectorElement::serialize( StreamBuffer& sb ) 
{
  reset() ; 
  DataObject::serialize( sb ) ; 
  if( 0 == m_de_iGeometry ) 
    { m_de_iGeometry = GeoInfo::createGeometryInfo( this ) ; } 
  sb >> *m_de_iGeometry ;
  return sb;
};
//
std::ostream& DetectorElement::printOut( std::ostream& os ) const
{ 
  os << "DetectorElement::"  << fullpath(); 
  return ( 0 == geometry() ? os : (os << "GeometryInfo::" << geometry()) ); 
};

MsgStream& DetectorElement::printOut( MsgStream& os ) const
{ 
  os << "DetectorElement::"  << fullpath(); 
  return ( 0 == geometry() ? os : (os << "GeometryInfo::" << geometry() ) );
};
/// reset to the initial state/////
IDetectorElement* DetectorElement::reset() 
{
  /// reset geometry
  if( 0 != geometry() ) { geometry()->reset() ;} 
  if( m_de_childrensLoaded ) 
    { std::for_each( childBegin() , childEnd() , 
                     std::mem_fun(&IDetectorElement::reset) );} 
  m_de_childrensLoaded = false ; 
  m_de_childrens.clear()       ; 
  return this;  
};
/////
const IGeometryInfo* 
DetectorElement::createGeometryInfo()
{
  Assert( 0 == geometry() , "Could not create GHOST: Geometry already exist!" );
  m_de_iGeometry = GeoInfo::createGeometryInfo( this );
  return geometry();
};
/////
const IGeometryInfo* 
DetectorElement::createGeometryInfo( const std::string& LogVol )
{
  Assert( 0 == geometry() , "Could not create ORPHAN: Geometry already exist!" );
  m_de_iGeometry = GeoInfo::createGeometryInfo( this , LogVol );
  return geometry();
};
/////
const IGeometryInfo* 
DetectorElement::createGeometryInfo( const std::string& LogVol   , 
                                     const std::string& Support  ,
                                     const std::string& NamePath )
{
  Assert( 0 == geometry() , 
          "Could not create REGULAR(1): Geometry already exist!" );
  m_de_iGeometry = GeoInfo::createGeometryInfo( this     , 
                                                LogVol   , 
                                                Support  , 
                                                NamePath );
  return geometry();
};
//
const IGeometryInfo* 
DetectorElement::createGeometryInfo( const std::string           & LogVol   , 
                                     const std::string           & Support  ,
                                     const ILVolume::ReplicaPath & rPath    ) 
{
  Assert( 0 == geometry() , 
          "Could not create REGULAR(2): Geometry already exist!" );
  m_de_iGeometry = GeoInfo::createGeometryInfo( this    , 
                                                LogVol  , 
                                                Support , 
                                                rPath   );
  return geometry();
};
/// functions from IValidity /
const ITime&  DetectorElement::validSince ()
{
  if ( 0 != m_de_validSince ){  setValiditySince( time_absolutepast ); }
  return *m_de_validSince; 
};
//
const ITime&  DetectorElement::validTill  () 
{
  if ( 0 != m_de_validTill ) { setValidityTill( time_absolutefuture ); }
  return *m_de_validTill; 
};
//
void          DetectorElement::setValidity       ( const ITime& Since , 
                                                                const ITime& Till )
{
  setValiditySince( Since );
  setValidityTill ( Till  );
};
//
void          DetectorElement::setValiditySince  ( const ITime& Since ) 
{
  if( 0 != m_de_validSince ) { delete m_de_validSince; m_de_validSince = 0 ;} 
  m_de_validSince = new(std::nothrow)  TimePoint( Since );   
};
//
void          DetectorElement::setValidityTill  ( const ITime& Till ) 
{
  if( 0 != m_de_validTill ){ delete m_de_validTill; m_de_validTill = 0 ;} 
  m_de_validTill = new(std::nothrow)  TimePoint( Till );   
};
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
//
StatusCode DetectorElement::initialize() {
  // this is a default implementation that does nothing.
  // it is up to the user to override this in a child of DetectorElement
  return StatusCode::SUCCESS;
}



/// addUserParameter

void DetectorElement::addUserParameter (std::string name,
                                        std::string type,
                                        std::string comment,
                                        std::string value,
                                        double d_value) {
  UserParam userParam;
  userParam.type = type;
  userParam.comment = comment;
  userParam.value = value;
  userParam.d_value = d_value;
  userParam.i_value = 0; // this is never used
  userParam.kind = DOUBLE;
  m_userParameters[name] = userParam;
}
  

/// addUserParameter

void DetectorElement::addUserParameter (std::string name,
                                        std::string type,
                                        std::string comment,
                                        std::string value,
                                        double d_value,
                                        int i_value) {
  UserParam userParam;
  userParam.type = type;
  userParam.comment = comment;
  userParam.value = value;
  userParam.d_value = d_value;
  userParam.i_value = i_value;
  userParam.kind = INT;
  m_userParameters[name] = userParam;
}
  

/// addUserParameter

void DetectorElement::addUserParameter (std::string name,
                                        std::string type,
                                        std::string comment,
                                        std::string value) {
  UserParam userParam;
  userParam.type = type;
  userParam.comment = comment;
  userParam.value = value;
  userParam.d_value = 0.0; // this is never used
  userParam.i_value = 0; // this is never used
  userParam.kind = OTHER;
  m_userParameters[name] = userParam;
}
  

/// addUserParameterVector

void DetectorElement::addUserParameterVector (std::string name,
                                              std::string type,
                                              std::string comment,
                                              std::vector<std::string> value,
                                              std::vector<double> d_value) {
  UserParamVector userParamVector;
  userParamVector.type = type;
  userParamVector.comment = comment;
  userParamVector.value = value;
  userParamVector.d_value = d_value;
  // userParamVector.i_value is is never used
  userParamVector.kind = DOUBLE;
  m_userParameterVectors[name] = userParamVector;
}


/// addUserParameterVector

void DetectorElement::addUserParameterVector (std::string name,
                                              std::string type,
                                              std::string comment,
                                              std::vector<std::string> value,
                                              std::vector<double> d_value,
                                              std::vector<int> i_value) {
  UserParamVector userParamVector;
  userParamVector.type = type;
  userParamVector.comment = comment;
  userParamVector.value = value;
  userParamVector.d_value = d_value;
  userParamVector.i_value = i_value;
  userParamVector.kind = INT;
  m_userParameterVectors[name] = userParamVector;
}


/// addUserParameterVector

void DetectorElement::addUserParameterVector (std::string name,
                                              std::string type,
                                              std::string comment,
                                              std::vector<std::string> value) {
  UserParamVector userParamVector;
  userParamVector.type = type;
  userParamVector.comment = comment;
  userParamVector.value = value;
  // userParamVector.d_value is is never used
  // userParamVector.i_value is is never used
  userParamVector.kind = OTHER;
  m_userParameterVectors[name] = userParamVector;
}


/// userParameterType

std::string DetectorElement::userParameterType (std::string name) {
  if (m_userParameters.find(name) == m_userParameters.end()) {
    throw DetectorElementException("No userParameter with this name : \""
                                   + name + "\" !");
  }
  return m_userParameters[name].type;
}
  

/// userParameterComment

std::string DetectorElement::userParameterComment (std::string name) {
  if (m_userParameters.find(name) == m_userParameters.end()) {
    throw DetectorElementException("No userParameter with this name : \""
                                   + name + "\" !");
  }
  return m_userParameters[name].comment;
}
  

/// userParameterAsString

std::string DetectorElement::userParameterAsString (std::string name) {
  if (m_userParameters.find(name) == m_userParameters.end()) {
    throw DetectorElementException("No userParameter with this name : \""
                                   + name + "\" !");
  }
  return m_userParameters[name].value;
}
  

/// userParameterAsInt

int DetectorElement::userParameterAsInt (std::string name) {
  if (m_userParameters.find(name) == m_userParameters.end()) {
    throw DetectorElementException("No userParameter with this name : \""
                                   + name + "\" !");
  }
  if (m_userParameters[name].kind != INT) {
    throw DetectorElementException("userParameter " + name +
                                   " does not have an integer value.");
  }
  return m_userParameters[name].i_value;
}


/// userParameterAsDouble

double DetectorElement::userParameterAsDouble (std::string name) {
  if (m_userParameters.find(name) == m_userParameters.end()) {
    throw DetectorElementException("No userParameter with this name : \""
                                   + name + "\" !");
  }
  if (m_userParameters[name].kind != DOUBLE &&
      m_userParameters[name].kind != INT ) {
    throw DetectorElementException("userParameter " + name +
                                   " does not have a numerical value.");
  }
  return m_userParameters[name].d_value;
}
  

/// userParameter

double DetectorElement::userParameter (std::string name) {
  return userParameterAsDouble (name);
}  
  

/// userParameterVectorType

std::string DetectorElement::userParameterVectorType (std::string name) {
  if (m_userParameterVectors.find(name) == m_userParameterVectors.end()) {
    throw DetectorElementException("No userParameterVector with this name : \""
                                   + name + "\" !");
  }
  return m_userParameterVectors[name].type;
}
  

/// userParameterVectorComment

std::string
DetectorElement::userParameterVectorComment (std::string name) {
  if (m_userParameterVectors.find(name) == m_userParameterVectors.end()) {
    throw DetectorElementException("No userParameterVector with this name : \""
                                   + name + "\" !");
  }
  return m_userParameterVectors[name].comment;
}
  

/// userParameterVectorAsString

std::vector<std::string>
DetectorElement::userParameterVectorAsString (std::string name) {
  if (m_userParameterVectors.find(name) == m_userParameterVectors.end()) {
    throw DetectorElementException("No userParameterVector with this name : \""
                                   + name + "\" !");
  }
  return m_userParameterVectors[name].value;
}
  

/// userParameterVectorAsInt

std::vector<int>
DetectorElement::userParameterVectorAsInt (std::string name) {
  if (m_userParameterVectors.find(name) == m_userParameterVectors.end()) {
    throw DetectorElementException("No userParameterVector with this name : \""
                                   + name + "\" !");
  }
  if (m_userParameterVectors[name].kind != INT) {
    throw DetectorElementException("userParameterVector " + name +
                                   " does not have integer values.");
  }
  return m_userParameterVectors[name].i_value;
}


/// userParameterVectorAsDouble

std::vector<double>
DetectorElement::userParameterVectorAsDouble (std::string name) {
  if (m_userParameterVectors.find(name) == m_userParameterVectors.end()) {
    throw DetectorElementException("No userParameterVector with this name : \""
                                   + name + "\" !");
  }
  if (m_userParameters[name].kind != DOUBLE &&
      m_userParameters[name].kind != INT ) {
    throw DetectorElementException("userParameterVector " + name +
                                   " does not have numerical values.");
  }
  return m_userParameterVectors[name].d_value;
}


/// userParameterVector

std::vector<double>
DetectorElement::userParameterVector (std::string name) {
  return userParameterVectorAsDouble (name);
}


/// userParameters

std::vector<std::string> DetectorElement::userParameters() {
  std::vector<std::string> result;
  for (UserParamMap::iterator it = m_userParameters.begin();
       m_userParameters.end() != it;
       ++it) {
    result.push_back(it->first);
  }
  return result;
}
   

/// userParameterVectors

std::vector<std::string> DetectorElement::userParameterVectors() {
  std::vector<std::string> result;
  for (UserParamVectorMap::iterator it = m_userParameterVectors.begin();
       m_userParameterVectors.end() != it;
       ++it) {
    result.push_back(it->first);
  }
  return result;
}





