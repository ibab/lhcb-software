// $Id: DetectorElement.cpp,v 1.19 2002-12-09 17:38:59 sponce Exp $ 
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/Bootstrap.h"
///
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/DetDesc.h"
/// local !!!
#include "GeoInfo.h"
#include "AlignmentInfo.h"
#include "CalibrationInfo.h"
#include "ReadOutInfo.h"
#include "SlowControlInfo.h"
#include "FastControlInfo.h"

/** @file DetectorElement.cpp
 *  
 * Implementation of class DetectorElement
 *
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @author Sebastien Ponce
 */

DetectorElement::DetectorElement( const std::string&   /*name*/    ,
                                  const ITime&         validSince  ,   
                                  const ITime&         validTill   )
  : DataObject           (         )
  , m_de_iGeometry       (    0    ) 
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
  , m_services           (    0    )
{
  ///
  m_de_validSince = new TimePoint( validSince ) ;
  m_de_validTill  = new TimePoint( validTill  ) ; 
  m_services = DetDesc::services();
};
///
DetectorElement::DetectorElement( const std::string&   /* name */ )
  : DataObject           (         )
  , m_de_iGeometry       (    0    ) 
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
  , m_services           (    0    )
{
  ///
  m_de_validSince = new TimePoint( time_absolutepast   ) ;
  m_de_validTill  = new TimePoint( time_absolutefuture ) ; 
  m_services = DetDesc::services();
};
////
DetectorElement::~DetectorElement()
{
  // release geometry
  if ( 0 != m_de_iGeometry ) 
    { delete m_de_iGeometry     ;  m_de_iGeometry     = 0 ; } 
  // release validity
  if ( 0 != m_de_validSince ) {
    delete m_de_validSince; m_de_validSince = 0;
  }
  if ( 0 != m_de_validTill ) {
    delete m_de_validTill; m_de_validTill = 0;
  }
  // release services
  m_services->release();
};

IDataProviderSvc* DetectorElement::dataSvc() const {
  return m_services->detSvc();
} 

IMessageSvc* DetectorElement::msgSvc() const {
  return m_services->msgSvc();
} 

IDetectorElement*  DetectorElement::parentIDetectorElement() const {
  IDataManagerSvc* mgr = 0;
  StatusCode sc = dataSvc()->queryInterface(IID_IDataManagerSvc,(void**)&mgr);
  if ( sc.isSuccess() ) {
    IRegistry* pRegParent = 0;      
    sc = mgr->objectParent(this, pRegParent);
    mgr->release();
    if ( sc.isSuccess() && 0 != pRegParent ) {
      return dynamic_cast<IDetectorElement*>(pRegParent->object());
    } 
  }
  return 0;
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
unsigned long DetectorElement::addRef  () { return DataObject::addRef(); }
unsigned long DetectorElement::release () { return DataObject::release(); }

///
// bool DetectorElement::acceptInspector( IInspector* pInspector ) 
// {
//   if( 0 == pInspector ) { return false; } 
//   pInspector->inspectByRef( m_de_iGeometry , this , "GeometryInfo" ); 
//   return DataObject::acceptInspector( pInspector ) ;
// };  
// //
// bool DetectorElement::acceptInspector( IInspector* pInspector ) const  
// {
//   if( 0 == pInspector ) { return false; } 
//   pInspector->inspectByRef( m_de_iGeometry , this , "GeometryInfo" ); 
//   return DataObject::acceptInspector( pInspector ) ;
// };  
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
  os << "DetectorElement::"  << name(); 
  return ( 0 == geometry() ? os : (os << "GeometryInfo::" << geometry()) ); 
};

MsgStream& DetectorElement::printOut( MsgStream& os ) const
{ 
  os << "DetectorElement::"  << name(); 
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
  Assert( 0 == geometry() ,
          "Could not create GHOST: Geometry already exist!" );
  m_de_iGeometry = GeoInfo::createGeometryInfo( this );
  return geometry();
};
/////
const IGeometryInfo* 
DetectorElement::createGeometryInfo( const std::string& LogVol )
{
  Assert( 0 == geometry() ,
          "Could not create ORPHAN: Geometry already exist!" );
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

const IAlignment*
DetectorElement::createAlignment (const std::string& condition) {
  Assert( 0 == alignment() ,
          "Could not create AlignmentInfo: it already exists!" );
  if( 0 == m_de_iAlignment )
    m_de_iAlignment = new AlignmentInfo( this, condition );
  return alignment();
}

const ICalibration*
DetectorElement::createCalibration (const std::string& condition) {
  Assert( 0 == calibration() ,
          "Could not create CalibrationInfo: it already exists!" );
  if( 0 == m_de_iCalibration )
    m_de_iCalibration = new CalibrationInfo( this, condition );
  return calibration();
}

const IReadOut*
DetectorElement::createReadOut (const std::string& condition) {
  Assert( 0 == readOut() ,
          "Could not create ReadOutCalibrationInfo: it already exists!" );
  if( 0 == m_de_iReadOut )
    m_de_iReadOut = new ReadOutInfo( this, condition );
  return readOut();
}

const ISlowControl*
DetectorElement::createSlowControl (const std::string& condition) {
  Assert( 0 == slowControl() ,
          "Could not create SlowControlInfo: it already exists!" );
  if( 0 == m_de_iSlowControl )
    m_de_iSlowControl = new SlowControlInfo( this, condition );
  return slowControl();
}

const IFastControl*
DetectorElement::createFastControl (const std::string& condition) {
  Assert( 0 == fastControl() ,
          "Could not create FastControlInfo: it already exists!" );
  if( 0 == m_de_iFastControl )
    m_de_iFastControl = new FastControlInfo( this, condition );
  return fastControl();
}

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
void DetectorElement::setValidity (const ITime& Since, const ITime& Till)
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
/// (reference to) container of pointers to child detector elements ///////////
IDetectorElement::IDEContainer&
DetectorElement::childIDetectorElements() const {
  /// already loaded? 
  if( m_de_childrensLoaded ) { return m_de_childrens; } 
  /// load them! 
  IDataManagerSvc* mgr = 0;
  StatusCode sc = dataSvc()->queryInterface(IID_IDataManagerSvc,(void**)&mgr);
  if ( sc.isSuccess() ) {
    typedef std::vector<IRegistry*> Leaves;
    Leaves leaves;
    sc = mgr->objectLeaves(this, leaves);
    if ( sc.isSuccess() ) {
      for ( Leaves::iterator it = leaves.begin(); it != leaves.end(); it++ ) {
        Assert (0 != *it , "DirIterator points to NULL!" );
        const std::string& nam = (*it)->identifier();
        SmartDataPtr<IDetectorElement> de( dataSvc() , nam );
        IDetectorElement* ide = de;
        Assert (0 != ide , "Could not load child object="+nam );
        m_de_childrens.push_back( ide  );
      }
      m_de_childrensLoaded = true; 
    }
    mgr->release();
  }
  return m_de_childrens;
};

// ============================================================================
// End 
// ============================================================================
