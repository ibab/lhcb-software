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
  DeSTSector( name ),
  m_parent(0)
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

    STChannelID parentID = m_parent->elementID();
    setElementID(parentID);

    // build the id
    setID(parentID.sector());

  }
  return StatusCode::SUCCESS;
}


