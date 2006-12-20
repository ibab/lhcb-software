// $Id: DeVeloSensor.cpp,v 1.25 2006-12-20 15:34:30 cattanem Exp $
//==============================================================================
#define VELODET_DEVELOSENSOR_CPP 1
//==============================================================================
// Include files 

// From Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

#include "DetDesc/Condition.h"

// From Velo 
#include "VeloDet/DeVeloSensor.h"

/** @file DeVeloSensor.cpp
 *
 *  Implementation of class : DeVeloSensor
 *
 *  @author Mark Tobin  Mark.Tobin@cern.ch
 *  @author Kurt Rinnert kurt.rinnert@cern.ch
 */

//==============================================================================
/// Standard constructor
//==============================================================================
DeVeloSensor::DeVeloSensor(const std::string& name) : 
  DetectorElement(name),
  m_stripCapacitanceConditionName("StripCapacitance"),
  m_stripInfoConditionName("StripInfo"),
  m_readoutConditionName("Readout"),
  m_isReadOut(true),
  m_tell1WithoutSensor(false)

{
  ;
}
//==============================================================================
/// Destructor
//==============================================================================
DeVeloSensor::~DeVeloSensor() {}; 
//==============================================================================
/// Object identification
//==============================================================================
const CLID& DeVeloSensor::clID() 
  const { return DeVeloSensor::classID(); }
