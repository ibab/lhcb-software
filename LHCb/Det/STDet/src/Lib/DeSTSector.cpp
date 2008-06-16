// $Id: DeSTSector.cpp,v 1.40 2008-06-16 14:24:04 mneedham Exp $
#include "STDet/DeSTSector.h"

#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

#include <algorithm>


// Kernel
#include "Kernel/LineTraj.h"
#include "Kernel/LHCbID.h"
#include "Kernel/PiecewiseTrajectory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/GaudiException.h"

#include "GaudiKernel/IUpdateManagerSvc.h"

#include "STDet/DeSTSensor.h"

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

/** @file DeSTSector.cpp
*
*  Implementation of class :  DeSTSector
*
*    @author Matthew Needham
*/

using namespace boost::lambda;
using namespace LHCb;

DeSTSector::DeSTSector( const std::string& name ) :
  DeSTBaseElement( name ),
  m_firstStrip(1),
  m_status(OK),
  m_statusString("Status"),
  m_versionString("DC06")
{ 
    // constructer (first strip means we number from 1)
}

DeSTSector::~DeSTSector() {
  // destructer
}

std::ostream& DeSTSector::printOut( std::ostream& os ) const{

  // stream to cout  
  os << " Sector :  "  << name() << std::endl;
  os   << " Nickname: " << m_nickname 
      << "\n ID " << id() 
      << "\n layer " << elementID().layer()
     << "\n type  " << type() 
     << "\n pitch " << m_pitch 
     << "\n strip " << m_nStrip
     << "\n capacitance " << m_capacitance/Gaudi::Units::picofarad
     << "\n dead width " << m_deadWidth
     << "\n center " << globalCentre()
     << std::endl; 
  return os;
}

MsgStream& DeSTSector::printOut( MsgStream& os ) const{

  // stream to Msg service
  os << " Sector : \n "  << name() << std::endl;
  os   << " Nickname: " << m_nickname 
     << "\n ID " << id() 
     << "type \n " << type() 
     << " pitch \n " << m_pitch 
     << "n strip \n " << m_nStrip
     << " capacitance \n " << m_capacitance/Gaudi::Units::picofarad
     << "dead width \n " << m_deadWidth
     << "\n center " << globalCentre()
     << std::endl;

  return os;
}

StatusCode DeSTSector::initialize() {

  // initialize method
  MsgStream msg(msgSvc(), name() );

  StatusCode sc = DeSTBaseElement::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq; 
    return sc;
  }
  else {

    // version 
    if (exists("version") == true) m_versionString = param<std::string>("version") ;
 
    m_pitch = param<double>("pitch");
    m_nStrip =  param<int>("numStrips");
    m_capacitance = param<double>("capacitance");
    m_type = param<std::string>("type");

    // guard ring
    m_deadWidth = param<double>("verticalGuardRing");  
    // if (m_versionString == "DCO6") m_deadWidth += 0.5*param<double>("bondGap");
 
    if (m_versionString != "DC06"){
      StatusCode sc = registerCondition(this,m_statusString,
                                        &DeSTSector::updateStatusCondition);
      if (sc.isFailure() ){
        msg << MSG::ERROR << "Failed to register status conditions" << endreq;
        return StatusCode::FAILURE; 
      }
    } // !DC06
  }
  return StatusCode::SUCCESS;
}

std::auto_ptr<LHCb::Trajectory> 
DeSTSector::trajectory(const STChannelID& aChan, const double offset) const 
{
  
  if (contains(aChan) == false){
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to link " << aChan.uniqueSector() << " " 
        << elementID().uniqueSector() << endmsg; 
    throw GaudiException( "Failed to make trajectory",
                           "DeSTSector.cpp", StatusCode::FAILURE );
  }
  
  return createTraj(aChan.strip(), offset);
}

std::auto_ptr<LHCb::Trajectory> DeSTSector::trajectoryFirstStrip() const 
{
  return createTraj(m_firstStrip,0.);
}

std::auto_ptr<LHCb::Trajectory> DeSTSector::trajectoryLastStrip() const 
{
  return createTraj(nStrip(), 0.);
}

std::auto_ptr<LHCb::Trajectory> DeSTSector::createTraj(const unsigned int strip, 
                                                      const double offset) const{
 
  // collect the individual traj
  const Sensors& theSensors = sensors();
  Sensors::const_iterator iterS = theSensors.begin();

  if (theSensors.size() == 1){
    // can just return a line traj
    return theSensors.front()->trajectory(strip,offset);  
  }
  else {
    // return a piecewise traj
    STTraj* traj = new STTraj(); 
    for (; iterS != theSensors.end(); ++iterS) {                
      std::auto_ptr<LHCb::Trajectory> sensTraj = (*iterS)->trajectory(strip,offset);
      if (traj->numberOfPieces() != 0) {
         // double d1 = (sensTraj->beginPoint()-traj->endPoint()).mag2();      
         //double d2 = (sensTraj->endPoint()-traj->beginPoint()).mag2();      
         //     if (d1 < d2) {
        double mu = sensTraj->muEstimate(traj->endPoint());        
        sensTraj->setRange(mu,sensTraj->endRange());              
      } 
      traj->append(sensTraj.release());          
 	//} else {                                                           
	//  double mu = sensTraj->muEstimate(traj->beginPoint());      
	//	sensTraj->setRange(sensTraj->beginRange(),mu);             
	//traj->prepend(sensTraj.release());                        
    } // loop
    return std::auto_ptr<LHCb::Trajectory>(traj);
  } // if 
}

