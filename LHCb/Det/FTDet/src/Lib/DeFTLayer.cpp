// $Id: $

// FTDet
#include "FTDet/DeFTLayer.h"

/** @file DeFTLayer.cpp
 *
 *  Implementation of class : DeFTLayer
 *
 *  @author Plamen Hopchev
 *  @author Eric Cogneras
 *  @date   2012-04-25
 */


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeFTLayer::DeFTLayer( const std::string& name ) :
  DetectorElement ( name )
{
  
}


//=============================================================================
// Destructor
//=============================================================================
DeFTLayer::~DeFTLayer(){} 

//=============================================================================
// classID function
//=============================================================================
const CLID& DeFTLayer::clID () const {
  return DeFTLayer::classID();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DeFTLayer::initialize(){

  StatusCode sc = DetectorElement::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  MsgStream msg( msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialize DeFTLayer" << endmsg;
  
  return StatusCode::SUCCESS;
}
