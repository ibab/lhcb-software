#include "STDet/DeUTLayer.h"
#include "STDet/DeUTModule.h"
#include "STDet/DeUTStation.h"

#include "DetDesc/IGeometryInfo.h"

#include "Kernel/UTNames.h"

#include <algorithm>
#include <numeric>

#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

using namespace LHCb;
using namespace boost::lambda;

/** @file DeUTLayer.cpp
 *
 *  Implementation of class :  DeUTLayer
 *
 *  @author Jianchun Wang, Matt Needham
 *  @date   2012-07-10
 *
 */

DeUTLayer::DeUTLayer( const std::string& name ) :
  DeSTLayer( name ),
  m_parent(NULL)
{ 
  m_modules.clear();
}


DeUTLayer::~DeUTLayer()
{
}


const CLID& DeUTLayer::clID () const
{
  return DeUTLayer::classID() ;
}


StatusCode DeUTLayer::initialize()
{
  // initialize method
  MsgStream msg(msgSvc(), name() );
  StatusCode sc = DeSTLayer::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg; 
  }
  else {
    m_parent = getParent<DeUTLayer>();
    STChannelID parentID = m_parent->elementID();
    STChannelID chan(STChannelID::typeUT, parentID.station(), id(), 0, 0, 0);
    setElementID(chan);
    m_modules = getChildren<DeUTLayer>();
    flatten();
    m_nickname = UTNames().UniqueLayerToString(chan);
  }
  return sc;
}


DeUTModule* DeUTLayer::findModule(const STChannelID aChannel)
{
  Children::iterator iter = std::find_if(m_modules.begin() , m_modules.end(),
                                         bind(&DeUTModule::contains, _1, aChannel));
  return (iter != m_modules.end() ? *iter: 0);
}


DeUTModule* DeUTLayer::findModule(const Gaudi::XYZPoint& point)
{
  Children::iterator iter = std::find_if(m_modules.begin(), m_modules.end(), 
                                         bind(&DeUTModule::isInside, _1, point)); 
  return (iter != m_modules.end() ? *iter: 0);
}


void DeUTLayer::flatten()
{
  DeUTLayer::Children::const_iterator iterModule = modules().begin();
  for (;iterModule != modules().end(); ++iterModule){
    DeUTModule* tModule = *iterModule;
    DeUTModule::Children::const_iterator iterSector = tModule->sectors().begin();
    for ( ; iterSector !=  tModule->sectors().end() ; ++iterSector ){
      DeSTSector* tSector = *iterSector;
      m_sectors.push_back(tSector);  
    }
  }
}


double DeUTLayer::fractionActive() const
{
  return std::accumulate(m_modules.begin(), m_modules.end(), 0.0,  _1 + 
                         bind(&DeUTModule::fractionActive,_2))/double(m_modules.size());   
}