StatusCode DeSTSector::cacheInfo()
{

  std::auto_ptr<LHCb::Trajectory> firstTraj = createTraj(m_firstStrip,-0.5);
  std::auto_ptr<LHCb::Trajectory> lastTraj = createTraj(nStrip(),0.5);

  // get the start point
  Gaudi::XYZPoint g1 = firstTraj->beginPoint();
  Gaudi::XYZPoint g2 = firstTraj->endPoint();

  const double activeWidth = m_sensors.front()->activeWidth();

  // direction
  Gaudi::XYZVector direction = g2 - g1;
  m_stripLength = sqrt(direction.Mag2());
  direction = direction.Unit();

  // cross with normal along z
  Gaudi::XYZVector zVec(0,0,1);
  Gaudi::XYZVector norm = direction.Cross(zVec);

  // trajectory of middle  
  Gaudi::XYZPoint g3 = g1 + 0.5*(g2 - g1);
  Gaudi::XYZPoint g4 = g3 + activeWidth*norm ;
  
  // creating the 'fast' trajectories  
  Gaudi::XYZVector vectorlayer = (g4-g3).unit() * m_pitch ;
  Gaudi::XYZPoint p0 = g3-0.5*m_stripLength*direction ;
  m_dxdy = direction.x()/direction.y() ;
  m_dzdy = direction.z()/direction.y() ;
  m_dy   = m_stripLength * direction.y() ;
  m_dp0di.SetX( vectorlayer.x() - vectorlayer.y() * m_dxdy ) ;
  m_dp0di.SetY( vectorlayer.y()  ) ;
  m_dp0di.SetZ( vectorlayer.z() - vectorlayer.y() * m_dzdy ) ;
  m_p0.SetX( p0.x() - p0.y() * m_dxdy ) ;
  m_p0.SetY( p0.y() ) ;
  m_p0.SetZ( p0.z() - p0.y() * m_dzdy ) ;
  
  // Update the stereo angle. We correct by 'pi' if necessary.
  Gaudi::XYZVector dir = direction ;
  if(dir.y()<0) dir *= -1 ;
  m_angle    = atan2(-dir.x(),dir.y()) ;
  m_cosAngle = cos( m_angle ) ;
  m_sinAngle = sin( m_angle ) ;
  
  return StatusCode::SUCCESS;

}

STChannelID DeSTSector::nextLeft(const STChannelID testChan) const
{
  if ((contains(testChan))&& (isStrip(testChan.strip()- 1u) == true)){
    return STChannelID(testChan.type(),
                      testChan.station(),
                      testChan.layer(), 
                      testChan.detRegion(),
                      testChan.sector(), 
                      testChan.strip() - 1u);
   }
   else {
     return LHCb::STChannelID(0u,0u,0u,0u,0u,0u);
   }
}

STChannelID DeSTSector::nextRight(const LHCb::STChannelID testChan) const
{
  if ((contains(testChan) == true)&& (isStrip(testChan.strip()+ 1u) == true)){
    return STChannelID(testChan.type(),
                       testChan.station(),
                       testChan.layer(), 
                       testChan.detRegion(),
                       testChan.sector(), 
                       testChan.strip() + 1u);
  }
  else {
    return LHCb::STChannelID(0u,0u,0u,0u,0u,0u);
  }
}


StatusCode DeSTSector::registerConditionsCallbacks(){

  // cache trajectories
  // initialize method
  MsgStream msg(msgSvc(), name() );

  if (sensors().empty()){
    msg << MSG::ERROR << "Sterile detector element ! No conditions registered" << endreq;
    return StatusCode::FAILURE;
  }

  StatusCode sc = registerCondition(this,sensors().front(),&DeSTSector::cacheInfo,true);
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to register geometry condition for first child" << endreq;
     return StatusCode::FAILURE; 
  }

  sc = registerCondition(this,sensors().back(),&DeSTSector::cacheInfo,true);
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to register geometry condition for first child" << endreq;
     return StatusCode::FAILURE; 
  }

  return StatusCode::SUCCESS;
}

 StatusCode DeSTSector::updateStatusCondition(){

  const Condition* aCon = condition(m_statusString);
  if (aCon == 0){
    MsgStream msg(msgSvc(), name());
    msg << "failed to find condition" << endmsg;
    return StatusCode::FAILURE; 
  }

  int tStatus = aCon->param<int>("SectorStatus");
  m_status = Status(tStatus);

  std::map<int,int> beetleMap = aCon->param<std::map<int,int> >("BeetleStatus");
  toEnumMap(beetleMap,m_beetleStatus);

  std::map<int,int> stripMap = aCon->param<std::map<int,int> >("StripStatus");
  toEnumMap(stripMap,m_stripStatus);
 
  return StatusCode::SUCCESS;
 }

void DeSTSector::toEnumMap(const std::map<int,int>& input, DeSTSector::StatusMap& output) {
  output.clear();
  std::map<int,int>::const_iterator iterM = input.begin();
  for (; iterM != input.end(); ++iterM){
    output[iterM->first] = Status(iterM->second);
  } // iterM

}


DeSTSensor* DeSTSector::findSensor(const Gaudi::XYZPoint& point) const{
  // return pointer to the layer from point
  std::vector<DeSTSensor*>::const_iterator iter = 
    std::find_if( m_sensors.begin(), m_sensors.end(), 
                 bind(&DeSTSensor::isInside, _1, point));
  return (iter != m_sensors.end() ? *iter: 0);
}

bool DeSTSector::globalInActive(const Gaudi::XYZPoint& point) const{

  const DeSTSensor* aSensor =  findSensor(point);
  return (aSensor ?  aSensor->globalInActive(point) : false );
}

bool DeSTSector::globalInBondGap(const Gaudi::XYZPoint& point, 
                                        double tol) const
{ 
  const DeSTSensor* aSensor =  findSensor(point);
  return (aSensor ?  aSensor->globalInBondGap(point, tol) : false ); 
};




