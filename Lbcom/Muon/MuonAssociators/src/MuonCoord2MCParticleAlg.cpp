// $Id: MuonCoord2MCParticleAlg.cpp,v 1.3 2002-07-02 11:21:13 dhcroft Exp $
// Include files 

#include "Event/MuonCoord.h"
#include "Event/MuonDigit.h"
#include "Event/MCParticle.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "MuonCoord2MCParticleAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonCoord2MCParticleAlg
//
// 2002-06-12 : David Hutchcroft
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MuonCoord2MCParticleAlg>          s_factory ;
const        IAlgFactory& MuonCoord2MCParticleAlgFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonCoord2MCParticleAlg::MuonCoord2MCParticleAlg( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : Algorithm (name,pSvcLocator) 
{
  // constructor
  declareProperty( "OutputData", 
                   m_outputData  = MuonCoord2MCParticleLocation );
  declareProperty("associatorName", 
                  m_nameAsct = "MuonDigit2MCParticleAsct" );
}

// Empty destructor
MuonCoord2MCParticleAlg::~MuonCoord2MCParticleAlg(){}


StatusCode MuonCoord2MCParticleAlg::initialize() {
  StatusCode sc = toolSvc()->retrieveTool(m_nameAsct, m_hAsct);
  if( sc.isFailure() || 0 == m_hAsct) {
    MsgStream log(msgSvc(), name());
    log << MSG::FATAL << "    Unable to retrieve Associator tool" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}


StatusCode MuonCoord2MCParticleAlg::execute() {

  // retrieve any existing table from the store
  DataObject *dObj;
  StatusCode sc = eventSvc()->findObject(outputData(),dObj);
  Table* aTable = dynamic_cast<Table*>(dObj);    
  if(0 == aTable){
    // Need a new table to add to the store
    aTable = new Table();

    // register table in store
    sc = eventSvc()->registerObject(outputData(), aTable);
    if( sc.isFailure() ) {
      MsgStream log(msgSvc(), name());
      log << MSG::FATAL << "     *** Could not register " << outputData()
          << endreq;
      delete aTable;
      return StatusCode::FAILURE;
    }
    
  }else{
    // clear the existing table
    sc = aTable->clear();
    if(!sc){
      MsgStream log(msgSvc(), name());
      log << MSG::ERROR << "Could not clear existing table " << outputData()
          << endreq;
      return sc;
    }
  }

  // get MuonCoords from TES
  char TESPath[100];
  int station;
  for(station = 0 ; station < 5 ; station++ ){
    sprintf(TESPath, MuonCoordLocation::MuonCoords.c_str(), station+1);
    SmartDataPtr<MuonCoords> coords(eventSvc(),TESPath);
    if(!coords){
      MsgStream log(msgSvc(), name());
      log << MSG::WARNING << "Failed to find MuonCoords at " 
          << TESPath
          << endreq;
      return StatusCode::FAILURE;
    }
    // loop over coordinates
    MuonCoords::const_iterator iCoord;
    for(iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){
      associateToTruth(*iCoord,aTable);
    }
  }
        
  return StatusCode::SUCCESS;
}

StatusCode MuonCoord2MCParticleAlg::finalize() {

  return StatusCode::SUCCESS;
}

StatusCode MuonCoord2MCParticleAlg::associateToTruth(const MuonCoord * coord,
                                                     Table *aTable){
  
  // loop over referenced digits
  SmartRefVector<MuonDigit>::const_iterator iDigit;
  for( iDigit = coord->MuonDigits().begin() ; 
       iDigit != coord->MuonDigits().end() ;
       iDigit++ ){
    
    // use MuonDigit->MCParticle table to make links
    MuonDigit2MCParticleAsct::MCParticles mcPartRange = 
      m_hAsct->rangeFrom(*iDigit);
    MuonDigit2MCParticleAsct::MCParticlesIterator iMCP;
    for( iMCP = mcPartRange.begin() ; 
         iMCP != mcPartRange.end(); iMCP ++){
      MCParticle *mcPart = iMCP->to();
      if(mcPart){
        aTable->relate(coord,mcPart);
      }
    }  
  }

  return StatusCode::SUCCESS;
}
