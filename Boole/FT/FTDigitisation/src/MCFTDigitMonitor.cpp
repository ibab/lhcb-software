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
    plot2D(mcDigit->deposit()->mcHitVec()[0]->midPoint().x(), mcDigit->deposit()->mcHitVec()[0]->midPoint().y(), "DigitsDistribution",
	   "Digits Distribution; x[mm]; y[mm]" , 
	   -3600. , 3600. , -2700. , 2700. , 100, 100);
    
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
//=============================================================================
