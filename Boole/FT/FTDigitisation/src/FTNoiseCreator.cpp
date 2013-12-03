// Include files
//#include <iterator>
#include <sstream>
#include <math.h>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from FTEvent
#include "Event/MCFTDigit.h"
#include "Event/FTCluster.h"

// local
#include "FTNoiseCreator.h"

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : FTNoiseCreator
//
// 2013-11-28 : Jacco de Vries
// TODO :
// - Add irradiation effect to afterpulse probability?
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FTNoiseCreator )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FTNoiseCreator::FTNoiseCreator( const std::string& name,
                                    ISvcLocator* pSvcLocator)
: GaudiHistoAlg ( name , pSvcLocator )
{

  declareProperty("InputLocation" ,     m_inputLocation     = LHCb::MCFTDigitLocation::Default, "Path to input MCDigits");
  declareProperty("OutputLocation" ,    m_outputLocation    = "MC/FT/Digits", "Path to output (MC + noise) Digits");

  declareProperty("SipmGain",           m_sipmGain          = 2.0,  "Sipm gain per pe"                );
  declareProperty("SipmGainVariation",  m_sipmGainVariation = 0.05, "Sipm gain variation per pe"      );
  declareProperty("SipmPedestal",       m_pedestal          = 0.0,  "Sipm pedestal"                   );
  declareProperty("ADCNoise",           m_adcNoise          = 0.5,  "ADC noise"                       );
  declareProperty("ClusterMinCharge",   m_clusterMinCharge  = 8,    "Minimum cluster charge"          );

  declareProperty("Temperature",        m_temperature       = 20.,  "Sipm temperature (deg.C)"        );  
  declareProperty("Irradiation",        m_irradiation       = 50.,  "Sipm received irradiation (fb-1)");
  declareProperty("ThermalNoiseRateBase", m_thermalNoiseRateBase = 0.1, "Sipm channel thermal (dark) noise rate at 20 deg.C, 0 fb-1, (MHz)");
  declareProperty("CrossTalkProbability", m_crossTalkProbability = 0.07, "Sipm cross-talk probability per pe");
  declareProperty("AfterpulseProbability", m_afterpulseProbability = 0.15, "Sipm random afterpulse probability of 1 pe to occur per hit PIXEL (not channel: so prob !~ charge!)");
  declareProperty("Nu",                 m_nu = 7.6, "Nu of collision run. Changes total number of afterpulses");

}
//=============================================================================
// Destructor
//=============================================================================
FTNoiseCreator::~FTNoiseCreator() {}

