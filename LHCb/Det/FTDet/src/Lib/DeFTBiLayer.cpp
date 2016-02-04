// FTDet
#include "FTDet/DeFTBiLayer.h"

/** @file DeFTBiLayer.cpp
 *
 *  Implementation of class : DeFTBiLayer
 *
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 */

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeFTBiLayer::DeFTBiLayer( const std::string& name ) :
  DetectorElement ( name )
{

}

//=============================================================================
// Destructor
//=============================================================================
DeFTBiLayer::~DeFTBiLayer() = default;

//=============================================================================
// classID function
//=============================================================================
const CLID& DeFTBiLayer::clID () const {
  return DeFTBiLayer::classID();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DeFTBiLayer::initialize(){

  StatusCode sc = DetectorElement::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;
  
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Initialize DeFTBiLayer" << endmsg;

  return StatusCode::SUCCESS;
}

