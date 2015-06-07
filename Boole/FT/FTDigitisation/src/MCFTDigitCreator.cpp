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
                                                  (30*Gaudi::Units::ns);//  tof (21+2+2)
                                                                        // + full fiber propagation time (15) 
                                                                        // - integration rise (15)

  declareProperty("InputLocation" ,       m_inputLocation        = LHCb::MCFTDepositLocation::Default );
  declareProperty("OutputLocation" ,      m_outputLocation       = LHCb::MCFTDigitLocation::Default   );
  declareProperty("PhotoElectronsPerMeV", m_photoElectronsPerMeV = 120. );  // 0.21 MeV per MIP, 25 photoelectrons per MIP
  declareProperty("SiPMGain",             m_sipmGain             = 2.0  );
  declareProperty("SiPMGainVariation",    m_sipmGainVariation    = 0.05 );  // relative fluctuation of the gain
  declareProperty("ADCNoise",             m_adcNoise             = 0.5  );
  declareProperty("IntegrationOffset",    m_integrationOffset    = tmp); // tof + full fiber propagation time
  declareProperty("Force2bitADC"     ,    m_force2bitADC         = 1); // force the use of 3-valued charges at the end

  // noise-related
  declareProperty("SimulateNoise",    m_doNoise          =   1,   "Simulate noise (thermal, crosstalk, afterpulses)");
  declareProperty("SipmPedestal",     m_pedestal         =   0.0, "Sipm pedestal (only implemented for noise)");
  declareProperty("ClusterMinCharge", m_clusterMinCharge =   9,   "Minimum cluster charge (for noise calculation)");
  declareProperty("Temperature",      m_temperature      = -30.,  "Sipm temperature (deg.C)    (for noise)");
  declareProperty("Irradiation",      m_irradiation      =  50.,  "Sipm received irradiation (fb-1) (for noise)");
  declareProperty("Nu",               m_nu               =  7.6,  "Nu of collision run. Nafterpulses ~nu (for noise)");
  declareProperty("ThermalNoiseRateBase",  m_thermalNoiseRateBase =  0.1, 
                  "Sipm channel thermal (dark) noise rate at 20 deg.C, 0fb-1 (MHz)" );
  declareProperty("CrossTalkProbability",  m_crossTalkProbability =  0.07, "Sipm cross-talk probability per pe");
  declareProperty("AfterpulseProbability", m_afterpulseProbability = 0.15, 
                  "Sipm random afterpulse probability of 1 pe to occur per hit PIXEL");
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
  if ( m_doNoise == 1 ) {
    info() << "Will simulate noise: T=" << m_temperature << ", irr=" << m_irradiation << ", nu=" << m_nu << endmsg; 
  }
  sc = m_gauss.initialize( randSvc(), Rndm::Gauss( 0., 1. ) );
  if ( sc.isFailure() ) return Error( "Failed to get Rndm::Gauss generator", sc );
  sc = m_flat.initialize( randSvc(), Rndm::Flat( 0., 1. ) );
  if ( sc.isFailure() ) return Error( "Failed to get Rndm::Flat generator", sc );
  sc = m_rndmLandau.initialize( randSvc(), Rndm::Landau(39.0, 7.8) ); // for noise, from a fit to the
                                                                      // MCHit cluster charge (6bit ADC)
  if ( sc.isFailure() ) return Error( "Failed to get Rndm::Landau generator", sc );

  // tools
  if ( msgLevel( MSG::DEBUG) ) debug() << ": initializing SiPMResponse" << endmsg;
  //m_SiPMResponse = tool<SiPMResponse>("SiPMResponse","SiPMResponse", this);
  m_SiPMResponse = tool<SiPMResponse>("SiPMResponse","SiPMResponse");
  
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
              << " fibremats()[200] z=" << m_deFT->fibremats()[200]->layerCenterZ()
              << endmsg;
    }
  }
  else{
    info() << "Unknown FTDet version" << endmsg; 
    return  StatusCode::FAILURE;
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

    plot((double)mcDeposit->channelID().sipmCell(), "FiredSiPMCell","Fired SiPM Cell; Fired SiPM Cell" , 0. , 130. , 130);
    plot((double)mcDeposit->channelID().sipmId(), "FiredSiPMID","Fired SiPM ID; Fired SiPM ID" , 0. , 96. , 96);
    plot((double)mcDeposit->channelID().mat(), "FiredMat","Fired Mat; Fired Mat" , 0. , 5. , 5);
    plot((double)mcDeposit->channelID().module(), "FiredModule","Fired Module; Fired Module" , 0. , 20. , 20);
    plot((double)mcDeposit->channelID().quarter(), "FiredQuarter","Fired Quarter; Fired Quarter" , 0. , 4. , 4);
    plot((double)mcDeposit->channelID().layer(), "FiredLayer","Fired Layer; Fired Layer" , 0. , 13. , 13);
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
           "Energy deposited by each Hit in SiPM Channel (reflected);Energy [MeV];Number of SiPM channels", 
           0, 10);
      plot(mcDeposit->energyRefVec()[idx],
           "EnergyRefPerHitPerChannelZOOM",
           "Energy deposited by each Hit in SiPM Channel (reflected);Energy [MeV];Number of SiPM channels", 
           0, 1);
      plot(mcDeposit->energyRefVec()[idx],
           "EnergyRefPerHitPerChannelBIGZOOM",
           "Energy deposited by each Hit in SiPM Channel (reflected);Energy [MeV];Number of SiPM channels", 0, 0.5);
      
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
      plot2D(HitEnergySumInChannel,(double)adc,"ADCGain","ADC Gain; Energy [MeV]; ADC", 0., .6 ,0., 100., 100, 100);
      plot2D(HitEnergySumInChannel,(double)adc,"ADCGainZOOM","ADC Gain; Energy [MeV]; ADC", 0., .2, 0., 80., 100, 80);
      plot2D(HitEnergySumInChannel,(double)adc,"ADCGainBIGZOOM","ADC Gain; Energy [MeV]; ADC", 0., .1, 0., 50., 100, 50);
      plot(adc,"ADCPerChannel",
           "ADC in SiPM Channel;ADC;Number of SiPM channels", 0, 20);
      if (HitEnergySumInChannel > 0) 
        counter("ADCPerMeV") += (double)adc/HitEnergySumInChannel ;


      MCFTDigit *mcDigit = new MCFTDigit(mcDeposit->channelID(), adc, mcDeposit );
      digitCont->insert(mcDigit);

    }
  } // loop on mcDeposits





  // Add noise to digits
  if( m_doNoise ) {
    debug() << " -- STARTING NOISE DIGIT CALCULATIONS" << endmsg;
    
    // Get channels containing an MCHit
    std::vector<MCFTDigit*> mcHitDigits;
    std::vector<FTChannelID> mcHitChannels;
    for (MCFTDigits::const_iterator iterDigit = digitCont->begin(); iterDigit!=digitCont->end();++iterDigit){
      MCFTDigit* mcDigit = *iterDigit;
      mcHitDigits.push_back( mcDigit );
      mcHitChannels.push_back( mcDigit -> channelID() );
    }
    debug() << "size of mcHitChannels: " << mcHitChannels.size() << endmsg;

    // Set geometry
    int Nlayers = 4*3;  // layers * stations
    //int Nquarters = 4;
    int NModule = 12;
    int NMat = 2;
    //int Nsipms = 4*4*6; // sipms * mats * modules
    int Nsipms = 16;
    int Nchannels = 128;
    int Ntotchannels = Nlayers * NModule * NMat * Nsipms * Nchannels;

    // create containers for new noise hits
    std::vector<FTChannelID> noiseChannels;
    std::vector<int> noiseADCs;


    //================================================================ 
    // Primary noise (/dark noise/thermal noise/dark current)
    //


    // Calculate number of thermal noise hits expected
    float readoutFreq = 40.; // MHz
    float irradCoef = 50.;
    float tempCoef = 10.;
    float roomTemp = 20.; // deg.C
    int NthermalNoise = Ntotchannels * (m_thermalNoiseRateBase / readoutFreq) * 
      (irradCoef * m_irradiation) * pow(2,(m_temperature-roomTemp)/tempCoef);

    // Calculate number of VISIBLE thermal noise hits, due to cross-talk:
    //int minPEforCluster = int( float(m_clusterMinCharge) / m_sipmGain );
    int NvisibleThermalNoise = int( NthermalNoise * pow(m_crossTalkProbability, 0) ); // ct^0 = generate all pulses
    debug() << "[THERMAL] NthermalNoise = " << NvisibleThermalNoise << endmsg;
    plot(NvisibleThermalNoise, "NvisibleThermalNoiseHits", "NvisibleThermalNoiseHits; NvisibleThermalNoiseHits", 
         0. , 1000000. ,10000);

    // Loop over all noise hits
    int totalThermalInMCHit  = 0;
    int totalThermalCreated  = 0;
    int totalThermalAppended = 0;
    for(int i=0; i<NvisibleThermalNoise; i++) {

      // Get random channel
      double r = m_flat()*(Nlayers);
      int layer = int(r);
      //r = m_flat()*(Nquarters);
      //int quarter = int(r);
      r = m_flat()*(NModule);
      int module = int(r);
      r = m_flat()*(NMat);
      int mat = int(r);
      r = m_flat()*(Nsipms);
      int sipmID = int(r);
      r = m_flat()*(Nchannels);
      int sipmCell = int(r);
      //FTChannelID noiseChannel(layer, quarter, sipmID, sipmCell);
      FTChannelID noiseChannel(layer, module, mat, sipmID, sipmCell);

      // Create random ADC value due to cross-talk
      int Npe = 1; // should be >1 if NvisibleThermalNoise has a ct^(>0);
      while(m_flat() < m_crossTalkProbability) { Npe++; } // allows for cross-talk of cross-talk
      double gain  = m_sipmGain * (1. + m_gauss() * m_sipmGainVariation );
      double noise = m_adcNoise * m_gauss();
      int noiseADCcount = int( m_pedestal + Npe * gain + noise + 0.5 );

      // Check if already in MCHit list
      std::vector<FTChannelID>::const_iterator chanIt = std::find(mcHitChannels.begin(), mcHitChannels.end(), noiseChannel);
      if (chanIt != mcHitChannels.end()) {
        // if it IS, add ADC value to existing MCHit channel
        MCFTDigit* MCHitdigit = 
          mcHitDigits[ std::distance<std::vector<FTChannelID>::const_iterator>(mcHitChannels.begin(),chanIt) ];
        MCHitdigit -> setAdcCount( (MCHitdigit -> adcCount()) + noiseADCcount );
        totalThermalInMCHit++;
      } else {
        // Check if not already in noise hit list
        chanIt = std::find(noiseChannels.begin(), noiseChannels.end(), noiseChannel);
        if( chanIt != noiseChannels.end() ) {
          // if it IS, Add ADC value to existing noise channel
          noiseADCs[ std::distance<std::vector<FTChannelID>::const_iterator>(noiseChannels.begin(),chanIt) ] += noiseADCcount;
          totalThermalAppended++;
        } else {
          // Add new noise hit
          noiseChannels.push_back( noiseChannel );
          noiseADCs.push_back( noiseADCcount );
          totalThermalCreated++;
        }
      }
    }

    debug() << "[THERMAL] Created: " << totalThermalCreated << ", Appended: " << totalThermalAppended << ", Added to MCHit: " 
            << totalThermalInMCHit << endmsg;


    //========================================================================
    // Hit afterpulsing
    //


    // Initialize some numbers for the occupancy distribution
    double N   = 102.;    // these numbers are the result of a fit
    double c   = 393.;    //  to an occupancy profile
    double e1  = 0.00168;
    double e2  = -0.00032;
    double off = 26.;

    double norm = N * exp( e1 * c ) / exp( e2 * c );
    double intBeg = 0.;
    double intEnd = Nchannels*Nsipms;
    double integralBeforCut = off*(c-intBeg) + (N   /e1) * ( exp(e1*c     ) - exp(e1*intBeg) );
    double integralAfterCut = off*(intEnd-c) + (norm/e2) * ( exp(e2*intEnd) - exp(e2*c     ) );
    //debug() << "[HIT AFTERPULSING] integralBeforCut = " << integralBeforCut << endmsg;
    //debug() << "[HIT AFTERPULSING] integralAfterCut = " << integralAfterCut << endmsg;

    // Determine number of hit afterpulsing hits expected
    int NavgClusInEvent = int(396788./100. / 7.6 * m_nu); // Based on 100 events, nu=7.6, Bs2phiphi, 
                                                          //pythia8. Assumes Nclusters goes linearly with nu
    int NhitAfterpulses = NavgClusInEvent;

    // Calculate number of additional hit afterpulse hits due to cross-talk
    float avgNoisePE = 1. + m_crossTalkProbability + pow(m_crossTalkProbability,2) + pow(m_crossTalkProbability,3) 
      + pow(m_crossTalkProbability,4);
    int NhitAfterpulses_AP = 0;
    float APLoop = float(NhitAfterpulses);
    while (APLoop > 1.) {
      APLoop *= m_afterpulseProbability; // effectively also takes into account afterpulses of cross-talk of 
                                         //(cross-talk of) MCHit afterpulses
      NhitAfterpulses_AP += int(APLoop*avgNoisePE+0.5);
    }

    // Determine number of VISIBLE hit afterpulses due to cross-talk
    int NvisibleHitAfterpulses = int( (NhitAfterpulses+NhitAfterpulses_AP) * pow(m_crossTalkProbability, 0) ); // ct^0 = do all
    debug() << "[HIT AFTERPULSING] N channel hits for afterpulses = " << NvisibleHitAfterpulses << " (of which "
      << float(NhitAfterpulses_AP) / float(NhitAfterpulses + NhitAfterpulses_AP) * 100. << " % afterpulses)" << endmsg;
    //plot(NvisibleHitAfterpulses, "NvisibleHitAfterpulses", "NvisibleHitAfterpulses; NvisibleHitAfterpulses", 
    //     0. , 1000000. ,10000);

    // determine how many hit afterpulses will have a charge of 1 p.e., 2 p.e., etc. (without cross-talk)
    float NsumBinFractions[20] = { 0.204691106081,  0.176326997846,  0.132542585516,  0.0997058439121,  0.0760373252544,
                                   0.0585607741894,  0.0453520643227,  0.0352234776303,  0.0273958443495,  0.0213224364803,  
                                   0.0166009440283,  0.0129269868938,0.010066868574,  0.00783983501193,  0.00610557755609,  
                                   0.00475499468632,  0.0037031811119,  0.00288403576027,  0.00224608761991,  0.001749254176 };
    float NsumBinFracCum[20] = {0};
    for (int j=0; j<20; j++){
      float sum=0;
      for (int k=j; k>=0; k--) {
        sum += NsumBinFractions[k];
      }
      NsumBinFracCum[j] = sum;
    }

    int Napcharges[20] = {0};
    for(int i=0; i<NvisibleHitAfterpulses; i++) {
      int Npe=100;
      while(Npe > 30) { Npe = (m_rndmLandau()/m_sipmGain) * m_afterpulseProbability; }
      Rndm::Numbers rndmPoisson(randSvc(), Rndm::Poisson( Npe ) );
      int Nap = rndmPoisson();

      // drop each pixel afterpulse in a time bin
      int Naptimes[21] = {0}; // 1 overflow time bin
      for(int ap=0; ap<Nap; ap++) {
        float r = m_flat();
        for (int j=0; j<20; j++) {
          if (r < NsumBinFracCum[j]) {
            Naptimes[j]++;
            plot(j,"hitAfterpulseTimeDist","hitAfterpulseTimeDist; hitAfterpulseTimeDist", 0., 21., 21);
            break;
          }
        }
      }

      // count bins with same number of entries
      for(int t=0; t<20; t++) {
        Napcharges[ Naptimes[t] ]++;
        plot(Naptimes[t], "hitAfterpulsePE","hitAfterpulsePE (no CT); hitAfterpulsePE", 0., 20., 20);
      }
    }

    // loop over all hit afterpulses
    int totalAPInMCHit  = 0;
    int totalAPCreated  = 0;
    int totalAPAppended = 0;
    for(int i=1; i<20; i++) { // skip the 0 adc count channels
      for( int j=0; j<Napcharges[i]; j++) {

        // Create random ADC value
        int Npebase = i;
        int Npe = Npebase;
        while(Npebase-- > 0) {
          while(m_flat() < m_crossTalkProbability) { Npe++; } // allow for crosstalk of crosstalk
        }
        double gain  = m_sipmGain * (1. + m_gauss() * m_sipmGainVariation );
        double noise = m_adcNoise * m_gauss();
        int noiseADCcount = int( m_pedestal + Npe * gain + noise + 0.5 );

        // create random channel according to occupancy
        // FUNCTION of IDCell occupancy distribution per quarter (fit):
        //if (x <= c) {     y = off + N * exp( e1 * x );   }
        //if (x > c) {    double norm = N * exp( e1 * c ) / exp( e2 * c );     y = off + norm * exp( e2 * x );   }

        int sipmID = -1;
        int sipmCell = -1;
        int IDCell;
        while(sipmID < 0 || sipmID > Nsipms || sipmCell < 0 || sipmCell > Nchannels) { // safety check for rawBankEncoder
          if (m_flat() * (integralBeforCut+integralAfterCut) < integralBeforCut) {
            // first expo
            double randRange = (off+N*exp(e1*intBeg)) + m_flat()*( (off+N*exp(e1*c)) - (off+N*exp(e1*intBeg)));
            double randExp = log( (randRange-off)/N ) / e1;
            IDCell = int( randExp + 0.5 );
          } else {
            // second expo
            double randRange = (off+norm*exp(e2*intEnd)) + m_flat()*( (off+norm*exp(e2*c)) - (off+norm*exp(e2*intEnd)));
            double randExp = log( (randRange-off)/norm ) / e2;
            IDCell = int( randExp + 0.5 );
          }
          int bitShiftsipmID = 7;
          sipmID = (IDCell >> bitShiftsipmID);
          sipmCell = IDCell - sipmID * pow(2,bitShiftsipmID);
        }

        // Now that we have the IDCell (= sipmID and sipmCell), get random quarter and layer:
        double r = m_flat()*(Nlayers);
        int layer = int(r);
        //r = m_flat()*(Nquarters);
        //int quarter = int(r);
      r = m_flat()*(NModule);
      int module = int(r);
      r = m_flat()*(NMat);
      int mat = int(r);
      //FTChannelID noiseChannel(layer, quarter, sipmID, sipmCell);
      FTChannelID noiseChannel(layer, module, mat, sipmID, sipmCell);
        // Check if not already in MCHit list
        std::vector<FTChannelID>::const_iterator chanIt = std::find(mcHitChannels.begin(), mcHitChannels.end(), noiseChannel);
        if (chanIt != mcHitChannels.end()) {
          // add ADC value to existing MCHit channel
          MCFTDigit* MCHitdigit = 
            mcHitDigits[ std::distance<std::vector<FTChannelID>::const_iterator>(mcHitChannels.begin(),chanIt) ];
          MCHitdigit -> setAdcCount( (MCHitdigit -> adcCount()) + noiseADCcount );
          totalAPInMCHit++;
        } else {
          // Check if not already in noise list
          chanIt = std::find(noiseChannels.begin(), noiseChannels.end(), noiseChannel);
          if( chanIt != noiseChannels.end() ) {
            // Add ADC value to existing noise channel
            noiseADCs[ std::distance<std::vector<FTChannelID>::const_iterator>(noiseChannels.begin(),chanIt) ] += noiseADCcount;
            totalAPAppended++;
          } else {
            // Add new noise channel
            noiseChannels.push_back( noiseChannel );
            noiseADCs.push_back( noiseADCcount );
            totalAPCreated++;
          }
        }

      }
    }

    debug() << "[HIT AFTERPULSING] Created: " << totalAPCreated << ", Appended: " << totalAPAppended 
            << ", Added to MCHit: " << totalAPInMCHit << endmsg;


    //==========================================================================  
    // Add noise digits to full digit set
    //


    int addOK = 0;
    int addFailed = 0;
    for (unsigned int i=0; i<noiseChannels.size(); i++) {
      plot(noiseADCs[i]    , "noiseADCcount", "Noise ADC count; Noise ADC count", 0. , 30. ,30);
      plot(noiseChannels[i], "noiseChannels", "Noise channelID; Noise channelID", 0., 589824., 589824);
      plot(noiseChannels[i], "noiseChannelsPerSipm", "Noise channelID; Noise channelID", 0., 589824., 4608);

      //const std::map< const LHCb::MCHit*, double > MCHitMapDummy;
      MCFTDeposit* dummyDeposit = 0;
      MCFTDigit* noiseMCDigit = new MCFTDigit( noiseChannels[i], noiseADCs[i], dummyDeposit );
      try { digitCont->add( noiseMCDigit ); }
      catch(GaudiException) { debug() << "Caught GaudiException: Noise hit " << addOK
         << " not inserted due to faulty random generated key" << endmsg;
         ++addFailed; --addOK;
      }
      ++addOK;
    }
    // sort the container. Is already done outside of noise loop!
    //std::stable_sort( digitCont->begin(), digitCont->end(), LHCb::MCFTDigit::lowerByChannelID );
    //debug() << "Succesfully added " << addOK << " noise hits to event (" << addFailed << " failed)" << endmsg;
    //debug() << "digitCont size after noise: " << digitCont->size() << endmsg;

    // Store (noise+MC) digits in TES
    //MCFTDigits* newContainer = digitCont;
    //put(newContainer, m_outputLocation);

  } // end of adding noise digits




  if(m_force2bitADC) {
    // Force 2-bits ADC charge values
    int adc, newAdc;
    for (MCFTDigits::const_iterator iterDigit = digitCont->begin(); iterDigit!=digitCont->end();++iterDigit){
      MCFTDigit* mcDigit = *iterDigit;   
      adc = mcDigit -> adcCount();

      newAdc = 0;
      if(adc >= 3) { newAdc = 3; }
      if(adc >= 5) { newAdc = 5; }
      if(adc >= 8) { newAdc = 8; }
      mcDigit -> setAdcCount( newAdc );
    
      plot( adc   , "force2bitADC_oldAdc", "force2bitADC oldADC" , 0. , 20. , 20);
      plot( newAdc, "force2bitADC_newAdc", "force2bitADC newADC" , 0. , 20. , 20);
      plot2D(adc, newAdc, "force2bitADC_NewvsOld","oldAdc; newAdc", 0., .20, 0., 20., 20, 20);
    }
  }


  // Digits are sorted according to their ChannelID to prepare the clusterisation stage
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
// function averagePhotoElectrons(double energy)
//=============================================================================
int MCFTDigitCreator::averagePhotoElectrons(double energy)
{
  if ( msgLevel( MSG::DEBUG) )
    debug() <<format("averagePhotoElectrons() : energy=%8.3f ",energy) << endmsg;
  // Compute the expected number of photoelectron, draw a poisson with that mean, and convert to ADC counts
  //== For large number of photo-electrons, take a gaussian.
  //== Else compute manually the value, by computing when the sum of terms if greater than a flat random.
  double averagePhotoElectrons = energy * m_photoElectronsPerMeV;
  int photoElectrons = 0;
  if ( averagePhotoElectrons > 50. ) {
    photoElectrons = averagePhotoElectrons + sqrt( averagePhotoElectrons ) * m_gauss();
  } else {
    double expL = 1/exp( averagePhotoElectrons );
    double cumul = expL;
    double last  = expL;
    double value = m_flat();
    while ( value > cumul ) {
      photoElectrons++;
      if (photoElectrons > 0) last = last * averagePhotoElectrons / photoElectrons;
      cumul += last;
    }
  }

  if ( msgLevel( MSG::DEBUG) )
    debug() <<format("averagePhotoElectrons() : averagePE=%8.2f realPE %4i",
                     averagePhotoElectrons, photoElectrons)
            << endmsg;

  return photoElectrons;
}

