// $Id: DeVelo.cpp,v 1.34 2004-02-03 16:55:56 mtobin Exp $
//
// ============================================================================
#define  VELODET_DEVELO_CPP 1
// ============================================================================
// from STL
#include <algorithm>

// From Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"

// from Det/VeloDet
#include "VeloDet/DeVelo.h"

/** @file DeVelo.cpp
 *
 *  Implementation of class :  DeVelo
 *
 *  @author David Hutchcroft David.Hutchcroft@cern.ch
 */


// **  Standard Constructors

DeVelo::DeVelo( const std::string& name )
  :  DetectorElement     ( name       )
{
  
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

  MsgStream msg( msgSvc(), "DeVelo" );
  msg << MSG::DEBUG << "Initialising DeVelo " << endreq;
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
  // Initialise the detector element
  sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc ; 
  }

  // get all of the pointers to the child detector elements
  // Childern of DeVelo (this) are the sensors
  std::vector<IDetectorElement*> veloSensors =
    this->childIDetectorElements();
  msg << MSG::INFO << "Found " << veloSensors.size() 
      << " sensors in the XML" << endreq;
  
  IDetectorElement* testDetElem= veloSensors[1];
  DeVeloSensor* testVeloSensor = dynamic_cast<DeVeloSensor*>(testDetElem);

  if (!testDetElem) {
    msg << MSG::ERROR << "Dodgy IDetectorElement"<< endreq;
  }
  if (!testVeloSensor) {
    msg << MSG::ERROR << "Cannot cast DeVeloSensor* onto IDetectorElement*" 
	<< endreq;
  }

  std::vector<IDetectorElement*>::iterator iDESensor;
  int detElemCount=0;
  unsigned int RLeft=0;
  unsigned int PhiLeft=1;
  unsigned int RRight=50;
  unsigned int PhiRight=51;
  unsigned int PileUp=100;
  for(iDESensor = veloSensors.begin() ; iDESensor != veloSensors.end() ; 
      ++iDESensor){
    //    IDetectorElement myDetElem=*iDESensor;
    DeVeloSensor * pSensor = dynamic_cast<DeVeloSensor*>(*iDESensor);
    if(!pSensor){
      msg << MSG::ERROR 
          << "Could not cast children of DeVelo to DeVeloSensors" << endreq;
      return StatusCode::FAILURE;
    }
    m_vpSensor.push_back(pSensor);
    unsigned int index=m_vpSensor.size()-1;
    if(m_vpSensor[index]->isR() && m_vpSensor[index]->isRight()){
      m_vpSensor[index]->sensorNumber(RRight);
      RRight += 2;
    } else if(m_vpSensor[index]->isR() && !m_vpSensor[index]->isRight()){
      m_vpSensor[index]->sensorNumber(RLeft);
      RLeft += 2;
    } else if(m_vpSensor[index]->isPhi() && m_vpSensor[index]->isRight()) {
      m_vpSensor[index]->sensorNumber(PhiRight);
      PhiRight += 2;
    } else if(m_vpSensor[index]->isPhi() && !m_vpSensor[index]->isRight()) {
      m_vpSensor[index]->sensorNumber(PhiLeft);
      PhiLeft += 2;
    } else if(m_vpSensor[index]->isPileUp()){
      m_vpSensor[index]->sensorNumber(PileUp);
      PileUp += 2;
    }
    msg << MSG::DEBUG << " Sensor number " << m_vpSensor[index]->sensorNumber()
        << " is type " << m_vpSensor[index]->type() 
        << " at z = " << m_vpSensor[index]->z()
        << endreq;
    detElemCount++;
  }

  // Build a list of phi sensors associated to R
  // Dog leg shape requires both phi of the station
  // need to sort sensors into accending order in z
  // get cute and use the STL sort routine with a custom comparator
  std::sort(m_vpSensor.begin(), m_vpSensor.end(), less_Z());

  for(unsigned int iSensor=0; iSensor < m_vpSensor.size() ; ++iSensor){
    //m_sensorZ.push_back(m_vpSensor[iSensor]->z());
    unsigned int sensor = m_vpSensor[iSensor]->sensorNumber();
    msg << MSG::INFO << "Index " << iSensor << " Sensor number " << sensor
        << " is type " << m_vpSensor[iSensor]->type() 
        << " at z = " << m_vpSensor[iSensor]->z()
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
  // Sort the sensors into increasing sensor number
  std::sort(m_vpSensor.begin(), m_vpSensor.end(), less_sensor());

  m_sensorZ.clear();
  for(unsigned int iSensor=0; iSensor < m_vpSensor.size() ; iSensor++){
    m_sensorZ.push_back(m_vpSensor[iSensor]->z());
    unsigned int sensor=m_vpSensor[iSensor]->sensorNumber();
    // work out what sensor type we have here
    if(this->isRSensor(sensor)){
      m_indexRType.push_back(sensor);
    }
    if(this->isPhiSensor(sensor)){
      m_indexPhiType.push_back(sensor);
    }
    if(this->isPileUpSensor(sensor)){
      m_indexPileUpType.push_back(sensor);
    }
    msg << MSG::INFO << "Index " << iSensor << " Sensor number " << sensor
        << " is type " << m_vpSensor[iSensor]->type() 
        << " at z = " << m_vpSensor[iSensor]->z()
        << endreq;
  }
  m_nRType = m_indexRType.size();
  m_nPhiType = m_indexPhiType.size();
  m_nPileUpType = m_indexPileUpType.size();
  
  msg << MSG::DEBUG 
      << " There are " << m_nRType << " R type, " 
      << m_nPhiType << " Phi type and "
      << m_nPileUpType << " pileup type sensors " << endreq;

  if(m_nRType < 2 || m_nPhiType < 2 || m_nPileUpType < 2){
    msg << MSG::ERROR 
        << " This code requies at least two of each type of sensor"
        << endreq;
    return StatusCode::FAILURE;
  }                                                         

  return StatusCode::SUCCESS;
}