//==============================================================================
/// Initialisation method
//==============================================================================
StatusCode DeVeloSensor::initialize() 
{
  // Trick from old DeVelo to set the output level
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel=0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if( sc.isSuccess() ) jobSvc->setMyProperties("DeVeloSensor", pmgr);
  if ( 0 < outputLevel ) {
    msgSvc()->setOutputLevel("DeVeloSensor", outputLevel);
  }
  delete pmgr;
  MsgStream msg(msgSvc(), "DeVeloSensor");
  sc = DetectorElement::initialize();
  if(!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DetectorElement" << endreq;
    return sc;
  }
  m_debug   = (msgSvc()->outputLevel("DeVeloSensor") == MSG::DEBUG  ) ;
  m_verbose = (msgSvc()->outputLevel("DeVeloSensor") == MSG::VERBOSE) ;
  if( m_verbose ){
    m_debug = true;
  }

  initSensor();
  IGeometryInfo* geom = geometry();
  m_geometry = geom;
  cacheGeometry();

  // get parent Velo Half box for pattern recognition alignment purposes
  // heirarchy should be sensor -> R/Phi Pair -> Module -> Velo(Left|Right)
  IDetectorElement* halfBox = 
    this->parentIDetectorElement()->parentIDetectorElement()
    ->parentIDetectorElement();
  if(m_debug)
    msg << MSG::DEBUG 
	<< "Great grandparent of " << this->name() << " is " 
	<< halfBox->name() <<endreq;
  m_halfBoxGeom = halfBox->geometry();

  if(m_debug)
    msg << MSG::DEBUG
	<< "Module " << m_module << " sensor " << m_sensorNumber 
	<< " full type " << m_fullType << " z= " << m_z
	<< " R " << isR() 
	<< " Phi " << isPhi()  
	<< " PU " << isPileUp()
	<< " Left " << m_isLeft
	<< " Right " << isRight() 
	<< " Downstream " << isDownstream() << endreq;
  
  sc = registerConditionCallBacks();
  if (sc.isFailure()) {
    msg << MSG::ERROR 
	<< "Failure to register condition update call backs." << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}
//==============================================================================
/// Cache geometry parameters
//==============================================================================
void DeVeloSensor::cacheGeometry() 
{
  m_z = m_geometry->toGlobal(Gaudi::XYZPoint(0,0,0)).z();
}
//==============================================================================
/// Convert local position to global position
//==============================================================================
StatusCode DeVeloSensor::localToGlobal(const Gaudi::XYZPoint& localPos, 
                                       Gaudi::XYZPoint& globalPos) const
{
  globalPos = m_geometry->toGlobal(localPos);
  if(m_verbose) {
    MsgStream msg(msgSvc(), "DeVeloSensor");
    msg << MSG::VERBOSE << "localToGlobal for sensor " << m_sensorNumber
        << " Local x " << localPos.x() << " y " << localPos.y() 
        << " z " << localPos.z()
        << " Global x " << globalPos.x() << " y " << globalPos.y()
        << " z " << globalPos.z()
        << endreq;
  }
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Convert global position to local position 
//==============================================================================
StatusCode DeVeloSensor::globalToLocal(const Gaudi::XYZPoint& globalPos, 
                         Gaudi::XYZPoint& localPos) const
{
  localPos = m_geometry->toLocal(globalPos);
  if(m_verbose) {
    MsgStream msg(msgSvc(), "DeVeloSensor");
    msg << MSG::VERBOSE << "GlobalToLocal for sensor " << m_sensorNumber
        << " Global x " << globalPos.x() << " y " << globalPos.y()
        << " z " << globalPos.z()
        << " Local x " << localPos.x() << " y " << localPos.y() 
        << " z " << localPos.z()
        << endreq;
  }
  return StatusCode::SUCCESS;
}

//============================================================================
/// Convert local position in a sensor to the position in Velo half box
//============================================================================
StatusCode DeVeloSensor::localToVeloHalfBox(const Gaudi::XYZPoint& localPos, 
                                            Gaudi::XYZPoint& boxPos) const
{
  Gaudi::XYZPoint globalPos = m_geometry->toGlobal(localPos);
  boxPos = m_halfBoxGeom->toLocal(globalPos);
  if(m_verbose) {
    MsgStream msg(msgSvc(), "DeVeloSensor");
    msg << MSG::VERBOSE << "localToBox for sensor " << m_sensorNumber
        << " Local " << localPos
        << " Global " << globalPos
	<< " Box " << boxPos
        << endreq;
  }
  return StatusCode::SUCCESS;
}
//============================================================================
/// Convert position in Velo half box to sensor local
//============================================================================
StatusCode DeVeloSensor::veloHalfBoxToLocal(const Gaudi::XYZPoint& boxPos, 
                                            Gaudi::XYZPoint& localPos) const
{
  Gaudi::XYZPoint globalPos = m_halfBoxGeom->toGlobal(boxPos);
  localPos = m_geometry->toLocal(globalPos);
  if(m_verbose) {
    MsgStream msg(msgSvc(), "DeVeloSensor");
    msg << MSG::VERBOSE << "bocToLocal for sensor " << m_sensorNumber
        << " Global " << globalPos
        << " Local " << localPos
	<< " Box " << boxPos
        << endreq;
  }
  return StatusCode::SUCCESS;
}


//============================================================================
/// Convert local position in a sensor to the position in Velo half box
//============================================================================
StatusCode DeVeloSensor::veloHalfBoxToGlobal(const Gaudi::XYZPoint& boxPos, 
                                             Gaudi::XYZPoint& globalPos) const
{
  globalPos = m_halfBoxGeom->toGlobal(boxPos);
  if(m_verbose) {
    MsgStream msg(msgSvc(), "DeVeloSensor");
    msg << MSG::VERBOSE << "localToBox for sensor " << m_sensorNumber
	<< " Box " << boxPos
        << " Global " << globalPos
        << endreq;
  }
  return StatusCode::SUCCESS;
}
//============================================================================
/// Convert position in Velo half box to sensor local
//============================================================================
StatusCode DeVeloSensor::globalToVeloHalfBox(const Gaudi::XYZPoint& globalPos, 
                                             Gaudi::XYZPoint& boxPos) const
{
  boxPos = m_halfBoxGeom->toLocal(globalPos);
  if(m_verbose) {
    MsgStream msg(msgSvc(), "DeVeloSensor");
    msg << MSG::VERBOSE << "bocToLocal for sensor " << m_sensorNumber
        << " Global " << globalPos
	<< " Box " << boxPos
        << endreq;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
/// Returns the number of channels between two channels
//=============================================================================
StatusCode DeVeloSensor::channelDistance(const LHCb::VeloChannelID& start,
                                         const LHCb::VeloChannelID& end,
                                         int& nOffset) const
{
  nOffset = 0;
  unsigned int startStrip = start.strip();
  unsigned int endStrip = end.strip();

  if(start.sensor() != end.sensor()) {
    nOffset=2048;
    return StatusCode::FAILURE;
  }

  if(numberOfStrips()<startStrip || numberOfStrips()<endStrip) {
    return StatusCode::FAILURE;
  }
  // put in some checks for boundaries etc...
  unsigned int startZone = zoneOfStrip(startStrip);
  unsigned int endZone = zoneOfStrip(endStrip);
  if(startZone != endZone) {
    return StatusCode::FAILURE;
  }
  nOffset = endStrip-startStrip;
  return StatusCode::SUCCESS;
}
//=============================================================================
void DeVeloSensor::initSensor()
{
  // Set the sensor type from name in XML
  m_sensorNumber=param<int>("SensorNumber");

  m_innerRadius = param<double>("InnerRadius");
  m_outerRadius = param<double>("OuterRadius");
  m_siliconThickness = param<double>("SiThick");

  m_module   = param<std::string>("Module");
  m_type     = param<std::string>("Type");

  m_isPileUp = m_type.find("Veto")==0;
  m_isR      = m_type.find("R")==0;
  m_isPhi    = m_type.find("Phi")==0;

  if ( m_type == "R" || m_type == "Phi" || m_type == "Veto" ) {
    m_isLeft = param<int>("Left");
    m_isDownstream=param<int>("Downstream");
    m_fullType = m_type + ( (m_isDownstream) ? "DnStrm" : "UpStrm" )
      + ( (m_isLeft) ? "Left" : "Right");
  } else {
    m_isLeft = (m_type == "PhiDL" || m_type == "PhiUL"
                || m_type == "RLeft" || m_type == "VetoL");
    m_isDownstream = (m_type == "PhiDL" || m_type == "PhiDR");
    m_fullType = m_type;
  }


}

//=========================================================================
// non-inlined condition cache accessors    
//=========================================================================

double DeVeloSensor::stripCapacitance(unsigned int strip) const
{
  if (strip < m_stripCapacitance.size()) {
    return m_stripCapacitance[strip];
  } else {
    return 0.0;
  }
}

DeVeloSensor::StripInfo DeVeloSensor::stripInfo(unsigned int strip) const
{
  // Defaults to not read out and not bonded if there is no such strip.
  // This behaviour is handled by the StripInfo default constructor.
  if (strip >=  m_stripInfos.size()) return DeVeloSensor::StripInfo();

  return m_stripInfos[strip];
}

//=========================================================================
// members related to condition caching   
//=========================================================================

StatusCode DeVeloSensor::registerConditionCallBacks() {

  StatusCode sc;
  MsgStream msg(msgSvc(), "DeVeloSensor");

  // strip capacitance condition
  updMgrSvc()->registerCondition(this,
                                 condition(m_stripCapacitanceConditionName.c_str()).path(),
                                 &DeVeloSensor::updateStripCapacitanceCondition);
  
  // strip info condition 
  updMgrSvc()->registerCondition(this,
                                 condition(m_stripInfoConditionName.c_str()).path(),
                                 &DeVeloSensor::updateStripInfoCondition);
  
  // readout flag condition
  updMgrSvc()->registerCondition(this,
                                 condition(m_readoutConditionName.c_str()).path(),
                                 &DeVeloSensor::updateReadoutCondition);

  sc = updMgrSvc()->update(this);
  if(!sc.isSuccess()) {
    msg << MSG::ERROR 
        << "Failed to update sensor conditions!"
        << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode DeVeloSensor::updateStripCapacitanceCondition () {

  m_stripCapacitanceCondition = condition(m_stripCapacitanceConditionName.c_str());
  m_stripCapacitance = m_stripCapacitanceCondition->paramAsDoubleVect("StripCapacitance");

  return StatusCode::SUCCESS;
}

StatusCode DeVeloSensor::updateStripInfoCondition () {

  m_stripInfoCondition = condition(m_stripInfoConditionName.c_str());
  const std::vector<int>& tmpStripInfos 
    = m_stripInfoCondition->paramAsIntVect("StripInfo");
  m_stripInfos.clear();
  m_stripInfos.resize(tmpStripInfos.size());
  std::transform(tmpStripInfos.begin(),tmpStripInfos.end(),
                 m_stripInfos.begin(),
                 ConvertIntToStripInfo());

  return StatusCode::SUCCESS;
}

StatusCode DeVeloSensor::updateReadoutCondition () {

  m_readoutCondition = condition(m_readoutConditionName.c_str());
  m_isReadOut = 
    static_cast<bool>(m_readoutCondition->paramAsInt("ReadoutFlag"));
  m_tell1WithoutSensor = 
    static_cast<bool>(m_readoutCondition->paramAsInt("Tell1WithoutSensor"));

  return StatusCode::SUCCESS;
}

DeVeloSensor::StripInfo DeVeloSensor::ConvertIntToStripInfo::operator() (int i) {

  DeVeloSensor::StripInfo stripInfo;
  
  stripInfo.m_info.set(DeVeloSensor::StripInfo::NOT_READ_OUT,
                       (1 << DeVeloSensor::StripInfo::NOT_READ_OUT) & i);

  stripInfo.m_info.set(DeVeloSensor::StripInfo::NOT_BONDED,
                       (1 << DeVeloSensor::StripInfo::NOT_BONDED) & i);

  stripInfo.m_info.set(DeVeloSensor::StripInfo::BONDED_WITH_NEXT,
                       (1 << DeVeloSensor::StripInfo::BONDED_WITH_NEXT) & i);

  stripInfo.m_info.set(DeVeloSensor::StripInfo::BONDED_WITH_PREVIOUS,
                       (1 << DeVeloSensor::StripInfo::BONDED_WITH_PREVIOUS) & i);

  return stripInfo;
}

int DeVeloSensor::StripInfo::asInt() const
{
  int i=0;

  if (m_info[DeVeloSensor::StripInfo::NOT_READ_OUT]) 
    i |= (1 << DeVeloSensor::StripInfo::NOT_READ_OUT);
  if (m_info[DeVeloSensor::StripInfo::NOT_BONDED]) 
    i |= (1 << DeVeloSensor::StripInfo::NOT_BONDED);
  if (m_info[DeVeloSensor::StripInfo::BONDED_WITH_NEXT]) 
    i |= (1 << DeVeloSensor::StripInfo::BONDED_WITH_NEXT);
  if (m_info[DeVeloSensor::StripInfo::BONDED_WITH_PREVIOUS]) 
    i |= (1 << DeVeloSensor::StripInfo::BONDED_WITH_PREVIOUS);

  return i;
}
