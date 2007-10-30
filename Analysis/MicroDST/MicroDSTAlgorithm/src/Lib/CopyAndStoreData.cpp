// $Id: CopyAndStoreData.cpp,v 1.10 2007-10-30 14:59:44 jpalac Exp $
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
  for (LocalDataStore::const_iterator i = localDataStore().begin();
       i != localDataStore().end();
       ++i ) {
    const std::string TESLocation = outputTESLocation( (*i).first );

    ObjectContainerBase* localContainer = (*i).second;
    verbose() << "Storing local clone of " << (*i).first 
              << " into TES location " << TESLocation << endmsg;
    put(localContainer, TESLocation);
    verbose() << "Stored clone!" << endmsg;
  }
  

  verbose() << "Now clear local data store" << endmsg;
  localDataStore().clear();
  
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode CopyAndStoreData::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
const std::string CopyAndStoreData::getNiceLocationName(const std::string& location) const
{
  std::string tmp(location);
  const std::string tmpString = "/Event/";
  const std::string::size_type loc       = tmp.find(tmpString);
  if ( loc != std::string::npos) {
    tmp.replace(loc, tmpString.length(), "");
    verbose() << "TES location ID is now " << tmp << endmsg;
  }
  return tmp;
}
//=============================================================================
