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
  declareProperty("InputLocation" ,       m_inputLocation        = LHCb::MCFTDepositLocation::Default );
  declareProperty("OutputLocation" ,      m_outputLocation       = LHCb::MCFTDigitLocation::Default   );
  declareProperty("PhotoElectronsPerMeV", m_photoElectronsPerMeV = 200. );
  declareProperty("SiPMGain",             m_sipmGain             =   1. );
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
        debug() << format( " aHit->midPoint().x()=%10.3f E=%10.3f from MCParticle %5d",
                           vecIter->first->midPoint().x(), vecIter->second, 
                           vecIter->first->mcParticle()->key() ) << endmsg;
      }
      mcParticleMap[vecIter->first->mcParticle()] += vecIter->second;
      EnergySum += vecIter->second;
    }

    // Define & store digit
    // The deposited energy to ADC conversion is made by the deposit2ADC method
    int adc = deposit2ADC(mcDeposit);
    if ( 0 < adc ) {
      MCFTDigit *mcDigit = new MCFTDigit(mcDeposit->channelID(), adc, mcParticleMap );
      digitCont->insert(mcDigit);
    }
  }

  // Digits are sorted according to there ChannelID to prepare the clusterisation stage
  // done at this point, before final writing in transient data store
  //std::stable_sort( digitCont->begin(), digitCont->end(), LHCb::FTSortingFunctor::LessByChannel<MCFTDigit*>());
  std::stable_sort( digitCont->begin(), digitCont->end(), LHCb::MCFTDigit::lowerByChannelID );
  
  // TEST : print Digit content after sorting
  if ( msgLevel( MSG::DEBUG) ) {
    for (MCFTDigits::const_iterator iterDigit = digitCont->begin(); iterDigit!=digitCont->end();++iterDigit){
      MCFTDigit* mcDigit = *iterDigit;
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
  // - add noise 
  double energySum = 0;
  std::vector<std::pair <LHCb::MCHit*,double> >::const_iterator vecIter= ftdeposit->mcHitVec().begin();
  for(;vecIter != ftdeposit->mcHitVec().end(); ++vecIter){
    energySum += vecIter->second;
  }

  // Convert energy sum in adc count
  // Compute the expected number of photoelectron, draw a poisson with that mean, and convert to ADC counts

  double averagePhotoElectrons = energySum * m_photoElectronsPerMeV;
  Rndm::Numbers poisson;
  poisson.initialize(randSvc(), Rndm::Poisson( averagePhotoElectrons ) );
  int photoElectrons = poisson();
  int adcCount = int( photoElectrons * m_sipmGain );
  
  if( msgLevel( MSG::DEBUG) ){
    debug() <<format("deposit2ADC() : energySum=%8.3f averagePE=%8.2f realPE %4i Gain=%8.3f adcCount = %4i",
                     energySum, averagePhotoElectrons, photoElectrons, m_sipmGain, adcCount)
            << endmsg;
  }
  return adcCount;
}