// return the sensor number for a point (global frame)
unsigned int DeVelo::sensorNumber(const HepPoint3D& point){
  double z = point.z();
  for(unsigned int index=0;numberSensors()>index;index++){
    if(0.250*mm > fabs(z - m_sensorZ[index])) {
      return this->sensorNumber(index);
    }
  }
  return 0;
}
// Return the index of the sensor in the list of sensors
unsigned int DeVelo::sensorIndex(unsigned int sensor)
{
  if(50 > sensor) {
    return sensor;
  } else if(50 <= sensor && 100 > sensor) {
    return sensor-8;
  } else {
    return ((sensor%100)/2)+84;
  }
}
// Gives the VeloChannelID and offset (in fraction of a pitch width) 
// associated to a 3D position. with pitch width in mm
// Sign convention is offset is +- 0.5 
// with +ve in increasing strip number  (global frame) 
StatusCode DeVelo::pointToChannel(const HepPoint3D &point, 
                                  VeloChannelID &channel,
                                  double &localOffset,
                                  double &pitch) {
  unsigned int sensor = this->sensorNumber(point);
  return this->pointToChannel(point,sensor,channel,localOffset,pitch);
}

// pointToChannel if sensor known (global frame)
StatusCode DeVelo::pointToChannel(const HepPoint3D &point, 
                                  const unsigned int &sensor,
                                  VeloChannelID &channel,
                                  double &localOffset,
                                  double &pitch) {
  unsigned int index=this->sensorIndex(sensor);
  return m_vpSensor[index]->pointToChannel(point,channel,localOffset,pitch);
}

// Residual of 3D point to a VeloChannelID
// returns offset in mm from closest point on channel 
/*StatusCode  DeVelo::residual(const HepPoint3D &point, 
                             const VeloChannelID &channel,
                             double &residual,
                             double &chi2) const{
  
  return this->residual(point,channel,0.,0.5,residual,chi2);
  }*/
StatusCode  DeVelo::residual(const HepPoint3D &point, 
                             const VeloChannelID &channel,
                             double &residual,
                             double &chi2) {
  unsigned int index=this->sensorIndex(channel.sensor());
  return m_vpSensor[index]->residual(point,channel,residual,chi2);
}

// Residual of 3D point to a VeloChannelID + offset in fraction of a channel
// and width of the cluster in channel widths (for the chi2)
// returns offset in mm, and chi^2 from position to point 
StatusCode DeVelo::residual(const HepPoint3D &point, 
                            const VeloChannelID &channel,
                            const double &localOffset,
                            const double &width,
                            double &residual,
                            double &chi2) {
  unsigned int index=sensorIndex(channel.sensor());
  return m_vpSensor[index]->residual(point,channel,localOffset,
                                     width,residual,chi2);
}

