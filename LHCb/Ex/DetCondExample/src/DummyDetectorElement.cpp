// $Id: DummyDetectorElement.cpp,v 1.1 2005-04-22 15:17:54 marcocle Exp $
// Include files 

#include <string>
#include "DetDesc/IReadOut.h"
#include "DetDesc/ISlowControl.h"
#include "DetDesc/Condition.h"

// local
#include "DummyDetectorElement.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DummyDetectorElement
//
// 2005-04-11 : Marco CLEMENCIC
//-----------------------------------------------------------------------------


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DummyDetectorElement::DummyDetectorElement(const std::string& name):
        DetectorElement(name)
{
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DummyDetectorElement::DummyDetectorElement(const std::string& name,
                                           const ITime& iValidSince,
                                           const ITime& iValidTill):
        DetectorElement(name,iValidSince,iValidTill)
{
}
//=============================================================================
// Destructor
//=============================================================================
DummyDetectorElement::~DummyDetectorElement() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DummyDetectorElement::initialize(){
  MsgStream log(msgSvc(),name());
  
  log << MSG::DEBUG << " --- initialize DummyDetectorElement --- " << endmsg;
  StatusCode sc = DetectorElement::initialize();
  if ( !sc.isSuccess() ) return sc;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Update methods
//=============================================================================
StatusCode DummyDetectorElement::i_updateTemperatures(){
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "Entering i_updateTemperatures()" << endmsg;
  try {
    m_slowTemp   = slowControl()->condition()->paramAsDouble("Temperature");
    m_cratesTemp = readOut()->condition()->paramVectorAsDouble("CrateTemps");
  } catch (...) {
    log << MSG::ERROR << "i_updateTemperatures: couldn't access condition" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
// Update methods
//=============================================================================
StatusCode DummyDetectorElement::i_updateChannels(){
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "Entering i_updateChannels()" << endmsg;
  try {
    m_maxChannels = readOut()->condition()->paramAsInt("NChannels");
    m_activeChannels = readOut()->condition()->paramVectorAsInt("Channels");
  } catch (...) {
    log << MSG::ERROR << "i_updateChannels: couldn't access condition" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
