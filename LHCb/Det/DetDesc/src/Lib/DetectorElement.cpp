// $Id: DetectorElement.cpp,v 1.37 2006-10-19 08:24:40 cattanem Exp $
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/SmartDataPtr.h"

///
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/DetDesc.h"

#include "DetDesc/Condition.h"

#include "DetDesc/GeoInfo.h"
#include "DetDesc/AlignmentInfo.h"
#include "DetDesc/CalibrationInfo.h"
#include "DetDesc/ReadOutInfo.h"
#include "DetDesc/SlowControlInfo.h"
#include "DetDesc/FastControlInfo.h"

/** @file DetectorElement.cpp
 *  
 * Implementation of class DetectorElement
 *
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @author Sebastien Ponce
 * @author Marco Clemencic <marco.clemencic@cern.ch>
 */
DetectorElement::DetectorElement( const std::string&   /* name */ )
  : ParamValidDataObject           (         )
  , m_de_iGeometry       (    0    ) 
  , m_de_iAlignment      (    0    ) 
  , m_de_iCalibration    (    0    )   
  , m_de_iReadOut        (    0    ) 
  , m_de_iSlowControl    (    0    ) 
  , m_de_iFastControl    (    0    ) 
  , m_de_childrensLoaded (  false  )
  , m_de_childrens       (         )
  , m_services           (    0    )
{
  m_services = DetDesc::services();
};

DetectorElement::~DetectorElement()
{
  // release geometry
  if ( 0 != m_de_iGeometry ) 
    { delete m_de_iGeometry     ;  m_de_iGeometry     = 0 ; } 
  // release alignment
  if ( 0 != m_de_iAlignment ) 
    { delete m_de_iAlignment; m_de_iAlignment = 0; } 
  // release calibration
  if ( 0 != m_de_iCalibration ) 
    { delete m_de_iCalibration; m_de_iCalibration = 0; }   
  // release readout
  if ( 0 != m_de_iReadOut ) 
    { delete m_de_iReadOut; m_de_iReadOut = 0; } 
  // release slowcontrol
  if ( 0 != m_de_iSlowControl ) 
    { delete m_de_iSlowControl; m_de_iSlowControl = 0; } 
  // release fastcontrol
  if ( 0 != m_de_iFastControl ) 
    { delete m_de_iFastControl; m_de_iFastControl = 0; }

  // release services
  m_services->release();
};

IDataProviderSvc* DetectorElement::dataSvc() const {
  return m_services->detSvc();
} 

IMessageSvc* DetectorElement::msgSvc() const {
  return m_services->msgSvc();
} 

IUpdateManagerSvc* DetectorElement::updMgrSvc() const {
  return m_services->updMgrSvc();
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

unsigned long DetectorElement::addRef  () {
  return ParamValidDataObject::addRef();
}

unsigned long DetectorElement::release () {
  return ParamValidDataObject::release();
}

StatusCode 
DetectorElement::queryInterface( const InterfaceID& ID , void** ppI )
{
  if (0 == ppI) {
    return StatusCode::FAILURE;
  }
  *ppI = 0 ;
  if (IDetectorElement::interfaceID() == ID) {
    *ppI = static_cast<IDetectorElement*> (this);
  } else if (IInterface::interfaceID() == ID) {
    *ppI = static_cast<IInterface*> (this);
  } else {
    return StatusCode::FAILURE;
  }
  /// add the reference 
  addRef();
  ///
  return StatusCode::SUCCESS;
};

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

// ----------------------------------------------------------------------
SmartRef<Condition> DetectorElement::condition(const std::string &name) const {
  ConditionMap::const_iterator cond = m_de_conditions.find(name);
  if ( cond != m_de_conditions.end() ) {
    return cond->second;
  }
  return SmartRef<Condition>();
}

void DetectorElement::createCondition(std::string &name, std::string &path) {
  ConditionMap::const_iterator cond = m_de_conditions.find(name);
  Assert(cond == m_de_conditions.end(),
         "Could not add condition: " + name + " already present!" );
  long hint = linkMgr()->addLink(path,0);
  m_de_conditions.insert(ConditionMap::value_type(name,SmartRef<Condition>(this,hint)));
}

//-- N. Gilardi; 2005.07.08 ---------------------------------------------
/// Get the list of existing conditions.
std::vector<std::string> DetectorElement::conditionNames() const {
  std::vector<std::string> v;
  ConditionMap::const_iterator      i;

  for (i = m_de_conditions.begin(); i != m_de_conditions.end(); ++i){
    v.push_back(i->first);
  }
  return v;
}
// ----------------------------------------------------------------------

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
/////
const IGeometryInfo*
DetectorElement::createGeometryInfo( const std::string& LogVol   , 
                                     const std::string& Support  ,
                                     const std::string& NamePath,
                                     const std::string& alignmentPath)
{
  Assert( 0 == geometry() , 
          "Could not create REGULAR(1): Geometry already exist!" );
  m_de_iGeometry = GeoInfo::createGeometryInfo( this     , 
                                                LogVol   , 
                                                Support  , 
                                                NamePath ,
                                                alignmentPath);
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
//=============================================================================
const IGeometryInfo* 
DetectorElement::createGeometryInfo( const std::string           & LogVol   , 
                                     const std::string           & Support  ,
                                     const ILVolume::ReplicaPath & rPath,
                                     const std::string& alignmentPath) 
{
  Assert( 0 == geometry() , 
          "Could not create REGULAR(2): Geometry already exist!" );
  m_de_iGeometry = GeoInfo::createGeometryInfo( this    , 
                                                LogVol  , 
                                                Support , 
                                                rPath   ,
                                                alignmentPath);
  return geometry();
};
//=============================================================================
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
//
StatusCode DetectorElement::initialize() {
  // this is a default implementation that does nothing.
  // it is up to the user to override this in a child of DetectorElement
  return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------

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

const ParamValidDataObject *DetectorElement::params() const {
  return this;
}
/// sensitive volume identifier ///////////////////////////////////////////////
const int DetectorElement::sensitiveVolumeID(const Gaudi::XYZPoint& globalPoint) const 
{
  if (!isInside(globalPoint)) return -1;
  const IDetectorElement* child = childDEWithPoint(globalPoint);
  return (child) ? child->sensitiveVolumeID(globalPoint) : -1;
};

bool DetectorElement::isInside(const Gaudi::XYZPoint& globalPoint) const {
  const IGeometryInfo* gi = geometry();
  return (0!=gi) ? gi->isInside(globalPoint) : false; 
  //  return (geometry()) ? geometry()->isInside(globalPoint) : false;
};

const IDetectorElement* DetectorElement::childDEWithPoint(const Gaudi::XYZPoint& globalPoint) const 
{
  IDetectorElement::IDEContainer::const_iterator iDE = childBegin();
  for (; iDE!=childEnd(); ++iDE) {
    DetectorElement* pDE = dynamic_cast<DetectorElement*>((*iDE));
    if ( pDE && pDE->isInside(globalPoint)) return *iDE;
  }
  return 0;
};
// ============================================================================
const std::string& DetectorElement::name () const {
  static std::string s_empty = "";
  IRegistry* pReg = registry();
  return (0!=pReg) ? pReg->identifier() : s_empty;
};
// ============================================================================
// End 
// ============================================================================
