//-----------------------------------------------------------------------------
// Implementation file for class : MCFTDigitCreator
//
// 2012-04-04 : COGNERAS Eric
//-----------------------------------------------------------------------------
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

// FTDet
#include "FTDet/DeFTDetector.h"
#include "FTDet/DeFTLayer.h"
#include "FTDet/DeFTFibreMat.h"

// from FTEvent
#include "Event/MCFTDeposit.h"
#include "Event/MCFTDigit.h"

// local
#include "MCFTDigitCreator.h"

// boost
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>


using namespace LHCb;
using namespace Gaudi;
using namespace boost;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCFTDigitCreator )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCFTDigitCreator::MCFTDigitCreator( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_SiPMResponse(0)
{
  std::vector<double> tmp = boost::assign::list_of(26*Gaudi::Units::ns)
                                                  (28*Gaudi::Units::ns)
                                                  (30*Gaudi::Units::ns);// tof (21+2+2) + full fiber propagation time (15) - integration rise (15)

  declareProperty("InputLocation" ,       m_inputLocation        = LHCb::MCFTDepositLocation::Default );
  declareProperty("OutputLocation" ,      m_outputLocation       = LHCb::MCFTDigitLocation::Default   );
  declareProperty("PhotoElectronsPerMeV", m_photoElectronsPerMeV = 120. );  // 0.21 MeV per MIP, 25 photoelectrons per MIP
  declareProperty("SiPMGain",             m_sipmGain             = 2.0  );
  declareProperty("SiPMGainVariation",    m_sipmGainVariation    = 0.05 );  // relative fluctuation of the gain
  declareProperty("ADCNoise",             m_adcNoise             = 0.5  );
  declareProperty("IntegrationOffset",    m_integrationOffset    = tmp); // tof + full fiber propagation time
  //declareProperty("ChannelNoiseEnabled",  m_channelNoiseEnabled  = false );
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

  // tools
  if ( msgLevel( MSG::DEBUG) ) debug() << ": initializing SiPMResponse" << endmsg;
  m_SiPMResponse = tool<SiPMResponse>("SiPMResponse","SiPMResponse", this);
  // plot response
  for (int i = -25; i < 75; ++i) plot(i, "SiPMResponse", "SiPMResponse; Pulse Height; Time (ns)" ,
				       -25. , 75., 100,m_SiPMResponse->response(i));

  // get the z position of the stations
  m_deFT = getDet<DeFTDetector>( DeFTDetectorLocation::Default );
  if(m_deFT->version() == 20 ){
    m_stationsZ.push_back(m_deFT->fibremats()[0]->layerCenterZ());
    m_stationsZ.push_back(m_deFT->fibremats()[100]->layerCenterZ());
    m_stationsZ.push_back(m_deFT->fibremats()[200]->layerCenterZ());
    if ( msgLevel( MSG::DEBUG) ) {
      debug() << "[initialize] RUN NEW GEOMETRY" << endmsg;
      debug() << "fibermats: "<< m_deFT->fibremats().size() 
	      << " fibremats()[0] z=" << m_deFT->fibremats()[0]->layerCenterZ()
	      << " fibremats()[100] z=" << m_deFT->fibremats()[100]->layerCenterZ()
	      << " fibremats()[200] z=" << m_deFT->fibremats()[200]->layerCenterZ();
    }
  }
  else{
    m_stationsZ.push_back(7859.95);
    m_stationsZ.push_back(8541.95);
    m_stationsZ.push_back(9226.95);
    if ( msgLevel( MSG::DEBUG) ) {
      debug() << "[initialize] RUN OLD GEOMETRY" << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCFTDigitCreator::execute() {

  if ( msgLevel( MSG::DEBUG) )  
    debug() << "[MCFTDigitCreator] ==> Execute NEW EVENT" << endmsg;

  // If channel noise is considered, call Noise Tool
  //std::list<FTChannelID> NoisyChannelList;
  //if(m_channelNoiseEnabled) 
  //{
  //
  //}
  

  // retrieve FTDeposits
  const MCFTDeposits* mcDepositsCont = get<MCFTDeposits>(m_inputLocation);
  if ( msgLevel( MSG::DEBUG) )
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

    plot((double)mcDeposit->channelID(), "FiredChannelID","Fired Channel; ChannelID" , 0. , 589824. , 4608);
    plot((double)mcDeposit->mcHitVec().size(), "HitPerChannel",
         "Number of Hits per Channel;Number of Hits per Channel; Number of channels" , 0. , 10., 10);

    // Plot a few informations
    double HitEnergySumInChannel = 0;
    double HitEnergyRefSumInChannel = 0;
    for (unsigned int idx = 0; idx < mcDeposit->mcHitVec().size(); idx++) {
      if ( msgLevel( MSG::DEBUG) ) {
        debug() << format( " aHit->midPoint().x()=%10.3f Ed=%10.3f Er=%10.3f Td=%10.3f Tr=%10.3f from MCParticle %5d",
                           mcDeposit->mcHitVec()[idx]->midPoint().x(), 
			   mcDeposit->energyVec()[idx], mcDeposit->energyRefVec()[idx], 
			   mcDeposit->timeVec()[idx], mcDeposit->timeRefVec()[idx],
                           mcDeposit->mcHitVec()[idx]->mcParticle()->key() ) << endmsg;
      }
      // plot direct energy
      plot(mcDeposit->energyVec()[idx],
           "EnergyDirPerHitPerChannel",
           "Energy deposited by each Hit in SiPM Channel (direct);Energy [MeV];Number of SiPM channels", 
           0, 10);
      plot(mcDeposit->energyVec()[idx],
           "EnergyDirPerHitPerChannelZOOM",
           "Energy deposited by each Hit in SiPM Channel (direct);Energy [MeV];Number of SiPM channels", 
           0, 1);
      plot(mcDeposit->energyVec()[idx],
           "EnergyDirPerHitPerChannelBIGZOOM",
           "Energy deposited by each Hit in SiPM Channel (direct);Energy [MeV];Number of SiPM channels", 0, 0.5);
      // plot energy from reflected light
      plot(mcDeposit->energyRefVec()[idx],
           "EnergyRefPerHitPerChannel",
           "Energy deposited by each Hit in SiPM Channel (direct);Energy [MeV];Number of SiPM channels", 
           0, 10);
      plot(mcDeposit->energyRefVec()[idx],
           "EnergyRefPerHitPerChannelZOOM",
           "Energy deposited by each Hit in SiPM Channel (direct);Energy [MeV];Number of SiPM channels", 
           0, 1);
      plot(mcDeposit->energyRefVec()[idx],
           "EnergyRefPerHitPerChannelBIGZOOM",
           "Energy deposited by each Hit in SiPM Channel (direct);Energy [MeV];Number of SiPM channels", 0, 0.5);
      
      HitEnergySumInChannel +=mcDeposit->energyVec()[idx];
      HitEnergySumInChannel +=mcDeposit->energyRefVec()[idx];
      HitEnergyRefSumInChannel +=mcDeposit->energyRefVec()[idx];
    } // loop on hits in mcDeposit

    // Plot total energy (from direct pulse) per channel
    plot(HitEnergySumInChannel,
         "EnergyPerChannel",
         "Energy deposited in SiPM Channel;Energy [MeV];Number of SiPM channels", 
         0, 1);
    plot(HitEnergySumInChannel,
         "EnergyPerChannelZOOM",
         "Energy deposited in SiPM Channel;Energy [MeV];Number of SiPM channels", 
         0, 0.1);
    plot(HitEnergySumInChannel,
         "EnergyPerChannelBIGZOOM",
         "Energy deposited in SiPM Channel;Energy [MeV];Number of SiPM channels", 0, 0.01);

    // Plot total energy (from reflected pulse) per channel
    plot(HitEnergyRefSumInChannel,
         "EnergyRefPerChannel",
         "Energy deposited in SiPM Channel (reflected);Energy [MeV];Number of SiPM channels", 
         0, 1);
    plot(HitEnergyRefSumInChannel,
         "EnergyRefPerChannelZOOM",
         "Energy deposited in SiPM Channel (reflected);Energy [MeV];Number of SiPM channels", 
         0, 0.1);
    plot(HitEnergyRefSumInChannel,
         "EnergyRefPerChannelBIGZOOM",
         "Energy deposited in SiPM Channel (reflected);Energy [MeV];Number of SiPM channels", 0, 0.01);


    // Define & store digit
    // The deposited energy to ADC conversion is made by the deposit2ADC method
    int adc = deposit2ADC(mcDeposit);
    if ( 0 < adc ) {
      plot2D(HitEnergySumInChannel,(double)adc,"ADCGain","ADC Gain; Energy [MeV]; ADC", 0, .6 ,0,100.,100,100);
      plot2D(HitEnergySumInChannel,(double)adc,"ADCGainZOOM","ADC Gain; Energy [MeV]; ADC", 0, .2 ,0,80.,100,100);
      plot2D(HitEnergySumInChannel,(double)adc,"ADCGainBIGZOOM","ADC Gain; Energy [MeV]; ADC", 0, .1 ,0,50.,100,100);
      counter("ADCPerMeV") += (double)adc/HitEnergySumInChannel ;
      MCFTDigit *mcDigit = new MCFTDigit(mcDeposit->channelID(), adc, mcDeposit );
      digitCont->insert(mcDigit);
      plot(adc,"ADCPerChannel",
           "ADC in SiPM Channel;ADC;Number of SiPM channels", 0, 20);
    }
  } // loop on mcDeposits

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

int
MCFTDigitCreator::averagePhotoElectrons(double energy)
{
  // Compute the expected number of photoelectron, draw a poisson with that mean, and convert to ADC counts
  //== For large number of photo-electrons, take a gaussian.
  //== Else compute manually the value, by computing when the sum of terms if greater than a flat random.
  double averagePhotoElectrons = energy * m_photoElectronsPerMeV;
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

  if ( msgLevel( MSG::DEBUG) )
    debug() <<format("averagePhotoElectrons() : energy=%8.3f averagePE=%8.2f realPE %4i", 
		     energy, averagePhotoElectrons, photoElectrons)
	    << endmsg;

  return photoElectrons;
}

std::pair<double,double>
MCFTDigitCreator::integrateResponse(const LHCb::MCFTDeposit* ftdeposit)
{
  std::pair<double,double> sum= std::make_pair(0.,0.);
  double toff(0.);
  for (unsigned int i = 0; i < m_stationsZ.size(); ++i)
    if ( fabs(ftdeposit->mcHitVec()[0]->entry().z() - m_stationsZ[i]) < 200*Gaudi::Units::mm ) toff = m_integrationOffset[i];

  // loop on the hits
  int avePE(0);
  double SiPMresponse(0.);
  for (unsigned int idx = 0; idx < ftdeposit->mcHitVec().size(); idx++) {
    // direct pulse
    double t= ftdeposit->timeVec()[idx]-toff;
    avePE = averagePhotoElectrons(ftdeposit->energyVec()[idx]);
    SiPMresponse = m_SiPMResponse->response(t);
    sum.first += avePE * SiPMresponse;
    // reflected pulse
    t= ftdeposit->timeRefVec()[idx]-toff;
    avePE = averagePhotoElectrons(ftdeposit->energyRefVec()[idx]);
    SiPMresponse = m_SiPMResponse->response(t);
    sum.second += avePE * SiPMresponse;
  }

  if ( msgLevel( MSG::DEBUG) )
    debug() <<format("integrateResponse() : toff=%8.3f responseDir=%8.2f responseRef=%8.2f", 
		     toff, sum.first, sum.second)
	    << endmsg;

  return sum;
}

//=============================================================================
int MCFTDigitCreator::deposit2ADC(const LHCb::MCFTDeposit* ftdeposit)
{
  // Convert energy sum in adc count
  std::pair<double,double> response = integrateResponse(ftdeposit);
  plot(response.first, "SiPMresponseDirect",
       "Response from SiPM Channel (direct);Charge;Number of SiPM channels", 0, 100.);
  plot(response.second, "SiPMresponseReflected",
       "Response from SiPM Channel (reflected);Charge;Number of SiPM channels", 0, 100.);
  plot(response.first, "SiPMresponseDirectZOOM",
       "Response from SiPM Channel (direct);Charge;Number of SiPM channels", 0, 10.);
  plot(response.second, "SiPMresponseReflectedZOOM",
       "Response from SiPM Channel (reflected);Charge;Number of SiPM channels", 0, 10.);

  //== Digitise the signal + noise, add 0.5 for rounding
  double gain  = m_sipmGain * ( 1 + m_gauss() * m_sipmGainVariation );
  double noise = m_adcNoise * m_gauss();
  int adcCountDir = int( response.first * gain + noise + 0.5 );  // Rounding, not truncation...
  gain  = m_sipmGain * ( 1 + m_gauss() * m_sipmGainVariation );
  noise = m_adcNoise * m_gauss();
  int adcCountRef = int( response.second * gain + noise + 0.5 );  // Rounding, not truncation...
  plot(adcCountDir, "ADCcountsDirect",
       "ADC counts (direct);ADC;Number of SiPM channels", 0, 200.);
  plot(adcCountRef, "ADCcountsReflected",
       "ADC counts (reflected);ADC;Number of SiPM channels", 0, 200.);
  plot(adcCountDir, "ADCcountsDirectZOOM",
       "ADC counts (direct);ADC;Number of SiPM channels", 0, 20.);
  plot(adcCountRef, "ADCcountsReflectedZOOM",
       "ADC counts (reflected);ADC;Number of SiPM channels", 0, 20.);
  plot(adcCountDir+adcCountRef, "ADCcounts",
       "ADC counts (direct);ADC;Number of SiPM channels", 0, 200.);
  plot(adcCountDir+adcCountRef, "ADCcountsZOOM",
       "ADC counts (reflected);ADC;Number of SiPM channels", 0., 20., 20);
 
  if( msgLevel( MSG::DEBUG) ){
    debug() <<format("deposit2ADC() : responseDir=%8.3f responseRef=%8.3f Gain=%8.3f noise=%4.2f adcCountDir = %4i adcCountRef = %4i",
                     response.first, response.second, gain, noise, adcCountDir, adcCountRef)
            << endmsg;
  }
  return adcCountDir+adcCountRef;
}
