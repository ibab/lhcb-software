// $Id: DeVelo.cpp,v 1.47 2005-06-02 09:05:18 jpalac Exp $
//
// ============================================================================
#define  VELODET_DEVELO_CPP 1
// ============================================================================
// from STL (for std::sort)
#include <algorithm>

// from CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

// From Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"

// Local
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
  unsigned int nextR=this->param<int>("FirstR");
  unsigned int nextPhi=this->param<int>("FirstPhi");
  unsigned int nextPileUp=this->param<int>("FirstPileUp");

  // get all of the pointers to the child detector elements
  std::vector<DeVeloSensor*> veloSensors = this->getVeloSensors();
  
  msg << MSG::DEBUG << "Found " << veloSensors.size() 
      << " sensors in the XML" << endreq;

  std::vector<DeVeloSensor*>::iterator iDESensor;
  int detElemCount=0;
  m_sensorZ.clear();
  m_vpSensor.clear();
  m_vpRSensor.clear();
  m_vpPhiSensor.clear();
  m_vpPUSensor.clear();
  m_nRSensors=m_nPhiSensors=m_nPileUpSensors=0;
  
  for(iDESensor = veloSensors.begin() ; iDESensor != veloSensors.end() ; 
      ++iDESensor){
    // Sensors are pre-sorted in XML such that they increase with z position
    m_vpSensor.push_back(*iDESensor);
    unsigned int index=m_vpSensor.size()-1;
    msg << MSG::DEBUG << "type " << (*iDESensor)->type() 
        << " R " << (*iDESensor)->isR() 
        << " PHI " << (*iDESensor)->isPhi()
        << " PU " << (*iDESensor)->isPileUp() << endmsg;
    if((*iDESensor)->isR()){
      m_vpSensor[index]->sensorNumber(nextR);
      m_vpRSensor.push_back(dynamic_cast<DeVeloRType*>((*iDESensor)));
      m_nRSensors++;
      m_RIndex.push_back(index);
      nextR++;
    } else if((*iDESensor)->isPhi()){
      m_vpSensor[index]->sensorNumber(nextPhi);
      m_vpPhiSensor.push_back(dynamic_cast<DeVeloPhiType*>((*iDESensor)));
      m_nPhiSensors++;
      m_PhiIndex.push_back(index);
      nextPhi++;
    } else if((*iDESensor)->isPileUp()){
      m_vpSensor[index]->sensorNumber(nextPileUp);
      m_vpPUSensor.push_back(dynamic_cast<DeVeloRType*>((*iDESensor)));
      m_nPileUpSensors++;
      m_PUIndex.push_back(index);
      nextPileUp++;
    } else {
      msg << MSG::ERROR << "Sensor type is unknown\n";
    }
    msg << MSG::DEBUG << "Sensor number " << m_vpSensor[index]->sensorNumber()
        << " pSensor " << (*iDESensor)->sensorNumber() << endreq;
    msg << MSG::DEBUG << " Sensor number " << m_vpSensor[index]->sensorNumber()
        << " is type " << m_vpSensor[index]->type() 
        << " at z = " << m_vpSensor[index]->z()
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
  std::sort(m_vpSensor.begin(), m_vpSensor.end(), less_Z());

  for(unsigned int iSensor=0; iSensor < m_vpSensor.size() ; ++iSensor){
    m_sensorZ.push_back(m_vpSensor[iSensor]->z());
    unsigned int sensor = m_vpSensor[iSensor]->sensorNumber();

    msg << MSG::DEBUG << "Index " << iSensor << " Sensor number " << sensor
        << " is type " << m_vpSensor[iSensor]->type() 
        << " at global z = " << m_vpSensor[iSensor]->z()
        << " and in VELO frame " 
        << this->geometry()->toLocal(HepPoint3D(0,0,m_vpSensor[iSensor]->z())).z()
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
  
  if(m_nRSensors < 2 || m_nPhiSensors < 2 || m_nPileUpSensors < 2){
    msg << MSG::ERROR 
        << " This code requies at least two of each type of sensor"
        << endreq;
    return StatusCode::FAILURE;
  } else {
    msg << MSG::DEBUG 
        << " There are " << m_nRSensors << " R type, " 
        << m_nPhiSensors << " Phi type and "
        << m_nPileUpSensors << " pileup type sensors " << endreq;
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

// Return the index of a sensor in the vector of pointers to the sensors 
// which increase with sensor number
unsigned int DeVelo::sensorIndex(unsigned int sensor)
{
  if(64 > sensor) return m_RIndex[sensor];
  else if(128 > sensor) return m_PhiIndex[sensor-64];
  else return m_PUIndex[sensor-128];
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
double DeVelo::rOfStrip( VeloChannelID channel ) {
  // check whether sensor is R type using m_vpSensor[sensor]->type()
  //  write method bool DeVelo::isR(unsigned int sensor), isPhi etc.
  unsigned int sensor=channel.sensor();
  unsigned int index=sensorIndex(sensor);
  unsigned int strip=channel.strip();
  if(DeVeloRType* rPtr = dynamic_cast<DeVeloRType*>(m_vpSensor[index])){
    return rPtr->rOfStrip(strip);
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "rOfStrip: asked for non-valid R sensor " 
        << channel.sensor() << endmsg;
    return 0.;
  }
}
// returns the local radius of the strip+fractional distance to strip
double DeVelo::rOfStrip( VeloChannelID channel, double fraction)
{
  // check whether sensor is R type using m_vpSensor[sensor]->type()
  //  write method bool DeVelo::isR(unsigned int sensor), isPhi etc.
  unsigned int index=sensorIndex(channel.sensor());
  if(DeVeloRType* rPtr = dynamic_cast<DeVeloRType*>(m_vpSensor[index])){
    return rPtr->rOfStrip(channel.strip(),fraction);
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "rOfStrip: asked for non-valid R sensor " 
        << channel.sensor() << endmsg;
    return 0.;
  }
}

// returns the R pitch at the given channelID
double DeVelo::rPitch( VeloChannelID channel )
{
  unsigned int index=sensorIndex(channel.sensor());
  if(DeVeloRType* rPtr = dynamic_cast<DeVeloRType*>(m_vpSensor[index])){
    return rPtr->rPitch(channel.strip());
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "rPitch: asked for non-valid R sensor " 
        << channel.sensor() << endmsg;
    return 0.;
  }
}

// returns the R pitch at the given channelID
double DeVelo::rPitch( VeloChannelID channel, double fraction)
{
  unsigned int sensor=channel.sensor();
  unsigned int index=sensorIndex(sensor);
  if(DeVeloRType* rPtr = dynamic_cast<DeVeloRType*>(m_vpSensor[index])){
    return rPtr->rPitch(channel.strip(),fraction);
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "rPitch: asked for non-valid R sensor " 
        << channel.sensor() << endmsg;
    return 0.;
  }
}

// returns the R pitch at a given radius
double DeVelo::rPitchAtLocalR( VeloChannelID channel, double radius)
{
  unsigned int sensor=channel.sensor();
  unsigned int index=sensorIndex(sensor);
  if(DeVeloRType* rPtr = dynamic_cast<DeVeloRType*>(m_vpSensor[index])){
    return rPtr->rPitch(radius);
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "rPitchAtLocalR: asked for non-valid R sensor " 
        << channel.sensor() << endmsg;
    return 0.;
  }
}

// returns the local phi of the strip at the specified radius for this sensor.
double DeVelo::phiOfStrip( VeloChannelID channel, double radius){
  return this->phiOfStrip(channel,0.,radius);
}

// returns the local phi of the strip +fractional distance to strip
// at the specified radius for this sensor.
double DeVelo::phiOfStrip( VeloChannelID channel,
                               double fraction, double radius)
{
  unsigned int sensor=channel.sensor();
  unsigned int index=sensorIndex(sensor);
  if(DeVeloPhiType* phiPtr = dynamic_cast<DeVeloPhiType*>(m_vpSensor[index])){
    return phiPtr->phiOfStrip(channel.strip(),fraction,radius);
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "phiOfStrip: asked for non-valid phi sensor " 
        << channel.sensor() << endmsg;
    return -999;
  }
}

// returns the angle of the strip wrt the x axis for the strip
double DeVelo::angleOfStrip( VeloChannelID channel)
{
  return this->angleOfStrip(channel,0.);
}

// returns the angle of the strip wrt the x axis for
// the strip+fractional distance to strip
double DeVelo::angleOfStrip( VeloChannelID channel,double fraction)
{
  
  unsigned int sensor=channel.sensor();
  unsigned int index=sensorIndex(sensor);
  if(DeVeloPhiType* phiPtr = dynamic_cast<DeVeloPhiType*>(m_vpSensor[index])){
    return phiPtr->angleOfStrip(channel.strip(),fraction);
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "angleOfStrip: asked for non-valid phi sensor " 
        << channel.sensor() << endmsg;
    return -999;
  }
}

// The stereo angle of the phi strips in radians,
// signed so that positive indicates phi increases with radius 
double DeVelo::phiStereo( VeloChannelID channel, double radius)
{
  unsigned int index=sensorIndex(channel.sensor());
  if(DeVeloPhiType * phiPtr = 
     dynamic_cast<DeVeloPhiType*>(m_vpSensor[index])){
    return phiPtr->phiOffset(radius);
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "phiStereo: asked for non-valid phi sensor " 
        << channel.sensor() << endmsg;
    return -999;
  }
}

// returns the Phi pitch (in mm) at the given radius (sensor local) 
double DeVelo::phiPitch(VeloChannelID channel, double radius)
{
  unsigned int index=sensorIndex(channel.sensor());
  DeVeloPhiType * phiPtr = 
    dynamic_cast<DeVeloPhiType*>(m_vpSensor[index]);
  if(phiPtr){
    return phiPtr->phiPitch(radius);
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "phiPitch: asked for non-valid phi sensor " 
        << channel.sensor() << endmsg;
    return 0.;
  }
}

// returns the Phi pitch (in radians) for a given strip
double DeVelo::phiPitch(VeloChannelID channel)
{
  unsigned int index=sensorIndex(channel.sensor());  
  DeVeloPhiType * phiPtr = 
    dynamic_cast<DeVeloPhiType*>(m_vpSensor[index]);
  if(phiPtr){
    return phiPtr->phiPitch(channel.strip());
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "phiPitch: asked for non-valid phi sensor " 
        << channel.sensor() << endmsg;
    return 0.;
  }
}

double DeVelo::distToOrigin( VeloChannelID channel )
{
  unsigned int index=sensorIndex(channel.sensor());
  DeVeloPhiType * phiPtr = 
    dynamic_cast<DeVeloPhiType*>(m_vpSensor[index]);
  if(phiPtr){
    return phiPtr->distToOrigin(channel.strip());
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "distToOrigin: asked for non-valid phi sensor " 
        << channel.sensor() << endmsg;
    return 0.;
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

// Smallest Phi (local frame) of the r strips in the zone
double DeVelo::phiMin(unsigned int sensor, unsigned int zone){
  if(this->isRSensor(sensor)){
    DeVeloRType * rPtr = 
      dynamic_cast<DeVeloRType*>(m_vpSensor[sensorIndex(sensor)]);
    return rPtr->phiMinZone(zone);
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "phiMin: asked for non-valid R sensor " 
        << sensor << endmsg;
    return -999.;
  }
}

// Largest Phi (local frame) of the R strips in the zone
double DeVelo::phiMax(unsigned int sensor, unsigned int zone){
  if(this->isRSensor(sensor)){
    DeVeloRType * rPtr = 
      dynamic_cast<DeVeloRType*>(m_vpSensor[sensorIndex(sensor)]);
    return rPtr->phiMaxZone(zone);
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "phiMax: asked for non-valid R sensor " 
        << sensor << endmsg;
    return -999.;
  }
}

// minimum phi at R (overlap in x) for a given zone
double DeVelo::phiMin(unsigned int sensor, unsigned int zone, double radius){
  if(this->isRSensor(sensor)){
    DeVeloRType * rPtr = 
      dynamic_cast<DeVeloRType*>(m_vpSensor[sensorIndex(sensor)]);
    return rPtr->phiMinZone(zone,radius);
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "phiMin: asked for non-valid R sensor " 
        << sensor << endmsg;
    return -999.;
  }
}

// maximum phi at R (overlap in x) for a given zone
double DeVelo::phiMax(unsigned int sensor, unsigned int zone,double radius){
  if(this->isRSensor(sensor)){
    DeVeloRType * rPtr = 
      dynamic_cast<DeVeloRType*>(m_vpSensor[sensorIndex(sensor)]);
    return rPtr->phiMaxZone(zone,radius);
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "phiMax: asked for non-valid R sensor " 
        << sensor << endmsg;
    return -999.;
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
  if(this->isRSensor(sensor) || this->isPileUpSensor(sensor)){    
    DeVeloRType * rPtr = 
      dynamic_cast<DeVeloRType*>(m_vpSensor[sensorIndex(sensor)]);
    StatusCode sc=rPtr->stripLimits(strip,radius,phiMin,phiMax);
    if(m_vpSensor[sensorIndex(sensor)]->isRight()){
      phiMin += pi;
      phiMax += pi;
    }
    return sc;
  }else {
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
// Construct 3d point in global frame from R/phi channels 
// (rFrac is fractional distance to strip (+/-0.5))
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

  MsgStream msg( msgSvc(), "DeVelo" );
  msg << MSG::VERBOSE << ">>>>>>>> Inputs; sensors, R " << rChan.sensor()
      << " strip " << (rChan.strip()+rFrac)
      << " Phi " << phiChan.sensor()
      << " strip " << (phiChan.strip()+phiFrac)
      << " at z = " << zSensor(rChan.sensor())
      << endmsg;

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
  double localR=rOfStrip(rChan, rFrac);
  // check some matching in the detector region.
  double rAtPhi = localR * ( zPhi - m_zVertex ) / ( zR - m_zVertex );

  double innerPitch=this->rPitch(VeloChannelID(rSensor,0));
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
  double phiLocal = phiOfStrip(phiChan, phiFrac, rAtPhi);
  if(this->isDownstreamSensor(phiSensor)) phiLocal = -phiLocal;
  // Test for R compatibility
  double phiMin = phiLocal + 0.02;    // Tolerance for tests
  double phiMax = phiLocal - 0.02;    // tolerance for tests
  unsigned int iFind=0;
  for(unsigned int iZone=0;iZone<numberOfZones(rChan.sensor());iZone++){
    if(iZone == static_cast<unsigned int>(rZone)) {
      double zoneMin = this->phiMin(rChan.sensor(),iZone);
      double zoneMax = this->phiMax(rChan.sensor(),iZone);
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
  //  double x=rAtPhi*cos(phiLocal);
  //  double y=rAtPhi*sin(phiLocal);
  StatusCode sc=localToGlobal(rSensor,HepPoint3D(x,y,0),point);
  if(!sc) return StatusCode::FAILURE;
  // Compute the pitches. 
  rPitch = this->rPitch(rChan);
  phiPitch = this->phiPitch(phiChan, rAtPhi);
  return StatusCode::SUCCESS;
}
//==============================================================================
//  Return true if the two zones are matching for R sensors. 
//  Also returns true for neighbouring phi zones
//==============================================================================
bool DeVelo::matchingZones (unsigned int zone1, unsigned int zone2) {
  if(0 == zone1) return (2 > zone2);
  else if(1 == zone1) return (3 > zone2);
  else if(2 == zone1) return (0 < zone2);
  else if(3 == zone1) return (1 < zone2);
  return false;
}
//=========================================================================
// REPLICATE OLD DEVELO CODE FOR TRIGGER....
//=========================================================================
//  Returns a range of strip matching the point, and the conversion factors
//=========================================================================
void DeVelo::trgPhiMatchingStrips( int sensor, double radius, 
                                   int rSensor, int zone, double angularTol,
                                   double& stripMin, double& stripMax, 
                                   double& pitch, double& offset ) {
  stripMin = -1.;
  stripMax = -1.;
  pitch    = 0.;
  offset   = 0.;

  MsgStream msg( msgSvc(), "DeVelo" );
  msg << MSG::VERBOSE << ">>>>>>>> Inputs; sensors, phi " << sensor 
      << " R " << rSensor 
      << " at z = " << zSensor(rSensor)+zSensor(sensor)/2
      << " zone " << zone 
      << " radius " << radius
      << " angularTol " << angularTol << endmsg;

  if(isRSensor(sensor)) return;    // R sensor
  if(rMin(sensor) > radius ) return;
  if(rMax(sensor) < radius ) return;
  bool isInner=false;
  if(rMax(sensor,0) > radius){
    isInner = true;
  }
  offset = phiStereo(VeloChannelID(sensor,0),radius);
  if(isInner){
    pitch = phiPitch(VeloChannelID(sensor,0));
  } else {
    pitch = phiPitch(VeloChannelID(sensor,stripsInZone(sensor,0)));
  }
  if ( isDownstreamSensor(sensor) ) {
    pitch  = -pitch;
    offset = -offset;
  }
  //== tolerance in phi angle to match R and Phi...
  double absDz =  fabs(m_vpSensor[sensorIndex(sensor)]->z() - 
                       m_vpSensor[sensorIndex(rSensor)]->z());
  double deltaPhi = angularTol * absDz / radius;
  
  double phiMin;
  double phiMax;
  if(0 == zone || 3 == zone){
    phiMin = this->phiMin(rSensor,zone,radius);
    phiMax = this->phiMax(rSensor,zone,radius);
  } else {
    phiMin = this->phiMin(rSensor,static_cast<unsigned int>(zone));
    phiMax = this->phiMax(rSensor,static_cast<unsigned int>(zone));
  }
  phiMin += -deltaPhi - offset;
  phiMax += deltaPhi - offset;
  
  // For unusual pairing, rotate Phi ranges to match the R zone...
  // But only in the appropriate zones...
  if(xSide(sensor) != xSide(rSensor)){
    if ( 0 == zone ) {
      phiMin += pi;
      phiMax += pi;
    } else if ( 3 == zone ) {
      phiMin -= pi;
      phiMax -= pi;
    } else {
      return;
    }
  }

  stripMin = phiMin / pitch;
  stripMax = phiMax / pitch;
  if ( stripMax < stripMin ) {
    double temp = stripMin;
    stripMin = stripMax;
    stripMax = temp;
  }
  //== Phi strips are defined from 0 to nbstrips. Strip center is at +.5
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
  if ( isRightSensor(sensor) ) offset += pi;
  msg << MSG::VERBOSE << "Outputs; strip Min " << stripMin 
      << " max " << stripMax << " pitch " << pitch 
      << " offset " << offset/degree << endmsg;
}

// returns the phi of the strip at the specified radius for this sensor.
double DeVelo::trgPhiOfStrip( VeloChannelID channel,double radius ) {
  return this->trgPhiOfStrip(channel,0.,radius);
}

// returns the local phi of the strip +fractional distance to strip
// at the specified radius for this sensor.
double DeVelo::trgPhiOfStrip( VeloChannelID channel,
                               double fraction, double radius ) {
  unsigned int sensor=channel.sensor();
  unsigned int index=sensorIndex(sensor);
  if(DeVeloPhiType* phiPtr = dynamic_cast<DeVeloPhiType*>(m_vpSensor[index])){
    return phiPtr->trgPhiOfStrip(channel.strip(),fraction,radius);
  }else{
    MsgStream msg(msgSvc(), "DeVelo");
    msg << MSG::ERROR << "trgPhiOfStrip: asked for non-valid phi sensor " 
        << channel.sensor() << endmsg;
    return -999.;
  }
}

// returns the angle of the strip wrt the x axis for the strip
double DeVelo::trgPhiDirectionOfStrip( VeloChannelID channel ) {
  return this->trgPhiDirectionOfStrip(channel,0.);
}

// returns the angle of the strip wrt the x axis for
// the strip+fractional distance to strip
double DeVelo::trgPhiDirectionOfStrip( VeloChannelID channel,
                               double fraction ) {
  unsigned int sensor=channel.sensor();
  unsigned int index=sensorIndex(sensor);
  if(DeVeloPhiType* phiPtr = dynamic_cast<DeVeloPhiType*>(m_vpSensor[index])){
    return phiPtr->trgPhiDirectionOfStrip(channel.strip(),fraction);
  }else{
    return StatusCode::FAILURE;
  }
}
//=============================================================================
std::vector<DeVeloSensor*> DeVelo::getVeloSensors() 
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
//=============================================================================
