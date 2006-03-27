// $Id: DeVelo.cpp,v 1.63 2006-03-27 17:26:22 krinnert Exp $
//
// ============================================================================
#define  VELODET_DEVELO_CPP 1
// ============================================================================
// from STL (for std::sort)
#include <algorithm>

// from Kernel
#include "Kernel/SystemOfUnits.h"
#include "Kernel/PhysicalConstants.h"
// trajectory class from LHCbKernel
#include "Kernel/CircleTraj.h"
#include "Kernel/LineTraj.h"

// From Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

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
  jobSvc->setMyProperties("DeVelo", pmgr);
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
  // get all of the pointers to the child detector elements
  std::vector<DeVeloSensor*> veloSensors = findVeloSensors();
  
  msg << MSG::DEBUG << "Found " << veloSensors.size() 
      << " sensors in the XML" << endreq;

  std::vector<DeVeloSensor*>::iterator iDESensor;
  int detElemCount=0;
  m_nRSensors=m_nPhiSensors=m_nPileUpSensors=0;

  // JPP sensors no longer pre-sorted by Z in XML so sort them before
  // storing.
  std::sort(veloSensors.begin(), veloSensors.end(), less_Z());
  
  for(iDESensor = veloSensors.begin() ; iDESensor != veloSensors.end() ; 
      ++iDESensor){
    // Sensors are pre-sorted in XML such that they increase with z position
    m_vpSensor.push_back(*iDESensor);
    unsigned int index=m_vpSensor.size()-1;
    msg << MSG::DEBUG << "type " << (*iDESensor)->fullType() 
        << " index " << index
        << " R " << (*iDESensor)->isR() 
        << " PHI " << (*iDESensor)->isPhi()
        << " PU " << (*iDESensor)->isPileUp() << endmsg;
    if((*iDESensor)->isR()){
      m_vpRSensor.push_back(dynamic_cast<DeVeloRType*>((*iDESensor)));
      m_nRSensors++;
      m_RIndex.push_back(index);
    } else if((*iDESensor)->isPhi()){
      m_vpPhiSensor.push_back(dynamic_cast<DeVeloPhiType*>((*iDESensor)));
      m_nPhiSensors++;
      m_PhiIndex.push_back(index);
    } else if((*iDESensor)->isPileUp()){
      m_vpPUSensor.push_back(dynamic_cast<DeVeloRType*>((*iDESensor)));
      m_nPileUpSensors++;
      m_PUIndex.push_back(index);
    } else {
      msg << MSG::ERROR << "Sensor type is unknown" << endreq;
    }
    msg << MSG::DEBUG << "Sensor number " << m_vpSensor[index]->sensorNumber()
        << " pSensor " << (*iDESensor)->sensorNumber()
        << " type " << m_vpSensor[index]->fullType() 
        << " z = " << m_vpSensor[index]->z()
        << endreq;
    detElemCount++;
  }

  // Check indices are correct
  if(msg.level() == MSG::VERBOSE) {
    for(unsigned int i=0; m_RIndex.size()>i; i++){
      msg << MSG::VERBOSE << "Index of R sensors " << i << " " 
          <<  m_RIndex[i];
      msg << " sensor number " << m_vpSensor[m_RIndex[i]]->sensorNumber()
          << endmsg;
    }
    
  }
  
  
  // Build a list of phi sensors associated to R
  // Dog leg shape requires both phi of the station
  // need to sort sensors into accending order in z
  // get cute and use the STL sort routine with a custom comparator
  //  std::sort(m_vpSensor.begin(), m_vpSensor.end(), less_Z());

  for(unsigned int iSensor=0; iSensor < m_vpSensor.size() ; ++iSensor){
    unsigned int sensor = m_vpSensor[iSensor]->sensorNumber();
    msg << MSG::DEBUG << "Index " << iSensor << " Sensor number " << sensor
        << " is type " << m_vpSensor[iSensor]->fullType() 
        << " at global z = " << m_vpSensor[iSensor]->z()
        << " and in VELO frame " 
        << geometry()->toLocal(Gaudi::XYZPoint(0,
                                                0,
                                                m_vpSensor[iSensor]->z())).z()
        << endreq;
    // Find phi sensors associated to R in each station (group of 4 sensors)
    int station=(iSensor-4)/4;
    unsigned int firstInStation=0;
    if(0 <= station) firstInStation= 4+4*static_cast<unsigned int>(station);
    if(m_vpSensor[iSensor]->isR()){
      for(unsigned int isens=firstInStation; isens<firstInStation+4; isens++){
        unsigned int aSensor = m_vpSensor[isens]->sensorNumber();
        if(m_vpSensor[isens]->isPhi()) {
          m_vpSensor[iSensor]->associateSensor(aSensor);
        }
      }
    }
  }
  
  msg << MSG::DEBUG 
      << " There are " << m_nRSensors << " R type, " 
      << m_nPhiSensors << " Phi type and "
      << m_nPileUpSensors << " pileup type sensors " << endreq;
  /*  msg << "Going to attempt to recalculate z positions\n";
      recalculateZs();*/

  sc = registerConditionCallBacks();
  if (sc.isFailure()) {
    msg << MSG::ERROR << "Failure to register condition update call backs." << endreq;    
    return sc;
  }

  return StatusCode::SUCCESS;
}

