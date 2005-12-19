#include "STDet/DeSTStation.h"

// DetDesc
#include "DetDesc/IGeometryInfo.h"

/** @file DeSTStation.cpp
*
*  Implementation of class :  DeSTStation
*
*    @author Matthew Needham
*/

using namespace LHCb;

DeSTStation::DeSTStation( const std::string& name ) :
  DeSTBaseElement( name )
{ 
  // constructer
}

DeSTStation::~DeSTStation() {
  // desctructer
}

StatusCode DeSTStation::initialize() {

  // initialize
  MsgStream msg(msgSvc(), name() );

  StatusCode sc = DeSTBaseElement::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq; 
  }
  else {
    m_id = (unsigned int)param<int>("stationID");
    STChannelID aChan(m_id,0,0,0,0);
    setElementID(aChan); 
  }

  return sc;
}

std::ostream& DeSTStation::printOut( std::ostream& os ) const{

  // stream to cout
  os << " Station : "  << m_id 
     << " n children "  << childIDetectorElements().size() 
     << std::endl;

  return os;
}

MsgStream& DeSTStation::printOut( MsgStream& os ) const{

  // stream to Msg service
  os << " Station : "  << m_id 
     << " n children "  << childIDetectorElements().size() 
     << endreq;

  return os;
}





