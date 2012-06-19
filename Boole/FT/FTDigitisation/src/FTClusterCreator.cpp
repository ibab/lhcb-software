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
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("InputLocation" , m_inputLocation = LHCb::MCFTDigitLocation::Default, "Path to input MCDigits");
  declareProperty("OutputLocation" , m_outputLocation =  LHCb::FTClusterLocation::Default, "Path to output Clusters");
  declareProperty("ADCThreshold" , m_adcThreshold = 1 , "Minimal ADC Count to be added in cluster");
  declareProperty("ClusterMaxWidth" , m_clusterMaxWidth= 8 , "Maximal allowed width for clusters");
  declareProperty("ClusterMinCharge" , m_clusterMinCharge = 2 , "Minimal charge to keep cluster");
  declareProperty("ClusterMaxCharge" , m_clusterMaxCharge = 8 , "Maximal charge to keep cluster");
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
  LinkerWithKey<LHCb::MCParticle,LHCb::FTCluster> myLink( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default);

  // TEST : print Digit content : should be sorted
  if(msgLevel(MSG::DEBUG)){
    for (MCFTDigits::const_iterator iterDigit = mcDigitsCont->begin(); iterDigit!=mcDigitsCont->end();++iterDigit){
      MCFTDigit* mcDigit = *iterDigit;
      std::map< const LHCb::MCParticle*, double> mcMap = mcDigit->mcParticleMap();
      debug() <<"Channel ="<<mcDigit->channelID()<< " : " <<"\t ADC ="<<mcDigit->adcCount() << endmsg;
      for(std::map<const LHCb::MCParticle*,double>::const_iterator mapiter=mcMap.begin(); mapiter!=mcMap.end(); ++mapiter){
        debug() <<"MCParticle : index="<<mapiter->first->index() <<" "<<mapiter->first->particleID()
                << " Energy=" <<mapiter->second << endmsg;
      }
    }
  }


  // Since Digit Container is sorted wrt channelID, clusters are defined searching for bumps of ADC Count
  MCFTDigits::const_iterator seedDigitIter = mcDigitsCont->begin();


  while(seedDigitIter != mcDigitsCont->end()){ // loop over digits
    MCFTDigit* seedDigit = *seedDigitIter;

    if(seedDigit->adcCount() > m_adcThreshold){ // ADC count in  digit is over threshold
      std::vector<int> clusterADCDistribution;

      double totalEnergyFromMC = 0;
      std::map< const LHCb::MCParticle*, double> mcContributionMap;
      if ( msgLevel( MSG::DEBUG) ) {
        debug() <<"++ Starts clustering with Channel "<<seedDigit->channelID()
                <<" (ADC = "<<seedDigit->adcCount() <<" )"<< endmsg;
      }
    
      MCFTDigits::const_iterator lastDigitIter = seedDigitIter;
      do{
        // Add digit to cluster
        MCFTDigit* lastDigit = *lastDigitIter;
        if ( msgLevel( MSG::DEBUG) ) {
          debug() <<"+Add to Cluster : "<<lastDigit->channelID()<<" (ADC = "<<lastDigit->adcCount() <<" )"<< endmsg;
        }
    
        clusterADCDistribution.push_back(lastDigit->adcCount());

        // Keep track of MCParticles involved in the cluster definition
        std::map<const LHCb::MCParticle*, double> mcMap = lastDigit->mcParticleMap();
        std::map<const LHCb::MCParticle*, double>::const_iterator mcMapIter=mcMap.begin();
        for(; mcMapIter!=mcMap.end(); ++mcMapIter){
          totalEnergyFromMC +=mcMapIter->second;
          mcContributionMap[mcMapIter->first] += mcMapIter->second;
        }
        ++lastDigitIter;

      }while((lastDigitIter != mcDigitsCont->end()) 
             && keepAdding(lastDigitIter)
             && (clusterADCDistribution.size()<m_clusterMaxWidth));
      

      // Compute total ADC counts and cluster barycenter position in ChannelID unit
      int totalCharge = 0;
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


      // Checks that total ADC charge of the cluster is within the defined range
      if((totalCharge > m_clusterMinCharge)&&(totalCharge < m_clusterMaxCharge)){
        
        // Define Cluster(channelID, fraction, width, charge)  and save it
        int meanChanPosition = std::floor(meanPosition);
        double fractionChanPosition = (meanPosition-std::floor(meanPosition));

        // Define Cluster(channelID, fraction, width, charge)  and save it
        FTCluster *aCluster = new FTCluster(meanChanPosition,fractionChanPosition,(lastDigitIter-seedDigitIter),totalCharge);

        clusterCont->insert(aCluster);

        // Define second member of mcContributionMap as the fraction of energy corresponding to each involved MCParticle
        for(std::map<const LHCb::MCParticle*,double>::iterator i = mcContributionMap.begin(); i != mcContributionMap.end(); ++i){
          myLink.link(aCluster, (i->first), (i->second)/totalEnergyFromMC ) ;
          if ( msgLevel( MSG::DEBUG) ) {
            debug() << "SHOULD CREATE LINKER WITH : ClusterChannel=" << aCluster->channelID()
                    << " MCIndex="<<i->first->index()
                    <<" EnergyFraction=" << (i->second)/totalEnergyFromMC
                    << endmsg;
          }
        }

        // TEST
        if ( msgLevel( MSG::DEBUG) ) {
          debug() <<"+ Finish clustering from : "<<(*seedDigitIter)->channelID()
                  <<" (ADC = "<<(*seedDigitIter)->adcCount() <<" ) to "<<(*(lastDigitIter-1))->channelID()
                  <<" (ADC = "<<(*(lastDigitIter-1))->adcCount() <<")"<< endmsg;

          debug() <<"= Cluster is : "<<aCluster->channelID()
                  <<" Charge = "<<aCluster->charge()
                  << " Width = " << aCluster->size()
                  << " Fraction = " <<aCluster->fraction()
                  << endmsg;
        }

      }
      seedDigitIter = (lastDigitIter-1);
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
          &&((*clusCandIter)->channelID() == ((*(clusCandIter - 1))->channelID() + 1))
          && ((*(clusCandIter - 1 ))->channelID().sipmCell() != 63 )
          && ((*(clusCandIter - 1 ))->channelID().sipmCell() != 127 )
          &&((*clusCandIter)->adcCount() > m_adcThreshold));

}




