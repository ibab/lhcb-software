// $Id: MuonCoord2MCParticleAlg.cpp,v 1.4 2002-07-03 09:32:11 dhcroft Exp $
// Include files 

#include "Event/MuonCoord.h"
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
}

// Empty destructor
MuonCoord2MCParticleAlg::~MuonCoord2MCParticleAlg(){}


StatusCode MuonCoord2MCParticleAlg::initialize() {

  return StatusCode::SUCCESS;
}


StatusCode MuonCoord2MCParticleAlg::execute() {

  // retrieve any existing table from the store
  DataObject *dObj;
  StatusCode sc = eventSvc()->findObject(outputData(),dObj);
  MuonCoord2MCParticleAsct::Table* aTable = 
    dynamic_cast<MuonCoord2MCParticleAsct::Table*>(dObj);    
  if(0 == aTable){
    // Need a new table to add to the store
    aTable = new MuonCoord2MCParticleAsct::Table();

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

StatusCode 
MuonCoord2MCParticleAlg::associateToTruth(const MuonCoord * coord,
                                          MuonCoord2MCParticleAsct::Table 
                                                          *aTable){
  
  // loop over referenced digits
  SmartRefVector<MuonDigit>::const_iterator iDigit;
  for( iDigit = coord->MuonDigits().begin() ; 
       iDigit != coord->MuonDigits().end() ;
       iDigit++ ){

    // get the MCMuonDigits
    SmartDataPtr<MCMuonDigits> mcDigits(eventSvc(), 
                                        MCMuonDigitLocation::MCMuonDigit);
  
    // match the MCMuonDigit to the MuonDigit via the Key
    MCMuonDigit * mcDigit = mcDigits->object((*iDigit)->key());
    if(!mcDigit) {
      MsgStream log(msgSvc(), name());
      log << MSG::ERROR << "Could not find the match for " << (*iDigit)->key()
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
            aTable->relate(coord,mcPart);
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}
