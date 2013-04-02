
#include "STDet/DeITLayer.h"
#include "STDet/DeITLadder.h"

/** @file DeITLadder.cpp
*
*  Implementation of class : DeITLadder
*
*    @author Matthew Needham
*/

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

using namespace boost::lambda;
using namespace LHCb;

DeITLadder::DeITLadder( const std::string& name ) :
  DeSTBaseElement( name ),
  m_child(0),
  m_parent(NULL),
  m_id(0u)
{ 
  // constructor
}

DeITLadder::~DeITLadder() {

}

const CLID& DeITLadder::clID () const
{
  return DeITLadder::classID() ;
}

StatusCode DeITLadder::initialize() {

  // initialize
  
  StatusCode sc = DeSTBaseElement::initialize();
  if (sc.isFailure() ){
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg; 
  }
  else {
    // get parent
    m_id = param<int>("sectorID");
    m_parent = getParent<DeITLadder>(); 
    const STChannelID parentID = m_parent->elementID();
    STChannelID chan(STChannelID::typeIT,
                     parentID.station(),parentID.layer(),
                     parentID.detRegion(), m_id, 0);
    setElementID(chan);

    std::vector<child_type*> tVector = getChildren<DeITLadder>();
    if (tVector.size() !=0) {
      m_child = tVector.front();
    }
    else {
      MsgStream msg(msgSvc(), name() );
      msg << MSG::ERROR << "No child sector found - failed to init" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return sc;
}

std::ostream& DeITLadder::printOut( std::ostream& os ) const{

  // stream to cout
  os << " Ladder : "  << name()  << std::endl
     << " id " << id() 
     << std::endl;

  return os;
}

MsgStream& DeITLadder::printOut( MsgStream& os ) const{

  // stream to Msg service
  os << " Ladder : "  << name() 
     << " id " << id() 
     << std::endl;

  return os;
}

double DeITLadder::fractionActive() const {

  return m_child->fractionActive();
}




