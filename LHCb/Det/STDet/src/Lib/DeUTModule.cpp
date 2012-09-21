#include "STDet/DeUTModule.h"
#include "STDet/DeUTSector.h"
#include "STDet/DeUTLayer.h"

#include "DetDesc/IGeometryInfo.h"

#include <algorithm>

#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <numeric>

using namespace LHCb;
using namespace boost::lambda;

/** @file DeUTModule.cpp
 *
 *  Implementation of class :  DeUTModule
 *
 *  @author Jianchun Wang, Matt Needham
 *  @date   2012-07-10
 *
 */

DeUTModule::DeUTModule( const std::string& name ) :
  DeSTBaseElement( name ),
  m_detRegion(0u),
  m_firstSector(0u),
  m_column(0u),
  m_prodID(0),
  m_versionString("DC11"),
  m_prodIDString("ProdID")
{ 
  m_sectors.clear();
}


DeUTModule::~DeUTModule()
{
}


const CLID& DeUTModule::clID () const
{
  return DeUTModule::classID() ;
}


StatusCode DeUTModule::initialize()
{
  // initialize method
  StatusCode sc = DeSTBaseElement::initialize();
  
  if ( sc.isFailure() ){
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg; 
  } else {
    m_detRegion   = param<int>("detRegion");
    m_firstSector = param<int>("firstReadoutSector");
    m_column      = param<int>("column");
    m_type        = param<std::string>("moduleType");

    m_numSectors  = param<int>("numSectors");
    m_moduleRotZ  = param<std::string>("moduleRotZ");

    m_parent = getParent<DeUTModule>();
    const STChannelID parentID = m_parent->elementID();
    STChannelID chan(STChannelID::typeUT, parentID.station(), parentID.layer(), m_detRegion, m_firstSector, 0);
    setElementID(chan);
    m_sectors = getChildren<DeUTModule>();
  }
  
  if (exists("version"))
    m_versionString = param<std::string>("version");
  
  sc = registerCondition(this, m_prodIDString, &DeUTModule::updateProdIDCondition, true);
  if (sc.isFailure() ) {
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to register prodID conditions" << endmsg;
    return StatusCode::FAILURE; 
  }

  return StatusCode::SUCCESS;
}


StatusCode DeUTModule::updateProdIDCondition()
{
  const Condition* aCon = condition(m_prodIDString);
  if (aCon == 0){
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to find condition" << endmsg;
    return StatusCode::FAILURE; 
  }
  m_prodID = aCon->param<int>("ProdID");
  
  return StatusCode::SUCCESS;
}


std::ostream& DeUTModule::printOut( std::ostream& os ) const
{
  // stream to cout
  os << " Module : "  << name() 
     << " type " << m_type 
     << " Det region " << m_detRegion
     << " Column " << m_column
     << std::endl;

  return os;
}

MsgStream& DeUTModule::printOut( MsgStream& os ) const{

  // stream to Msg service
  os << " Module : "  << name()
     << " type " << m_type 
     << " Det region " << m_detRegion
     << " Column " << m_column  
     << std::endl;

  return os;
}


DeUTSector* DeUTModule::findSector(const STChannelID aChannel)
{
  Children::iterator iter = std::find_if(m_sectors.begin() , m_sectors.end(),
                                         bind(&DeSTSector::contains, _1, aChannel));

  return (iter != m_sectors.end() ? *iter: 0);
}


DeUTSector* DeUTModule::findSector(const Gaudi::XYZPoint& point)
{
  Children::iterator iter = std::find_if(m_sectors.begin(),m_sectors.end(), 
                                         bind(&DeSTSector::isInside, _1, point)); 

  return (iter != m_sectors.end() ? *iter: 0);
}


double DeUTModule::fractionActive() const
{
  return std::accumulate(m_sectors.begin(), m_sectors.end(),
                         0.0, _1 + bind(&DeSTSector::fractionActive,_2))
    / double(m_sectors.size());
}
