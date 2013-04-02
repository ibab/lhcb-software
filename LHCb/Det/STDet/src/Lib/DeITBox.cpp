
#include "STDet/DeITStation.h"
#include "STDet/DeITLayer.h"
#include "STDet/DeITBox.h"
#include "Kernel/ITNames.h"

/** @file DeITBox.cpp
*
*  Implementation of class : DeITBox
*
*    @author Matthew Needham
*/

//STL
#include <algorithm>
#include <numeric>

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

using namespace boost::lambda;
using namespace LHCb;

DeITBox::DeITBox( const std::string& name ) :
  DeSTBaseElement( name ),
  m_parent(NULL),
  m_id(0u),
  m_firstLayer(NULL),
  m_lastLayer(NULL)
{ 
  // constructer
}

DeITBox::~DeITBox() {

}

const CLID& DeITBox::clID () const
{
  return DeITBox::classID() ;
}

StatusCode DeITBox::initialize() {

  // initialize 
  StatusCode sc = DeSTBaseElement::initialize();
  if (sc.isFailure() ){
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg; 
  }
  else {
    // get the children and parent
    m_id = param<int>("boxID");
    m_parent = getParent<DeITBox>(); 

    const STChannelID parentID = m_parent->elementID();
    STChannelID chan(STChannelID::typeIT, parentID.station(),parentID.layer(),
                     m_id, 0, 0);
    setElementID(chan);
    m_layers = getChildren<DeITBox>();

    // set the first and last layers
    DeITBox::Children::const_iterator start = m_layers.begin();
    DeITBox::Children::const_iterator childEnd = m_layers.end();
    m_firstLayer = *start; m_lastLayer = *start;
    for (; start != childEnd; ++start){
      unsigned int testID = (*start)->id();
      if (testID < m_firstLayer->id()) m_firstLayer = *start;
      if (testID > m_lastLayer->id()) m_lastLayer = *start;
    }

    m_nickname = ITNames().UniqueBoxToString(chan);

  }
  return sc;
}

std::ostream& DeITBox::printOut( std::ostream& os ) const{

  // stream to cout
  os << " Box : "  << name() << std::endl
     << " id " << id()  << std::endl
     << " nickname " << nickname() 
     << std::endl;

  return os;
}

MsgStream& DeITBox::printOut( MsgStream& os ) const{

  // stream to Msg service
  os << " Box : "  << name() 
     << " id " << id() 
     << " nickname " << nickname()  
     << std::endl;

  return os;
}

DeITLayer* DeITBox::findLayer(const STChannelID aChannel){

  // return pointer to the station from channel
  Children::iterator iter = std::find_if(m_layers.begin() , m_layers.end(), bind(&DeITLayer::contains, _1, aChannel));
  return (iter != m_layers.end() ? *iter: 0);
}

DeITLayer* DeITBox::findLayer(const Gaudi::XYZPoint& point) {

  // return pointer to the station from point in global system
  Children::iterator iter = std::find_if(m_layers.begin(), m_layers.end(), 
                                                        bind(&DeITLayer::isInside, _1, point)); 
  return (iter != m_layers.end() ? *iter: 0);
}

double DeITBox::fractionActive() const {

  return std::accumulate(m_layers.begin(), m_layers.end(), 0.0,  _1 + bind(&DeITLayer::fractionActive,_2))/double(m_layers.size());   
}

bool DeITBox::contains(const LHCb::STChannelID aChannel) const{
  return ((aChannel.detRegion() == elementID().detRegion()) 
         && m_parent->contains(aChannel)) ;
}




