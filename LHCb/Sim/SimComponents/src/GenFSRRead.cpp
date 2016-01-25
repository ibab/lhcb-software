// Include files 

 // from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/Auditor.h"

// local
#include "GenFSRRead.h"

// from Event
#include "Event/GenFSR.h"
#include "Event/GenCountersFSR.h"
#include "Event/CrossSectionsFSR.h"
#include "Event/GenHeader.h"

// to write a file                
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>

//-----------------------------------------------------------------------------
// Implementation file for class : GenFSRRead
//
// 2015-06-23 : Davide Fazzini
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GenFSRRead )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GenFSRRead::GenFSRRead( const std::string& name,
                          ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator ),
  m_fileRecordSvc(NULL),
  m_navigatorTool(NULL)
{
  // expect the data to be written at LHCb::GenFSRLocation::Default
  declareProperty( "FileRecordLocation" , m_fileRecordName = "/FileRecords"        );  
  declareProperty( "FSRName"            , m_FSRName        = "/GenFSR"             );
}
//=============================================================================
// Destructor
//=============================================================================
GenFSRRead::~GenFSRRead() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GenFSRRead::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  // get the File Records service              
  m_fileRecordSvc = svc<IDataProviderSvc>("FileRecordDataSvc", true);
  
  // prepare navigator tool  
  m_navigatorTool = tool<IFSRNavigator>("FSRNavigator", "FSRNavigator"); // gen, Gen
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GenFSRRead::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GenFSRRead::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;

  GenFSRRead::dumpFSR();

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================


//=============================================================================      
//  Dump the GenFSR informations
//=============================================================================      

void GenFSRRead::dumpFSR()
{
  // root of store                                                                                                                            
  std::string fileRecordRoot = m_fileRecordName;
  // make an inventory of the FileRecord store                                                                                                
  std::vector< std::string > addresses = m_navigatorTool->navigate(fileRecordRoot, m_FSRName);
  
  int nFSRs = 0;  
  
  for(std::vector< std::string >::iterator iAddr=addresses.begin(); iAddr!=addresses.end(); iAddr++)
    nFSRs += 1;
    
  if(nFSRs != 0)
  {    
    always() << "Dump the genFSR informations!" << endmsg;
    
    for(std::vector< std::string >::iterator iAddr=addresses.begin(); iAddr!=addresses.end(); iAddr++)
    {
      std::string genRecordAddress = *iAddr;
      always() << "Address : " << genRecordAddress << endmsg;
      
      // read GenFSR                                                               
      LHCb::GenFSR* genFSR = getIfExists<LHCb::GenFSR>(m_fileRecordSvc, genRecordAddress);
        
      if (genFSR==NULL) 
      {
        Warning("A genFSR record was not found").ignore();     
        if (msgLevel(MSG::DEBUG)) debug() << genRecordAddress << " not found" << endmsg;
      }
      else 
      {
        always() << "READ FSR: " << genRecordAddress << endmsg;
        always() << *genFSR << endmsg;
        always() << "**********************************   " << endmsg;
      }
      always() << "*****************************************************" << endmsg;
    }
  }
  else
    if (msgLevel(MSG::DEBUG)) debug() << "There are not FSRs to write" << endmsg;

}