// Convert global 3D Point to local 3D point in frame of the sensor 
StatusCode DeVelo::globalToLocal(const unsigned int &sensor,
                                 const HepPoint3D &global,
                                 HepPoint3D &local) {
  return m_vpSensor[sensorIndex(sensor)]->globalToLocal(global,local);
}

// Convert local 3D Point to global 3D point in frame of LHCb
StatusCode DeVelo::localToGlobal(const unsigned int &sensor,
                                 const HepPoint3D &local,
                                 HepPoint3D &global) {
  return m_vpSensor[sensorIndex(sensor)]->localToGlobal(local,global);
}

// Get the nth (signed) neighbour strip to a given VeloChannelID
StatusCode DeVelo::neighbour(const VeloChannelID &startChannel,
                             const int &offset,
                             VeloChannelID &channel) {
  unsigned int index=sensorIndex(startChannel.sensor());
  return m_vpSensor[index]->neighbour(startChannel,offset,channel);
}

// Check the distance in strips between two channelIDs
StatusCode DeVelo::channelDistance(const VeloChannelID &startChannel,
                                   const VeloChannelID &endChannel,
                                   int &offset) {
  unsigned int sen1= startChannel.sensor();
  unsigned int sen2= endChannel.sensor();
  if(sen1 != sen2){
    offset = 2048;
    return StatusCode::FAILURE;
  }else{
    sen1 = sensorIndex(sen1);
    return m_vpSensor[sen1]->channelDistance(startChannel,endChannel,offset);
  }
}

