// $Id: MuonDigit2MCParticleAlg.cpp,v 1.5 2002-07-03 09:32:12 dhcroft Exp $
// Include files 

#include "Event/MuonDigit.h"
#include "Event/MCMuonDigit.h"
#include "Event/MCMuonHit.h"
#include "Event/MCParticle.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "MuonDigit2MCParticleAlg.h"
#include "MuonAssociators/MuonDigit2MCParticleAsct.h"


//-----------------------------------------------------------------------------
// Implementation file for class : MuonDigit2MCParticleAlg.cpp
//
// 12/06/2002 David Hutchcroft
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MuonDigit2MCParticleAlg>          s_factory ;
const        IAlgFactory& MuonDigit2MCParticleAlgFactory = s_factory ; 

MuonDigit2MCParticleAlg::MuonDigit2MCParticleAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm (name,pSvcLocator) 
{
  // constructor
  declareProperty( "OutputData", 
                   m_outputData  = MuonDigit2MCParticleLocation );
}

MuonDigit2MCParticleAlg::~MuonDigit2MCParticleAlg() {
  // destructor
} 

StatusCode MuonDigit2MCParticleAlg::initialize() {

  return StatusCode::SUCCESS;
}


StatusCode MuonDigit2MCParticleAlg::execute() {

  // retrieve any existing table from the store
  DataObject *dObj;
  StatusCode sc = eventSvc()->findObject(outputData(),dObj);
  MuonDigit2MCParticleAsct::Table* aTable = 
    dynamic_cast<MuonDigit2MCParticleAsct::Table*>(dObj);    
  if(0 == aTable){
    // Need a new table to add to the store
    aTable = new MuonDigit2MCParticleAsct::Table();

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

  // get MuonDigits
  SmartDataPtr<MuonDigits> digits(eventSvc(),MuonDigitLocation::MuonDigit);
  if (0 == digits){ 
    MsgStream log(msgSvc(), name());
    log << MSG::WARNING << "Failed to find MuonDigits" << endreq;
    return StatusCode::FAILURE;
  }

  // loop and link MuonDigits to MC truth
  MuonDigits::const_iterator iDigit;
  for(iDigit = digits->begin(); iDigit != digits->end(); iDigit++){
    associateToTruth(*iDigit,aTable);
  } 

  return StatusCode::SUCCESS;
}

StatusCode MuonDigit2MCParticleAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode 
MuonDigit2MCParticleAlg::associateToTruth(const MuonDigit* digit,
                                          MuonDigit2MCParticleAsct::Table* 
                                          table) {

  // get the MCMuonDigits
  SmartDataPtr<MCMuonDigits> mcDigits(eventSvc(), 
                                      MCMuonDigitLocation::MCMuonDigit);
  
  // match the MCMuonDigit to the MuonDigit via the Key
  MCMuonDigit * mcDigit = mcDigits->object(digit->key());
  if(!mcDigit) {
    MsgStream log(msgSvc(), name());
    log << MSG::ERROR << "Could not find the match for " << digit->key()
        << " in " << MCMuonDigitLocation::MCMuonDigit << endreq;
    return StatusCode::FAILURE;
  }

  // loop over MCMuonHits attached to this MCMuonDigit
  SmartRefVector<MCMuonHit>::const_iterator iHit;
  for( iHit = mcDigit->mcMuonHits().begin() ;
       iHit != mcDigit->mcMuonHits().end() ;
       iHit++ ){
    const MCMuonHit * mcHit = *iHit;
    // check the MCMuonHit is still available
    if(mcHit) {
      const MCParticle * mcPart = mcHit->mcParticle();
      // check found mcParticle
      if(mcPart){
        // do not make links to MCParticles with no parent vertex
        // ("fake" ATMC entries added in FORTRAN to fix MURW-ATMC links)
        if( 0 != mcPart->originVertex() ){
          table->relate(digit,mcPart);
        }
      }
    }
  }

  return StatusCode::SUCCESS;
   
}
