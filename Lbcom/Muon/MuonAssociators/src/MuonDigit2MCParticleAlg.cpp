// $Id: MuonDigit2MCParticleAlg.cpp,v 1.7 2006-01-27 17:15:59 asarti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "MuonDigit2MCParticleAlg.h"
//#include "Event/LinksByKey.h"
#include "Linker/LinkerWithKey.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonDigit2MCParticleAlg
//
// 2005-12-29 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonDigit2MCParticleAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonDigit2MCParticleAlg::MuonDigit2MCParticleAlg( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
MuonDigit2MCParticleAlg::~MuonDigit2MCParticleAlg() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonDigit2MCParticleAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonDigit2MCParticleAlg::execute() {

  debug() << "==> Execute" << endmsg;

  LinkerWithKey<LHCb::MCParticle,LHCb::MuonDigit> myLink(eventSvc(),
                                             msgSvc(),
                                             LHCb::MuonDigitLocation::MuonDigit);
  
  SmartDataPtr<LHCb::MuonDigits> digits(eventSvc(),
                                        LHCb::MuonDigitLocation::MuonDigit);
  if (0 == digits){    
    error() << "Failed to find MuonDigits" << endreq;
    return StatusCode::FAILURE;
  }
  
  // loop and link MuonDigits to MC truth
  LHCb::MuonDigits::const_iterator iDigit;
  for(iDigit = digits->begin(); iDigit != digits->end(); iDigit++){
    const LHCb::MCParticle* mcpart=NULL;
    
    associateToTruth(*iDigit,mcpart);
    if(mcpart!=NULL){
      myLink.link( *iDigit, mcpart);
    }    
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonDigit2MCParticleAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
StatusCode MuonDigit2MCParticleAlg:: associateToTruth(LHCb::MuonDigit* digit,
                                                      const LHCb::MCParticle*& 
                                                      mcpart){
  mcpart=NULL;  
  SmartDataPtr<LHCb::MCParticles> mcParticles(eventSvc(),
                                        LHCb::MCParticleLocation::Default);
  if(!mcParticles){
    error() << "Could not find MCParticles in " 
        << LHCb::MCParticleLocation::Default << endreq;
    return StatusCode::FAILURE;
  }

  // get the MCMuonDigits
  SmartDataPtr<LHCb::MCMuonDigits> mcDigits(eventSvc(), 
                                      LHCb::MCMuonDigitLocation::MCMuonDigit);
  
  // match the MCMuonDigit to the MuonDigit via the Key
  LHCb::MCMuonDigit * mcDigit = mcDigits->object(digit->key());
  if(!mcDigit) {
    error() << "Could not find the match for " << digit->key()
        << " in " << LHCb::MCMuonDigitLocation::MCMuonDigit << endreq;
    return StatusCode::FAILURE;
  }

  LHCb::MCMuonDigitInfo digitinfo=mcDigit->DigitInfo();  
  SmartRefVector<LHCb::MCHit>::const_iterator iHit;
  std::vector<LHCb::MCMuonHitHistory>::iterator itHistory=
    mcDigit->HitsHistory().begin();
  
  for( iHit = mcDigit->mcHits().begin() ;
       iHit != mcDigit->mcHits().end() ;
       iHit++ ){
    const LHCb::MCHit * mcHit = *iHit;
    // check the MCMuonHit is still available
    if(mcHit) {
      const LHCb::MCParticle * mcPart = mcHit->mcParticle();
      // check found mcParticle
      if(mcPart){
          // check in the current event container
        if( mcParticles == mcPart->parent() ){  
          //check if muon          
          if(mcPart->particleID().abspid()==13){
            //if muon then ok and exit
            mcpart=mcPart;
            return StatusCode::SUCCESS;
          }          
        }        
      }
    }
    itHistory++;    
  }
  //no muon then check the origin of the digit genat hit in current event?
  if(digitinfo.isGeantHit()){
    if(digitinfo.doesFiringHitBelongToCurrentEvent()){
      itHistory=mcDigit->HitsHistory().begin();
      for( iHit = mcDigit->mcHits().begin() ;
           iHit != mcDigit->mcHits().end() ;
           iHit++ ){
        const LHCb::MCHit * mcHit = *iHit;
        if(itHistory->hasFired()){
          const LHCb::MCParticle * mcPart = mcHit->mcParticle();
           if(mcPart){
             mcpart=mcPart;
             return StatusCode::SUCCESS;
           }           
        }         
        itHistory++;   
      }       
    }
  }
  mcpart=NULL;  
  return StatusCode::SUCCESS;
}

