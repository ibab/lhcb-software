#include "STDet/DeUTStation.h"
#include "STDet/DeUTDetector.h"
#include "STDet/DeUTLayer.h"
#include "Kernel/UTNames.h"

#include <algorithm>
#include <numeric>

#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include "Kernel/STChannelID.h"

using namespace boost::lambda;
using namespace LHCb;

/** @file DeUTStation.cpp
 *
 *  Implementation of class :  DeUTStation
 *
 *  @author Jianchun Wang, Matt Needham
 *  @date   2012-07-10
 *
 */

DeUTStation::DeUTStation( const std::string& name ) :
  DeSTStation( name ),
  m_parent(NULL)
{
}


DeUTStation::~DeUTStation()
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
  Children::iterator iter = std::find_if(m_layers.begin() , m_layers.end(),
                                         bind(&DeUTLayer::contains, _1, aChannel));
  return (iter != m_layers.end() ? *iter: 0);
}


DeUTLayer* DeUTStation::findLayer(const Gaudi::XYZPoint& point)
{
  Children::iterator iter = std::find_if(m_layers.begin(), m_layers.end(),
                                         bind(&DeUTLayer::isInside, _1, point));
  return (iter != m_layers.end() ? *iter: 0);
}


double DeUTStation::fractionActive() const
{
  return std::accumulate(m_layers.begin(), m_layers.end(), 0.0,  _1 +
                         bind(&DeUTLayer::fractionActive,_2))/double(m_layers.size());
}
