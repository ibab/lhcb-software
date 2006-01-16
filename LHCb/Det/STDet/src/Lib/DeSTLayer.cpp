
#include "STDet/DeSTLayer.h"


/** @file DeSTLayer.cpp
*
*  Implementation of class :  DeSTLayer
*
*    @author Matthew Needham
*/


DeSTLayer::DeSTLayer( const std::string& name ) :
  DeSTBaseElement( name )
{ 
  // constructer
}


DeSTLayer::~DeSTLayer() {
  // destructer
}

StatusCode DeSTLayer::initialize() {

  // initialize method
  MsgStream msg(msgSvc(), name() );

  StatusCode sc = DeSTBaseElement::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq; 
  }
  else {

    m_id = param<unsigned int>("layerID");
    m_angle = param<double>("stereoAngle");
    m_cosAngle = cos(m_angle);
    m_sinAngle = sin(m_angle);
  }

  return StatusCode::SUCCESS;
}

std::ostream& DeSTLayer::printOut( std::ostream& os ) const{

  // stream to cout
  os << " Layer: "  << m_id << std::endl;
  os << "stereo angle " << m_angle
     << " n children "  << childIDetectorElements().size() 
     << std::endl;

  return os;
}

MsgStream& DeSTLayer::printOut( MsgStream& os ) const{

  // stream to Msg service
  os << " Station : "  << m_id << endreq;
  os << "stereo angle " << m_angle
     << " n children "  << childIDetectorElements().size() 
     << endreq;

  return os;
}
