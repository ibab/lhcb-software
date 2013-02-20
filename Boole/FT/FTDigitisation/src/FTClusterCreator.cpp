// $Id$
// Include files
//#include <iterator>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from Linker
#include "Linker/LinkerWithKey.h"

// from FTEvent
#include "Event/MCFTDigit.h"
#include "Event/FTCluster.h"

// local
#include "FTClusterCreator.h"

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : FTClusterCreator
//
// 2012-04-06 : Eric Cogneras
// TO DO :
// - split cluster once cluster width is above
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FTClusterCreator )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  FTClusterCreator::FTClusterCreator( const std::string& name,
                                      ISvcLocator* pSvcLocator)
    : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty("InputLocation" ,     m_inputLocation     = LHCb::MCFTDigitLocation::Default, "Path to input MCDigits");
  declareProperty("OutputLocation" ,    m_outputLocation    = LHCb::FTClusterLocation::Default, "Path to output Clusters");
  declareProperty("ADCThreshold" ,      m_adcThreshold      = 1 , "Minimal ADC Count to be added in cluster");
  declareProperty("ClusterMinWidth" ,   m_clusterMinWidth   = 1 , "Minimal allowed width for clusters");
  declareProperty("ClusterMaxWidth" ,   m_clusterMaxWidth   = 8 , "Maximal allowed width for clusters");
  declareProperty("ClusterMinCharge" ,  m_clusterMinCharge  = 8 , "Minimal charge to keep cluster ~4 p.e.");
  declareProperty("ClusterMinADCPeak" , m_clusterMinADCPeak = 5 , "Minimal ADC for cluster peaks, ~2.5 pe.");

  m_nberOfLostHit = 0;
  m_nberOfKeptHit = 0;
}
//=============================================================================
// Destructor
//=============================================================================
FTClusterCreator::~FTClusterCreator() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode FTClusterCreator::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ){
    debug() << "==> Initialize" << endmsg;
    debug() << ": InputLocation is " <<m_inputLocation << endmsg;
    debug() << ": OutputLocation is " <<m_outputLocation << endmsg;
    debug() << ":m_adcThreshold is " <<m_adcThreshold << endmsg;
  }
  m_nCluster = 0;
  m_sumCharge = 0.;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FTClusterCreator::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;


  // retrieve FTDigits
  const MCFTDigits* mcDigitsCont = get<MCFTDigits>(m_inputLocation);
  debug() <<"mcDigitsCont->size() : " << mcDigitsCont->size()<< endmsg;

  // define clusters container
  FTClusters *clusterCont = new FTClusters();
  clusterCont->reserve(mcDigitsCont->size());

  // Register FTClusters in the transient data store
  put(clusterCont, m_outputLocation);

  // Create a link between the FTCluster and the MCParticle which leave a track
  LinkerWithKey<LHCb::MCParticle,LHCb::FTCluster> mcToClusterLink( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default);

  LinkerWithKey<LHCb::MCHit,LHCb::FTCluster> mchitToClusterLink( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default);

  // DEBUG : print Digit content : should be sorted
  if(msgLevel(MSG::DEBUG)){
    for (MCFTDigits::const_iterator iterDigit = mcDigitsCont->begin(); iterDigit!=mcDigitsCont->end();++iterDigit){
      MCFTDigit* mcDigit = *iterDigit;
      std::map< const LHCb::MCHit*, double> hitMap = mcDigit->mcHitMap();
      debug() <<"Channel ="<<mcDigit->channelID()<< " : " <<"\t ADC ="<<mcDigit->adcCount() << endmsg;
      for(std::map<const LHCb::MCHit*,double>::const_iterator mapiter=hitMap.begin(); mapiter!=hitMap.end(); ++mapiter){
        debug() <<"MCParticle : index="<<mapiter->first->mcParticle()->index() <<" "<<mapiter->first->mcParticle()->particleID()
                << " Energy=" <<mapiter->second << endmsg;
      }
    }
  }


  // Since Digit Container is sorted wrt channelID, clusters are defined searching for bumps of ADC Count
  MCFTDigits::const_iterator seedDigitIter = mcDigitsCont->begin();


  while(seedDigitIter != mcDigitsCont->end()){ // loop over digits
    MCFTDigit* seedDigit = *seedDigitIter;



    if(seedDigit->adcCount() > m_adcThreshold){ // ADC count in  digit is over threshold : start cluster
      bool hasSeed = seedDigit->adcCount() > m_clusterMinADCPeak;
      // vector of ADC count of each cell from the cluster
      std::vector<int> clusterADCDistribution;

      // total energy from MC
      double totalEnergyFromMC = 0;

      // map of contributing MCParticles with their relative energy deposit
      std::map< const LHCb::MCParticle*, double> mcContributionMap;


      if ( msgLevel( MSG::DEBUG) ) {
        debug() <<"++ Starts clustering with Channel "<<seedDigit->channelID()
                <<" (ADC = "<<seedDigit->adcCount() <<" )"<< endmsg;
      }
    

      /** Construction of the cluster from its first (seedDigit) to last (lastDigit) channel
      */
      int NbOfHitInvolvedInCluster = 0;
      MCFTDigits::const_iterator lastDigitIter = seedDigitIter;
      do{
        // Add digit to cluster
        MCFTDigit* lastDigit = *lastDigitIter;
        if ( msgLevel( MSG::DEBUG) ) {
          debug() <<"+Add to Cluster : "<<lastDigit->channelID()<<" (ADC = "<<lastDigit->adcCount() <<" )"<< endmsg;
        }
        if ( lastDigit->adcCount() > m_clusterMinADCPeak ) hasSeed = true;
        
        clusterADCDistribution.push_back(lastDigit->adcCount());
        NbOfHitInvolvedInCluster += lastDigit->mcHitMap().size();


        // Keep track of MCParticles involved in the cluster definition
        std::map<const LHCb::MCHit*, double> hitMap = lastDigit->mcHitMap();
        std::map<const LHCb::MCHit*, double>::const_iterator hitMapIter=hitMap.begin();
        for(; hitMapIter!=hitMap.end(); ++hitMapIter){
          totalEnergyFromMC +=hitMapIter->second;
          mcContributionMap[hitMapIter->first->mcParticle()] += hitMapIter->second;
        }
        ++lastDigitIter;

      }while((lastDigitIter != mcDigitsCont->end()) 
             && keepAdding(lastDigitIter)
             && (clusterADCDistribution.size()<m_clusterMaxWidth));
      

      /** Compute total ADC counts and cluster barycenter position in ChannelID unit
       */
      unsigned int totalCharge = 0;
      double meanPosition = 0;
      for(std::vector<int>::const_iterator i = clusterADCDistribution.begin();i != clusterADCDistribution.end(); ++i){
        totalCharge += *i;
        meanPosition += (i-clusterADCDistribution.begin()) * (*i);
        if(msgLevel(MSG::DEBUG)){
          debug() <<"- Distance="<< (i-clusterADCDistribution.begin())
                  <<" adc="<<*i << " totalCharge=" << totalCharge
                  << endmsg;
        }
      }
      meanPosition =(*seedDigitIter)->channelID() + meanPosition/totalCharge;


      // Before Cluster record, checks :
      // - total ADC charge of the cluster is over threshold and
      // - A seed has been found and
      // - number of cells above the minimal allowed value      
      if( ( totalCharge > m_clusterMinCharge) &&
          hasSeed &&
          (clusterADCDistribution.size() > m_clusterMinWidth) ) {
        
        // Define Cluster(channelID, fraction, width, charge)  and save it
        int meanChanPosition = std::floor(meanPosition);
        double fractionChanPosition = (meanPosition-std::floor(meanPosition));

        // Define Cluster(channelID, fraction, width, charge)  and save it
        FTCluster *newCluster = new FTCluster(meanChanPosition,fractionChanPosition,(lastDigitIter-seedDigitIter),totalCharge);

        ++m_nCluster;
        m_sumCharge += totalCharge;

        clusterCont->insert(newCluster);


        // Define second member of mcContributionMap as the fraction of energy corresponding to each involved MCParticle
        for(std::map<const LHCb::MCParticle*,double>::iterator i = mcContributionMap.begin(); i != mcContributionMap.end(); ++i){
          mcToClusterLink.link(newCluster, (i->first), (i->second)/totalEnergyFromMC ) ;
          if ( msgLevel( MSG::DEBUG) ) {
            debug() << "Linked ClusterChannel=" << newCluster->channelID()
                    << " to MCIndex="<<i->first->index()
                    << " with EnergyFraction=" << (i->second)/totalEnergyFromMC
                    << endmsg;
          }
        }

        // DEBUG
        if ( msgLevel( MSG::DEBUG) ) {
          debug() <<"+ Finish clustering from : "<<(*seedDigitIter)->channelID()
                  <<" (ADC = "<<(*seedDigitIter)->adcCount() <<" ) to "<<(*(lastDigitIter-1))->channelID()
                  <<" (ADC = "<<(*(lastDigitIter-1))->adcCount() <<")"<< endmsg;

          debug() <<"= Cluster is : "<<newCluster->channelID()
                  <<" Charge = "<<newCluster->charge()
                  << " Width = " << newCluster->size()
                  << " Fraction = " <<newCluster->fraction()
                  << endmsg;
        }

        // increment lost Hit counter
        m_nberOfKeptHit += NbOfHitInvolvedInCluster;   
      }
      else{
        // increment lost Hit counter
        m_nberOfLostHit += NbOfHitInvolvedInCluster;      
        
      }
      
      seedDigitIter = (lastDigitIter-1);
    }
    else{
      // increment lost Hit counter
      m_nberOfLostHit += seedDigit->mcHitMap().size();
    }

    

    ++seedDigitIter;

  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FTClusterCreator::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  info() << "*** Average cluster charge = " << m_sumCharge / m_nCluster << endmsg;

  info() << "*** Lost Hits = " <<m_nberOfLostHit << "\t Kept Hits = "<< m_nberOfKeptHit 
         << "\t Eff = "<< (double)m_nberOfKeptHit/(m_nberOfKeptHit + m_nberOfLostHit)<< endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
bool FTClusterCreator::keepAdding(LHCb::MCFTDigits::const_iterator clusCandIter)
{
  /** checks that channelID is :
      - 1. in the same SiPM
      - 2. follows the previous one
      - 3. previous channel not 63
      - 4. previous channel not 127
      - 5. above threshold

  */
  return (((*clusCandIter)->channelID().sipmId() == ((*(clusCandIter-1))->channelID().sipmId()))
          && ((*clusCandIter)->channelID() == ((*(clusCandIter - 1))->channelID() + 1))
          && ((*(clusCandIter - 1 ))->channelID().sipmCell() != 63 )
          && ((*(clusCandIter - 1 ))->channelID().sipmCell() != 127 )
          && ((*clusCandIter)->adcCount() > m_adcThreshold));

}




