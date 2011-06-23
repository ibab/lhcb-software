// ============================================================================
#define  VELODET_DEVELO_CPP 1
// ============================================================================
// from STL (for std::sort)
#include <algorithm>

// From Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "DetDesc/Condition.h"

// Local
#include "VeloDet/DeVelo.h"

/** @file DeVelo.cpp
 *
 *  Implementation of class :  DeVelo
 *
 *  @author David Hutchcroft David.Hutchcroft@cern.ch
 *  @author Mark Tobin Mark.Tobin@cern.ch
 *  @author Kurt Rinnert kurt.rinnert@cern.ch
 */


// **  Standard Constructors

DeVelo::DeVelo( const std::string& name ) :
  DetectorElement(name),
  m_tell1ToSensorsConditionName("TELL1ToSensors"),
  m_msgStream(NULL)
{
  ;
}

//
// Standard Destructor
DeVelo::~DeVelo() {
  delete m_msgStream;
}

// ============================================================================
// object identification
// ============================================================================
const CLID& DeVelo::clID () const { return DeVelo::classID() ; }


// ============================================================================
// intialization method
// ============================================================================
StatusCode DeVelo::initialize() {

  // Trick from old DeVelo to set the output level
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel=0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if( sc.isSuccess() ) sc = jobSvc->setMyProperties("DeVelo", pmgr);
  if ( 0 < outputLevel ) {
    msgSvc()->setOutputLevel("DeVelo", outputLevel);
  }
  delete pmgr;
  if( !sc ) return sc;
  m_debug   = (msgSvc()->outputLevel("DeVelo") == MSG::DEBUG  ) ;
  m_verbose = (msgSvc()->outputLevel("DeVelo") == MSG::VERBOSE) ;
  if( m_verbose ) m_debug = true;

  if( m_debug ) msg() << MSG::DEBUG << "Initialising DeVelo " << endmsg;
  // Initialise the detector element
  sc = DetectorElement::initialize();
  if( sc.isFailure() ) {
    msg() << MSG::ERROR << "Failure to initialize DetectorElement" << endmsg;
    return sc ;
  }
  // get all of the pointers to the child detector elements
  std::vector<DeVeloSensor*> veloSensors = findVeloSensors();

  if( m_debug ) msg() << MSG::DEBUG << "Found " << veloSensors.size()
		      << " sensors in the XML" << endmsg;

  std::vector<DeVeloSensor*>::iterator iDESensor;
  m_nSensors=m_nRSensors=m_nPhiSensors=m_nPileUpSensors=0;
  m_nLeftSensors=m_nRightSensors=0;
  m_nLeftRSensors=m_nRightRSensors=0;
  m_nLeftPhiSensors=m_nRightPhiSensors=0;
  m_nLeftPUSensors=m_nRightPUSensors=0;

  // JPP sensors no longer pre-sorted by Z in XML so sort them before
  // storing.
  std::sort(veloSensors.begin(), veloSensors.end(), less_Z());

  // this determines the size of our pseudo map
  unsigned int maxSensorNumber=0;
  for(iDESensor = veloSensors.begin() ; iDESensor != veloSensors.end() ;
      ++iDESensor){
    if (maxSensorNumber < (*iDESensor)->sensorNumber())
      maxSensorNumber = (*iDESensor)->sensorNumber();
  }

  // ok, now we now the size of our pseudo map
  m_sensors.resize(maxSensorNumber+1,0);

  for(iDESensor = veloSensors.begin() ; iDESensor != veloSensors.end() ;
      ++iDESensor,++m_nSensors){
    // Sensors are pre-sorted in XML such that they increase with z position
    m_vpSensors.push_back(*iDESensor);
    unsigned int index=m_vpSensors.size()-1;
    if(m_debug) msg() << MSG::DEBUG << "type " << (*iDESensor)->fullType()
		      << " index " << index
		      << " R " << (*iDESensor)->isR()
		      << " PHI " << (*iDESensor)->isPhi()
		      << " PU " << (*iDESensor)->isPileUp()
		      << " SNO " <<  (*iDESensor)->sensorNumber()
		      << endmsg;

    bool isLeftSensor=false;
    // Check if sensor is on Left/Right side of LHCb
    if((*iDESensor)->isLeft()){
      isLeftSensor=true;
      m_vpLeftSensors.push_back(m_vpSensors.back());
      m_nLeftSensors++;
    } else {
      m_vpRightSensors.push_back(m_vpSensors.back());
      m_nRightSensors++;
    }
    if((*iDESensor)->isR()){
      m_vpRSensors.push_back(dynamic_cast<DeVeloRType*>((*iDESensor)));
      m_nRSensors++;
      if(isLeftSensor){
        m_vpLeftRSensors.push_back(m_vpRSensors.back());
        m_nLeftRSensors++;
      } else {
        m_vpRightRSensors.push_back(m_vpRSensors.back());
        m_nRightRSensors++;
      }

    } else if((*iDESensor)->isPhi()){
      m_vpPhiSensors.push_back(dynamic_cast<DeVeloPhiType*>((*iDESensor)));
      m_nPhiSensors++;
      if(isLeftSensor){
        m_vpLeftPhiSensors.push_back(m_vpPhiSensors.back());
        m_nLeftPhiSensors++;
      } else {
        m_vpRightPhiSensors.push_back(m_vpPhiSensors.back());
        m_nRightPhiSensors++;
      }

    } else if((*iDESensor)->isPileUp()){
      m_vpPUSensors.push_back(dynamic_cast<DeVeloRType*>((*iDESensor)));
      m_nPileUpSensors++;
      if(isLeftSensor){
        m_vpLeftPUSensors.push_back(m_vpPUSensors.back());
        m_nLeftPUSensors++;
      } else {
        m_vpRightRSensors.push_back(m_vpPUSensors.back());
        m_nRightPUSensors++;
      }
    } else {
      msg() << MSG::ERROR << "Sensor type is unknown" << endmsg;
    }
    m_sensors[m_vpSensors[index]->sensorNumber()]= m_vpSensors[index];
    if(m_debug) msg() << MSG::DEBUG << "Module " 
		      << m_vpSensors[index]->module()
		      << " sensor " << m_vpSensors[index]->sensorNumber()
		      << " type " << m_vpSensors[index]->fullType()
		      << " z = " << m_vpSensors[index]->z()
		      << " and in VELO frame "
		      << geometry()->toLocal(Gaudi::XYZPoint(0,0,m_vpSensors[index]->z())).z()
		      << endmsg;
  }

  // Set the associated and other side sensor links.  This makes assumptions about the
  // semantics of sensor number.  While this is a bad idea in general it is
  // defendable inside the detector element itself.
  for (std::vector<DeVeloRType*>::const_iterator iRS=leftRSensorsBegin();
       iRS != leftRSensorsEnd();
       ++iRS) {

    // associated sensors on the left side
    DeVeloRType*   lRS = *iRS;
    DeVeloPhiType* lPS = const_cast<DeVeloPhiType*>(phiSensor(lRS->sensorNumber()+64));
    lRS->setAssociatedPhiSensor(lPS);
    if (lPS) lPS->setAssociatedRSensor(lRS);

    // associated sensors on the right side
    DeVeloRType*   rRS = const_cast<DeVeloRType*>(rSensor(lRS->sensorNumber()+1));
    DeVeloPhiType* rPS = const_cast<DeVeloPhiType*>(phiSensor(lPS->sensorNumber()+1));
    if (rRS) rRS->setAssociatedPhiSensor(rPS);
    if (rPS) rPS->setAssociatedRSensor(rRS);

    // other side sensor links
    if (rRS) rRS->setOtherSideRSensor(lRS);
    if (rRS) rRS->setOtherSidePhiSensor(lPS);
    if (lRS) lRS->setOtherSideRSensor(rRS);
    if (lRS) lRS->setOtherSidePhiSensor(rPS);
    if (rPS) rPS->setOtherSidePhiSensor(lPS);
    if (rPS) rPS->setOtherSideRSensor(lRS);
    if (lPS) lPS->setOtherSidePhiSensor(rPS);
    if (lPS) lPS->setOtherSideRSensor(rRS);

  }

  if(m_debug){
    msg() << MSG::DEBUG << "There are " << m_nSensors << " sensors: Left " << m_nLeftSensors
	  << " Right " << m_nRightSensors << endmsg;
    msg() << MSG::DEBUG << "There are " << m_nRSensors << " R sensors: Left " << m_nLeftRSensors
	  << " Right " << m_nRightRSensors << endmsg;
    msg() << MSG::DEBUG << "There are " << m_nPhiSensors << " Phi sensors: Left " << m_nLeftPhiSensors
	  << " Right " << m_nRightPhiSensors << endmsg;
    msg() << MSG::DEBUG << "There are " << m_nPileUpSensors << " Pile Up sensors: Left " << m_nLeftPUSensors
	  << " Right " << m_nRightPUSensors << endmsg;
  }
  sc = registerConditionCallBacks();
  if (sc.isFailure()) {
    msg() << MSG::ERROR << "Failure to register condition update call backs." << endmsg;
    return sc;
  }

  // Get the value of the sensitiveVolumeCut from the XML.
  m_sensVolCut=param<double>("sensitiveVolumeCut");

  return StatusCode::SUCCESS;
}

