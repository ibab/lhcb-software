// $Id: DeVelo.cpp,v 1.80 2007-03-22 10:27:50 dhcroft Exp $
//
// ============================================================================
#define  VELODET_DEVELO_CPP 1
// ============================================================================
// from STL (for std::sort)
#include <algorithm>

// From Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
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
  m_tell1ToSensorsConditionName("TELL1ToSensors")
{
  ; 
} 

//
// Standard Destructor
DeVelo::~DeVelo() {
  // should be all handled by the TDS and the automatic deletion of the vectors
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
  if( !sc ) return sc;
  if ( 0 < outputLevel ) {
    msgSvc()->setOutputLevel("DeVelo", outputLevel);
  }
  delete pmgr;
  MsgStream msg( msgSvc(), "DeVelo" );
  msg << MSG::DEBUG << "Initialising DeVelo " << endreq;
  // Initialise the detector element
  sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc ; 
  }
  m_debug   = (msgSvc()->outputLevel("DeVelo") == MSG::DEBUG  ) ;
  m_verbose = (msgSvc()->outputLevel("DeVelo") == MSG::VERBOSE) ;
  // get all of the pointers to the child detector elements
  std::vector<DeVeloSensor*> veloSensors = findVeloSensors();
  
  msg << MSG::DEBUG << "Found " << veloSensors.size() 
      << " sensors in the XML" << endreq;

  std::vector<DeVeloSensor*>::iterator iDESensor;
  m_nSensors=m_nRSensors=m_nPhiSensors=m_nPileUpSensors=0;
  m_nLeftSensors=m_nRightSensors=0;
  m_nLeftRSensors=m_nRightRSensors=0;
  m_nLeftPhiSensors=m_nRightPhiSensors=0;
  m_nLeftPUSensors=m_nRightPUSensors=0;

  // JPP sensors no longer pre-sorted by Z in XML so sort them before
  // storing.
  std::sort(veloSensors.begin(), veloSensors.end(), less_Z());
  
  for(iDESensor = veloSensors.begin() ; iDESensor != veloSensors.end() ; 
      ++iDESensor,++m_nSensors){
    // Sensors are pre-sorted in XML such that they increase with z position
    m_vpSensors.push_back(*iDESensor);
    unsigned int index=m_vpSensors.size()-1;
    msg << MSG::DEBUG << "type " << (*iDESensor)->fullType() 
        << " index " << index
        << " R " << (*iDESensor)->isR() 
        << " PHI " << (*iDESensor)->isPhi()
        << " PU " << (*iDESensor)->isPileUp() << endmsg;
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
      msg << MSG::ERROR << "Sensor type is unknown" << endreq;
    }
    m_sensors[m_vpSensors[index]->sensorNumber()]= m_vpSensors[index];
    msg << MSG::DEBUG << "Module " << m_vpSensors[index]->module()
        << " sensor " << m_vpSensors[index]->sensorNumber()
        << " type " << m_vpSensors[index]->fullType() 
        << " z = " << m_vpSensors[index]->z()
        << " and in VELO frame " 
        << geometry()->toLocal(Gaudi::XYZPoint(0,0,m_vpSensors[index]->z())).z()
        << endreq;
  }

  
  msg << MSG::DEBUG << "There are " << m_nSensors << " sensors: Left " << m_nLeftSensors
      << " Right " << m_nRightSensors << endreq;
  msg << MSG::DEBUG << "There are " << m_nRSensors << " R sensors: Left " << m_nLeftRSensors
      << " Right " << m_nRightRSensors << endreq;
  msg << MSG::DEBUG << "There are " << m_nPhiSensors << " Phi sensors: Left " << m_nLeftPhiSensors
      << " Right " << m_nRightPhiSensors << endreq;
  msg << MSG::DEBUG << "There are " << m_nPileUpSensors << " Pile Up sensors: Left " << m_nLeftPUSensors
      << " Right " << m_nRightPUSensors << endreq;

  sc = registerConditionCallBacks();
  if (sc.isFailure()) {
    msg << MSG::ERROR << "Failure to register condition update call backs." << endreq;    
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
const int DeVelo::sensitiveVolumeID(const Gaudi::XYZPoint& point) const {
  std::vector<DeVeloSensor*>::const_iterator iDeVeloSensor;
  for(iDeVeloSensor=m_vpSensors.begin(); iDeVeloSensor!=m_vpSensors.end(); ++iDeVeloSensor){
    Gaudi::XYZPoint localPoint=(*iDeVeloSensor)->globalToLocal(point);
    double z = localPoint.z();
    if(m_sensVolCut > fabs(z)) {
      return ((*iDeVeloSensor)->sensorNumber());
    }
  }
  MsgStream msg(msgSvc(), "DeVelo");
  msg << MSG::ERROR << "sensitiveVolumeID: no sensitive volume at z = " 
      << point.z() << endmsg;
  return -999;
}

// return pointer to sensor
const DeVeloSensor* DeVelo::sensor(unsigned int sensorNumber) const
{
  return m_sensors[sensorNumber];
}

// return pointer to R sensor
const DeVeloRType* DeVelo::rSensor(unsigned int sensorNumber) const
{
  return dynamic_cast<DeVeloRType*>(m_sensors[sensorNumber]);
}

// return pointer to Phi sensor
const DeVeloPhiType* DeVelo::phiSensor(unsigned int sensorNumber) const
{
  return dynamic_cast<DeVeloPhiType*>(m_sensors[sensorNumber]);
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
  MsgStream msg( msgSvc(), "DeVelo" );  
  std::vector<IDetectorElement*> veloSensors =
    detElem->childIDetectorElements();

  msg << MSG::DEBUG << "scanDetectorElement" << endreq;
  
  std::vector<IDetectorElement*>::iterator iVeloSensors=veloSensors.begin();

  for (;iVeloSensors!=veloSensors.end(); ++iVeloSensors ) {
    msg << MSG::DEBUG << std::setw(12) << std::setiosflags(std::ios::left)
        << (*iVeloSensors)->name() << endreq;
    DeVeloSensor* pSensor = dynamic_cast<DeVeloSensor*>((*iVeloSensors));
    if (pSensor) {
      sensors.push_back(pSensor);
      msg << MSG::DEBUG << "Storing detector " <<   (*iVeloSensors)->name()
          << endreq;
      
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
  MsgStream msg(msgSvc(), "DeVelo");

  // TELL1 to sensor mapping condition
  updMgrSvc()->registerCondition(this,
                                 condition(m_tell1ToSensorsConditionName.c_str()).path(),
                                 &DeVelo::updateTell1ToSensorsCondition);
  
  sc = updMgrSvc()->update(this);
  if(!sc.isSuccess()) {
    msg << MSG::ERROR 
        << "Failed to update VELO conditions!"
        << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode DeVelo::updateTell1ToSensorsCondition()
{
  MsgStream msg(msgSvc(), "DeVelo");

  m_tell1ToSensorsCondition = condition(m_tell1ToSensorsConditionName.c_str());
  const std::vector<int>& tell1Ids 
    = m_tell1ToSensorsCondition->paramAsIntVect("Tell1Id");
  const std::vector<int>& sensorNumbers 
    = m_tell1ToSensorsCondition->paramAsIntVect("SensorId");
  
  m_sensorByTell1Id.clear();

  std::vector<int>::const_iterator i = tell1Ids.begin();
  std::vector<int>::const_iterator j = sensorNumbers.begin();

  for (; i != tell1Ids.end() && j != sensorNumbers.end(); ++i, ++j) {
    unsigned int tell1Id      = static_cast<unsigned int>(*i);
    unsigned int sensorNumber = static_cast<unsigned int>(*j);

    m_sensorByTell1Id[tell1Id] = sensor(sensorNumber);
    m_tell1IdBySensorNumber[sensorNumber] = tell1Id;
  }

  // check for trivial size mismatch bug in CondDB
  if (i != tell1Ids.end() || j != sensorNumbers.end()) {
    msg << MSG::ERROR 
        << "Number of TELL1 and sensor IDs do not match!"
        << endreq;
    return StatusCode::FAILURE;
  }

  // check consistency with sensor readout flags. this assumes the latter are updated first.
  unsigned int tell1Id;
  for (std::vector<DeVeloSensor*>::const_iterator si = m_vpSensors.begin();
       si != m_vpSensors.end(); 
       ++si) {
    const DeVeloSensor* sensor = *si;
    if (sensor->isReadOut() && !tell1IdBySensorNumber(sensor->sensorNumber(),tell1Id)) {
      msg << MSG::ERROR 
          << "Sensor " << sensor->sensorNumber()
          << " is considered active but not mapped to a TELL1 source ID!"
          << endreq;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}
