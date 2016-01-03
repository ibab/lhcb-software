
#include "STDet/DeTTStation.h"
#include "STDet/DeTTDetector.h"
#include "STDet/DeTTLayer.h"
#include "Kernel/TTNames.h"

//STL
#include <algorithm>
#include <numeric>

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include "Kernel/STChannelID.h"

using namespace boost::lambda;
using namespace LHCb;

/** @file DeTTStation.cpp
*
*  Implementation of class :  DeTTStation
*
*    @author Matthew Needham
*/


DeTTStation::DeTTStation( const std::string& name ) :
  DeSTStation( name ),
  m_parent(NULL)
{ 
  // constructor
}


DeTTStation::~DeTTStation() {

}

const CLID& DeTTStation::clID () const
{
  return DeTTStation::classID() ;
}

StatusCode DeTTStation::initialize() {

  // initialize 
  StatusCode sc = DeSTStation::initialize();
  if (sc.isFailure() ){
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg; 
  }
  else {
  
   // and the parent
   m_parent = getParent<parent_type>();

   STChannelID aChan(STChannelID::typeTT, this->id(),0,0,0,0);
   setElementID(aChan); 
   m_nickname = TTNames().StationToString(aChan);

   // get the children 
   m_layers = getChildren<DeTTStation>();

  }
  return sc;
} 

DeTTLayer* DeTTStation::findLayer(const STChannelID aChannel){

  // return pointer to the station from channel
  Children::iterator iter = std::find_if(m_layers.begin() , m_layers.end(), bind(&DeTTLayer::contains, _1, aChannel));
  return (iter != m_layers.end() ? *iter: 0);
}

DeTTLayer* DeTTStation::findLayer(const Gaudi::XYZPoint& point) {

  // return pointer to the station from point in global system
  Children::iterator iter = std::find_if(m_layers.begin(), m_layers.end(), 
                                                        bind(&DeTTLayer::isInside, _1, point)); 
  return (iter != m_layers.end() ? *iter: 0);
}


double DeTTStation::fractionActive() const {

  return std::accumulate(m_layers.begin(), m_layers.end(), 0.0,  _1 + bind(&DeTTLayer::fractionActive,_2))/double(m_layers.size());   
}