// return the sensor number for a point (global frame)
const DeVeloSensor* DeVelo::sensor(const Gaudi::XYZPoint& point) const {
  return sensor(sensitiveVolumeID(point));
}

// return the sensitive volume if for a point in the global frame
int DeVelo::sensitiveVolumeID(const Gaudi::XYZPoint& point) const {
  std::vector<DeVeloSensor*>::const_iterator iDeVeloSensor;
  for(iDeVeloSensor=m_vpSensors.begin(); iDeVeloSensor!=m_vpSensors.end(); ++iDeVeloSensor){
    Gaudi::XYZPoint localPoint=(*iDeVeloSensor)->globalToLocal(point);
    double z = localPoint.z();
    if(m_sensVolCut > fabs(z)) {
      return ((*iDeVeloSensor)->sensorNumber());
    }
  }
  msg() << MSG::ERROR << "sensitiveVolumeID: no sensitive volume at z = "
        << point.z() << endmsg;
  return -999;
}

//=============================================================================
std::vector<DeVeloSensor*> DeVelo::findVeloSensors()
{

  std::vector<DeVeloSensor*> mySensors;

  scanDetectorElement(this, mySensors);
  return mySensors;

}
//=============================================================================
void DeVelo::scanDetectorElement(IDetectorElement* detElem,
                                 std::vector<DeVeloSensor*>& sensors)
{
  std::vector<IDetectorElement*> veloSensors =
    detElem->childIDetectorElements();

  if(m_debug) msg() << MSG::DEBUG << "scanDetectorElement" << endmsg;

  std::vector<IDetectorElement*>::iterator iVeloSensors=veloSensors.begin();

  for (;iVeloSensors!=veloSensors.end(); ++iVeloSensors ) {
    if(m_debug) msg() << MSG::DEBUG << std::setw(12) 
		      << std::setiosflags(std::ios::left)
		      << (*iVeloSensors)->name() << endmsg;
    DeVeloSensor* pSensor = dynamic_cast<DeVeloSensor*>((*iVeloSensors));
    if (pSensor) {
      sensors.push_back(pSensor);
      if(m_debug) msg() << MSG::DEBUG << "Storing detector " 
			<<   (*iVeloSensors)->name()
			<< endmsg;

    }

    scanDetectorElement(*iVeloSensors, sensors);
  }
}
//=========================================================================
// members related to condition caching
//=========================================================================

