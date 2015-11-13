// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from FTEvent
#include "Event/MCFTDigit.h"

// local
#include "MCFTDigitMonitor.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : MCFTDigitMonitor
//
// 2012-07-05 : Eric Cogneras
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCFTDigitMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  MCFTDigitMonitor::MCFTDigitMonitor( const std::string& name,
                                      ISvcLocator* pSvcLocator)
    : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty("DigitLocation" , m_digitLocation =  LHCb::MCFTDigitLocation::Default, "Path to MCFTDeposits");
}
//=============================================================================
// Destructor
//=============================================================================
MCFTDigitMonitor::~MCFTDigitMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MCFTDigitMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode MCFTDigitMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  
  // Set geometry
  //int Nlayers = 4*3;  // layers * stations
  //int NModule = 12;
  //int NMat = 2;
  //int Nsipms = 16;
  //int Nchannels = 128;
  //int Ntotchannels = Nlayers * NModule * NMat * Nsipms * Nchannels;

  // retrieve FTDigits
  const MCFTDigits* mcDigitsCont = get<MCFTDigits>(m_digitLocation);
  if ( msgLevel(MSG::DEBUG) ) 
    debug() <<"mcDigitsCont->size() : " << mcDigitsCont->size()<< endmsg;

  // number of clusters
  plot(mcDigitsCont->size(), "ClustersNumber",
       "# Clusters; # Clusters; " , 
       0. , 50000. , 100);

  // Loop over MCFTDigits
  for (MCFTDigits::const_iterator iterDigit = mcDigitsCont->begin(); iterDigit!=mcDigitsCont->end();++iterDigit){
    MCFTDigit* mcDigit = *iterDigit; 

    // plot number of hits
    plot((int)mcDigit->deposit()->mcHitVec().size(), "# Hits",
         "# Hits; # Hits; Number of SiPM channels" , 
         0. , 50. ,50);

    // plot digit adc count
    plot((double)mcDigit->adcCount(), "DigADCCount",
         "ADC count [Channel level]; ADC count;Number of SiPM channels" , 
         0. , 100. ,100);
    plot((double)mcDigit->adcCount(), "DigADCCountZOOM",
         "ADC count [Channel level]; ADC count;Number of SiPM channels" , 
         0. , 10. ,10);
    plot((double)mcDigit->adcCount(), "DigADCCountZOOMZOOM",
         "ADC count [Channel level]; ADC count;Number of SiPM channels" , 
         -1. , 1.);

    // average adc counts per hits
    plot((double)mcDigit->adcCount()/(int)mcDigit->deposit()->mcHitVec().size(), "DigADCCountPerHits",
         "ADC count [Channel level] / nHits; ADC count / nHits;Number of SiPM channels" , 
         0. , 100. ,100);
    plot((double)mcDigit->adcCount()/(int)mcDigit->deposit()->mcHitVec().size(), "DigADCCountPerHitsZOOM",
         "ADC count [Channel level] / nHits; ADC count / nHits;Number of SiPM channels" , 
         0. , 10. ,10);
    plot((double)mcDigit->adcCount()/(int)mcDigit->deposit()->mcHitVec().size(), "DigADCCountPerHitsZOOMZOOM",
         "ADC count [Channel level] / nHits; ADC count / nHits;Number of SiPM channels" , 
         -1. ,1.);

    // deposits distribution in (x,y)
    if( mcDigit->deposit()->mcHitVec()[0] != 0 ) {
      plot2D(mcDigit->deposit()->mcHitVec()[0]->midPoint().x(), mcDigit->deposit()->mcHitVec()[0]->midPoint().y(), "DigitsDistribution",
       "Digits Distribution; x[mm]; y[mm]" , 
       -3600. , 3600. , -2700. , 2700. , 100, 100);
    }
 
    // plot occupancy plot of ADC counts
    const FTChannelID mcChannel = mcDigit -> channelID();
    unsigned int mcCell = mcChannel.sipmCell(); // 128 = 2^7
    unsigned int mcSipm = mcChannel.sipmId();   // 16  = 2^4
    unsigned int mcModu = mcChannel.module();   // (0,1,2,3,4,10,11,5,6,7,8,9)
    unsigned int mcQuar = mcChannel.quarter();  // 4
    unsigned int quarterModule = QuarterModule( mcModu ); // (0,1,2,3,4,5,6,7,8,9,10,11)
    unsigned int quarterCell = quarterModule * 128 * 16 + mcSipm * 128 + mcCell;
    //plot2D(quarterCell, mcDigit->adcCount(), "adcCountPerQuarterCell",
    //  "ADC count per quarterCell;quarterCell;ADC count", 0, 128*16*12, 0, 80, 128*16*12, 80); 
    
    int Nthermal = mcDigit -> deposit() -> thNoiseCont();
    int Nafterpl = mcDigit -> deposit() -> afterplCont();
    int Nxtalk   = mcDigit -> deposit() -> xtalkCont();
    
    if ( msgLevel( MSG::DEBUG) )
      debug() << "Noise PEs == Nthermal:" << Nthermal << "  Nafterpl:" << Nafterpl << "  Nxtalk:" << Nxtalk << endmsg;

    plot(Nthermal, "NthermalPerChannel", "NthermalPerChannel;Nthermal", 0., 30., 30);
    plot(Nafterpl, "NafterplPerChannel", "NafterplPerChannel;Nafterpl", 0., 30., 30);
    plot(Nxtalk, "NxtalkPerChannel", "NxtalkPerChannel;Nxtalk", 0., 30., 30);

    if(mcQuar==0) {
      for(int count=0;count<mcDigit->adcCount();++count) {
        plot(quarterCell, "adcCountsPerQuarterCellQ0", "ADC counts per quarterCell Q0;quarterCell;ADC counts", 0., 128*16*12., 128*16*12);
        plot(quarterModule, "adcCountsPerQuarterModuleQ0", "ADC counts per quarterModule Q0;quarterModule;ADC counts", 0., 12., 12);
        plot(mcSipm, "adcCountsPerSipmQ0", "ADC counts per SiPM Q0;SiPM;ADC counts", 0., 16., 16);
        plot(mcCell, "adcCountsPerCellQ0", "ADC counts per Cell Q0;Cell;ADC counts", 0., 128., 128);
        if(mcChannel.layer() == 0) {
          plot(quarterCell, "adcCountsPerQuarterCellL0Q0", "ADC counts per quarterCell L0Q0;quarterCell;ADC counts", 0., 128*16*12., 128*16*12);
        }
      }
      if(mcChannel.layer() == 0) {

        // noise
        int i;
        for(i=0; i<Nthermal; i++) {
          plot(quarterModule, "digitThNoise_QM_Q0L0", "digitThNoise_QM_Q0L0; quarterModule;PE", 0., 12., 12);
          plot(quarterCell,   "digitThNoise_QC_Q0L0", "digitThNoise_QC_Q0L0; quarterModule;PE", 0., 128*16*12., 128*16*12);
        }
        for(i=0; i<Nafterpl; i++) {
          plot(quarterModule, "digitAfterplCont_QM_Q0L0", "digitAfterplCont_QM_Q0L0; quarterModule;PE", 0., 12., 12);
          plot(quarterCell,   "digitAfterplCont_QC_Q0L0", "digitAfterplCont_QC_Q0L0; quarterModule;PE", 0., 128*16*12., 128*16*12);
        }
        for(i=0; i<Nxtalk; i++) {
          plot(quarterModule, "digitXtalkCont_QM_Q0L0", "digitXtalkCont_QM_Q0L0; quarterModule;PE", 0., 12., 12);
          plot(quarterCell,   "digitXtalkCont_QC_Q0L0", "digitXtalkCont_QC_Q0L0; quarterModule;PE", 0., 128*16*12., 128*16*12);
        }
      }
    }


    //plot2D(mcDigit -> deposit() -> channelID(), mcDigit -> adcCount(), "digitADC", "digitADC; channelID; ADC", 0., float(Ntotchannels), 0., 120, Ntotchannels, 120);
    //plot2D(mcDigit -> deposit() -> channelID(), mcDigit -> deposit() -> thNoiseCont(), "digitThNoise", "digitThNoise; channelID; PE", 0., float(Ntotchannels), 0., 30, Ntotchannels, 30);
    //plot2D(mcDigit -> deposit() -> channelID(), mcDigit -> deposit() -> afterplCont(), "digitAfterpl", "digitAfterpl; channelID; PE", 0., float(Ntotchannels), 0., 30, Ntotchannels, 30);
    //plot2D(mcDigit -> deposit() -> channelID(), mcDigit -> deposit() -> xtalkCont(), "digitXtalk", "digitXtalk; channelID; PE", 0., float(Ntotchannels), 0., 30, Ntotchannels, 30);
    

    // profile plot of the adc counts vs time
    

    /*
    // plot digit adc count vs energy deposited in the channel - to be fixed with new MCFTDigit
    double EnergySum = 0;
    std::map< const LHCb::MCHit*, double>::const_iterator mchit = (mcDigit->mcHitMap()).begin();
    for(;mchit != (mcDigit->mcHitMap()).end(); ++mchit)
      EnergySum += mchit->second;
    
    plot2D(EnergySum, mcDigit->adcCount(), "DigADCCountvsEnergy",
           "ADC count vs deposited energy [Channel level]; Deposited energy [MeV]; ADC count" ,
           0. , 5. ,100, 0. , 20., 20);
    */

    

  }

  return StatusCode::SUCCESS;
}

unsigned int MCFTDigitMonitor::QuarterModule(unsigned int module)
{
  unsigned int quarterModuleNber = 9;
  if(module < 5) quarterModuleNber = module;
  else 
  {
    quarterModuleNber = module - 4;
    if(module == 10) quarterModuleNber = 5;
    if(module == 11) quarterModuleNber = 0;
  }
  return quarterModuleNber;
}

//=============================================================================
