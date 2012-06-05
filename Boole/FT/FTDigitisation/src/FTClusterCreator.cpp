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

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  debug() << "==> Initialize" << endmsg;
  debug() << ": InputLocation is " <<m_inputLocation << endmsg;
  debug() << ": OutputLocation is " <<m_outputLocation << endmsg;
  debug() << ":m_adcThreshold is " <<m_adcThreshold << endmsg;
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


  LinkerWithKey<LHCb::MCParticle,LHCb::FTCluster> myLink( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default);

  // TEST : print Digit content : must be sorted
  if(msgLevel(MSG::DEBUG)){

    for (MCFTDigits::const_iterator iterDigit = mcDigitsCont->begin(); iterDigit!=mcDigitsCont->end();++iterDigit){
      MCFTDigit* aDigit = *iterDigit;
      std::map< const LHCb::MCParticle*, double> aMCMap = aDigit->mcParticleMap();

      debug() <<"Channel ="<<aDigit->channelID()<< " : " <<"\t ADC ="<<aDigit->adcCount() << endmsg;
      for(std::map<const LHCb::MCParticle*,double>::const_iterator mapiter=aMCMap.begin(); mapiter!=aMCMap.end(); ++mapiter){
        debug() <<"MCParticle : index="<<mapiter->first->index() <<" "<<mapiter->first->particleID()
                << " Energy=" <<mapiter->second << endmsg;
      }
    }
  }


  // Since Digit Container is sorted wrt channelID, clusters are defined searching for bumps of ADC Count
  // (> m_adcThreshold) looping over digit container
  //bool IsClustering = false;
  MCFTDigits::const_iterator iterDigit = mcDigitsCont->begin();


  while(iterDigit != mcDigitsCont->end()){ // loop over digits
    MCFTDigit* aDigit = *iterDigit;

    if(aDigit->adcCount() > m_adcThreshold){ // ADC count in  digit is over threshold
      std::vector<int> clusterADCDistribution;

      double totalEnergyFromMC = 0;
      std::map< const LHCb::MCParticle*, double> mcContributionMap;

      debug() <<"++ Starts clustering with Channel "<<aDigit->channelID()<<" (ADC = "<<aDigit->adcCount() <<" )"<< endmsg;
      MCFTDigits::const_iterator jterDigit = iterDigit;

      do{
        // Add digit to cluster
        MCFTDigit* anotherDigit = *jterDigit;
        debug() <<"+Add to Cluster : "<<anotherDigit->channelID()<<" (ADC = "<<anotherDigit->adcCount() <<" )"<< endmsg;
        clusterADCDistribution.push_back(anotherDigit->adcCount());

        // Keep track of MCParticles involved in the cluster definition
        std::map<const LHCb::MCParticle*, double> aMCMap = anotherDigit->mcParticleMap();
        std::map<const LHCb::MCParticle*, double>::const_iterator mapiter=aMCMap.begin();
        for(; mapiter!=aMCMap.end(); ++mapiter){
          totalEnergyFromMC +=mapiter->second;
          mcContributionMap[mapiter->first] += mapiter->second;
        }
        ++jterDigit;

      }while((jterDigit != mcDigitsCont->end()) && keepAdding(jterDigit)&& (clusterADCDistribution.size()<m_clusterMaxWidth));
      

      // Compute total ADC counts and cluster barycenter position in ChannelID unit
      int totalCharge = 0;
      double meanPosition = 0;
      std::vector<int>::const_iterator veciter = clusterADCDistribution.begin();
      for(;veciter != clusterADCDistribution.end(); ++veciter){
        totalCharge += *veciter;
        meanPosition += (veciter-clusterADCDistribution.begin()) * (*veciter);
        debug() <<"- Distance="<< (veciter-clusterADCDistribution.begin())
                <<" adc="<<*veciter << " totalCharge=" << totalCharge
                << endmsg;

      }
      meanPosition =(*iterDigit)->channelID() + meanPosition/totalCharge;



      // Define Cluster(channelID, fraction, width, charge)  and save it
      int meanChannelPosition = std::floor(meanPosition);
      double fractionnalChannelPosition = (meanPosition-std::floor(meanPosition));

      // Define Cluster(channelID, fraction, width, charge)  and save it
      FTCluster *aCluster = new FTCluster(meanChannelPosition,fractionnalChannelPosition,(jterDigit-iterDigit),totalCharge);

      clusterCont->insert(aCluster);

      // Define second member of mcContributionMap as the fraction of energy corresponding to each involved MCParticle
      std::map< const LHCb::MCParticle*, double>::iterator mapiter = mcContributionMap.begin();
      for(; mapiter != mcContributionMap.end(); ++mapiter){
        debug() << "SHOULD CREATE LINKER WITH : ClusterChannel=" << aCluster->channelID()
                << " MCIndex="<<mapiter->first->index()
                <<" EnergyFraction=" << (mapiter->second)/totalEnergyFromMC
                << endmsg;

        myLink.link(aCluster, (mapiter->first), (mapiter->second)/totalEnergyFromMC ) ;
      }

      debug() <<"+ Finish clustering from : "<<(*iterDigit)->channelID()
              <<" (ADC = "<<(*iterDigit)->adcCount() <<" ) to "<<(*(jterDigit-1))->channelID()
              <<" (ADC = "<<(*(jterDigit-1))->adcCount() <<")"<< endmsg;

      debug() <<"= Cluster is : "<<aCluster->channelID()
              <<" Charge = "<<aCluster->charge()
              << " Width = " << aCluster->size()
              << " Fraction = " <<aCluster->fraction()
              << endmsg;

      iterDigit = (jterDigit-1);
    }

    ++iterDigit;

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
      - in the same SiPM
      - follows the previous one
      - previous channel not 63
      - previous channel not 127
      - above threshold

  */
    return (((*clusCandIter)->channelID().sipmId() == ((*(clusCandIter-1))->channelID().sipmId()))
            &&((*clusCandIter)->channelID() == ((*(clusCandIter - 1))->channelID() + 1))
            && ((*(clusCandIter - 1 ))->channelID().sipmCell() != 63 )
            && ((*(clusCandIter - 1 ))->channelID().sipmCell() != 127 )
            &&((*clusCandIter)->adcCount() > m_adcThreshold));

}




