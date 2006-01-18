#include "STDet/DeITSector.h"
#include "STDet/DeITLayer.h"

/** @file DeITSector.cpp
*
*  Implementation of class :  DeITSector
*
*    @author Matthew Needham
*/

using namespace LHCb;

DeITSector::DeITSector( const std::string& name ) :
  DeSTSector( name )
{ 
  // constructer
}

DeITSector::~DeITSector() {
  // destructer
}

const CLID& DeITSector::clID () const
{
  return DeITSector::classID() ;
}

StatusCode DeITSector::initialize() {
  
  // initialize method
  MsgStream msg(msgSvc(), name() );

  StatusCode sc = DeSTSector::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq;
  }
  else {
    // get the parent
    m_parent = getParent<DeITSector>();

    // build the id
    unsigned int tid = param<int>("sectorID");
    setID(tid);
    STChannelID parentID = m_parent->elementID();
    STChannelID chan(parentID.station(),parentID.layer(), 
                     parentID.detRegion(),  id(), 0);
    setElementID(chan);

  }
  return StatusCode::SUCCESS;
}


