// Include files
//#include <iterator>
#include <sstream>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from Linker
#include "Linker/LinkerWithKey.h"

// FTDet
#include "FTDet/DeFTDetector.h"
#include "FTDet/DeFTLayer.h"

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
  declareProperty("DynamicsLow" ,       m_dynamicsLowLimit = 0. , "Lower limit of digitisation");
  declareProperty("DynamicsUp" ,        m_dynamicsUpLimit = 32. , "Saturation limit for digitisation");
  declareProperty("DynamicsBits" ,      m_dynamicsBitsNber = 2 , "Number of available bits for digitisation");

  declareProperty("ADCThreshold" ,      m_adcThreshold      = 3 , "Minimal ADC Count to be added in cluster");
  declareProperty("ClusterMinWidth" ,   m_clusterMinWidth   = 1 , "Minimal allowed width for clusters");
  declareProperty("ClusterMaxWidth" ,   m_clusterMaxWidth   = 4 , "Maximal allowed width for clusters");
  declareProperty("ClusterMinCharge" ,  m_clusterMinCharge  = 8 , "Minimal charge to keep cluster ~4 p.e.");
  declareProperty("ClusterMinADCPeak" , m_clusterMinADCPeak = 5 , "Minimal ADC for cluster peaks, ~2.5 pe.");
  declareProperty("SplitPrevNextMoni" , m_splitPrevNextMoni = false ,
                  "In case of spillover, make split Prev/Next plots");
  declareProperty("RemoveITRegion"    , m_removeITRegion    = 0 , 
                  "0 = keep all clusters, 1 = remove IT region, 2 = keep only IT region");
  declareProperty("ITScale"           , m_ITScale           = 1 , 
                  "Scale of IT to be removed / kept (only relevant if RemoveITRegion)");

  m_thresholdWeightMap.clear();
  

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
    debug() << ": m_adcThreshold is " <<m_adcThreshold << endmsg;
  }

  int StepPE = std::pow(2,m_dynamicsBitsNber); // (6-bit)
  if(m_dynamicsBitsNber != 2 )
    for(int i = 0; i<StepPE; ++i) m_thresholdWeightMap[(double)i]=(double)i/2.;
  else
  {
    m_thresholdWeightMap[0]=0;
    m_thresholdWeightMap[3]=m_adcThreshold;
    m_thresholdWeightMap[5]=m_clusterMinADCPeak;
    m_thresholdWeightMap[8]=m_clusterMinCharge;
  }

  m_nCluster = 0;
  m_sumCharge = 0.;
  
  m_nberOfLostHitFromMap = 0;
  m_nberOfKeptHitFromMap = 0;
  m_evtNbCluster64 = 0;

  if (m_removeITRegion == 1) {
    info() << "Will remove IT Region, with IT scale = " << m_ITScale << endmsg;
  } if (m_removeITRegion == 2) {
    info() << "Will keep only IT Region, with IT scale = " << m_ITScale << endmsg;
  }

  /// Retrieve and initialize DeFT (no test: exception in case of failure)
  m_deFT = getDet<DeFTDetector>( DeFTDetectorLocation::Default );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FTClusterCreator::execute(){

  if ( msgLevel(MSG::DEBUG) ) debug() << "[FTClusterCreator] ==> Execute NEW EVENT" << endmsg;


  
  if ( msgLevel(MSG::DEBUG) ){
    std::map<double, double>::const_iterator thmapittest = m_thresholdWeightMap.begin();
    while((thmapittest !=  m_thresholdWeightMap.end()))
    {
      debug() << "=== TEST1  thmapittest->first= " << thmapittest->first
              << "  thmapittest->second=" << thmapittest->second 
              << endmsg;
      ++thmapittest;
    }
    debug() << "[Clustering_Test] ==> Execute NEW EVENT" << endmsg;
  }
  

  // retrieve FTDigits
  const MCFTDigits* mcDigitsCont = get<MCFTDigits>(m_inputLocation);
  if ( msgLevel(MSG::DEBUG) )
    debug() <<"mcDigitsCont->size() : " << mcDigitsCont->size()<< endmsg;

  // Get the default (signal) spill (to check for spillover cluster origin)
  const MCHits* mcHits = get<MCHits>(LHCb::MCHitLocation::FT);
  const MCHits* mcHitsNext = 0;
  const MCHits* mcHitsPrev = 0;
  if( m_splitPrevNextMoni ) {
    mcHitsNext = getIfExists<MCHits>("Next/"+LHCb::MCHitLocation::FT); //"/Event/Next/MC/FT/Hits"
    mcHitsPrev = getIfExists<MCHits>("Prev/"+LHCb::MCHitLocation::FT);  // "/Event/Prev/MC/FT/Hits"
  } 


  // define clusters container
  FTClusters *clusterCont = new FTClusters();
  clusterCont->reserve(mcDigitsCont->size());


  // Register FTClusters in the transient data store
  put(clusterCont, m_outputLocation);


  // Create a link between the FTCluster and the MCHit which leave a track
  LinkerWithKey<LHCb::MCHit,LHCb::FTCluster> hitToClusterLink( evtSvc(), msgSvc(), (LHCb::FTClusterLocation::Default));


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
          debug() <<"MCParticle : index="
                  << mcDeposit->mcHitVec()[idx]->mcParticle()->index() <<" "
                  <<mcDeposit->mcHitVec()[idx]->mcParticle()->particleID()
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
    } /*else {
      debug() << "[hitBoolMap] no mcDeposit for hit found! (noise hit?). No hitmap added." << endmsg;
      }*/
  }

  // Since Digit Container is sorted wrt channelID, clusters are defined searching for bumps of ADC Count
  MCFTDigits::const_iterator seedDigitIter = mcDigitsCont->begin();

  int ClusterPerSiPMCounter = 0;
  unsigned int ReferenceSiPMID = 0;
  unsigned int LastSiPMID = 0;

  MCFTDigits::const_iterator lastStopDigitIter = mcDigitsCont->begin();

  while(seedDigitIter != mcDigitsCont->end()){ // loop over digits
    MCFTDigit* seedDigit = *seedDigitIter;

    if(seedDigit->adcCount() >= m_clusterMinADCPeak){ // ADC above seed : start clustering
      if ( msgLevel(MSG::DEBUG) ) {
        debug() << "===================================================\n" 
                << "   START NEW CLUSTER WITH SEED @" << seedDigit->channelID()
                << "\n  Address=(" << (uint)seedDigit->channelID() 
                <<")  ADC=" << seedDigit->adcCount()
                << "\n===================================================" << endmsg;
      }
      
      MCFTDigits::const_iterator startDigitIter = seedDigitIter;
      MCFTDigits::const_iterator stopDigitIter = seedDigitIter;

      // vector of ADC count of each cell from the cluster
      //std::vector<int> clusterADCDistribution;
      std::vector<const LHCb::MCHit*> clusterHitDistribution;

      // total energy from MC
      double totalEnergyFromMC = 0;

      // map of contributing MCParticles with their relative energy deposit
      //std::map< const LHCb::MCParticle*, double> mcContributionMap;
      std::map< const LHCb::MCHit*, double> mcHitContributionMap;

      // test neighbours to define starting and ending channels of Cluster
      bool ContinueStartLoop = true;
      bool ContinueStopLoop = true;
      int DebugCheckLooping = 0;
      
      while(((stopDigitIter-startDigitIter)< m_clusterMaxWidth ) // cluster size =< m_clusterMaxWidth  SiPM Channels
            && (ContinueStartLoop || ContinueStopLoop) // continu looping
            && (DebugCheckLooping < 2000)
            )
      {
        ++DebugCheckLooping;
        if ( msgLevel(MSG::DEBUG) ) 
        {
          debug() << "=... LOOP WHILE " << DebugCheckLooping<< endmsg;
          debug() << "=... Check 1:  ContinueStopLoop="<<ContinueStopLoop <<" ContinueStartLoop="<<ContinueStartLoop
                  << " ((stopDigitIter+1) != mcDigitsCont->end())" << ((stopDigitIter+1) != mcDigitsCont->end())
                  << endmsg;
        }
        

        /// define the last channel of the cluster
        if(ContinueStopLoop && ((stopDigitIter+1) != mcDigitsCont->end()) )
        {
          if ( msgLevel(MSG::DEBUG) ){
            debug() << "=...  IF ContinueStopLoop = "<<ContinueStopLoop << endmsg;
          }
          
          MCFTDigit* stopDigit = *(stopDigitIter+1); 
 
          /// should be in the same SiPM, the channel next door, and above adcThreshold
          if((stopDigit->channelID().sipmId() == seedDigit->channelID().sipmId())
             &&(stopDigit->channelID().sipmCell()==((*stopDigitIter)->channelID().sipmCell()+1))
             && (stopDigit->adcCount() >=m_adcThreshold))
          {
            // if ADC of channel next door is in addition above the seedThreshold, redefine the seed
            if(stopDigit->adcCount() > seedDigit->adcCount())
            {
              if ( msgLevel(MSG::DEBUG) ){
                debug() << "=... stop ADC above Seed ADC : redefine seed from "  
                        << seedDigit->channelID();
              }
              
              seedDigitIter = stopDigitIter+1;
              seedDigit = *seedDigitIter;
              if ( msgLevel(MSG::DEBUG) ){
                debug() << " TO "  
                        << seedDigit->channelID()
                        << " ADC = " << seedDigit->adcCount()
                        << endmsg;
              }
              
              startDigitIter = seedDigitIter;
              stopDigitIter = seedDigitIter;
              ContinueStartLoop = true;
              ContinueStartLoop = true;    

              if ( msgLevel(MSG::DEBUG) ){
                debug() << "=... Redefine Start "  << (*startDigitIter)->channelID()
                        << " and stop " << (*stopDigitIter)->channelID()
                        << endmsg; 
              }
              
            }else
            {
              stopDigitIter++;

              if ( msgLevel(MSG::DEBUG) ){
                debug() << "=... Redefine stop @ "  << (*stopDigitIter)->channelID()
                        << " ADC=" << (*stopDigitIter)->adcCount()
                        << endmsg;
              }
              
            }
  
            
          }else{ 
            ContinueStopLoop = false;
            if ( msgLevel(MSG::DEBUG) ){
              debug() << "=...  SET ContinueStopLoop = "<<ContinueStopLoop << endmsg;
            }
            
          }
        }else{      
          ContinueStopLoop = false;
          if ( msgLevel(MSG::DEBUG) ){
            debug() << "=...  SET ContinueStopLoop = "<<ContinueStopLoop 
                    << " { [(stopDigitIter+1) != mcDigitsCont->end()]=" 
                    <<((stopDigitIter+1) != mcDigitsCont->end()) 
                    << "}"<< endmsg;
          }
        
        }
        if ( msgLevel(MSG::DEBUG) ){
          debug() << "=... Check 2:  ContinueStopLoop="<<ContinueStopLoop <<" ContinueStartLoop="<<ContinueStartLoop 
                  << " ((startDigitIter) != mcDigitsCont->begin())" << ((startDigitIter) != mcDigitsCont->begin())
                  << endmsg;
        }
        

        /// define the first channel of the cluster
        if(ContinueStartLoop && ((startDigitIter) != mcDigitsCont->begin()))
        {
          if ( msgLevel(MSG::DEBUG) ){
            debug() << "=...  IF ContinueStartLoop = "<<ContinueStartLoop << endmsg;
          }
          MCFTDigit* startDigit = *(startDigitIter-1);

          /// should be in the same SiPM, the channel next door, above adcThreshold
          /// and after the ending channel of the previous cluster
          if((startDigit->channelID().sipmId() == seedDigit->channelID().sipmId()) 
             &&(startDigit->channelID().sipmCell()==((*startDigitIter)->channelID().sipmCell()-1))
             && (startDigit->adcCount() >=m_adcThreshold)
             &&((startDigitIter-1) > lastStopDigitIter))
          {
            startDigitIter--;
            if ( msgLevel(MSG::DEBUG) ){
              debug() << "=... redefine start @ "  << (*startDigitIter)->channelID()
                      << " ADC=" << (*startDigitIter)->adcCount()
                      << endmsg;
            }
            
          }else{
            ContinueStartLoop = false;
            if ( msgLevel(MSG::DEBUG) ){
              debug() << "=...  SET ContinueStartLoop = "<<ContinueStartLoop << endmsg;
            }
            
          }
          
        }else{      
          ContinueStartLoop = false;
          if ( msgLevel(MSG::DEBUG) ){
            debug() << "=...  SET ContinueStartLoop = "<<ContinueStartLoop 
                    << " { [(startDigitIter) != mcDigitsCont->begin()]=" <<((startDigitIter) != mcDigitsCont->begin()) 
                    << "}"<< endmsg;
          }
          
        }



      };
      // check cluster size < 5 and choose highest sum for cluster
      while((stopDigitIter - startDigitIter) >= m_clusterMaxWidth)
      {
        if ( msgLevel(MSG::DEBUG) ){
          debug() << "=& Cluster size is redefined ... From Start="<<(*startDigitIter)->channelID()
                  << " Stop="<<(*stopDigitIter)->channelID()
                  << endmsg;
        }
        
        if((*stopDigitIter)->adcCount() > (*startDigitIter)->adcCount())   startDigitIter++;
        else stopDigitIter--;
        if ( msgLevel(MSG::DEBUG) ){
          debug() << "To Start="<<(*startDigitIter)->channelID()
                  << " Stop="<<(*stopDigitIter)->channelID()
                  << endmsg;
        }
        
      }

      lastStopDigitIter = stopDigitIter;
      
      // Keep track of MCParticles involved in the cluster definition
      // and define total ADC counts and cluster barycenter position in ChannelID 
      if ( msgLevel(MSG::DEBUG) ){
        debug() << "=========\n=========   COMPUTE ADC SUM AND BARYCENTER \n=========" << endmsg;
      }
      
      unsigned int totalCharge = 0;
      double meanPosition = 0;
      for(MCFTDigits::const_iterator iterDigit = startDigitIter; iterDigit <(stopDigitIter+1); 
          ++iterDigit)
      {
        MCFTDigit* digit = *iterDigit;
        int ChannelWeightREF =  std::min(std::floor((digit->adcCount())*std::pow(2,m_dynamicsBitsNber)/
                                                    (m_dynamicsUpLimit-m_dynamicsLowLimit)),
                                         m_dynamicsUpLimit);
        std::map<double, double>::const_iterator thmapit = m_thresholdWeightMap.begin();
        double ChannelWeight = thmapit->first;
        while((thmapit != m_thresholdWeightMap.end())&&(digit->adcCount() >= thmapit->second))
        {
         
          if(m_dynamicsBitsNber == 6 ) // 6 bit old treatment
            ChannelWeight = (thmapit->first)/2;
          else
            ChannelWeight = (thmapit->first);

          if ( msgLevel(MSG::DEBUG) ){ 
            debug() << "=... Test   ChannelWeight= " << ChannelWeight << " [ChannelWeightREF= " <<ChannelWeightREF
                    << "] (adc=" <<digit->adcCount() <<" >= " << thmapit->second << " criteria)"
                    << endmsg;
          }
          ++thmapit; 
        }
        ChannelWeight =  std::min(ChannelWeight,(double)m_dynamicsUpLimit);
        
        totalCharge += ChannelWeight;
        meanPosition += (iterDigit-startDigitIter) * (ChannelWeight);
        if(msgLevel(MSG::DEBUG)){
          debug() <<"- channel="<<digit->channelID()
                  <<"- Distance="<< (iterDigit-startDigitIter)
                  <<" adc="<<digit->adcCount() <<" ChannelWeight="<<ChannelWeight 
                  << " totalCharge=" << totalCharge 
                  << " meanposition="<<meanPosition
                  << endmsg;
        }

        const MCFTDeposit* mcDeposit = digit->deposit();
        //clusterADCDistribution.push_back(digit->adcCount());
        if( mcDeposit != 0 ) { 
          for (unsigned int idx = 0; idx<mcDeposit->mcHitVec().size(); ++idx) {
            totalEnergyFromMC += mcDeposit->energyVec()[idx] + mcDeposit->energyRefVec()[idx]; 
            //mcContributionMap[mcDeposit->mcHitVec()[idx]->mcParticle()] 
            //  += mcDeposit->energyVec()[idx];// only direct pulse energy is stored!!! TO BE UPDATED
            mcHitContributionMap[mcDeposit->mcHitVec()[idx]] += (mcDeposit->energyVec()[idx] + mcDeposit->energyRefVec()[idx]); 
            clusterHitDistribution.push_back(mcDeposit->mcHitVec()[idx]);
          }
        }
      }
      
      // vector of ADC count of each cell from the cluster
      //std::vector<int> clusterADCDistribution;
      //clusterADCDistribution.push_back(seedDigit->adcCount());



      if(msgLevel(MSG::DEBUG)){
        debug() <<"(*startDigitIter)->channelID()="<< (*startDigitIter)->channelID()
                <<" meanPosition="<<meanPosition << " totalCharge=" << totalCharge
                << endmsg;
      }
      if( 0 < totalCharge) 
        meanPosition =(*startDigitIter)->channelID() + meanPosition/totalCharge;
      if(msgLevel(MSG::DEBUG)){
        debug() << format("==> Final meanPosition==%10.2f; floor=%10.2f; fractionnalPart=%2.5f",
                          meanPosition,std::floor(meanPosition),(meanPosition-std::floor(meanPosition)))
                << endmsg;
      }


      // Before Cluster record, checks :
      // - total ADC charge of the cluster is over threshold and
      // - A seed has been found and
      // - number of cells above the minimal allowed value
      if( ( totalCharge >= m_clusterMinCharge) &&
          ((stopDigitIter-startDigitIter+1) >= m_clusterMinWidth) ){

        if(ReferenceSiPMID == 0 ) ReferenceSiPMID = (*startDigitIter)->channelID().sipmId();

        LastSiPMID = (*stopDigitIter)->channelID().sipmId();

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
        if(fractionChanPosition > .5) // keep fractional part within [-.5,.5] range
        {
          meanChanPosition += 1;
          fractionChanPosition -= 1.;
        }
        
        
        // Define Cluster(channelID, fraction, width, charge)  and save it
        if ( msgLevel( MSG::DEBUG) ) {
          debug() <<"===>> Record new cluster with meanChanPosition="<< meanChanPosition
                  <<" fractionChanPosition="<<fractionChanPosition <<" width="<<(stopDigitIter-startDigitIter+1)
                  <<" totalCharge=" << totalCharge << endmsg;
        }
        FTCluster *newCluster = new FTCluster(meanChanPosition,fractionChanPosition,(stopDigitIter-startDigitIter+1),totalCharge);

        // draw cluster channelID
        plot(newCluster->channelID().sipmCell(), "ClusSiPMCell", "Cluster SiPM Cell; Cluster SiPM Cell" , 
             0. , 130. ,130);
        plot(newCluster->channelID().sipmId(), "ClusSiPMID", "Cluster SiPMID; Cluster SiPMID" , 
             0. , 96. ,96);
        plot(newCluster->channelID().quarter(), "ClusQuarter", "Cluster Quarter; Cluster Quarter" , 
             0. , 4. ,4);
        plot(newCluster->channelID().layer(), "ClusLayer", "Cluster Layer; Cluster layer" , 
             0. , 13. ,13);
        plot(newCluster->channelID(), "ClusChannelID", "Cluster ChannelID; Cluster ChannelID" , 
             0. , 589824. ,4608);
        // draw cluster fractional part
        plot(newCluster->fraction(), "ClusFraction", 
             "Cluster Fraction Part Distribution;Cluster Fractional part; Nber of events" , -.5, .5 , 100);
        plot(newCluster->fraction()*250., "ClusFractionPosition", 
             "Cluster Fraction position Distribution;Cluster Fractional position; Nber of events" , -1000.,1000.,100);
        plot(newCluster->fraction()*250., "ClusFractionPositionZOOM", 
             "Cluster Fraction position Distribution;Cluster Fractional position; Nber of events" ,-150.,150.,300 );
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
        if (newCluster->size() == 1 ) plot(newCluster->charge(),"Clus_Size1_Charge",
                                           "Cluster Charge Distribution (cluster size = 1);Cluster Charge;Nber of events", 
                                           0 , 100);
        else if (newCluster->size() == 2 ) plot(newCluster->charge(),"Clus_Size2_Charge",
                                                "Cluster Charge Distribution (cluster size = 2) ;Cluster Charge;Nber of events", 
                                                0 , 100);
        else if (newCluster->size() == 3 ) plot(newCluster->charge(),"Clus_Size3_Charge",
                                                "Cluster Charge Distribution (cluster size = 3) ;Cluster Charge;Nber of events", 
                                                0 , 100);
        else if (newCluster->size() == 4 ) plot(newCluster->charge(),"Clus_Size4_Charge",
                                                "Cluster Charge Distribution (cluster size = 4) ;Cluster Charge;Nber of events", 
                                                0 , 100);
        else if (newCluster->size() == 5 ) plot(newCluster->charge(),"Clus_Size5_Charge",
                                                "Cluster Charge Distribution (cluster size = 5) ;Cluster Charge;Nber of events",
                                                0 , 100);
        else if (newCluster->size() == 6 ) plot(newCluster->charge(),"Clus_Size6_Charge",
                                                "Cluster Charge Distribution (cluster size = 6) ;Cluster Charge;Nber of events",
                                                0 , 100);
        else if (newCluster->size() == 7 ) plot(newCluster->charge(),"Clus_Size7_Charge",
                                                "Cluster Charge Distribution (cluster size = 7) ;Cluster Charge;Nber of events",
                                                0 , 100);
        else if (newCluster->size() == 8 ) plot(newCluster->charge(),"Clus_Size8_Charge",
                                                "Cluster Charge Distribution (cluster size = 8) ;Cluster Charge;Nber of events",
                                                0 , 100);
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
            
            for(MCFTDigits::const_iterator iterDigit = startDigitIter; iterDigit <(stopDigitIter+1); ++iterDigit){
              plot(sipmChannel,plotName.str().c_str(),
                   "Cluster with size = 64;Cluster channels;ADC Count" , 
                   0. , 70., 70, (*iterDigit)->adcCount());
              sipmChannel++;   
            }  
          } 
        }
         
        // DEBUG
        if ( msgLevel( MSG::DEBUG) ) {
          debug() <<"=... Finish clustering which start from : "<<(*startDigitIter)->channelID()
                  <<" (ADC = "<<(*startDigitIter)->adcCount() <<" ) to "<<(*(stopDigitIter))->channelID()
                  <<" (ADC = "<<(*(stopDigitIter))->adcCount() <<")"<< endmsg;

          debug() <<"=== Cluster is : "<<newCluster->channelID()
                  <<" Charge = "<<newCluster->charge()
                  << " Width = " << newCluster->size()
                  << " Fraction = " <<newCluster->fraction()
                  << endmsg;
        }


 

        // Get largest MCHit object contributing to cluster
        if( mcHitContributionMap.size() > 0 ) {
          // NOT a noise cluster
          plot( (mcHitContributionMap.size()) , "Cluster_NberOfHits", 
                "Number of Hits in Cluster ; Number of Hits ; Number of clusters",0.,50.,50); 

          if ( msgLevel(MSG::DEBUG) ){
            debug() << "===   COMPUTE RESOLUTION " << endmsg;
          }
          
          double largestDeposit = -999;
          const LHCb::MCHit* largestHit = 0; 
          for(std::map<const LHCb::MCHit*,double>::iterator i = mcHitContributionMap.begin(); 
              i != mcHitContributionMap.end(); ++i) {

            if ( msgLevel(MSG::DEBUG) ){
              const DeFTFibreMat* pL = m_deFT->findFibreMat((i->first)->midPoint());
              if ( pL) {
                std::pair<LHCb::FTChannelID, double> meanCha;
                std::vector< std::pair<LHCb::FTChannelID, double> > meanChaTest;              
                if( pL->calculateMeanChannel((i->first), meanCha)){
                  debug() << "|calculateMeanChannel| Hit->midPoint.x=" << (i->first)->midPoint().x()
                          << "  converted in channelID="<< meanCha.first << " + " << meanCha.second
                          << "  Deposit=" << (i->second) << endmsg;
                }

                if( pL->calculateListOfFiredChannels( (i->first), meanChaTest)){
                  
                  debug() << "|calculateListOfFiredChannels| Hit->midPoint.x=" << (i->first)->midPoint().x()
                          << "  Deposit=" 
                          << (i->second)<< endmsg;
                  for(std::vector< std::pair<LHCb::FTChannelID,double> >::const_iterator testiter=meanChaTest.begin();
                      testiter!= meanChaTest.end();++testiter)
                  { 
                    debug() << "...  converted in channelID="<< testiter->first << " + " << testiter->second
                            << endmsg;
                  } 
                }
              }
            }
            
       
            if(largestDeposit < (i->second)) {
              largestDeposit = (i->second);
              largestHit = (i->first);
            }
          }

          
          bool acceptCluster = 1;
          if( m_removeITRegion != 0 ) {
            const MCHit* cHit = largestHit;  
            float hitX = cHit -> midPoint().x();
            float hitY = cHit -> midPoint().y();
            // Check if in IT region
            float ITXmin = 99.  , ITYmin = 99. ;
            float ITXcen = 264.5, ITXmax = 621.;
            float ITYcen = 109. , ITYmax = 207.;
            float ITXcenp = ITXcen * sqrt(m_ITScale);
            float ITXmaxp = ITXmin + (ITXmax-ITXmin) * m_ITScale;
            float ITYmaxp = ITYmin + (ITYmax-ITYmin) * sqrt(m_ITScale);
            bool hitInIT = ( ( hitX > -ITXmaxp && hitX < -ITXmin  && hitY > -ITYcen  && hitY < ITYcen ) || //L
                             ( hitX >  ITXmin  && hitX <  ITXmaxp && hitY > -ITYcen  && hitY < ITYcen ) || //R
                             ( hitX > -ITXcenp && hitX <  ITXcenp && hitY >  ITYmin  && hitY < ITYmaxp) || //T
                             ( hitX > -ITXcenp && hitX <  ITXcenp && hitY > -ITYmaxp && hitY < -ITYmin) || //B
                             ( hitX > -ITXmin  && hitX <  ITXmin  && hitY > -ITYmin  && hitY < ITYmin ) ); //C
            float sqX = (536+2*2)/2., sqY = 99. + 50. * m_ITScale ; 
            bool hitInSq = ( hitX > -sqX && hitX < sqX && hitY > -sqY && hitY < sqY );  
            if ( ( m_removeITRegion == 1 &&  hitInIT ) ||
                 ( m_removeITRegion == 2 && !hitInIT ) ||
                 ( m_removeITRegion == 3 &&  hitInSq ) ) {
              // if this hit is (or isn't) in the IT region
              acceptCluster = 0;
              plot2D( largestHit->midPoint().x(), largestHit->midPoint().y(),
                      "MCCluster_rejected","Rejected MCClusters; x [mm]; y [mm]",
                      -3000, 3000, -2500, 2500, 200, 200 );
            }
          } // end of if removeITRegion

          if ( acceptCluster ) {
            clusterCont -> insert(newCluster);

            // Cluster X-position resolution projected along the fiber direction (according to the angle X-U-V) 
            // on the SiPM horizontal plane
            //if ( msgLevel(MSG::DEBUG) ) 
 
            const DeFTFibreMat* pL = m_deFT->findFibreMat(largestHit->midPoint());//[Eric Test]

            //if ( pL  && ((newCluster->channelID().sipmCell()) > 3) && ((newCluster->channelID().sipmCell())<125)) {
            if ( pL) {
              std::pair<LHCb::FTChannelID, double> hitMeanChannel;
              std::vector < std::pair<LHCb::FTChannelID, double> > meanChannels;
 
              if( pL->calculateMeanChannel(largestHit, hitMeanChannel)){
 
                if ( msgLevel(MSG::DEBUG) ){
                  debug() << "===*** largestHit->midPoint().x()= " << largestHit->midPoint().x()
                          << "\t converted in channelID="<< hitMeanChannel.first << " + " << hitMeanChannel.second
                          << "\t compared to Reco Cluster ID="<< newCluster->channelID() << " + " << newCluster->fraction()
                          << endmsg;
                }
              

                double dXCluster = (((double)hitMeanChannel.first-(double)newCluster->channelID()) + 
                                    (hitMeanChannel.second - newCluster->fraction()))*250.; // in micrometer
                if ( msgLevel(MSG::DEBUG) ){
                  debug() << "===*** PLOT hitMeanChannel= " 
                          << hitMeanChannel.first <<" ("<<(double)hitMeanChannel.first <<") - "<<  hitMeanChannel.second
                          << "\t newCluster="<<newCluster->channelID() <<" ("<<(int)newCluster->channelID() <<") - "
                          << newCluster->fraction()<< "\t dXCluster="<< dXCluster   << endmsg;
                }
              
                plot2D(hitMeanChannel.first.layer(),hitMeanChannel.first.sipmId(), "Cluster_MCHit_layer_VS_SiPMID", 
                       "Cluster  MCHit Layer; Cluster MCHit Layer ; Cluster MCHit SiPMID",0.,20.,0.,128.,20,128); 

                plot2D(hitMeanChannel.first.layer(),hitMeanChannel.first.sipmCell(), "Cluster_MCHit_layer_VS_SiPMCell", 
                       "Cluster  MCHit Layer; Cluster MCHit Layer ; Cluster MCHit SiPMCell",0.,20.,0.,128.,20,128); 
                plot(hitMeanChannel.first.layer(), "Cluster_MCHit_layer", 
                     "Cluster  MCHit Layer; Cluster MCHit Layer ; Number of clusters",0.,20.,20); 
                plot(dXCluster, "Cluster_x_position_resolution", 
                     "Cluster x position (from MCHit) ; Cluster position resolution [#mum] ; Number of clusters",-1000,1000,100); 
                plot(dXCluster, "Cluster_x_position_resolutionZOOM", 
                     "Cluster x position (from MCHit) ; Cluster position resolution [#mum] ; Number of clusters",-200,200,400); 
                plot2D(dXCluster, newCluster->fraction(),"Cluster_resolutionVSFraction", 
                       "Cluster x position (from MCHit) VS Fraction; Cluster position resolution [#mum] ; Fraction",
                       -60000., 60000., -.5, .5 ,400, 100); 
                plot2D(dXCluster, newCluster->channelID().layer(),"Cluster_resolutionVSLayer", 
                       "Cluster x position (from MCHit) VS Layer ; Cluster position resolution [#mum] ; Lyer",
                       -60000., 60000., 0., 12. ,400, 12); 
                plot2D(dXCluster, hitMeanChannel.first.layer(),"Cluster_resolutionVSHitLayer", 
                       "Cluster x position (from MCHit) VS Layer ; Cluster position resolution [#mum] ; Hit Layer",
                       -60000., 60000., 0., 16. ,400, 16); 
                plot2D(dXCluster, newCluster->channelID().sipmCell(),"Cluster_resolutionVSCell", 
                       "Cluster x position (from MCHit) VS Cell ; Cluster position resolution [#mum] ; SiPM Cell",
                       -60000., 60000., 0., 128., 400, 128); 
                plot2D(dXCluster, newCluster->channelID(),"Cluster_resolutionVSChannelID", 
                       "Cluster x position (from MCHit) VS ChannelID ; Cluster position resolution [#mum] ; SiPM ChannelID",
                       -60000., 60000., 0., 589824., 100, 4068); 
                plot2D(dXCluster, newCluster->fraction(),"Cluster_resolutionVSFractionZOOM", 
                       "Cluster x position (from MCHit) VS Fraction; Cluster position resolution [#mum] ; Fraction",
                       -1000., 1000., -.5, .5 , 100, 100); 
                plot2D(dXCluster, newCluster->channelID().layer(),"Cluster_resolutionVSLayerZOOM", 
                       "Cluster x position (from MCHit) VS Layer ; Cluster position resolution [#mum] ; Lyer",
                       -1000., 1000., 0., 12., 100, 12); 
                plot2D(dXCluster, newCluster->channelID().sipmCell(),"Cluster_resolutionVSCellZOOM", 
                       "Cluster x position (from MCHit) VS Cell ; Cluster position resolution [#mum] ; SiPM Cell",
                       -1000., 1000., 0., 128. ,100, 128); 
                plot2D(dXCluster, newCluster->channelID(),"Cluster_resolutionVSChannelIDZOOM", 
                       "Cluster x position (from MCHit) VS ChannelID ; Cluster position resolution [#mum] ; SiPM ChannelID",
                       -1000., 1000., 0., 589824. ,100, 4068); 

                if (newCluster->size() == 1 ) 
                {
                
                  plot(dXCluster, "Cluster_x_position_resolutionZOOM_1Ch",
"Cluster x position (from MCHit, 1 channel); Cluster (1 channel) position resolution [#mum];Number of clusters"
                       ,-200., 200., 400);
                  plot(dXCluster, "Cluster_x_position_resolution_1Ch",
"Cluster x position (from MCHit, 1 channel); Cluster (1 channel) position resolution [#mum];Number of clusters"
                       ,-1000., 1000., 100);
                  plot2D(dXCluster,  newCluster->channelID().sipmCell(),
                         "Resol_vs_Cell_1CH", "resol vs Cell ;Cluster (1 channel) position resolution [#mum]; sipmCell",
                         -1000., 1000., 0., 134., 100, 134);

                  if(std::abs(dXCluster) > 200)
                  {
                    plot2D(newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
                           "SiPM_vs_Layer_1CH_up200", "SiPM vs Layer (1CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                           0., 134.,0., 16., 134,16);
                    if(newCluster->channelID().sipmCell() <10)
                      plot2D(newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
"SiPMbegin_vs_Layer_1CH_up200", "SiPM begin vs Layer (1CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                             0., 10.,0., 16., 10,16);
                    if(newCluster->channelID().sipmCell() >100)
                      plot2D(newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
"SiPMend_vs_Layer_1CH_up200", "SiPM end vs Layer (1CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                             120., 130.,0., 16., 10,16);
                    if((newCluster->channelID().sipmCell() <70)&&(newCluster->channelID().sipmCell() >60))
                      plot2D(newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
"SiPMmiddle_vs_Layer_1CH_up200", "SiPM middle vs Layer (1CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                             60., 70.,0., 16., 10,16);
                  }
                }
                else if (newCluster->size() == 2 ) 
                {
                
                  plot(dXCluster, "Cluster_x_position_resolutionZOOM_2Ch",
"Cluster x position (from MCHit, 2 channels); Cluster (2 channels) position resolution [#mum] ; Number of clusters", 
                       -200., 200., 400);
                  plot(dXCluster, "Cluster_x_position_resolution_2Ch",
"Cluster x position (from MCHit, 2 channels); Cluster (2 channels) position resolution [#mum] ; Number of clusters",
                       -1000., 1000., 100);
                  if(std::abs(dXCluster) > 200)
                  {
                    plot2D(newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
                           "SiPM_vs_Layer_2CH_up200", "SiPM vs Layer (2CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                           0., 134., 0., 16., 134, 16);
                    if(newCluster->channelID().sipmCell() <10)
                      plot2D(newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
"SiPMbegin_vs_Layer_2CH_up200", "SiPM begin vs Layer (2CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                             0., 10., 0., 16., 10, 16);
                    if(newCluster->channelID().sipmCell() >100)
                      plot2D(newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
"SiPMend_vs_Layer_2CH_up200", "SiPM end vs Layer (2CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                             120., 130., 0., 16., 10, 16);
                    if((newCluster->channelID().sipmCell() <70)&&(newCluster->channelID().sipmCell() >60))
                      plot2D(newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
"SiPMmiddle_vs_Layer_2CH_up200", "SiPM middle vs Layer (2CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                             60., 70., 0., 16., 10, 16);
                  }
                }
                else if (newCluster->size() == 3 ) 
                {
                
                  plot(dXCluster, "Cluster_x_position_resolutionZOOM_3Ch",
"Cluster x position (from MCHit, 3 channels); Cluster (3 channels) position resolution [#mum] ; Number of clusters",
                       -200., 200., 400);
                  plot(dXCluster, "Cluster_x_position_resolution_3Ch",
"Cluster x position (from MCHit, 3 channels); Cluster (3 channels) position resolution [#mum] ; Number of clusters",
                       -1000., 1000., 100);
                  if(std::abs(dXCluster) > 200)
                  {
                    plot2D(newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
                             "SiPM_vs_Layer_3CH_up200", "SiPM vs Layer (3CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                           0., 134., 0., 16., 134, 16);
                    if(newCluster->channelID().sipmCell() <10)
                      plot2D(newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
"SiPMbegin_vs_Layer_3CH_up200", "SiPM begin vs Layer (3CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                             0., 10., 0., 16., 10, 16);
                    if(newCluster->channelID().sipmCell() >100)
                      plot2D(newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
"SiPMend_vs_Layer_3CH_up200", "SiPM end vs Layer (3CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                             120., 130., 0., 16., 10, 16);
                    if((newCluster->channelID().sipmCell() <70)&&(newCluster->channelID().sipmCell() >60))
                      plot2D(newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
"SiPMmiddle_vs_Layer_3CH_up200", "SiPM middle vs Layer (3CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                             60., 70., 0., 16., 10, 16);
                  }
                }
                else if (newCluster->size() == 4 ) 
                {
                
                  plot(dXCluster, "Cluster_x_position_resolutionZOOM_4Ch",
"Cluster x position (from MCHit, 4 channels); Cluster (4 channels) position resolution [#mum] ; Number of clusters",
                       -200., 200., 400);
                  plot(dXCluster, "Cluster_x_position_resolution_4Ch",
"Cluster x position (from MCHit, 4 channels); Cluster (4 channels) position resolution [#mum] ; Number of clusters",
                       -1000., 1000., 100);
                  if(std::abs(dXCluster) > 200)
                  {
                    plot2D(  newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
                             "SiPM_vs_Layer_4CH_up200", "SiPM vs Layer (4CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                             0., 134., 0., 16., 134, 16);
                    if(newCluster->channelID().sipmCell() <10)
                      plot2D(newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
"SiPMbegin_vs_Layer_4CH_up200", "SiPM begin vs Layer (4CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                             0., 10., 0., 16., 10, 16);
                    if(newCluster->channelID().sipmCell() >100)
                      plot2D(  newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
"SiPMend_vs_Layer_4CH_up200", "SiPM end vs Layer (4CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                               120., 130., 0., 16., 10, 16);
                    if((newCluster->channelID().sipmCell() <70)&&(newCluster->channelID().sipmCell() >60))
                      plot2D(  newCluster->channelID().sipmCell(), newCluster->channelID().layer(),
"SiPMmiddle_vs_Layer_4CH_up200", "SiPM middle vs Layer (4CH cluster with resol > 200 #mum) ;sipmCell ; layer",
                               60., 70., 0., 16., 10,16);
                  }
                }            



                // Check if the MCHit belongs to signal spill (for PLOTS)
                //info() << largestHit -> parent() -> name() << endmsg;
                if( largestHit -> parent() == mcHits ) {
                  // is no spillover
                  plot(largestHit -> midPoint().x(), "MCCluster_x_position", 
                       "MC Cluster x position (from MCHit) ; x [mm] ; Number of clusters",-3000,3000,200 ); 
                  plot(largestHit -> midPoint().y(), "MCCluster_y_position", 
                       "MC Cluster y position (from MCHit) ; y [mm] ; Number of clusters",-2500,2500,200 ); 
                  plot2D(largestHit -> midPoint().x(), largestHit -> midPoint().y(), 
                          "MCCluster_xy_position", "MC Cluster xy position ; x [mm]; y[mm]",
                          -3000,3000,-2500,2500,200,200);
                  plot(newCluster->size(),"MCClusSize","MC Cluster Size Distribution;Cluster Size;Nber of events" , 0. , 70., 70);
                  plot(newCluster->charge(),"MCClusCharge",
                       "MC Cluster Charge Distribution;Cluster Charge;Nber of events" , 0 , 100);
                  plot(newCluster->channelID().sipmId(), "MCClusSiPMID", "MC Cluster SiPMID; Cluster SiPMID" , 0. , 96. ,96);
                } else {
                  // is spillover
                  plot(largestHit -> midPoint().x(), "SpilloverCluster_x_position",
                       "Spillover Cluster x position (from MCHit) ; x [mm] ; Number of clusters",-3000,3000,200 ); 
                  plot(largestHit -> midPoint().y(), "SpilloverCluster_y_position",
                       "Spillover Cluster y position (from MCHit) ; y [mm] ; Number of clusters",-2500,2500,200 ); 
                  plot2D(largestHit -> midPoint().x(), largestHit -> midPoint().y(), 
                          "SpilloverCluster_xy_position", "Spillover Cluster xy position ; x [mm]; y[mm]",
                          -3000,3000,-2500,2500,200,200);
                  plot(newCluster->size(),"SpilloverClusSize",
                       "Spillover Cluster Size Distribution;Cluster Size;Nber of events" , 0. , 70., 70);
                  plot(newCluster->charge(),"SpilloverClusCharge",
                       "Spillover Cluster Charge Distribution;Cluster Charge;Nber of events" , 0 , 100);
                  plot(newCluster->channelID().sipmId(), "SpilloverClusSiPMID",
                       "Spillover Cluster SiPMID; Cluster SiPMID" , 0. , 96. ,96);

                  // check for Prev or Next
                  if ( m_splitPrevNextMoni ) {
                    if ( mcHitsNext == 0 || mcHitsPrev == 0 ) {
                      info() << "WARNING: SplitPrevNextMoni set to 1 but no spillover locations found!" << endmsg;
                      break;
                    }
                    if ( largestHit -> parent() == mcHitsNext ) {
                      plot(largestHit -> midPoint().x(), "SpillNextCluster_x_position",
                           "SpillNext Cluster x position (from MCHit) ; x [mm] ; Number of clusters",-3000,3000,200 ); 
                      plot(largestHit -> midPoint().y(), "SpillNextCluster_y_position",
                           "SpillNext Cluster y position (from MCHit) ; y [mm] ; Number of clusters",-2500,2500,200 ); 
                      plot2D(largestHit -> midPoint().x(), largestHit -> midPoint().y(), 
                              "SpillNextCluster_xy_position", "SpillNext Cluster xy position ; x [mm]; y[mm]",
                              -3000,3000, -2500,2500,200,200);
                      plot(newCluster->size(),"SpillNextClusSize",
                           "SpillNext Cluster Size Distribution;Cluster Size;Nber of events" , 0. , 70., 70);
                      plot(newCluster->charge(),"SpillNextClusCharge",
                           "SpillNext Cluster Charge Distribution;Cluster Charge;Nber of events" , 0 , 100);
                      plot(newCluster->channelID().sipmId(), "SpillNextClusSiPMID",
                           "SpillNext Cluster SiPMID; Cluster SiPMID" , 0. , 96. ,96);
                    }
                    if ( largestHit -> parent() == mcHitsPrev ) {
                      plot(largestHit -> midPoint().x(), "SpillPrevCluster_x_position",
                           "SpillPrev Cluster x position (from MCHit) ; x [mm] ; Number of clusters",-3000,3000,200 ); 
                      plot(largestHit -> midPoint().y(), "SpillPrevCluster_y_position", 
                           "SpillPrev Cluster y position (from MCHit) ; y [mm] ; Number of clusters",-2500,2500,200 ); 
                      plot2D(largestHit -> midPoint().x(), largestHit -> midPoint().y(), 
                              "SpillPrevCluster_xy_position", "SpillPrev Cluster xy position ; x [mm]; y[mm]",
                              -3000,3000, -2500,2500,200,200);
                      plot(newCluster->size(),"SpillPrevClusSize",
                           "SpillPrev Cluster Size Distribution;Cluster Size;Nber of events" , 0. , 70., 70);
                      plot(newCluster->charge(),"SpillPrevClusCharge",
                           "SpillPrev Cluster Charge Distribution;Cluster Charge;Nber of events" , 0 , 100);
                      plot(newCluster->channelID().sipmId(), "SpillPrevClusSiPMID",
                           "SpillPrev Cluster SiPMID; Cluster SiPMID" , 0. , 96. ,96);
                    }
                  } // end of split Prev/Next monitoring
                } // end of IS spillover 

              } // end of if accept cluster

            }
          }

          plot( largestHit -> midPoint().x(), "Cluster_x_position", 
                "Cluster x position (from MCHit) ; x [mm] ; Number of clusters",-3500,3500,200 ); 
          plot( largestHit -> midPoint().y(), "Cluster_y_position", 
                "Cluster y position (from MCHit) ; y [mm] ; Number of clusters",-3000,3000,200 ); 
          plot2D( largestHit -> midPoint().x(), largestHit -> midPoint().y(), 
                  "Cluster_xy_position", "Cluster xy position ; x [mm]; y[mm]",
                  -3500,3500,200, -3000,3000,200);
        } else { // end of if cluster is NO noise cluster
          // no MCHit contributions --> noise cluster
          clusterCont -> insert(newCluster);
          plot(newCluster->size(),"NoiseClusSize","Noise Cluster Size Distribution;Cluster Size;Nber of events" , 0. , 70., 70);
          plot(newCluster->charge(),"NoiseClusCharge",
               "Noise Cluster Charge Distribution;Cluster Charge;Nber of events" , 0 , 100);
          plot(newCluster->channelID().sipmId(), "NoiseClusSiPMID", "Noise Cluster SiPMID; Cluster SiPMID" , 0. , 96. ,96);
        }


        // DEAL WITH  hitToClusterLink
        if ( msgLevel(MSG::DEBUG) ){
          debug() << "===   DEAL WITH  hitToClusterLink" << endmsg;
        }
        for(std::map<const LHCb::MCHit*,double>::iterator i = mcHitContributionMap.begin(); i != mcHitContributionMap.end(); ++i){
          hitToClusterLink.link(newCluster, (i->first), (i->second)/totalEnergyFromMC ) ;
          //           if ( msgLevel( MSG::DEBUG) ) {
          //             debug() << "Linked ClusterChannel=" << newCluster->channelID()
          //                     << " to MCIndex="<<i->first->index()
          //                     << " with EnergyFraction=" << (i->second)/totalEnergyFromMC
          //                     << endmsg;
          //           }
        }

      }

      seedDigitIter = stopDigitIter;
     
        
    } // END if(seedDigit->adcCount() > m_adcThreshold)
    

    // Prepare for next cluster
    
    ++seedDigitIter;
  }

  // split hits into 2 categories : those kept by clustering, and the others
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
      // plot(hitboolMapiter->first->entry().Pt(),"KeptHitPT", 
      //      "PT of Hits kept by Clusterisation; p_T [MeV];Number of hits" , 0 , 1 );
      plot(hitboolMapiter->first->mcParticle()->particleID().pid(),"KeptHitPDGId", 
           "PDGId of Hits kept by Clusterisation; Energy [MeV];Number of hits" , 0. , 100., 100);
    }
    else{
      ++m_nberOfLostHitFromMap;
      counter("NberOfLostHitFromMap")++;
      plot2D( hitboolMapiter->first->entry().x(), hitboolMapiter->first->entry().y(), 
              "LostHitEntryPosition","Entry position of Hits lost in Clusterisation; x [mm]; y [mm]",
              -500., 500., -500.,500., 100, 100);  
      plot(hitboolMapiter->first->energy(),"LostHitEnergy", 
           "Energy of Hits lost in Clusterisation; Energy [MeV];Number of hits" , 0 , 1 );
      // plot(hitboolMapiter->first->entry().Pt(),"LostHitPT", 
      //      "Energy of Hits lost in Clusterisation; p_T [MeV];Number of hits" , 0 , 1 );
      plot(hitboolMapiter->first->mcParticle()->particleID().pid(),"LostHitPDGId", 
           "PDGId of Hits lost in Clusterisation; Energy [MeV];Number of hits" , 0. , 100., 100 );
    }
  }

  return StatusCode::SUCCESS;
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode FTClusterCreator::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  if (m_nCluster > 0) info() << "*** Average cluster charge = " << m_sumCharge / m_nCluster << endmsg;

  info() << "*** Lost Hits = " <<m_nberOfLostHitFromMap << "\t Kept Hits = "<< m_nberOfKeptHitFromMap
         <<"\t Sum="<< (m_nberOfKeptHitFromMap + m_nberOfLostHitFromMap)
         << "\t Eff = "<< (double)m_nberOfKeptHitFromMap/(m_nberOfKeptHitFromMap + m_nberOfLostHitFromMap)<< endmsg;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}