const DeVeloSensor* DeVelo::sensorByTell1Id(unsigned int tell1Id) const
{
  std::map<unsigned int, const DeVeloSensor*>::const_iterator mi;

  mi =  m_sensorByTell1Id.find(tell1Id);

  if (m_sensorByTell1Id.end() == mi) return 0;

  return (*mi).second;
}

bool DeVelo::tell1IdBySensorNumber(unsigned int sensorNumber, unsigned int& tell1Id) const
{
  std::map<unsigned int, unsigned int>::const_iterator mi;

  mi =  m_tell1IdBySensorNumber.find(sensorNumber);

  if (m_tell1IdBySensorNumber.end() == mi) return false;

  tell1Id = (*mi).second;

  return true;
}

StatusCode DeVelo::registerConditionCallBacks()
{
  StatusCode sc;

  // TELL1 to sensor mapping condition
  // Also contains mapping of sensor to Liverpool module condition
  updMgrSvc()->registerCondition(this,
                                 condition(m_tell1ToSensorsConditionName.c_str()).path(),
                                 &DeVelo::updateTell1ToSensorsCondition);

  // Half box offset  cache
  if(m_nLeftSensors > 0) {
    updMgrSvc()->
      registerCondition(this,(*leftSensorsBegin())->geometry(),&DeVelo::updateLeftHalfBoxOffset);
  }
  if(m_nRightSensors > 0) {
    updMgrSvc()->
      registerCondition(this,(*rightSensorsBegin())->geometry(),&DeVelo::updateRightHalfBoxOffset);
  }
  sc = updMgrSvc()->update(this);
  if(!sc.isSuccess()) {
    msg() << MSG::ERROR
          << "Failed to update VELO conditions!"
          << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode DeVelo::updateTell1ToSensorsCondition()
{
  m_tell1ToSensorsCondition = condition(m_tell1ToSensorsConditionName.c_str());
  const std::vector<int>& tell1Ids
    = m_tell1ToSensorsCondition->paramAsIntVect("Tell1Id");
  const std::vector<int>& sensorNumbers
    = m_tell1ToSensorsCondition->paramAsIntVect("SensorId");
  const std::vector<int>& moduleIds
    = m_tell1ToSensorsCondition->paramAsIntVect("ModuleId");

  // check for trivial size mismatch bug in CondDB
  if ( tell1Ids.size() != sensorNumbers.size() || tell1Ids.size() != sensorNumbers.size()) {
    msg() << MSG::ERROR
          << "Number of TELL1 and sensor/module IDs do not match!"
          << endmsg;
    return StatusCode::FAILURE;
  }

  m_sensorByTell1Id.clear();

  std::vector<int>::const_iterator i = tell1Ids.begin();
  std::vector<int>::const_iterator j = sensorNumbers.begin();
  std::vector<int>::const_iterator k = moduleIds.begin();

  for (; i != tell1Ids.end() && j != sensorNumbers.end() && k != moduleIds.end(); ++i, ++j, ++k) {
    unsigned int tell1Id      = static_cast<unsigned int>(*i);
    unsigned int sensorNumber = static_cast<unsigned int>(*j);
    unsigned int moduleId      = static_cast<unsigned int>(*k);

    const DeVeloSensor* sens=sensor(sensorNumber);
    if(!sens) {
      msg() << MSG::ERROR << "No such sensor " << sensorNumber << endmsg;
      return StatusCode::FAILURE;
    }
    sens->m_moduleId=moduleId;
    m_sensorByTell1Id[tell1Id] = sensor(sensorNumber);
    m_tell1IdBySensorNumber[sensorNumber] = tell1Id;
  }

  // check consistency with sensor readout flags. this assumes the latter are updated first.
  unsigned int tell1Id;
  for (std::vector<DeVeloSensor*>::const_iterator si = m_vpSensors.begin();
       si != m_vpSensors.end();
       ++si) {
    const DeVeloSensor* sensor = *si;
    if (sensor->isReadOut() && !tell1IdBySensorNumber(sensor->sensorNumber(),tell1Id)) {
      msg() << MSG::ERROR
            << "Sensor " << sensor->sensorNumber()
            << " is considered active but not mapped to a TELL1 source ID!"
            << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode DeVelo::updateLeftHalfBoxOffset() {

  Gaudi::XYZPoint localZero(0.,0.,0.);

  Gaudi::XYZPoint global = (*leftSensorsBegin())->veloHalfBoxToGlobal(localZero);
  m_halfBoxOffsets[LeftHalf] = global-localZero;

  return StatusCode::SUCCESS;
}

StatusCode DeVelo::updateRightHalfBoxOffset() {

  Gaudi::XYZPoint localZero(0.,0.,0.);

  Gaudi::XYZPoint global = (*rightSensorsBegin())->veloHalfBoxToGlobal(localZero);
  m_halfBoxOffsets[RightHalf] = global-localZero;

  return StatusCode::SUCCESS;
}
