// Include files
//#include <iterator>
#include <sstream>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from Linker
#include "Linker/LinkerWithKey.h"

// from FTEvent
#include "Event/MCFTDeposit.h"
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
  , m_nCluster(0)
  , m_sumCharge(0.)
  , m_evtNbCluster64(0)
{
  declareProperty("InputLocation" ,     m_inputLocation     = LHCb::MCFTDigitLocation::Default, "Path to input MCDigits");
  declareProperty("OutputLocation" ,    m_outputLocation    = LHCb::FTClusterLocation::Default, "Path to output Clusters");
  declareProperty("ADCThreshold" ,      m_adcThreshold      = 1 , "Minimal ADC Count to be added in cluster");
  declareProperty("ClusterMinWidth" ,   m_clusterMinWidth   = 2 , "Minimal allowed width for clusters");
  declareProperty("ClusterMaxWidth" ,   m_clusterMaxWidth   = 9 , "Maximal allowed width for clusters");
  declareProperty("ClusterMinCharge" ,  m_clusterMinCharge  = 9 , "Minimal charge to keep cluster ~4 p.e.");
  declareProperty("ClusterMinADCPeak" , m_clusterMinADCPeak = 6 , "Minimal ADC for cluster peaks, ~2.5 pe.");


  m_nberOfLostHitFromMap = 0;
  m_nberOfKeptHitFromMap = 0;
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
  
  m_nberOfLostHitFromMap = 0;
  m_nberOfKeptHitFromMap = 0;
  m_evtNbCluster64 = 0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FTClusterCreator::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "[FTClusterCreator] ==> Execute NEW EVENT" << endmsg;

  
  // retrieve FTDigits
  const MCFTDigits* mcDigitsCont = get<MCFTDigits>(m_inputLocation);
  if ( msgLevel(MSG::DEBUG) )
    debug() <<"mcDigitsCont->size() : " << mcDigitsCont->size()<< endmsg;


  // define clusters container
  FTClusters *clusterCont = new FTClusters();
  clusterCont->reserve(mcDigitsCont->size());


  // Register FTClusters in the transient data store
  put(clusterCont, m_outputLocation);


  // Create a link between the FTCluster and the MCParticle which leave a track
  LinkerWithKey<LHCb::MCParticle,LHCb::FTCluster> mcToClusterLink( evtSvc(), msgSvc(), LHCb::FTClusterLocation::Default);


  // DEBUG : print Digit content : should be sorted
  if(msgLevel(MSG::DEBUG)){
    for (MCFTDigits::const_iterator iterDigit = mcDigitsCont->begin(); iterDigit!=mcDigitsCont->end();++iterDigit){
      MCFTDigit* mcDigit = *iterDigit;
      //      std::map< const LHCb::MCHit*, double> hitMap = mcDigit->mcHitMap();
      debug() <<"Channel ="<<mcDigit->channelID()<< " : " <<"\t ADC ="<<mcDigit->adcCount() << endmsg;
      // print deposit content
      const MCFTDeposit* mcDeposit = mcDigit->deposit();
      if (mcDeposit != 0) {
        for (unsigned int idx = 0; idx<mcDeposit->mcHitVec().size(); ++idx) {
          debug() <<"MCParticle : index="<< mcDeposit->mcHitVec()[idx]->mcParticle()->index() <<" "<<mcDeposit->mcHitVec()[idx]->mcParticle()->particleID()
                  << " Energy=" << mcDeposit->energyVec()[idx] << " Time=" << mcDeposit->timeVec()[idx] 
                  << " EnergyRef=" <<mcDeposit->energyRefVec()[idx] << " TimeRef=" << mcDeposit->timeRefVec()[idx] << endmsg;
          }
        } else {
          debug() << "MCParticle : no mcDeposit found: noise channel?" << endmsg;
        }
      }
    }


  // Create map of involved hits, all of them associated to false (not yet in clusters)
  std::map<const LHCb::MCHit*, bool> hitBoolMap;
  for (MCFTDigits::const_iterator iterDigit = mcDigitsCont->begin(); iterDigit!=mcDigitsCont->end();++iterDigit){
    MCFTDigit* mcDigit = *iterDigit;
    const MCFTDeposit* mcDeposit = mcDigit->deposit();
    if (mcDeposit != 0) {
      for (unsigned int idx = 0; idx<mcDeposit->mcHitVec().size(); ++idx) {
        hitBoolMap[ mcDeposit->mcHitVec()[idx] ]=false;
      }
    } else {
      //debug() << "[hitBoolMap] no mcDeposit for hit found! (noise hit?). No hitmap added." << endmsg;
    }
  }

  // Since Digit Container is sorted wrt channelID, clusters are defined searching for bumps of ADC Count
  MCFTDigits::const_iterator seedDigitIter = mcDigitsCont->begin();

  int ClusterPerSiPMCounter = 0;
  unsigned int ReferenceSiPMID = 0;
  unsigned int LastSiPMID = 0;
  
  while(seedDigitIter != mcDigitsCont->end()){ // loop over digits
    MCFTDigit* seedDigit = *seedDigitIter;

    if(seedDigit->adcCount() > m_adcThreshold){ // ADC count in  digit is over threshold : start cluster
      bool hasSeed = seedDigit->adcCount() >= m_clusterMinADCPeak;
      // vector of ADC count of each cell from the cluster
      std::vector<int> clusterADCDistribution;
      std::vector<const LHCb::MCHit*> clusterHitDistribution;

      // total energy from MC
      double totalEnergyFromMC = 0;

      // map of contributing MCParticles with their relative energy deposit
      std::map< const LHCb::MCParticle*, double> mcContributionMap;
      std::map< const LHCb::MCHit*, double> mcHitContributionMap; // Jacco



 

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
        if ( lastDigit->adcCount() >= m_clusterMinADCPeak ) hasSeed = true;
    
	      const MCFTDeposit* mcDeposit = lastDigit->deposit();
        if( mcDeposit != 0 )
          NbOfHitInvolvedInCluster += mcDeposit->mcHitVec().size();


        // Keep track of MCParticles involved in the cluster definition
        clusterADCDistribution.push_back(lastDigit->adcCount());
        if( mcDeposit != 0 ) { 
          for (unsigned int idx = 0; idx<mcDeposit->mcHitVec().size(); ++idx) {
            totalEnergyFromMC += mcDeposit->energyVec()[idx]; // only direct pulse energy is stored!!! TO BE UPDATED
            mcContributionMap[mcDeposit->mcHitVec()[idx]->mcParticle()] += mcDeposit->energyVec()[idx];// only direct pulse energy is stored!!! TO BE UPDATED
            mcHitContributionMap[mcDeposit->mcHitVec()[idx]] += mcDeposit->energyVec()[idx]; //Jacco
            clusterHitDistribution.push_back(mcDeposit->mcHitVec()[idx]);
          }
        }
        ++lastDigitIter;

      }while((lastDigitIter != mcDigitsCont->end()) 
             && keepAdding(lastDigitIter)
             && (clusterADCDistribution.size()<=m_clusterMaxWidth));
      

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
                  << " meanposition="<<meanPosition
                  << endmsg;
        }
      }
      if(msgLevel(MSG::DEBUG)){
        debug() <<"(*seedDigitIter)->channelID()="<< (*seedDigitIter)->channelID()
                <<" meanPosition="<<meanPosition << " totalCharge=" << totalCharge
                << endmsg;
      }
      if( 0 < totalCharge) 
        meanPosition =(*seedDigitIter)->channelID() + meanPosition/totalCharge;
      if(msgLevel(MSG::DEBUG)){
          debug() << format("==> Final meanPosition==%10.2f; floor=%10.2f; fractionnalPart=%2.5f",meanPosition,
                            std::floor(meanPosition),
                            ( meanPosition-std::floor(meanPosition)) )
                  << endmsg;
      }
      

      // Before Cluster record, checks :
      // - total ADC charge of the cluster is over threshold and
      // - A seed has been found and
      // - number of cells above the minimal allowed value      
      if( ( totalCharge >= m_clusterMinCharge) &&
          hasSeed &&
          (clusterADCDistribution.size() >= m_clusterMinWidth) ) {

        if(ReferenceSiPMID == 0 ) ReferenceSiPMID = (*seedDigitIter)->channelID().sipmId();

        LastSiPMID = (*seedDigitIter)->channelID().sipmId();

        // Check number of cluster per SiPM
        if ( msgLevel( MSG::DEBUG) ) {
          debug() << "ClusterPerSiPMCounter Checks : ReferenceSiPMID = " << ReferenceSiPMID
                  << "  LastSiPMID = " <<LastSiPMID<< endmsg;
        }

        if (ReferenceSiPMID == LastSiPMID) ++ClusterPerSiPMCounter;
        else {
          plot(ClusterPerSiPMCounter, "ClusterPerSiPM", "Cluster per SiPM distribution; Nb of clusters" , 
               0. , 50., 50);
          if ( msgLevel( MSG::DEBUG) ) {
            if(ClusterPerSiPMCounter == 0) debug() << "|||| Event with ClusterPerSiPMCounter == 0 : Event ="
                                                   << endmsg;
          }
         
          ReferenceSiPMID = LastSiPMID;
          ClusterPerSiPMCounter = 1;
        }
        if ( msgLevel( MSG::DEBUG) ) {
          debug() << "ClusterPerSiPMCounter = " << ClusterPerSiPMCounter<< endmsg;
        }
        for(std::vector<const LHCb::MCHit*>::const_iterator hitIter = clusterHitDistribution.begin(); 
            hitIter != clusterHitDistribution.end(); ++hitIter){
          hitBoolMap[*hitIter]=true;     
        }

        // Define Cluster(channelID, fraction, width, charge)  and save it
        uint meanChanPosition = std::floor(meanPosition);
        double fractionChanPosition = (meanPosition-std::floor(meanPosition));

        // Define Cluster(channelID, fraction, width, charge)  and save it
        if ( msgLevel( MSG::DEBUG) ) {
          debug() <<"+ Define new cluster with meanChanPosition="<< meanChanPosition
                  <<" fractionChanPosition="<<fractionChanPosition <<" width="<<(lastDigitIter-seedDigitIter)
                  <<" totalCharge=" << totalCharge << endmsg;
        }
        FTCluster *newCluster = new FTCluster(meanChanPosition,fractionChanPosition,(lastDigitIter-seedDigitIter),totalCharge);

        // draw cluster channelID
        plot(newCluster->channelID().sipmId(), "ClusSiPMID", "Cluster SiPMID; Cluster SiPMID" , 
             0. , 96. ,96);
        plot(newCluster->channelID(), "ClusChannelID", "Cluster ChannelID; Cluster ChannelID" , 
             0. , 589824. ,4608);
        // draw cluster fractional part
        plot(newCluster->fraction(), "ClusFraction", 
             "Cluster Fraction Part Distribution;Cluster Fractional part; Nber of events" , 0 , 1);
        // draw cluster width
        plot(newCluster->size(),"ClusSize","Cluster Size Distribution;Cluster Size;Nber of events" , 0. , 70., 70);
        plot(newCluster->size(),"ClusSizeZOOM","Cluster Size Distribution;Cluster Size;Nber of events" , 0. , 10., 10);
        // draw cluster total adc charge
        plot(newCluster->charge(),"ClusCharge","Cluster Charge Distribution;Cluster Charge;Nber of events" , 0 , 100);
        plot(newCluster->charge(),"ClusChargeZOOM","Cluster Charge Distribution;Cluster Charge;Nber of events" , 
             0. , 50., 50);
        plot2D(newCluster->size(), newCluster->charge(), "ClusChargevsSize",
               "Cluster charge vs. size;Cluster size [Nb of Channels];Cluster Charge [ADC counts]",
               0. , 16. , 0. , 100.,16, 100);
        counter("ClusterAccumulatedCharge") += newCluster->charge();
        counter("ClusterSize") += newCluster->size();
        if (newCluster->size() <9) counter("ClusterSizeUpto8")++;
        else counter("ClusterSizeAbove8")++;
        ++m_nCluster;
        m_sumCharge += totalCharge;

        if ( msgLevel( MSG::DEBUG) ) {
          if (newCluster->size() ==64 ){
            debug() << "|||| Event with newCluster->size() ==64 : Event ="
                    << endmsg;
            std::stringstream plotName;
            plotName << "Cluster64_" << m_evtNbCluster64;//(*seedDigitIter)->channelID();
            m_evtNbCluster64++;
            int sipmChannel = 0;
            
            for(std::vector<int>::const_iterator i = clusterADCDistribution.begin();i != clusterADCDistribution.end(); ++i){
              plot(sipmChannel,plotName.str().c_str(),"Cluster with size = 64;Cluster channels;ADC Count" , 0. , 70., 70, *i);
              sipmChannel++;
               
            }
             
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


        clusterCont->insert(newCluster);


        // Define second member of mcContributionMap as the fraction of energy corresponding to each involved MCParticle
        for(std::map<const LHCb::MCParticle*,double>::iterator i = mcContributionMap.begin(); i != mcContributionMap.end(); ++i){
          mcToClusterLink.link(newCluster, (i->first), (i->second)/totalEnergyFromMC ) ;
          //           if ( msgLevel( MSG::DEBUG) ) {
          //             debug() << "Linked ClusterChannel=" << newCluster->channelID()
          //                     << " to MCIndex="<<i->first->index()
          //                     << " with EnergyFraction=" << (i->second)/totalEnergyFromMC
          //                     << endmsg;
          //           }
        }

        //[Jacco]
        if( mcHitContributionMap.size() > 0 ) {
          double largestDeposit = 0;
          const LHCb::MCHit* largestHit = 0; 
          for(std::map<const LHCb::MCHit*,double>::iterator i = mcHitContributionMap.begin(); i != mcHitContributionMap.end(); ++i) {
            if(largestDeposit < (i->second)) {
              largestDeposit = (i->second);
              largestHit = (i->first);
            }
          }
          plot( largestHit -> midPoint().x(), "Cluster_x_position", "Cluster x position (from MCHit) ; x [mm] ; Number of clusters",-3500,3500,200 ); 
          plot( largestHit -> midPoint().y(), "Cluster_y_position", "Cluster y position (from MCHit) ; y [mm] ; Number of clusters",-3000,3000,200 ); 
          plot2D( largestHit -> midPoint().x(), largestHit -> midPoint().y(), 
              "Cluster_xy_position", "Cluster xy position ; x [mm]; y[mm]",
              -3500,3500,200, -3000,3000,200);
        }
        //[/Jacco]


      }
      seedDigitIter = (lastDigitIter-1);
    } // END if(seedDigit->adcCount() > m_adcThreshold)
    ++seedDigitIter;
  }

  for(std::map<const LHCb::MCHit*, bool>::const_iterator hitboolMapiter = hitBoolMap.begin(); hitboolMapiter != hitBoolMap.end();
      ++hitboolMapiter){
    if(hitboolMapiter->second == true){
      ++m_nberOfKeptHitFromMap;
      counter("NberOfKeptHitFromMap")++;
      plot2D( hitboolMapiter->first->entry().x(), hitboolMapiter->first->entry().y(), 
              "KeptHitEntryPosition","Entry position of Hits kept by Clusterisation ; x [mm]; y [mm]",
              -500., 500., -500.,500., 100, 100);  
      plot(hitboolMapiter->first->energy(),"KeptHitEnergy", 
           "Energy of Hits kept by Clusterisation; Energy [MeV];Number of hits" , 0 , 1 );
      plot(hitboolMapiter->first->mcParticle()->particleID().pid(),"KeptHitPDGId", 
           "PDGId of Hits kept by Clusterisation; Energy [MeV];Number of hits" , 0 , 100 );
    }
    else{
      ++m_nberOfLostHitFromMap;
      counter("NberOfLostHitFromMap")++;
      plot2D( hitboolMapiter->first->entry().x(), hitboolMapiter->first->entry().y(), 
              "LostHitEntryPosition","Entry position of Hits lost in Clusterisation; x [mm]; y [mm]",
              -500., 500., -500.,500., 100, 100);  
      plot(hitboolMapiter->first->energy(),"LostHitEnergy", 
           "Energy of Hits lost in Clusterisation; Energy [MeV];Number of hits" , 0 , 1 );
      plot(hitboolMapiter->first->mcParticle()->particleID().pid(),"LostHitPDGId", 
           "PDGId of Hits lost in Clusterisation; Energy [MeV];Number of hits" , 0 , 100 );
    }
    
    
    
  }
  

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FTClusterCreator::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  info() << "*** Average cluster charge = " << m_sumCharge / m_nCluster << endmsg;

  info() << "*** Lost Hits = " <<m_nberOfLostHitFromMap << "\t Kept Hits = "<< m_nberOfKeptHitFromMap
         <<"\t Sum="<< (m_nberOfKeptHitFromMap + m_nberOfLostHitFromMap)
         << "\t Eff = "<< (double)m_nberOfKeptHitFromMap/(m_nberOfKeptHitFromMap + m_nberOfLostHitFromMap)<< endmsg;
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




