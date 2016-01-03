#include "STDet/DeUTStation.h"
#include "STDet/DeUTDetector.h"
#include "STDet/DeUTLayer.h"
#include "Kernel/UTNames.h"

#include <algorithm>
#include <numeric>

#include "Kernel/STChannelID.h"

using namespace LHCb;

/** @file DeUTStation.cpp
 *
 *  Implementation of class :  DeUTStation
 *
 *  @author Jianchun Wang, Matt Needham
 *  @date   2012-07-10
 *
 */

DeUTStation::DeUTStation( std::string name ) :
  DeSTStation( std::move(name) )
{
}

const CLID& DeUTStation::clID () const
{
  return DeUTStation::classID();
}

StatusCode DeUTStation::initialize()
{
  // initialize
  StatusCode sc = DeSTStation::initialize();
  if (sc.isFailure() ){
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg;
  }
  else {
    // and the parent
    m_parent = getParent<parent_type>();

    STChannelID aChan(STChannelID::typeUT, this->id(),0,0,0,0);
    setElementID(aChan);
    m_nickname = UTNames().StationToString(aChan);

    // get the children
    m_layers = getChildren<DeUTStation>();
  }

  return sc;
}


DeUTLayer* DeUTStation::findLayer(const STChannelID aChannel)
{
  auto iter = std::find_if(m_layers.begin() , m_layers.end(),
                           [&](const DeUTLayer *l) { return l->contains(aChannel); } );
  return (iter != m_layers.end() ? *iter: nullptr);
}


DeUTLayer* DeUTStation::findLayer(const Gaudi::XYZPoint& point)
{
  auto iter = std::find_if(m_layers.begin(), m_layers.end(),
                           [&](const DeUTLayer *l) { return l->isInside(point); } );
  return (iter != m_layers.end() ? *iter: nullptr);
}


double DeUTStation::fractionActive() const
{
  return std::accumulate(m_layers.begin(), m_layers.end(), 0.0,  [&](double f,const DeUTLayer* l)
                         { return f + l->fractionActive(); } )/double(m_layers.size());
}