// return the sensor number for a point (global frame)
const DeVeloSensor* DeVelo::sensor(const Gaudi::XYZPoint& point) const {
  MsgStream msg(msgSvc(), "DeVelo");
  return sensor(sensitiveVolumeID(point));
}

// return the sensitive volume if for a point in the global frame
const int DeVelo::sensitiveVolumeID(const Gaudi::XYZPoint& point) const {
  MsgStream msg(msgSvc(), "DeVelo");
  std::vector<DeVeloSensor*>::const_iterator iDeVeloSensor;
  for(iDeVeloSensor=m_vpSensor.begin(); iDeVeloSensor!=m_vpSensor.end(); ++iDeVeloSensor){
    Gaudi::XYZPoint localPoint;
    StatusCode sc=(*iDeVeloSensor)->globalToLocal(point,localPoint);
    if(sc.isSuccess()) {
      double z = localPoint.z();
      if(0.20*mm > fabs(z)) {
        return ((*iDeVeloSensor)->sensorNumber());
      }
    }
  }
  msg << MSG::ERROR << "sensitiveVolumeID: no sensitive volume at z = " 
      << point.z() << endmsg;
  return -999;
}

// Return the index of a sensor in the vector of pointers to the sensors 
// which increase with sensor number
unsigned int DeVelo::sensorIndex(unsigned int sensor) const
{
  if(64 > sensor) return m_RIndex[sensor];
  else if(128 > sensor) return m_PhiIndex[sensor-64];
  else return m_PUIndex[sensor-128];
}

// return pointer to sensor
const DeVeloSensor* DeVelo::sensor(unsigned int sensorNumber) const
{
  return m_vpSensor[sensorIndex(sensorNumber)];
}

// return pointer to R sensor
const DeVeloRType* DeVelo::rSensor(unsigned int sensorNumber) const
{
  return dynamic_cast<DeVeloRType*>(m_vpSensor[sensorIndex(sensorNumber)]);
}

// return pointer to Phi sensor
const DeVeloPhiType* DeVelo::phiSensor(unsigned int sensorNumber) const
{
  return dynamic_cast<DeVeloPhiType*>(m_vpSensor[sensorIndex(sensorNumber)]);
}

// Access to a strip's geometry, for Panoramix
// from strip number and R sensor number, returns Z, R and a phi range.
// in local frame 
StatusCode DeVelo::stripLimitsR( unsigned int sensor, 
                                 unsigned int strip,
                                 double& z, 
                                 double& radius, 
                                 double& phiMin, 
                                 double& phiMax )  const {
  DeVeloRType * rPtr = dynamic_cast<DeVeloRType*>(m_vpSensor[sensorIndex(sensor)]);
  if(rPtr){    
    StatusCode sc=rPtr->stripLimits(strip,radius,phiMin,phiMax);
    if(rPtr->isRight()){
      phiMin += pi;
      phiMax += pi;
    }
    z = rPtr->z();
    return sc;
  }else {
      MsgStream msg( msgSvc(), "DeVelo" );
      msg << MSG::ERROR 
          << "Asked for phi type sensor as if r/pu type" << endreq;
      return StatusCode::FAILURE;
  }
}
  
