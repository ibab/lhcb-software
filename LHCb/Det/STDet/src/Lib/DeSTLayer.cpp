
#include "STDet/DeSTLayer.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

/** @file DeSTLayer.cpp
*
*  Implementation of class :  DeSTLayer
*
*    @author Matthew Needham
*/


DeSTLayer::DeSTLayer( const std::string& name ) :
  DeSTBaseElement( name ),
  m_id(0u),
  m_angle(0.0),
  m_sinAngle(0.0),
  m_cosAngle(0.0)
{ 
  // constructer
}


DeSTLayer::~DeSTLayer() {
  // destructer
}

StatusCode DeSTLayer::initialize() {

  // initialize method
 
  StatusCode sc = DeSTBaseElement::initialize();
  if (sc.isFailure() ){
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg; 
  }
  else {

    m_id = param<int>("layerID");
    m_angle = param<double>("stereoangle");
    m_cosAngle = cos(m_angle);
    m_sinAngle = sin(m_angle);
     
    // cache trajectories
    sc = registerCondition(this,this->geometry(),&DeSTLayer::cachePlane);
    if (sc.isFailure() ){
      MsgStream msg(msgSvc(), name() );
      msg << MSG::ERROR << "Failed to register conditions" << endmsg;
      return StatusCode::FAILURE; 
    }
  }

  return StatusCode::SUCCESS;
}

std::ostream& DeSTLayer::printOut( std::ostream& os ) const{

  // stream to cout
  os << " Layer: "  << m_id << std::endl;
  os << "stereo angle " << m_angle << std::endl
     << " Nickname: " << m_nickname 
     << std::endl;

  return os;
}

MsgStream& DeSTLayer::printOut( MsgStream& os ) const{

  // stream to Msg service
  os << " Station : "  << m_id << endmsg;
  os << "stereo angle " << m_angle
     << " Nickname: " << m_nickname 
     << endmsg;

  return os;
}

StatusCode DeSTLayer::cachePlane(){

 Gaudi::XYZPoint p1 = globalPoint(0,0,0);
 Gaudi::XYZPoint p2 = globalPoint(3*Gaudi::Units::m,0,0);
 Gaudi::XYZPoint p3 = globalPoint(0,3*Gaudi::Units::cm,0);

 m_plane = Gaudi::Plane3D(p1,p2,p3);

 return StatusCode::SUCCESS;
}

