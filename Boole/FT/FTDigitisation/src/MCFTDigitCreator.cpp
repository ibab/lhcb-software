//-----------------------------------------------------------------------------
// Implementation file for class : MCFTDigitCreator
//
// 2012-04-04 : COGNERAS Eric
//-----------------------------------------------------------------------------
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"

// from FTEvent
#include "Event/MCFTDeposit.h"
#include "Event/MCFTDigit.h"

// local
#include "MCFTDigitCreator.h"
#include "FTSortingFunctor.h"

using namespace LHCb;



// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCFTDigitCreator );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCFTDigitCreator::MCFTDigitCreator( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("InputLocation" , m_inputLocation = LHCb::MCFTDepositLocation::Default, "Path to input MCDeposits");
  declareProperty("OutputLocation" , m_outputLocation = LHCb::MCFTDigitLocation::Default, "Path to output MCDigits");
  declareProperty("MeanPhotoElectronPerMeV",m_meanPhotoElectronPerMeV=20,"Poisson Law Exp. Value for photoelectron production");
  declareProperty("SiPMGain",m_sipmGain = 1,"Gain of SiPM");

}
//=============================================================================
// Destructor
//=============================================================================
MCFTDigitCreator::~MCFTDigitCreator() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode MCFTDigitCreator::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel( MSG::DEBUG) ) {
    debug() << "==> Initialize" << endmsg;
    debug() << ": InputLocation is " <<m_inputLocation << endmsg;
    debug() << ": OutputLocation is " <<m_outputLocation << endmsg;
  }
  
  // Random Service definition : call Poisson distribution
  StatusCode sc3 = m_pePoissonDist.initialize(randSvc(), 
                                              Rndm::Poisson(m_meanPhotoElectronPerMeV));
  if(!sc3){
    error() << "Random number init failure" << endmsg;
    return sc3;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCFTDigitCreator::execute() {

  debug() << "==> Execute" << endmsg;

  // retrieve FTDeposits
  const MCFTDeposits* mcDepositsCont = get<MCFTDeposits>(m_inputLocation);
  debug() <<"mcDepositsCont->size() : " << mcDepositsCont->size()<< endmsg;


  // define digits container
  MCFTDigits *digitCont = new MCFTDigits();
  digitCont->reserve(mcDepositsCont->size());

  // Register MCFTDigits in the transient data store
  put(digitCont, m_outputLocation);

  // For each FTchannel, converts the deposited energy in ADC count
  MCFTDeposits::const_iterator iterDeposit = mcDepositsCont->begin();
  for (; iterDeposit!=mcDepositsCont->end();++iterDeposit){
    MCFTDeposit* mcDeposit = *iterDeposit;
    if ( msgLevel( MSG::DEBUG) ) {
      debug() <<"Channel ="<<mcDeposit->channelID()<< " : " << endmsg;
    }

    // Fill map linking the deposited energy to the MCParticle which deposited it.
    std::map<const LHCb::MCParticle*, double> mcParticleMap;
    std::vector<std::pair <LHCb::MCHit*,double> >::const_iterator vecIter=mcDeposit->mcHitVec().begin();
    double EnergySum = 0;
    for(;vecIter != mcDeposit->mcHitVec().end(); ++vecIter){
      if ( msgLevel( MSG::DEBUG) ) {
        debug() <<" aHit->midPoint().x()="<<vecIter->first->midPoint().x()<< " \tE= " <<vecIter->second
                << " \tfrom PDGId= " <<vecIter->first->mcParticle()->particleID()
                << endmsg;
      }
      mcParticleMap[vecIter->first->mcParticle()] += vecIter->second;
      EnergySum += vecIter->second;
    }

    // Define & store digit
    // The deposited energy to ADC conversion is made by the deposit2ADC method

    MCFTDigit *mcDigit = new MCFTDigit(mcDeposit->channelID(),deposit2ADC(mcDeposit),mcParticleMap);
    digitCont->insert(mcDigit);
  }



  // TEST : print Digit content before sorting
  for (MCFTDigits::const_iterator iterDigit = digitCont->begin(); iterDigit!=digitCont->end();++iterDigit){
    MCFTDigit* mcDigit = *iterDigit;
    if ( msgLevel( MSG::DEBUG) ) {
      debug() <<"Channel ="<<mcDigit->channelID()<< " : " <<"\t ADC ="<<mcDigit->adcCount() << endmsg;
      if(iterDigit > digitCont->begin()){
        debug() <<"Previous Channel " << (mcDigit-1)->channelID()<<" is "
                << (((mcDigit-1)->channelID() < mcDigit->channelID()) ? "lower" : "greater")
                <<" than current channel "<< mcDigit->channelID()<< endmsg;
        debug() <<"and  " 
                << (((*(mcDigit-1)) < *mcDigit) ? "lower" : "greater")
                << endmsg;
      }
    }
    
  }

  // Digits are sorted according to there ChannelID to prepare the clusterisation stage
  // done at this point, before final writing in transient data store
  std::stable_sort( digitCont->begin(), digitCont->end(), LHCb::FTSortingFunctor::LessByChannel<MCFTDigit*>());

  // TEST : print Digit content after sorting
  for (MCFTDigits::const_iterator iterDigit = digitCont->begin(); iterDigit!=digitCont->end();++iterDigit){
    MCFTDigit* mcDigit = *iterDigit;
    if ( msgLevel( MSG::DEBUG) ) {
      debug() <<"Channel ="<<mcDigit->channelID()<< " : " 
              <<"\t ADC ="<<mcDigit->adcCount() << endmsg;
    } 
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCFTDigitCreator::finalize() {

  debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
int MCFTDigitCreator::deposit2ADC(const LHCb::MCFTDeposit* ftdeposit)
{
  /// Compute energy sum
  // TODO :
  // - attenuation of the signal along the fibre
  // - add noise 
  double energySum = 0;
  std::vector<std::pair <LHCb::MCHit*,double> >::const_iterator vecIter= ftdeposit->mcHitVec().begin();
  for(;vecIter != ftdeposit->mcHitVec().end(); ++vecIter){
    energySum += vecIter->second;
  }

  // Convert energy sum in adc count
  // First implementation for pe to ADC according to poisson Law
  int randPE = m_pePoissonDist();
  int adcCount = int(randPE * energySum * m_sipmGain);
  if( msgLevel( MSG::DEBUG) ){
    debug() <<format("deposit2ADC() : energySum=%5f; randPE=%4i; Gain=%4i; adcCount = %4i",energySum,randPE,m_sipmGain,adcCount)
            << endmsg;
  }

  return adcCount;
}