int factorial(int n) {
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode FTNoiseCreator::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ){
    debug() << "==> Initialize" << endmsg;
    debug() << "Temperature is " << m_temperature << endmsg;
    debug() << "Irradiation is " << m_irradiation << endmsg;
    debug() << "CrosstalkProbability is " << m_crossTalkProbability << endmsg;
    debug() << "AfterpulseProbability is " << m_afterpulseProbability << endmsg;
    debug() << "Nu is " << m_nu << endmsg;
    debug() << ": InputLocation is " <<m_inputLocation << endmsg;
    debug() << ": OutputLocation is " <<m_outputLocation << endmsg;
  }


  StatusCode sc1 = m_flatDist.initialize( randSvc(), Rndm::Flat(0.0,1.0) );  
  StatusCode sc2 = m_gauss.initialize( randSvc(), Rndm::Gauss(0.0,1.0) );  
  StatusCode sc3 = m_rndmLandau.initialize( randSvc(), Rndm::Landau(39.0, 7.8) ); // from a fit to the MCHit cluster charge


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FTNoiseCreator::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "[FTNoiseCreator] ==> Execute NEW EVENT" << endmsg;
  
  info() << " -- START NEW EVENT" << endmsg;

  // retrieve FTDigits
  MCFTDigits* mcDigitsCont = get<MCFTDigits>(m_inputLocation);
  debug() <<"mcDigitsCont->size() : " << mcDigitsCont->size()<< endmsg;


  // Get channels containing an MCHit
  std::vector<MCFTDigit*> mcHitDigits;
  std::vector<FTChannelID> mcHitChannels;
  for (MCFTDigits::const_iterator iterDigit = mcDigitsCont->begin(); iterDigit!=mcDigitsCont->end();++iterDigit){
    MCFTDigit* mcDigit = *iterDigit;
    mcHitDigits.push_back( mcDigit );
    mcHitChannels.push_back( mcDigit -> channelID() );
  }
  info() << "size of mcHitChannels: " << mcHitChannels.size() << endmsg;

  // Set geometry
  int Nlayers = 4*3;  // layers * stations
  int Nquarters = 4;
  int Nsipms = 4*4*6; // sipms * mats * modules
  int Nchannels = 128;
  int Ntotchannels = Nlayers * Nquarters * Nsipms * Nchannels;


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
  int NthermalNoise = Ntotchannels * (m_thermalNoiseRateBase / readoutFreq) * (irradCoef * m_irradiation) * pow(2,(m_temperature-roomTemp)/tempCoef);

  // Calculate number of expected afterpulses due to thermal noise hits
  float avgNoisePE = 1. + m_crossTalkProbability + pow(m_crossTalkProbability,2) + pow(m_crossTalkProbability,3) + pow(m_crossTalkProbability,4); 
  int NthermalNoise_AP = 0;
  float APLoop = float(NthermalNoise);
  while (APLoop > 1.) {
    APLoop *= m_afterpulseProbability ; // effectively also takes into account afterpulses of cross-talk (of cross-talk) of thermal noise
    NthermalNoise_AP += int(APLoop*avgNoisePE+0.5);
  }

  // Calculate number of VISIBLE thermal noise hits, due to cross-talk:
  //int minPEforCluster = int( float(m_clusterMinCharge) / m_sipmGain );
  int NvisibleThermalNoise = int( (NthermalNoise+NthermalNoise_AP) * pow(m_crossTalkProbability, 0) ); // ct^0 = generate all pulses
  info() << "[THERMAL] NthermalNoise = " << NvisibleThermalNoise << " (of which "
    << float(NthermalNoise_AP) / (float(NthermalNoise + NthermalNoise_AP)) * 100. << " \% afterpulses)" << endmsg;
  plot(NvisibleThermalNoise, "NvisibleThermalNoiseHits", "NvisibleThermalNoiseHits; NvisibleThermalNoiseHits", 0. , 1000000. ,10000);


  // Loop over all noise hits
  int totalThermalInMCHit  = 0;
  int totalThermalCreated  = 0;
  int totalThermalAppended = 0;
  for(int i=0; i<NvisibleThermalNoise; i++) {

    // Get random channel
    double r = m_flatDist()*(Nlayers);
    int layer = int(r);
    r = m_flatDist()*(Nquarters);
    int quarter = int(r);
    r = m_flatDist()*(Nsipms);
    int sipmID = int(r);
    r = m_flatDist()*(Nchannels);
    int sipmCell = int(r);
    FTChannelID noiseChannel(layer, quarter, sipmID, sipmCell);
    
    // Create random ADC value due to cross-talk
    int Npe = 1; // should be >1 if NvisibleThermalNoise has a ct^(>0);
    while(m_flatDist() < m_crossTalkProbability) { Npe++; } // allows for cross-talk of cross-talk
    double gain  = m_sipmGain * (1. + m_gauss() * m_sipmGainVariation );
    double noise = m_adcNoise * m_gauss();
    int noiseADCcount = int( m_pedestal + Npe * gain + noise + 0.5 );
 
    // Check if not already in MCHit list
    std::vector<FTChannelID>::const_iterator chanIt = std::find(mcHitChannels.begin(), mcHitChannels.end(), noiseChannel);
    if (chanIt != mcHitChannels.end()) {
      // add ADC value to existing MCHit channel
      MCFTDigit* MCHitdigit = mcHitDigits[ std::distance<std::vector<FTChannelID>::const_iterator>(mcHitChannels.begin(),chanIt) ];
      MCHitdigit -> setAdcCount( (MCHitdigit -> adcCount()) + noiseADCcount );
      totalThermalInMCHit++;
    } else {
      // Check if not already in noise list
      chanIt = std::find(noiseChannels.begin(), noiseChannels.end(), noiseChannel);
      if( chanIt != noiseChannels.end() ) {
        // Add ADC value to existing noise channel
        noiseADCs[ std::distance<std::vector<FTChannelID>::const_iterator>(noiseChannels.begin(),chanIt) ] += noiseADCcount;
        totalThermalAppended++;
      } else {
        // Add new noise channel
        noiseChannels.push_back( noiseChannel );
        noiseADCs.push_back( noiseADCcount );
        totalThermalCreated++;
      }
    }
  }

  info() << "[THERMAL] Created: " << totalThermalCreated << ", Appended: " << totalThermalAppended << ", Added to MCHit: " << totalThermalInMCHit << endmsg;









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
  debug() << "[HIT AFTERPULSING] integralBeforCut = " << integralBeforCut << endmsg;
  debug() << "[HIT AFTERPULSING] integralAfterCut = " << integralAfterCut << endmsg;



  // Determine number of hit afterpulsing hits expected
  int NavgClusInEvent = int(396788./100. / 7.6 * m_nu); // Based on 100 events, nu=7.6, Bs2phiphi, pythia8. Assumes Nclusters goes linearly with nu
  int NhitAfterpulses = NavgClusInEvent; 
     
  // Calculate number of additional hit afterpulse hits due to afterpulsing
  int NhitAfterpulses_AP = 0;
  APLoop = float(NhitAfterpulses);
  while (APLoop > 1.) {
    APLoop *= m_afterpulseProbability; // effectively also takes into account afterpulses of cross-talk of (cross-talk of) MCHit afterpulses
    NhitAfterpulses_AP += int(APLoop*avgNoisePE+0.5);
  }

  // Determine number of VISIBLE hit afterpulses due to cross-talk
  int NvisibleHitAfterpulses = int( (NhitAfterpulses+NhitAfterpulses_AP) * pow(m_crossTalkProbability, 0) ); // ct^0 = do all
  info() << "[HIT AFTERPULSING] N channel hits for afterpulses = " << NvisibleHitAfterpulses << " (of which "
    << float(NhitAfterpulses_AP) / float(NhitAfterpulses + NhitAfterpulses_AP) * 100. << " \% afterpulses)" << endmsg;
  plot(NvisibleHitAfterpulses, "NvisibleHitAfterpulses", "NvisibleHitAfterpulses; NvisibleHitAfterpulses", 0. , 1000000. ,10000);


  // determine how many hit afterpulses will have a charge of 1 p.e., 2 p.e., etc. (without cross-talk)
  float NsumBinFractions[20] = { 0.204691106081,  0.176326997846,  0.132542585516,  0.0997058439121,  0.0760373252544,  
    0.0585607741894,  0.0453520643227,  0.0352234776303,  0.0273958443495,  0.0213224364803,  0.0166009440283,  0.0129269868938,  
    0.010066868574,  0.00783983501193,  0.00610557755609,  0.00475499468632,  0.0037031811119,  0.00288403576027,  0.00224608761991,  0.001749254176 };
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
      float r = m_flatDist();
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
        while(m_flatDist() < m_crossTalkProbability) { Npe++; } // allow for crosstalk of crosstalk
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
        if (m_flatDist() * (integralBeforCut+integralAfterCut) < integralBeforCut) {
          // first expo
          double randRange = (off+N*exp(e1*intBeg)) + m_flatDist()*( (off+N*exp(e1*c)) - (off+N*exp(e1*intBeg)));
          double randExp = log( (randRange-off)/N ) / e1;
          IDCell = int( randExp + 0.5 );
        } else {
          // second expo
          double randRange = (off+norm*exp(e2*intEnd)) + m_flatDist()*( (off+norm*exp(e2*c)) - (off+norm*exp(e2*intEnd)));
          double randExp = log( (randRange-off)/norm ) / e2;
          IDCell = int( randExp + 0.5 );
        }
        int bitShiftsipmID = 7;
        sipmID = (IDCell >> bitShiftsipmID);
        sipmCell = IDCell - sipmID * pow(2,bitShiftsipmID);
      }

      // Now that we have the IDCell (= sipmID and sipmCell), get random quarter and layer:
      double r = m_flatDist()*(Nlayers);
      int layer = int(r);
      r = m_flatDist()*(Nquarters);
      int quarter = int(r);

      FTChannelID noiseChannel(layer, quarter, sipmID, sipmCell);


      // Check if not already in MCHit list
      std::vector<FTChannelID>::const_iterator chanIt = std::find(mcHitChannels.begin(), mcHitChannels.end(), noiseChannel);
      if (chanIt != mcHitChannels.end()) {
        // add ADC value to existing MCHit channel
        MCFTDigit* MCHitdigit = mcHitDigits[ std::distance<std::vector<FTChannelID>::const_iterator>(mcHitChannels.begin(),chanIt) ];
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

  info() << "[HIT AFTERPULSING] Created: " << totalAPCreated << ", Appended: " << totalAPAppended << ", Added to MCHit: " << totalAPInMCHit << endmsg;








  //==========================================================================  
  // Add noise digits to full digit set
  //
 

  int addOK = 0;
  int addFailed = 0;
  for (unsigned int i=0; i<noiseChannels.size(); i++) {
    plot(noiseADCs[i]    , "noiseADCcount", "Noise ADC count; Noise ADC count", 0. , 30. ,30);
    plot(noiseChannels[i], "noiseChannels", "Noise channelID; Noise channelID", 0., 800000., 800000);

    const std::map< const LHCb::MCHit*, double > MCHitMapDummy;
    MCFTDigit* noiseMCDigit = new MCFTDigit( noiseChannels[i], noiseADCs[i], MCHitMapDummy );
    try { mcDigitsCont->add( noiseMCDigit ); }
    catch(GaudiException) { debug() << "Caught GaudiException: Noise hit " << addOK
       << " not inserted due to faulty random generated key" << endmsg; 
       ++addFailed; --addOK;
    } 
    ++addOK;
  }
  std::stable_sort( mcDigitsCont->begin(), mcDigitsCont->end(), LHCb::MCFTDigit::lowerByChannelID );
  info() << "Succesfully added " << addOK << " noise hits to event (" << addFailed << " failed)" << endmsg;
  debug() << "mcDigitsCont size after noise: " << mcDigitsCont->size() << endmsg;


  // Store (noise+MC) digits in TES
  MCFTDigits* newContainer = mcDigitsCont;
  put(newContainer, m_outputLocation);




  return StatusCode::SUCCESS;
}




//=============================================================================
//  Finalize
//=============================================================================
StatusCode FTNoiseCreator::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================



