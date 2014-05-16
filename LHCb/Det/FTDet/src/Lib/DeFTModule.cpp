// $Id: $

// FTDet
#include "FTDet/DeFTModule.h"

/** @file DeFTModule.cpp
 *
 *  Implementation of class : DeFTModule
 *
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 */

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeFTModule::DeFTModule( const std::string& name ) :
  DetectorElement ( name )
{

}

//=============================================================================
// Destructor
//=============================================================================
DeFTModule::~DeFTModule(){} 

//=============================================================================
// classID function
//=============================================================================
const CLID& DeFTModule::clID () const {
  return DeFTModule::classID();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DeFTModule::initialize(){

  StatusCode sc = DetectorElement::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;
  
  MsgStream msg( msgSvc(), name() );
  if(msg.level() <= MSG::DEBUG) msg << MSG::DEBUG << "==> Initialize DeFTModule" << endmsg;

  return StatusCode::SUCCESS;
}

