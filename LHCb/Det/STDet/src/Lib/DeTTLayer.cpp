
#include "STDet/DeTTSector.h"
#include "STDet/DeTTLayer.h"
#include "STDet/DeTTHalfModule.h"
#include "STDet/DeTTStation.h"

#include "DetDesc/IGeometryInfo.h"

#include "Kernel/TTNames.h"

//STL
#include <algorithm>
#include <numeric>

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

using namespace LHCb;
using namespace boost::lambda;

/** @file DeTTLayer.cpp
*
*  Implementation of class :  DeTTLayer
*
*    @author Matthew Needham
*/


DeTTLayer::DeTTLayer( const std::string& name ) :
  DeSTLayer( name ),
  m_parent(NULL)
{ 
  // constructor
  m_modules.clear();
}


DeTTLayer::~DeTTLayer() {
  // destructer
}

const CLID& DeTTLayer::clID () const
{
  return DeTTLayer::classID() ;
}

StatusCode DeTTLayer::initialize() {

  // initialize method
  MsgStream msg(msgSvc(), name() );
  StatusCode sc = DeSTLayer::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg; 
  }
  else {
    m_parent = getParent<DeTTLayer>();
    STChannelID parentID = m_parent->elementID();
    STChannelID chan(STChannelID::typeTT, parentID.station(), id(), 0, 0, 0);
    setElementID(chan);
    m_modules = getChildren<DeTTLayer>();
    flatten();
    m_nickname = TTNames().UniqueLayerToString(chan);
  }
  return sc;
}

DeTTHalfModule* DeTTLayer::findHalfModule(const STChannelID aChannel){

  // return pointer to the station from channel
  Children::iterator iter = std::find_if(m_modules.begin() , m_modules.end(), bind(&DeTTHalfModule::contains, _1, aChannel));
  return (iter != m_modules.end() ? *iter: 0);
}

DeTTHalfModule* DeTTLayer::findHalfModule(const Gaudi::XYZPoint& point) {

  // find the half module 
  Children::iterator iter = std::find_if(m_modules.begin(), m_modules.end(), 
                                                            bind(&DeTTHalfModule::isInside, _1, point)); 

  return (iter != m_modules.end() ? *iter: 0);
}


void DeTTLayer::flatten() {

  DeTTLayer::Children::const_iterator iterModule = halfModules().begin();
  for (;iterModule != halfModules().end(); ++iterModule){
     DeTTHalfModule* tModule = *iterModule;
     DeTTHalfModule::Children::const_iterator iterSector = tModule->sectors().begin();
     for ( ; iterSector !=  tModule->sectors().end() ; ++iterSector ){
       DeSTSector* tSector = *iterSector;
       m_sectors.push_back(tSector);  
     } //sectors     
  } // half module

}


double DeTTLayer::fractionActive() const {

  return std::accumulate(m_modules.begin(), m_modules.end(), 0.0,  _1 + bind(&DeTTHalfModule::fractionActive,_2))/double(m_modules.size());   
}