// Returns the vector of phi sensor numbers one can match with the 
// specified R sensor number and vice versa
StatusCode DeVelo::sensorAssociated( unsigned int sensor, 
                               std::vector<unsigned int> &assocSensor ) 
{
  if(isRSensor(sensor)){
    assocSensor = m_vpSensor[sensorIndex(sensor)]->associatedSensors();
  } else {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// returns the number of zones in this sensor
unsigned int DeVelo::numberOfZones( unsigned int sensor) {
  return m_vpSensor[sensorIndex(sensor)]->numberOfZones();
}

// returns the phi "zone" of the r strip or r zone of phi strip
unsigned int DeVelo::zoneOfStrip( VeloChannelID channel) {
  unsigned int index=sensorIndex(channel.sensor());
  return m_vpSensor[index]->zoneOfStrip(channel.strip());
}

// returns the phi "zone" of the r strip or r zone of phi strip
unsigned int DeVelo::zoneOfStrip( unsigned int strip, 
                                  unsigned int sensor ) {
  return m_vpSensor[sensorIndex(sensor)]->zoneOfStrip(strip);
}

// Number of strips in each zone
unsigned int DeVelo::stripsInZone( unsigned int sensor, 
                                   unsigned int zone ) {
  return m_vpSensor[sensorIndex(sensor)]->stripsInZone(zone);
}

// returns the local radius of the strip
StatusCode DeVelo::rOfStrip( VeloChannelID channel,
			     double &radius) {
  // check whether sensor is R type using m_vpSensor[sensor]->type()
  //  write method bool DeVelo::isR(unsigned int sensor), isPhi etc.
  unsigned int sensor=channel.sensor();
  unsigned int index=sensorIndex(sensor);
  unsigned int strip=channel.strip();
  if(DeVeloRType* rPtr = dynamic_cast<DeVeloRType*>(m_vpSensor[index])){
    radius = rPtr->rOfStrip(strip);
  }else{
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
// returns the local radius of the strip+fractional distance to strip
StatusCode DeVelo::rOfStrip( VeloChannelID channel, double fraction,
			     double &radius) {
  // check whether sensor is R type using m_vpSensor[sensor]->type()
  //  write method bool DeVelo::isR(unsigned int sensor), isPhi etc.
  unsigned int index=sensorIndex(channel.sensor());
  if(DeVeloRType* rPtr = dynamic_cast<DeVeloRType*>(m_vpSensor[index])){
    radius = rPtr->rOfStrip(channel.strip(),fraction);
  }else{
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// returns the R pitch at the given channelID
StatusCode DeVelo::rPitch( VeloChannelID channel,
			   double &rPitch ) {
  unsigned int index=sensorIndex(channel.sensor());
  if(DeVeloRType* rPtr = dynamic_cast<DeVeloRType*>(m_vpSensor[index])){
    rPitch = rPtr->rPitch(channel.strip());
    return StatusCode::SUCCESS;
  }else{
    return StatusCode::FAILURE;
  }
}

// returns the R pitch at the given channelID
StatusCode DeVelo::rPitch( VeloChannelID channel, double fraction,
			   double &rPitch ) {
  unsigned int sensor=channel.sensor();
  unsigned int index=sensorIndex(sensor);
  if(DeVeloRType* rPtr = dynamic_cast<DeVeloRType*>(m_vpSensor[index])){
    rPitch = rPtr->rPitch(channel.strip(),fraction);
    return StatusCode::SUCCESS;
  }else{
    return StatusCode::FAILURE;
  }
}

// returns the phi of the strip at the specified radius for this sensor.
StatusCode DeVelo::phiOfStrip( VeloChannelID channel,
			       double radius, double &phiOfStrip ) {
  return this->phiOfStrip(channel,0.,radius,phiOfStrip);
}

// returns the phi of the strip +fractional distance to strip
// at the specified radius for this sensor.
StatusCode DeVelo::phiOfStrip( VeloChannelID channel,
                               double fraction, double radius, 
                               double &phiOfStrip ) {
  unsigned int sensor=channel.sensor();
  unsigned int index=sensorIndex(sensor);
  if(DeVeloPhiType* phiPtr = dynamic_cast<DeVeloPhiType*>(m_vpSensor[index])){
    phiOfStrip = phiPtr->phiOfStrip(channel.strip(),fraction,radius);
    return StatusCode::SUCCESS;
  }else{
    return StatusCode::FAILURE;
  }
}

// The stereo angle of the phi strips in radians,
// signed so that positive indicates phi increases with radius 
StatusCode DeVelo::phiStereo( VeloChannelID channel, double radius,
                              double &phiStereo) {
  unsigned int index=sensorIndex(channel.sensor());
  DeVeloPhiType * phiPtr = 
    dynamic_cast<DeVeloPhiType*>(m_vpSensor[index]);
  if(phiPtr){
    phiStereo = phiPtr->phiOffset(radius);
    return StatusCode::SUCCESS;
  }else{
    return StatusCode::FAILURE;
  }
}

// returns the Phi pitch (in mm) at the given radius (sensor local) 
StatusCode DeVelo::phiPitch(VeloChannelID channel, double radius,
			     double &phiPitch ) {
  unsigned int index=sensorIndex(channel.sensor());
  DeVeloPhiType * phiPtr = 
    dynamic_cast<DeVeloPhiType*>(m_vpSensor[index]);
  if(phiPtr){
    phiPitch = phiPtr->phiPitch(radius);
    return StatusCode ::SUCCESS;
  }else{
    return StatusCode::FAILURE;
  }
}

// returns the Phi pitch (in radians) for a given strip
StatusCode DeVelo::phiPitch(VeloChannelID channel,
			     double &phiPitch ) {
  unsigned int index=sensorIndex(channel.sensor());  
  DeVeloPhiType * phiPtr = 
    dynamic_cast<DeVeloPhiType*>(m_vpSensor[index]);
  if(phiPtr){
    phiPitch = phiPtr->phiPitch(channel.strip());
    return StatusCode::SUCCESS;
  }else{
    return StatusCode::FAILURE;
  }
}

StatusCode DeVelo::distToOrigin( VeloChannelID channel,
                             double &distance)  
{
  unsigned int index=sensorIndex(channel.sensor());
  DeVeloPhiType * phiPtr = 
    dynamic_cast<DeVeloPhiType*>(m_vpSensor[index]);
  if(phiPtr){
    distance = phiPtr->distToOrigin(channel.strip());
    return StatusCode::SUCCESS;
  }else{
    return StatusCode::FAILURE;
  }
}

// return the minimum sensitive radius of an R wafer, local frame
double DeVelo::rMin(unsigned int sensor) {
  return m_vpSensor[sensorIndex(sensor)]->innerRadius();
}
// return the maximum sensitive radius of an R wafer, local frame
double DeVelo::rMax(unsigned int sensor) {
  return m_vpSensor[sensorIndex(sensor)]->outerRadius();
}

// return the minimum sensitive radius of an R wafer in a zone, local frame
// 4 zones (different phi) for R sensors and 2 zones (different R and stereo)
// for the phi sensors 
double DeVelo::rMin(unsigned int sensor, unsigned int zone) {
  return m_vpSensor[sensorIndex(sensor)]->rMin(zone);
}
// return the maximum sensitive radius of an R wafer in a zone, local frame
// 4 zones (different phi) for R sensors and 2 zones (different R and stereo)
// for the phi sensors 
double DeVelo::rMax(unsigned int sensor, unsigned int zone) {
  return m_vpSensor[sensorIndex(sensor)]->rMax(zone);
}

// Smallest Phi at R (local frame) of the phi strips at R
StatusCode DeVelo::phiMin(unsigned int sensor, double radius, 
			     double &phiMin) {
  DeVeloPhiType * phiPtr = 
     dynamic_cast<DeVeloPhiType*>(m_vpSensor[sensorIndex(sensor)]);
  if(phiPtr){
    phiMin = phiPtr->phiMin(radius);
    return StatusCode::SUCCESS;
  }else{
    return StatusCode::FAILURE;
  }
}

// Smallest Phi at R (local frame) of the phi strips at R
StatusCode DeVelo::phiMax(unsigned int sensor, double r, 
			     double &phiMax) {
  DeVeloPhiType * phiPtr = 
     dynamic_cast<DeVeloPhiType*>(m_vpSensor[sensorIndex(sensor)]);
  if(phiPtr){
    phiMax = phiPtr->phiMax(r);
    return StatusCode::SUCCESS;
  }else{
    return StatusCode::FAILURE;
  }
}

// Smallest Phi at R (local frame) of the r strips in the zone
StatusCode DeVelo::phiMin(unsigned int sensor, unsigned int zone,
			   double &phiMin) {
  if(this->isRSensor(sensor)){
    DeVeloRType * rPtr = 
      dynamic_cast<DeVeloRType*>(m_vpSensor[sensorIndex(sensor)]);
    phiMin = rPtr->phiMinZone(zone);
    return StatusCode::SUCCESS;
  }else{
    return StatusCode::FAILURE;
  }
}

// Largest Phi (local frame) of the R strips in the zone
StatusCode DeVelo::phiMax(unsigned int sensor, unsigned int zone,
			   double &phiMax) {
  if(this->isRSensor(sensor)){
    DeVeloRType * rPtr = 
      dynamic_cast<DeVeloRType*>(m_vpSensor[sensorIndex(sensor)]);
    phiMax = rPtr->phiMaxZone(zone);
    return StatusCode::SUCCESS;
  }else{
    return StatusCode::FAILURE;
  }
}

// minimum phi at R (overlap in x) for a given zone
StatusCode DeVelo::phiMin(unsigned int sensor, unsigned int zone,
                          double radius, double &phiMin) {
  if(this->isRSensor(sensor)){
    DeVeloRType * rPtr = 
      dynamic_cast<DeVeloRType*>(m_vpSensor[sensorIndex(sensor)]);
    phiMin = rPtr->phiMinZone(zone,radius);
    return StatusCode::SUCCESS;
  }else{
    return StatusCode::FAILURE;
  }
}

// maximum phi at R (overlap in x) for a given zone
StatusCode DeVelo::phiMax(unsigned int sensor, unsigned int zone,
                          double radius, double &phiMax) {
  if(this->isRSensor(sensor)){
    DeVeloRType * rPtr = 
      dynamic_cast<DeVeloRType*>(m_vpSensor[sensorIndex(sensor)]);
    phiMax = rPtr->phiMaxZone(zone,radius);
    return StatusCode::SUCCESS;
  }else{
    return StatusCode::FAILURE;
  }
}

// returns the silicon thickness
double DeVelo::siliconThickness ( unsigned int sensor ) {
  return m_vpSensor[sensorIndex(sensor)]->siliconThickness();
}

// returns the number of strips per sensor.
unsigned int DeVelo::numberStrips(unsigned int sensor) {
  return m_vpSensor[sensorIndex(sensor)]->numberOfStrips();
}
    
// returns the capacitance of the strip.
double DeVelo::stripCapacitance(VeloChannelID channel) {
  unsigned int index=sensorIndex(channel.sensor());
  return m_vpSensor[index]->stripCapacitance(channel.strip());
}

// Access to a strip's geometry, for Panoramix
// from strip number and R sensor number, returns Z, R and a phi range.
// in local frame 
StatusCode DeVelo::stripLimitsR( unsigned int sensor, unsigned int strip,
				 double& z, double& radius, 
				 double& phiMin, double& phiMax )  {
  z = zSensor(sensor);
  VeloChannelID channel(sensor,strip);
  if(this->isRSensor(sensor)){    
    DeVeloRType * rPtr = 
      dynamic_cast<DeVeloRType*>(m_vpSensor[sensorIndex(sensor)]);
    StatusCode sc=rPtr->stripLimits(strip,radius,phiMin,phiMax);
    if(m_vpSensor[sensorIndex(sensor)]->isRight()){
      phiMin += pi;
      phiMax += pi;
    }
    return sc;
  }else if(this->isPileUpSensor(sensor)){
    //DeVeloPileUpType * puPtr = 
    //dynamic_cast<DeVeloPileUpType*>(m_vpSensor[sensorIndex(sensor)]);
  } else {
      MsgStream msg( msgSvc(), "DeVelo" );
      msg << MSG::ERROR 
          << "Asked for phi type sensor as if r/pu type" << endreq;
      return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
  
// from strip number and phi sensor number, returns the two end points
// in local frame 
StatusCode DeVelo::stripLimitsPhi( unsigned int sensor, unsigned int strip,
				   HepPoint3D& begin, HepPoint3D& end )  {
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
// Construct 3d point from R/phi channels (rFrac is fractional distance 
// to strip (+/-0.5))
//=============================================================================
StatusCode DeVelo::makeSpacePoint( VeloChannelID rChan, 
                                   double rFrac,
                                   VeloChannelID phiChan,
                                   double phiFrac,
                                   HepPoint3D& point, 
                                   double&  rPitch,
                                   double&  phiPitch )  {
  point.set( 0., 0., 0. );
  rPitch   = 0.;
  phiPitch = 0.;
  // check that the sensor types are valid
  if(VeloChannelID::RType != rChan.type()) return StatusCode::FAILURE;
  if(VeloChannelID::PhiType != phiChan.type()) return StatusCode::FAILURE;
  
  unsigned int rSensor=rChan.sensor();
  unsigned int phiSensor=phiChan.sensor();
  // Nearby sensor...
  double zR   = m_sensorZ[sensorIndex(rSensor)];
  double zPhi = m_sensorZ[sensorIndex(phiSensor)];
  
  // Compute R from strip.
  unsigned int rZone; rZone=zoneOfStrip(rChan);
  double localR;
  StatusCode sc=rOfStrip(rChan, rFrac, localR);
  if(!sc) return sc;
  // check some matching in the detector region.
  double rAtPhi = localR * ( zPhi - m_zVertex ) / ( zR - m_zVertex );

  double innerPitch;
  sc=this->rPitch(VeloChannelID(rSensor,0),innerPitch);
  if(sc.isFailure()) return sc;
  double tolPhiBoundary = 5. * innerPitch;

  double innerRadius=rMin(rSensor);
  double outerRadius=rMax(rSensor);
  
  if(innerRadius-tolPhiBoundary > rAtPhi) return StatusCode::FAILURE;
  if(outerRadius+tolPhiBoundary < rAtPhi) return StatusCode::FAILURE;

  // Coherence in the Phi detector region, with some tolerance
  double phiBoundRadius=this->rMin(phiSensor,0);
  unsigned int nbPhiInner=this->stripsInZone(phiSensor,0);
  if(phiBoundRadius+tolPhiBoundary < rAtPhi) {
    if(nbPhiInner > phiChan.strip()) {
      return false;
    }
  } else if(phiBoundRadius-tolPhiBoundary > rAtPhi) {
    if (nbPhiInner <= phiChan.strip()) {
      return false;
    }
  }
  double phiLocal ;
  sc=phiOfStrip(phiChan, phiFrac, rAtPhi, phiLocal);
  if(sc.isFailure()) return sc;
  // Test for R compatibility
  double phiMin = phiLocal + 0.02;    // Tolerance for tests
  double phiMax = phiLocal - 0.02;    // tolerance for tests
  unsigned int iFind=0;
  for(unsigned int iZone=0;iZone<numberOfZones(rChan.sensor());iZone++){
    if(iZone == static_cast<unsigned int>(rZone)) {
      double zoneMin;
      StatusCode sc = this->phiMin(rChan.sensor(),iZone,zoneMin);
      if(!sc) return StatusCode::FAILURE;
      double zoneMax;
      sc = this->phiMax(rChan.sensor(),iZone,zoneMax);
      if(!sc) return StatusCode::FAILURE;
      if ((zoneMin < phiMin) && ( zoneMax > phiMax ) ){
        iFind = iZone;
      }
    }
  }
  if(static_cast<unsigned int>(rZone) != iFind) return StatusCode::FAILURE;
  // Convert a local point to a global point, using the 
  // localToGlobal method of the R sensor
  double x=localR*cos(phiLocal);
  double y=localR*sin(phiLocal);
  sc=localToGlobal(rSensor,HepPoint3D(x,y,0),point);
  // Compute the pitches. 
  sc = this->rPitch(rChan, rPitch);
  if(!sc) return sc;
  sc = this->phiPitch(phiChan, rAtPhi, phiPitch);
  if(!sc) return sc;
  return StatusCode::SUCCESS;
}
//=========================================================================
//  Returns a range of strip matching the point, and the conversion factors
//=========================================================================
void DeVelo:: phiMatchingStrips( int sensor, double radius, 
                                 int rSensor, int zone, double angularTol,
                                 double& stripMin, double& stripMax, 
                                 double& pitch, double& offset ) {
  stripMin = -1.;
  stripMax = -1.;
  pitch    = 0.;
  offset   = 0.;
  StatusCode sc;
  if(isRSensor(sensor)) return;    // R sensor
  if(rMin(sensor) > radius ) return;
  if(rMax(sensor) < radius ) return;
  
  bool isInner=false;
  if(rMin(sensor,0) < radius && rMax(sensor,0) > radius){
    isInner = true;
  }
  sc = phiStereo(VeloChannelID(sensor,0),radius,offset);
  if(isInner){
    sc = phiPitch(VeloChannelID(sensor,0),pitch);
  } else {
    sc = phiPitch(VeloChannelID(sensor,stripsInZone(sensor,0)),pitch);
  }
  //== tolerance in phi angle to match R and Phi...
  double absDz =  fabs(m_vpSensor[sensorIndex(sensor)]->z() - 
                       m_vpSensor[sensorIndex(rSensor)]->z());
  double deltaPhi = angularTol * absDz / radius;
  
  double phiMin;
  double phiMax;
  if(0 == zone || 3 == zone){
    sc = this->phiMin(rSensor,zone,radius,phiMin);
    sc = this->phiMax(rSensor,zone,radius,phiMax);
  } else {
    sc = this->phiMin(rSensor,static_cast<unsigned int>(zone),phiMin);
    sc = this->phiMax(rSensor,static_cast<unsigned int>(zone),phiMax);
  }
  phiMin += (-deltaPhi-offset);
  phiMax += (deltaPhi-offset);
  // For unusual pairing, rotate Phi ranges to match the R zone...
  // But only in the appropriate zones...
  /*  if ( ( phiType + rType )%2 != 0 ) {
      if ( 0 == zone || 2 == zone ) {
      phiMin += pi;
      phiMax += pi;
      } else if ( 4 == zone || 5 == zone ) {
      phiMin -= pi;
      phiMax -= pi;
      } else {
      return;
      }
      }*/

  stripMin = phiMin / pitch;
  stripMax = phiMax / pitch;
  
  if ( stripMax < stripMin ) {
    double temp = stripMin;
    stripMin = stripMax;
    stripMax = temp;
  }
  //== Phi strips are defined from 0 to nbStrips().
  unsigned int nbPhiInner=stripsInZone(sensor,0);
  unsigned int nbStrips=numberStrips(sensor);
  if ( isInner ) {
    if ( 0. > stripMax ) {
      stripMin = -1.;
      stripMax = -1.;
    } else if ( 0. > stripMin ) {
      stripMin = 0.;
    } else if ( nbPhiInner <= stripMin ) {
      stripMin = -1.;
      stripMax = -1.;
    } else if ( nbPhiInner <= stripMax ) {
      stripMax = nbPhiInner;
    }
  } else {
    stripMin += nbPhiInner;
    stripMax += nbPhiInner;
    offset   -= nbPhiInner * pitch;
    if ( nbPhiInner > stripMax ) {
      stripMin = -1.;
      stripMax = -1.;
    } else if ( nbPhiInner > stripMin ) {
      stripMin = nbPhiInner;
    } else if ( nbStrips <= stripMin ) {
      stripMin = -1.;
      stripMax = -1.;
    } else if ( nbStrips <= stripMax ) {
      stripMax = nbStrips;
    }
  }
}
//=========================================================================
