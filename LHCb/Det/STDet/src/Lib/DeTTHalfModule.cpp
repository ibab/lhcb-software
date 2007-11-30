
#include "STDet/DeTTHalfModule.h"
#include "STDet/DeTTSector.h"
#include "STDet/DeTTLayer.h"

#include "DetDesc/IGeometryInfo.h"


/** @file DeTTHalfModule.cpp
*
*  Implementation of class :  DeTTHalfModule
*
*    @author Matthew Needham
*/


//STL
#include <algorithm>

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

using namespace LHCb;
using namespace boost::lambda;


DeTTHalfModule::DeTTHalfModule( const std::string& name ) :
  DeSTBaseElement( name ),
  m_prodID(0)
{ 
  // constructer
  m_sectors.clear();
}

DeTTHalfModule::~DeTTHalfModule() {
  // destructer
}

const CLID& DeTTHalfModule::clID () const
{
  return DeTTHalfModule::classID() ;
}

StatusCode DeTTHalfModule::initialize() {

  // initialize method
  MsgStream msg(msgSvc(), name() );

  StatusCode sc = DeSTBaseElement::initialize();
  
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq; 
  }
  else {
    m_detRegion = param<int>("detRegion");
    m_firstSector = param<int>("firstReadoutSector");
    m_position = param<std::string>("top_bottom");
    m_column = param<int>("column");
    m_type = param<std::string>("moduleType");   

    m_parent = getParent<DeTTHalfModule>();
    STChannelID parentID = m_parent->elementID();
    STChannelID chan(STChannelID::typeTT, parentID.station(), parentID.layer(), m_detRegion,m_firstSector,0);
    setElementID(chan);
    m_sectors = getChildren<DeTTHalfModule>();   
  }
 
  return StatusCode::SUCCESS;
}

std::ostream& DeTTHalfModule::printOut( std::ostream& os ) const{

  // stream to cout
  os << " Module : "  << name() 
     << " type " << m_type 
     << "position " << m_position
     << " Det region " << m_detRegion
     << " Column " << m_column
     << std::endl;

  return os;
}

MsgStream& DeTTHalfModule::printOut( MsgStream& os ) const{

  // stream to Msg service
  os << " Module : "  << name()
     << " type " << m_type 
     << "position " << m_position 
     << " Det region " << m_detRegion
     << " Column " << m_column  
     << std::endl;

  return os;
}

DeTTSector* DeTTHalfModule::findSector(const STChannelID aChannel){

  // return pointer to the station from channel
  /*
  int sectorNum = -1;
  if ((m_detRegion == aChannel.detRegion()) && ((m_parent->contains(aChannel) == true))) {
    sectorNum = aChannel.sector() - m_firstSector;    
  }
  return ((sectorNum >= 0 && (unsigned int) sectorNum < m_sectors.size()) ? m_sectors[sectorNum]: 0  );
  */


  Children::iterator iter = std::find_if(m_sectors.begin() , m_sectors.end(), bind(&DeSTSector::contains, _1, aChannel));
  return (iter != m_sectors.end() ? *iter: 0);
}

DeTTSector* DeTTHalfModule::findSector(const Gaudi::XYZPoint& point) {

  // find the half module 
  Children::iterator iter = std::find_if(m_sectors.begin(), m_sectors.end(), 
                                                            bind(&DeTTHalfModule::isInside, _1, point)); 

  return (iter != m_sectors.end() ? *iter: 0);
}



