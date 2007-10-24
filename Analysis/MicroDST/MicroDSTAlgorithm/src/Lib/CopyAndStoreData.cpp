// $Id: CopyAndStoreData.cpp,v 1.7 2007-10-24 17:28:09 jpalac Exp $
// Include files 

// from Gaudi
#include <GaudiKernel/AlgFactory.h>
#include <GaudiKernel/KeyedContainer.h>
// local
#include "MicroDST/CopyAndStoreData.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyAndStoreData
//
// 2007-10-11 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyAndStoreData::CopyAndStoreData( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : 
  GaudiAlgorithm ( name , pSvcLocator ),
  m_inputTESLocation(""),
  m_outputPrefix("microDST")
{
  declareProperty( "InputLocation", m_inputTESLocation );
  declareProperty( "OutputPrefix",  m_outputPrefix  );
}
//=============================================================================
// Destructor
//=============================================================================
CopyAndStoreData::~CopyAndStoreData() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyAndStoreData::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  // Remove any leading "/Event" in input TES location.
  getNiceLocationName( this->inputTESLocation() );
  setFullOutputTESLocation();
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode CopyAndStoreData::execute() {

  debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyAndStoreData::copyLocalStoreToTES() 
{
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode CopyAndStoreData::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
void CopyAndStoreData::getNiceLocationName(std::string& location)
{
  const std::string tmpString = "/Event/";
  const std::string::size_type loc       = location.find(tmpString);
  if ( loc != std::string::npos) {
    location.replace(loc, tmpString.length(), "");
    verbose() << "TES location ID is now " << location << endmsg;
  }
}
//=============================================================================
const std::string CopyAndStoreData::getNiceLocationName(const std::string& location) const
{
  std::string return_value(location);
  getNiceLocationName(return_value);
  return return_value;
}
//=============================================================================