// from strip number and phi sensor number, returns the two end points
// in local frame 
StatusCode DeVelo::stripLimitsPhi( unsigned int sensor, 
                                   unsigned int strip,
                                   Gaudi::XYZPoint& begin, 
                                   Gaudi::XYZPoint& end ) const {
  DeVeloPhiType * phiPtr = 
    dynamic_cast<DeVeloPhiType*>(m_vpSensor[sensorIndex(sensor)]);
  if(phiPtr){
    StatusCode sc = phiPtr->stripLimits(strip, begin, end);
   return sc;
  } else {
    MsgStream msg( msgSvc(), "DeVelo" );
    msg << MSG::ERROR 
        << "Asked for r/pu type sensor as if phi type" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
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
  }

  if (i != tell1Ids.end() || j != sensorNumbers.end()) {
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR 
        << "Number of TELL1 and sensor IDs do not match!"
        << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

LHCb::Trajectory* DeVelo::trajectory(const LHCb::LHCbID& lID, 
                                     const double offset) const {

  // look up the trajectory
 
  LHCb::Trajectory* tTraj = 0;

  if ( !lID.isVelo()){
     throw GaudiException( "The LHCbID is not of VELO type!",
                           "DeVELO.cpp",StatusCode::FAILURE );
  }
  
  LHCb::VeloChannelID id = lID.veloID();

  if( id.isRType() || id.isPileUp() ){
    // r type is a circle
    double z = 0.;
    double radius = 0.;
    double phiMin = 0.;
    double phiMax = 0.;
    unsigned int rSensor=id.sensor();
    unsigned int rStrip = id.strip();
    DeVeloRType * rPtr = dynamic_cast<DeVeloRType*>(m_vpSensor[sensorIndex(rSensor)]);
    z = rPtr->z();
    StatusCode sc = rPtr->stripLimits( rStrip, radius, phiMin, phiMax );
    if(!sc){
      throw GaudiException( "The trajectory could not be made",
                            "DeVELO.cpp",StatusCode::FAILURE );
    }
    // offset is offset on R
    radius += rPtr->rPitch(rStrip) * offset;
    
    // start with coords of center and both ends in local frame
    Gaudi::XYZPoint lOrigin(0.,0.,0.);
    Gaudi::XYZPoint lEnd1(radius*cos(phiMin),radius*sin(phiMin),z);
    Gaudi::XYZPoint lEnd2(radius*cos(phiMax),radius*sin(phiMax),z);
    
    // move to global frame
    Gaudi::XYZPoint gOrigin, gEnd1, gEnd2;
    rPtr->localToGlobal(lOrigin, gOrigin);
    rPtr->localToGlobal(lEnd1, gEnd1);
    rPtr->localToGlobal(lEnd2, gEnd2);
    
    // put into trajectory
    tTraj = new LHCb::CircleTraj(gOrigin,gEnd1-gOrigin,gEnd2-gOrigin,radius);
    
  }else{
    
    // phi type is a line
    Gaudi::XYZPoint lEnd1, lEnd2;
    unsigned int phiSensor=id.sensor();
    unsigned int phiStrip=id.strip();
    DeVeloPhiType * phiPtr = dynamic_cast<DeVeloPhiType*>(m_vpSensor[sensorIndex(phiSensor)]);
    StatusCode sc = phiPtr->stripLimits(phiStrip,lEnd1,lEnd2);
    if(!sc){
      throw GaudiException( "The trajectory could not be made",
                            "DeVELO.cpp",StatusCode::FAILURE );
    }
    
    // need to also grab next strip in local frame to get offset effect
    Gaudi::XYZPoint lNextEnd1, lNextEnd2;
    // check direction of offset
    if(offset >= 0.){
      sc = phiPtr->stripLimits(phiStrip+1,lNextEnd1,lNextEnd2);
      if(!sc){
        throw GaudiException( "The trajectory could not be made",
                              "DeVELO.cpp",StatusCode::FAILURE );
      }
      lEnd1 += (lNextEnd1-lEnd1)*offset;
      lEnd2 += (lNextEnd2-lEnd2)*offset;
    }else{
      sc = phiPtr->stripLimits( phiStrip-1,lNextEnd1,lNextEnd2);
      if(!sc){
        throw GaudiException( "The trajectory could not be made",
                              "DeVELO.cpp",StatusCode::FAILURE );
      }
      lEnd1 += (lEnd1-lNextEnd1)*offset;
      lEnd2 += (lEnd2-lNextEnd2)*offset;
    }

    Gaudi::XYZPoint gEnd1, gEnd2;
    phiPtr->localToGlobal(lEnd1, gEnd1);
    phiPtr->localToGlobal(lEnd2, gEnd2);

    // put into trajectory
    tTraj = new LHCb::LineTraj(gEnd1,gEnd2);
  }
    
  return tTraj;  
}
