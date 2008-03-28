// $Id: DeVeloSensor.cpp,v 1.34 2008-03-28 16:31:15 dhcroft Exp $
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
#include "Kernel/VeloChannelID.h"

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
  m_stripNoiseConditionName("StripNoise"),
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
  if( sc.isSuccess() ) sc = jobSvc->setMyProperties("DeVeloSensor", pmgr);
  if( !sc ) return sc;
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
  sc = cacheGeometry();
  if(!sc){    
    msg << MSG::ERROR <<"Failed to cache geometry"<<endmsg; 
    return sc;
  }

  // get parent Velo Half box for pattern recognition alignment purposes
  // heirarchy should be sensor -> R/Phi Pair -> Module -> Velo(Left|Right)
  IDetectorElement* halfBox = 
    this->parentIDetectorElement()->parentIDetectorElement()->parentIDetectorElement();
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
StatusCode DeVeloSensor::cacheGeometry() 
{
  m_z = m_geometry->toGlobal(Gaudi::XYZPoint(0,0,0)).z();
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
    nOffset=m_numberOfStrips;
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
    m_isLeft = 0 != param<int>("Left");
    m_isDownstream = 0 != param<int>("Downstream");
    m_fullType = m_type + ( (m_isDownstream) ? "DnStrm" : "UpStrm" )
      + ( (m_isLeft) ? "Left" : "Right");
  } else {
    m_isLeft = (m_type == "PhiDL" || m_type == "PhiUL"
                || m_type == "RLeft" || m_type == "VetoL");
    m_isDownstream = (m_type == "PhiDL" || m_type == "PhiDR");
    m_fullType = m_type;
  }
  // test new parameters ....
  if(m_isR) { 
    m_isDownstream = 0 != param<int>("DownstreamR");
  } else if (m_isPhi) {
    m_isDownstream = 0 != param<int>("DownstreamPhi");
  }
}

//=========================================================================
// members related to condition caching   
//=========================================================================

StatusCode DeVeloSensor::registerConditionCallBacks() {

  StatusCode sc;
  MsgStream msg(msgSvc(), "DeVeloSensor");

  // strip capacitance condition
  updMgrSvc()->registerCondition(this,
                                 condition(m_stripNoiseConditionName.c_str()).path(),
                                 &DeVeloSensor::updateStripNoiseCondition);
  
  // strip info condition 
  updMgrSvc()->registerCondition(this,
                                 condition(m_stripInfoConditionName.c_str()).path(),
                                 &DeVeloSensor::updateStripInfoCondition);
  
  // readout flag condition
  updMgrSvc()->registerCondition(this,
                                 condition(m_readoutConditionName.c_str()).path(),
                                 &DeVeloSensor::updateReadoutCondition);

  // geometry conditions (just m_z for now)
  updMgrSvc()->
    registerCondition(this,this->m_geometry,&DeVeloSensor::cacheGeometry);

  sc = updMgrSvc()->update(this);
  if(!sc.isSuccess()) {
    msg << MSG::ERROR 
        << "Failed to update sensor conditions!"
        << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode DeVeloSensor::updateStripNoiseCondition () {

  m_stripNoiseCondition = condition(m_stripNoiseConditionName.c_str());
  m_stripNoise = m_stripNoiseCondition->paramAsDoubleVect("StripNoise");

  if (m_stripNoise.size() != m_numberOfStrips) {
    MsgStream msg(msgSvc(), "DeVeloSensor");
    msg << MSG::ERROR << "Strip noise condition size does not match number of strips!" << endmsg;
    return StatusCode::FAILURE;
  }
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

  if (m_stripInfos.size() != m_numberOfStrips) {
    MsgStream msg(msgSvc(), "DeVeloSensor");
    msg << MSG::ERROR << "Strip info condition size does not match number of strips!" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode DeVeloSensor::updateReadoutCondition () {

  m_readoutCondition = condition(m_readoutConditionName.c_str());
  m_isReadOut          = (0 != m_readoutCondition->paramAsInt("ReadoutFlag"));
  m_tell1WithoutSensor = (0 != m_readoutCondition->paramAsInt("Tell1WithoutSensor"));
  
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