//=============================================================================
// function integrateResponse(const LHCb::MCFTDeposit* ftdeposit)
//=============================================================================
std::pair<double,double>
MCFTDigitCreator::integrateResponse(const LHCb::MCFTDeposit* ftdeposit)
{
  std::pair<double,double> sum= std::make_pair(0.,0.);
  double toff(0.);
  for (unsigned int i = 0; i < m_stationsZ.size(); ++i)
  {
    if(msgLevel( MSG::DEBUG))
    {
      debug()<<format("[integrateResponse()] : ftdeposit->entry().z()==%8.3f m_stationsZ[i]=%8.2f m_integrationOffset[i]=%8.2f", 
                      ftdeposit->mcHitVec()[0]->entry().z(), m_stationsZ[i], m_integrationOffset[i])
             << endmsg;
    }
    
    if ( fabs(ftdeposit->mcHitVec()[0]->entry().z() - m_stationsZ[i]) < 200*Gaudi::Units::mm ) toff = m_integrationOffset[i];

  }
  

  // loop on the hits
  int avePE(0);
  double SiPMresponse(0.);
  for (unsigned int idx = 0; idx < ftdeposit->mcHitVec().size(); idx++) {
    // direct pulse
    double t= ftdeposit->timeVec()[idx]-toff;

    SiPMresponse = m_SiPMResponse->response(t);
    if(msgLevel( MSG::DEBUG))
    {
      debug()<<format("[integrateResponse()] : idx==%4i t==%8.3f  SiPMresponse== %8.3f ftdeposit->energyVec()[idx]==%8.3f" ,
                      idx, t, SiPMresponse,ftdeposit->energyVec()[idx])<< endmsg;
    }
    avePE = averagePhotoElectrons(ftdeposit->energyVec()[idx]);

    sum.first += avePE * SiPMresponse;
    // reflected pulse
    t= ftdeposit->timeRefVec()[idx]-toff;
    SiPMresponse = m_SiPMResponse->response(t);
    if(msgLevel( MSG::DEBUG))
    {
      debug()<<format("[integrateResponse()] : t==%8.3f SiPMresponse== %8.3f ftdeposit->energyRefVec()[idx]==%8.3f" ,
                      t, SiPMresponse,ftdeposit->energyRefVec()[idx])<< endmsg;
    }
    avePE = averagePhotoElectrons(ftdeposit->energyRefVec()[idx]);
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
       "ADC counts (reflected);ADC;Number of SiPM channels", 0., 200.);
  plot(adcCountDir, "ADCcountsDirectZOOM",
       "ADC counts (direct);ADC;Number of SiPM channels", 0., 20., 20);
  plot(adcCountRef, "ADCcountsReflectedZOOM",
       "ADC counts (reflected);ADC;Number of SiPM channels", 0., 20., 20);
  plot(adcCountDir+adcCountRef, "ADCcounts",
       "ADC counts (direct+ reflected);ADC;Number of SiPM channels", 0., 200.);
  plot(adcCountDir+adcCountRef, "ADCcountsZOOM",
       "ADC counts (direct + reflected);ADC;Number of SiPM channels", 0., 20., 20);
 
  if( msgLevel( MSG::DEBUG) ){
    debug()<<format("deposit2ADC() : responsDir=%8.3f responsRef=%8.3f Gain=%8.3f nois=%4.2f adcCountDir = %4i adcCountRef = %4i"
                    ,response.first, response.second, gain, noise, adcCountDir, adcCountRef)
           << endmsg;
  }
  return adcCountDir+adcCountRef;
}
