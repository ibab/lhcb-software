// $Id: MuonCoord2MCParticleAlg.cpp,v 1.7 2004-04-19 11:54:53 cattanem Exp $
// Include files 

#include "Event/MuonCoord.h"
#include "Event/MuonDigit.h"
#include "Event/MCMuonDigit.h"
#include "Event/MCMuonHit.h"
#include "Event/MCParticle.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IRegistry.h"

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
  // get the MCParticles for the event, do not make links to spillover
  SmartDataPtr<MCParticles> mcParticles(eventSvc(),
                                        MCParticleLocation::Default);
  MsgStream log(msgSvc(), name());
  if(!mcParticles){
    
    log << MSG::ERROR << "Could not find MCParticles in " 
        << MCParticleLocation::Default << endreq;
    return StatusCode::FAILURE;
  }
  
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

      log << MSG::ERROR << "Could not find the match for " << (*iDigit)->key()
          << " in " << MCMuonDigitLocation::MCMuonDigit << endreq;
      return StatusCode::FAILURE;
    }

    unsigned int bcross=mcDigit->DigitInfo().BX();
    log<<MSG::DEBUG<<"bunch crossing "<<bcross<<endreq;
    if( bcross != 0 ) return StatusCode::SUCCESS;
    
    // loop over MCMuonHits attached to this MCMuonDigit
    SmartRefVector<MCMuonHit>::const_iterator iHit;

    //    std::vector<MCMuonHitHistory>& mcDigit->HitsHistory();
    std::vector<MCMuonHitHistory>::iterator 
      iterHistory=(mcDigit)->HitsHistory().begin();
    for( iHit = mcDigit->mcMuonHits().begin() ;
         iHit != mcDigit->mcMuonHits().end() ;
         iHit++, iterHistory++ ){
      // check the MCMuonHit is still available
      log<<MSG::DEBUG<<"bunch crossing "<< iterHistory->BX()<<" "<<
        iterHistory->isGeantHit()<<endreq;

      // Make association only for hits in main event
      if( (0 == iterHistory->BX()) && iterHistory->isGeantHit() ){
        const MCMuonHit * mcHit = *iHit;
        if(mcHit) {
          const MCParticle * mcPart = mcHit->mcParticle();
          // check found mcParticle
          if(mcPart){
            // check in the current event container
            if( mcParticles == mcPart->parent() ){
              log<<MSG::DEBUG<<mcPart->parent()->registry()->identifier()
                 <<endmsg;
              aTable->relate(coord,mcPart);
            }
          }
        }  
      }      
    }    
  }

  return StatusCode::SUCCESS;
}
