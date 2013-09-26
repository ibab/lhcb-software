#include <vector>

#include "GaudiKernel/AlgFactory.h"
#include "BcmDigitization.h"
#include "Event/BcmDigit.h"
#include "Event/MCBcmDigit.h"

DECLARE_ALGORITHM_FACTORY( BcmDigitization )

BcmDigitization::BcmDigitization(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty("BcmHitsLocation", m_bcmHitsLocation = "MC/Bcm/Hits" );
  //  declareProperty("BcmUpDetLocation", m_bcmUpDetLocation = DeBcmLocation::BcmUp);
  //  declareProperty("BcmDownDetLocation", m_bcmDownDetLocation = DeBcmLocation::BcmDown);
  declareProperty("BcmDigitLocation", m_bcmDigitLocation = LHCb::BcmDigitLocation::Default);
  declareProperty("BcmMCDigitLocation", m_bcmMCDigitLocation = LHCb::MCBcmDigitLocation::Default);
  declareProperty("MakeMCBcmDigits",m_bcmMCDigits = true);
}

StatusCode BcmDigitization::initialize()
{  
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by  GaudiAlgorithm	

  // Get detectors (not acutally needed at the moment...)
  //  if(!existDet<DeBcm>(m_bcmUpDetLocation)){
  //    warning() << "There is no detector at " << m_bcmUpDetLocation << endmsg;
  //  }
  //  else{
  //    debug() << "Getting BCM Up" << endmsg;
  //    m_bcmUpDet = getDet<DeBcm>(m_bcmUpDetLocation);
  //  }
  //  if(!existDet<DeBcm>(m_bcmDownDetLocation)){
  //    warning() << "There is no detector at " << m_bcmDownDetLocation << endmsg;
  //  }
  //  else{
  //    debug() << "Getting BCM Down" << endmsg;
  //    m_bcmDownDet = getDet<DeBcm>(m_bcmDownDetLocation);
  //  }
  return StatusCode::SUCCESS;
}
 
 					
StatusCode BcmDigitization::execute()
{

  //  MsgStream log(msgSvc(), name());
 	StatusCode sc=StatusCode::SUCCESS ;
  if(msgLevel(MSG::DEBUG)) debug()<<"starting the BCM Digitization algorithm "<<endmsg;

  // Check existance of hit location
  const LHCb::MCHits* bcmMCHits = getIfExists<LHCb::MCHits>(m_bcmHitsLocation);
  if( NULL == bcmMCHits ) 
    return Error("There are no MCHits at " + m_bcmHitsLocation + " in TES!", StatusCode::SUCCESS );
  
  // Set up digi sensors
  //std::vector<BcmDigi>* sensors = new std::vector<BcmDigi>;
  LHCb::BcmDigits* sensors = new LHCb::BcmDigits;
  LHCb::MCBcmDigits* mcsensors = new LHCb::MCBcmDigits;
  for(int i=0; i<2; i++){
    for(int j=0; j<8; j++){
      LHCb::BcmDigit* sens = new LHCb::BcmDigit();
      sens->setStation(i);
      sens->setSensor(j);
      sensors->insert(sens,i*8+j);
      LHCb::MCBcmDigit* mcsens = new LHCb::MCBcmDigit();
      mcsensors->insert(mcsens,i*8+j);
    }
  } 
  
  // Run over hits
  LHCb::MCHits::const_iterator i;
  int index=0;
  for (i=bcmMCHits->begin();i<bcmMCHits->end();i++){         
    const LHCb::MCParticle* origparticle=(*i)->mcParticle();
    if(origparticle)  {
      if(msgLevel(MSG::DEBUG)) debug()<<"Particle from which the hit originates (PDG code)"<<
      origparticle->particleID().abspid()<<endmsg;
      if(msgLevel(MSG::DEBUG)) 
        debug() << "EnDep " << (*i)->energy() << " in sensor " << (*i)->sensDetID() << endmsg;
      ((LHCb::BcmDigit*) sensors->object((*i)->sensDetID()))->setSignal(((LHCb::BcmDigit*)sensors->containedObject((*i)->sensDetID()))->signal()+(*i)->energy());
      std::vector<int> keys = ((LHCb::MCBcmDigit*) mcsensors->object((*i)->sensDetID()))->mcHitKeys();
      keys.push_back(index);
      ((LHCb::MCBcmDigit*) mcsensors->object((*i)->sensDetID()))->setMCHitKeys(keys);
    }
    else{
      warning()<<
      "Particle from which the hit originates is not defined "<< endmsg;
    }
    index++;
	}
  put( evtSvc(), sensors, m_bcmDigitLocation );
  if(m_bcmMCDigits){
    put( evtSvc(), mcsensors, m_bcmMCDigitLocation );
  }
  else
    delete mcsensors;
  
 	if(msgLevel(MSG::DEBUG)) debug()<<"End of the BCM Digitization"<<endmsg;

  return StatusCode::SUCCESS;
  
}
