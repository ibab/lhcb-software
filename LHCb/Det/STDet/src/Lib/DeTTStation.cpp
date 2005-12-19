
#include "STDet/DeTTStation.h"
#include "STDet/DeTTDetector.h"
#include "STDet/DeTTLayer.h"

//STL
#include <algorithm>

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>



using namespace boost::lambda;
using namespace LHCb;

/** @file DeTTStation.cpp
*
*  Implementation of class :  DeTTStation
*
*    @author Matthew Needham
*/


DeTTStation::DeTTStation( const std::string& name ) :
  DeSTStation( name )
{ 
  // constructer
}


DeTTStation::~DeTTStation() {

}

const CLID& DeTTStation::clID () const
{
  return DeTTStation::classID() ;
}

StatusCode DeTTStation::initialize() {

  // initialize 
  MsgStream msg(msgSvc(), name() );
  StatusCode sc = DeSTStation::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq; 
  }
  else {
   // get the children 
   
   m_layers = getChildren<DeTTStation>();

   // and the parent
   m_parent = getParent<parent_type>();

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



