//-----------------------------------------------------------------------------
// Implementation file for class : MCFTDigitCreator
//
// 2012-04-04 : COGNERAS Eric
//-----------------------------------------------------------------------------
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from FTEvent
#include "Event/MCFTDeposit.h"
#include "Event/MCFTDigit.h"

// local
#include "MCFTDigitCreator.h"

using namespace LHCb;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCFTDigitCreator );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCFTDigitCreator::MCFTDigitCreator( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty("InputLocation" ,       m_inputLocation        = LHCb::MCFTDepositLocation::Default );
  declareProperty("OutputLocation" ,      m_outputLocation       = LHCb::MCFTDigitLocation::Default   );
  declareProperty("PhotoElectronsPerMeV", m_photoElectronsPerMeV = 120. );  // 0.21 MeV per MIP, 25 photoelectrons per MIP
  declareProperty("SiPMGain",             m_sipmGain             = 2.0  );
  declareProperty("SiPMGainVariation",    m_sipmGainVariation    = 0.05 );  // relative fluctuation of the gain
  declareProperty("ADCNoise",             m_adcNoise             = 0.5  );
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
  m_gauss.initialize( randSvc(), Rndm::Gauss( 0., 1. ) );
  m_flat.initialize( randSvc(), Rndm::Flat( 0., 1. ) );
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

    plot((double)mcDeposit->channelID(), "FiredChannelID","Fired Channel; ChannelID" , 0. , 800000.);
    plot((double)mcDeposit->mcHitVec().size(), "HitPerChannel",
         "Number of Hits per Channel;Number of Hits per Channel; Number of channels" , 0. , 10., 10);

    // Fill map linking the deposited energy to the MCHit which deposited it.
    std::map<const LHCb::MCHit*, double> mcHitMap;
    std::vector<std::pair <LHCb::MCHit*,double> >::const_iterator vecIter=mcDeposit->mcHitVec().begin();
    double HitEnergySumInChannel = 0;
    
    for(;vecIter != mcDeposit->mcHitVec().end(); ++vecIter){
      if ( msgLevel( MSG::DEBUG) ) {
        debug() << format( " aHit->midPoint().x()=%10.3f E=%10.3f from MCParticle %5d",
                           vecIter->first->midPoint().x(), vecIter->second, 
                           vecIter->first->mcParticle()->key() ) << endmsg;
      }
      plot(vecIter->second,
           "EnergyPerHitPerChannel",
           "Energy deposited by each Hit in SiPM Channel;Energy [MeV];Number of SiPM channels", 
           0, 10);
      plot(vecIter->second,
           "EnergyPerHitPerChannelZOOM",
           "Energy deposited by each Hit in SiPM Channel;Energy [MeV];Number of SiPM channels", 
           0, 1);
      plot(vecIter->second,
           "EnergyPerHitPerChannelBIGZOOM",
           "Energy deposited by each Hit in SiPM Channel;Energy [MeV];Number of SiPM channels", 0, 0.5);

      mcHitMap[vecIter->first] += vecIter->second;
      HitEnergySumInChannel +=vecIter->second;
    }

    plot(HitEnergySumInChannel,
         "EnergyPerChannel",
         "Energy deposited in SiPM Channel;Energy [MeV];Number of SiPM channels", 
         0, 10);
    plot(HitEnergySumInChannel,
         "EnergyPerChannelZOOM",
         "Energy deposited in SiPM Channel;Energy [MeV];Number of SiPM channels", 
         0, 1);
    plot(HitEnergySumInChannel,
         "EnergyPerChannelBIGZOOM",
         "Energy deposited in SiPM Channel;Energy [MeV];Number of SiPM channels", 0, 0.5);
    // Define & store digit
    // The deposited energy to ADC conversion is made by the deposit2ADC method
    int adc = deposit2ADC(mcDeposit);
    if ( 0 < adc ) {
      plot2D(HitEnergySumInChannel,(double)adc,"ADCGain","ADC Gain; Energy [MeV]; ADC", 0, .6 ,0,100.,100,100);
      plot2D(HitEnergySumInChannel,(double)adc,"ADCGainZOOM","ADC Gain; Energy [MeV]; ADC", 0, .2 ,0,80.,100,100);
      plot2D(HitEnergySumInChannel,(double)adc,"ADCGainBIGZOOM","ADC Gain; Energy [MeV]; ADC", 0, .1 ,0,50.,100,100);
      counter("ADCPerMeV") += (double)adc/HitEnergySumInChannel ;
      MCFTDigit *mcDigit = new MCFTDigit(mcDeposit->channelID(), adc, mcHitMap );
      digitCont->insert(mcDigit);
      plot(adc,"ADCPerChannel",
           "ADC in SiPM Channel;ADC;Number of SiPM channels", 0, 20);
    }
  }

  // Digits are sorted according to there ChannelID to prepare the clusterisation stage
  // done at this point, before final writing in transient data store
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
  double energySum = 0;
  std::vector<std::pair <LHCb::MCHit*,double> >::const_iterator vecIter= ftdeposit->mcHitVec().begin();
  for(;vecIter != ftdeposit->mcHitVec().end(); ++vecIter){
    energySum += vecIter->second;
  }

  // Convert energy sum in adc count
  // Compute the expected number of photoelectron, draw a poisson with that mean, and convert to ADC counts
  //== For large number of photo-electrons, take a gaussian.
  //== Else compute manually the value, by computing when the sum of terms if greater than a flat random.

  double averagePhotoElectrons = energySum * m_photoElectronsPerMeV;
  int photoElectrons = 0;
  if ( averagePhotoElectrons > 50. ) {
    photoElectrons = averagePhotoElectrons + sqrt( averagePhotoElectrons ) * m_gauss();
  } else {
    double expL = exp( -averagePhotoElectrons );
    double cumul = expL;
    double last  = expL;
    double value = m_flat();
    while ( value > cumul ) {
      photoElectrons++;
      last = last * averagePhotoElectrons / photoElectrons;
      cumul += last;
    }
  }
  //== Digitise the signal + noise, add 0.5 for rounding
  double gain  = m_sipmGain * ( 1 + m_gauss() * m_sipmGainVariation );
  double noise = m_adcNoise * m_gauss();
  
  int adcCount = int( photoElectrons * gain + noise + 0.5 );  // Rounding, not truncation...
  
  if( msgLevel( MSG::DEBUG) ){
    debug() <<format("deposit2ADC() : energySum=%8.3f averagePE=%8.2f realPE %4i Gain=%8.3f noise=%4.2f adcCount = %4i",
                     energySum, averagePhotoElectrons, photoElectrons, gain, noise, adcCount)
            << endmsg;
  }
  return adcCount;
}
