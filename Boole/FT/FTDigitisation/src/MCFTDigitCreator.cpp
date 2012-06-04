// $Id$
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from FTEvent
#include "Event/MCFTDeposit.h"
#include "Event/MCFTDigit.h"
//#include "Event/MCParticle.h"



// local
#include "MCFTDigitCreator.h"
#include "FTDataFunctor.h"
//-----------------------------------------------------------------------------
// Implementation file for class : MCFTDigitCreator
//
// 2012-04-04 : COGNERAS Eric
//-----------------------------------------------------------------------------

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

  debug() << "==> Initialize" << endmsg;
  debug() << ": InputLocation is " <<m_inputLocation << endmsg;
  debug() << ": OutputLocation is " <<m_outputLocation << endmsg;
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

  // Converts all deposits in FTChannelID in ADC Count
  MCFTDeposits::const_iterator iterDeposit = mcDepositsCont->begin();
  for (; iterDeposit!=mcDepositsCont->end();++iterDeposit){
    MCFTDeposit* aDeposit = *iterDeposit;

    debug() <<"Channel ="<<aDeposit->channelID()<< " : " << endmsg;
    std::map<const LHCb::MCParticle*, double> aMCParticleMap;

    std::vector<std::pair <LHCb::MCHit*,double> >::const_iterator vecIter=aDeposit->mcHitVec().begin();
    for(;vecIter != aDeposit->mcHitVec().end(); ++vecIter){
      debug() <<" \t aHit->midPoint().x()="<<vecIter->first->midPoint().x()<< " \tE= " <<vecIter->second
              << " \tfrom PDGId= " <<vecIter->first->mcParticle()->particleID()
              << endmsg;
      aMCParticleMap[vecIter->first->mcParticle()] += vecIter->second;
    }

    MCFTDigit *aDigit = new MCFTDigit(aDeposit->channelID(),Deposit2ADC(aDeposit),aMCParticleMap);
    digitCont->insert(aDigit);
  }


  // sort Digits
  // TEST : print Digit content before sorting
  for (MCFTDigits::const_iterator iterDigit = digitCont->begin(); iterDigit!=digitCont->end();++iterDigit){
    MCFTDigit* aDigit = *iterDigit;
    debug() <<"Channel ="<<aDigit->channelID()<< " : " <<"\t ADC ="<<aDigit->adcCount() << endmsg;
    if(iterDigit > digitCont->begin()){
      debug() <<"Previous Channel " << (aDigit-1)->channelID()<<" is "
              << (((aDigit-1)->channelID() < aDigit->channelID()) ? "lower" : "greater")
              <<" than current channel "<< aDigit->channelID()<< endmsg;
    }
  }

  std::stable_sort(digitCont->begin(),digitCont->end(),FTDataFunctor::Less_by_Channel<MCFTDigit*>());
  for (MCFTDigits::const_iterator iterDigit = digitCont->begin(); iterDigit!=digitCont->end();++iterDigit){
    MCFTDigit* aDigit = *iterDigit;
    debug() <<"Channel ="<<aDigit->channelID()<< " : " <<"\t ADC ="<<aDigit->adcCount() << endmsg;
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
int MCFTDigitCreator::Deposit2ADC(MCFTDeposit* ftdeposit)
{
  /// Compute energy sum
  double energySum = 0;
  std::vector<std::pair <LHCb::MCHit*,double> >::const_iterator vecIter= ftdeposit->mcHitVec().begin();
  for(;vecIter != ftdeposit->mcHitVec().end(); ++vecIter){
    energySum += vecIter->second;
  }

  // Convert energy sum in adc count
  // Test : trivial : adc = (int) 100 * energySum
  int adcCount = (int) 100 * energySum;
  return adcCount;

}
