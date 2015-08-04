// Include files 

#include "Event/MCHit.h"
#include "Event/MCMuonDigit.h"
#include "Linker/LinkerWithKey.h"

// local
#include "MuonDigit2MCParticleAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonDigit2MCParticleAlg
//
// 2005-12-29 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonDigit2MCParticleAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonDigit2MCParticleAlg::MuonDigit2MCParticleAlg( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
 declareProperty("AssociateAll" , m_associateAll=true) ;
}
//=============================================================================
// Destructor
//=============================================================================
MuonDigit2MCParticleAlg::~MuonDigit2MCParticleAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonDigit2MCParticleAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonDigit2MCParticleAlg::execute() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LinkerWithKey<LHCb::MCParticle,LHCb::MuonDigit> myLink(eventSvc(),
                                             msgSvc(),
                                             rootInTES()+LHCb::MuonDigitLocation::MuonDigit);
  
  LHCb::MuonDigits* digits =
    get<LHCb::MuonDigits>(LHCb::MuonDigitLocation::MuonDigit);
  
  // loop and link MuonDigits to MC truth
  LHCb::MuonDigits::const_iterator iDigit;
  //  debug()<<" qui "<<endmsg;
  for(iDigit = digits->begin(); iDigit != digits->end(); iDigit++){
    //    debug()<<" qui 1 "<<endmsg;
    const LHCb::MCParticle* mcpart=NULL;
    //    debug()<<" qui 2 "<<endmsg;
    
    StatusCode sc = associateToTruth(*iDigit,mcpart);
    //    debug()<<" qui 3 "<<endmsg;

    if( sc.isSuccess() && mcpart!=NULL ){
      myLink.link( *iDigit, mcpart);
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode MuonDigit2MCParticleAlg:: associateToTruth(LHCb::MuonDigit* digit,
                                                      const LHCb::MCParticle*& 
                                                      mcpart){
  mcpart=NULL;  
  LHCb::MCParticles* mcParticles = 
    get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default,IgnoreRootInTES);

  LHCb::MCMuonDigits* mcDigits = 
    get<LHCb::MCMuonDigits>(LHCb::MCMuonDigitLocation::MCMuonDigit);
  
  // match the MCMuonDigit to the MuonDigit via the Key
  LHCb::MCMuonDigit * mcDigit = mcDigits->object(digit->key());
  if(!mcDigit) {
    error() << "Could not find the match for " << digit->key()
        << " in " << LHCb::MCMuonDigitLocation::MCMuonDigit << endmsg;
    return StatusCode::FAILURE;
  }
  // debug()<<" qui "<<endmsg;
  LHCb::MCMuonDigitInfo digitinfo=mcDigit->DigitInfo();  
  SmartRefVector<LHCb::MCHit>::const_iterator iHit;
  std::vector<LHCb::MCMuonHitHistory>::iterator itHistory=
    mcDigit->HitsHistory().begin();
  //    info()<<" prima ricerca mu"<<mcDigit->HitsHistory().size()<<" "<<
  //	mcDigit->mcHits().size()<<endmsg;

  for( iHit = mcDigit->mcHits().begin() ;
       iHit != mcDigit->mcHits().end() ;
       iHit++ ){
    const LHCb::MCHit * mcHit = *iHit;
    // check the MCMuonHit is still available
    if(mcHit) {
      //      info()<<" dopo "<<endmsg;
      
      const LHCb::MCParticle * mcPart = mcHit->mcParticle();
      // check found mcParticle
      if(mcPart){  
        //  info()<<" dopo 1 "<<mcPart<<endmsg;
      
        //  info()<<" dopo 1 "<<mcPart->particleID().abspid()<<endmsg;
          // check in the current event container
        if( mcParticles == mcPart->parent() ){  
          //check if muon    
          //  info()<<" dopo 2"<<endmsg;
          if(mcPart->particleID().abspid()==13){
            //if muon then ok and exit
            mcpart=mcPart;
            return StatusCode::SUCCESS;
          }          
        }  
        //info()<<" dopo 4"<<endmsg;
      }
    }
    itHistory++;    
  }
  //no muon then check the origin of the digit genat hit in current event?
  //info()<<" dopo ricerca mu"<<mcDigit->HitsHistory().size()<<" "<<
  //	mcDigit->mcHits().size()<<endmsg;

  if(digitinfo.isGeantHit()){
    if(digitinfo.doesFiringHitBelongToCurrentEvent()||m_associateAll){
      itHistory=mcDigit->HitsHistory().begin();
      for( iHit = mcDigit->mcHits().begin() ;
           iHit != mcDigit->mcHits().end() ;
           iHit++ ){
        const LHCb::MCHit * mcHit = *iHit; 
        if(mcHit) {
          //          info()<<" dopo hit "<<endmsg;
          //info()<<" fired ? "<<itHistory->hasFired()<<endmsg;
        
          if(itHistory->hasFired()){
            //    info()<<mcHit<<" "<<endmsg;
            //info()<<"part "<<mcHit->mcParticle()<<endmsg;
            
            const LHCb::MCParticle * mcPart = mcHit->mcParticle();
            if(mcPart){
              mcpart=mcPart;
              //  info()<<" eccoci "<<endmsg;
              
              return StatusCode::SUCCESS;
            }           
          }       
        }
        
        itHistory++;   
      }       
    }
  }
  mcpart=NULL;  
  // info()<<" eccoci qui "<<endmsg;
  return StatusCode::SUCCESS;
}

