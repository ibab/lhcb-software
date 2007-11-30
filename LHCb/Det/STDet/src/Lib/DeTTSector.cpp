#include "STDet/DeTTSector.h"
#include "STDet/DeTTHalfModule.h"

/** @file DeTTSector.cpp
*
*  Implementation of class :  DeTTSector
*
*    @author Matthew Needham
*/

using namespace LHCb;

DeTTSector::DeTTSector( const std::string& name ) :
  DeSTSector( name )
{ 
  // constructer
}

DeTTSector::~DeTTSector() {
  // destructer
}

const CLID& DeTTSector::clID () const
{
  return DeTTSector::classID() ;
}

StatusCode DeTTSector::initialize() {
  
  // initialize method
  MsgStream msg(msgSvc(), name() );

  StatusCode sc = DeSTSector::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq;
  }
  else {

    // get the parent
    m_parent = getParent<DeTTSector>();

    unsigned int tSize;
    m_parent->type() == "KLM" ? tSize = 3 : tSize = 2;

    // sub id
    unsigned subID = param<int>("subID");

    // sector number needs info from mother
    if (m_parent->position() == "B"){
     setID(m_parent->firstSector()+subID-1);
    }
    else {
      setID(m_parent->firstSector() + tSize - subID);
    }
   
    // row..
    if (m_parent->type() == "KLM"){
      if (m_parent->position() == "B"){
        m_row = id() - m_parent->firstSector() + 1;
      }
      else {
        m_row = id() - m_parent->firstSector() + 4;
      }  
    } 
    else {
      if (m_parent->position() == "B"){
        m_row = id() - m_parent->firstSector() + 1;
      }
      else {
        m_row = id() - m_parent->firstSector() + 3;
      }
    }

    // build the id
    STChannelID parentID = m_parent->elementID();
    STChannelID chan(STChannelID::typeTT,parentID.station(),parentID.layer(), 
                     parentID.detRegion(),  id(), 0);
    setElementID(chan);

  }
  return StatusCode::SUCCESS;
}

inline unsigned int DeTTSector::prodID() const{
  return m_parent->prodID();
}
