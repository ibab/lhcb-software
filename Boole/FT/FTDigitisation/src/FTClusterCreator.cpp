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

{
  declareProperty("InputLocation" ,     m_inputLocation     = LHCb::MCFTDigitLocation::Default, "Path to input MCDigits");
  declareProperty("OutputLocation" ,    m_outputLocation    = LHCb::FTClusterLocation::Default, "Path to output Clusters");
  declareProperty("ADCThreshold" ,      m_adcThreshold      = 3 , "Minimal ADC Count to be added in cluster");
  declareProperty("ClusterMinWidth" ,   m_clusterMinWidth   = 1 , "Minimal allowed width for clusters");
  declareProperty("ClusterMaxWidth" ,   m_clusterMaxWidth   = 8 , "Maximal allowed width for clusters");
  declareProperty("ClusterMinCharge" ,  m_clusterMinCharge  = 9 , "Minimal charge to keep cluster ~4.5 p.e.");
  declareProperty("ClusterMinADCPeak" , m_clusterMinADCPeak = 5 , "Minimal ADC for cluster peaks, ~2.5 pe.");
  declareProperty("FixClusterWidth" ,   m_fixClusterWidth   = 0 , "Fix cluster width for tracking res. studies");
  declareProperty("FixClusterCharge" ,  m_fixClusterCharge  = 0 , "Fix cluster charge for tracking res. studies");
  declareProperty("FixFracPos" ,        m_fixFracPos        = 0., "Fix cluster frac. pos. for tracking res. studies");

  // MONITORING PURPOSES
  declareProperty("MakeSpillPlots"    , m_makeSpillPlots    = false , "In case of spillover, add extra monitoring");
  declareProperty("RemoveITRegion"    , m_removeITRegion    = 0     , "1 = remove IT region, 2 = keep only IT region, 3 = remove square");
  declareProperty("ITScale"           , m_ITScale           = 1     , "Scale of IT to be removed / kept (only relevant if RemoveITRegion)");
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

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FTClusterCreator::execute(){

  debug() << "[FTClusterCreator] ==> Execute NEW EVENT" << endmsg;

  // retrieve FTDigits
  const MCFTDigits* mcDigitsCont = get<MCFTDigits>(m_inputLocation);

  // define clusters container
  FTClusters *clusterCont = new FTClusters();
  clusterCont->reserve(mcDigitsCont->size());

  // Register FTClusters in the transient data store
  put(clusterCont, m_outputLocation);

  // Create a link between the FTCluster and the MCParticle which leave a track
  LinkerWithKey<LHCb::MCParticle,LHCb::FTCluster> mcToClusterLink( evtSvc(), msgSvc(),(m_outputLocation));

  // Create a link between the FTCluster and the MCHit
  LinkerWithKey<LHCb::MCHit,LHCb::FTCluster> hitToClusterLink( evtSvc(), msgSvc(),(m_outputLocation + "2MCHits"));


  // MONITORING PURPOSES

  // Get the default (signal) spill (to check for spillover cluster origin)
  const MCHits* mcHits = get<MCHits>(LHCb::MCHitLocation::FT);
  const MCHits* mcHitsNext = 0;
  const MCHits* mcHitsPrev = 0;
  if( m_makeSpillPlots ) {
    mcHitsNext = getIfExists<MCHits>("Next/"+LHCb::MCHitLocation::FT); //"/Event/Next/MC/FT/Hits"
    mcHitsPrev = getIfExists<MCHits>("Prev/"+LHCb::MCHitLocation::FT); //"/Event/Prev/MC/FT/Hits"
  } 

  // Create map of involved hits, all of them associated to false (not yet in clusters)
  std::map<const LHCb::MCHit*, bool> hitBoolMap;
  for (MCFTDigits::const_iterator iterDigit = mcDigitsCont->begin(); iterDigit!=mcDigitsCont->end();++iterDigit){
    MCFTDigit* mcDigit = *iterDigit;
    const MCFTDeposit* mcDeposit = mcDigit->deposit();
    if (mcDeposit != 0) {
      for (unsigned int idx = 0; idx<mcDeposit->mcHitVec().size(); ++idx) {
        if( mcDeposit->mcHitVec()[idx] != 0 ) {
          hitBoolMap[ mcDeposit->mcHitVec()[idx] ]=false;
        }
      }
    }
  }




  //***********************
  //* MAIN LOOP
  //***********************

  MCFTDigits::const_iterator lastStopDigitIter = mcDigitsCont->begin(); // end digit of last cluster, to prevent overlap

  // Since Digit Container is sorted wrt channelID, clusters are defined searching for bumps of ADC Count
  MCFTDigits::const_iterator seedDigitIter = mcDigitsCont->begin();
  
  // Counters for clusters per SiPM plot
  int ClusterPerSiPMCounter = 0;
  unsigned int ReferenceSiPMID = 0;
  unsigned int LastSiPMID = 0;

  while(seedDigitIter != mcDigitsCont->end()){ 
    // loop over digits

    MCFTDigit* seedDigit = *seedDigitIter; // the seed of the cluster

    if(seedDigit->adcCount() >= m_clusterMinADCPeak){ 
      // ADC above seed : start clustering

      if ( msgLevel( MSG::DEBUG) )
        debug() << " ---> START NEW CLUSTER WITH SEED @ " << seedDigit->channelID() << endmsg;
      
      MCFTDigits::const_iterator startDigitIter = seedDigitIter; // begin channel of cluster
      MCFTDigits::const_iterator stopDigitIter  = seedDigitIter; // end   channel of cluster

      // vector of MCHits that contributed to the cluster
      std::vector<const LHCb::MCHit*> clusterHitDistribution;

      // total energy in the cluster from MC
      double totalEnergyFromMC = 0;

      // map of contributing MCParticles (MCHits) with their relative energy deposit
      std::map< const LHCb::MCParticle*, double> mcContributionMap;
      std::map< const LHCb::MCHit*, double> mcHitContributionMap;


      //
      // test neighbours to define starting and ending channels of Cluster
      bool ContinueStartLoop = true;
      bool ContinueStopLoop  = true;
      
      while(((stopDigitIter-startDigitIter)< m_clusterMaxWidth ) 
            && (ContinueStartLoop || ContinueStopLoop) ) {
        // IF cluster size =< m_clusterMaxWidth SiPM Channels
 

        // EXTEND TO THE RIGHT
        if(ContinueStopLoop && ((stopDigitIter+1) != mcDigitsCont->end()) ) {
          // IF the next digit exists: try to extend cluster to the 'right'
         
          MCFTDigit* stopDigit = *(stopDigitIter+1); // the next digit 
 
          // next digit should be in the same SiPM, the channel next door, and above adcThreshold
          if((stopDigit->channelID().sipmId() == seedDigit->channelID().sipmId())
             &&(stopDigit->channelID().sipmCell()==((*stopDigitIter)->channelID().sipmCell()+1))
             && (stopDigit->adcCount() >= m_adcThreshold)) {
            
            // if ADC of next digit > the current seed digit, redefine the seed
            if(stopDigit->adcCount() > seedDigit->adcCount()) {
              
              seedDigitIter = stopDigitIter+1; // increment loop iterator
              seedDigit = *seedDigitIter;      // increment seed channel
              if ( msgLevel( MSG::DEBUG) )
                debug() << "  ==> Redefined the seed @ " << seedDigit->channelID() << endmsg;
              
              // set min and max channel of cluster to the new seed (i.e. reset the cluster finding)
              startDigitIter = seedDigitIter; 
              stopDigitIter  = seedDigitIter;
              ContinueStartLoop = true;
              
            } else {
              // IF next digit ADC < current seed, but passes clustering requirements
              stopDigitIter++; // extend cluster to the 'right'
            }
  
            
          } else {
            // IF next digit does not satisfy clustering requirements
            ContinueStopLoop = false;
          }

        } else {
          // IF the next digit does not exist in the container (i.e. done with all clusterisation)
          ContinueStopLoop = false;
        }


        // So far, we have extended the cluster to the 'right' as far as we could, 
        //  redefining the seed as we go (pulling our left-side 'tail' with us).
        // We now need to extend to the 'left' side.


        // EXTEND TO THE LEFT
        if(ContinueStartLoop && ((startDigitIter) != mcDigitsCont->begin())) {
          // IF the previous digit exists: try to extend cluster to the 'left'
          
          MCFTDigit* startDigit = *(startDigitIter-1); // the 'previous' digit

          // previous digit should be in the same SiPM, the channel next door, above adcThreshold,
          //  and also after the ending channel of the previous cluster
          if((startDigit->channelID().sipmId() == seedDigit->channelID().sipmId()) 
             &&(startDigit->channelID().sipmCell()==((*startDigitIter)->channelID().sipmCell()-1))
             && (startDigit->adcCount() >= m_adcThreshold)
             &&((startDigitIter-1) > lastStopDigitIter)) {

            // extend cluster to the 'left'
            startDigitIter--;
            
          } else {
            // previous channel does not satisfy clustering requirements
            ContinueStartLoop = false;
          }
          
        } else {      
          // there is no previous digit in the container
          ContinueStartLoop = false;
        }

      }; 
      // MaxClusterSize has been reached, or iterator stop was set due to criteria
      // Cluster spans from startDigitIter to stopDigitIter


      // Check if cluster size < maxWidth. If not: choose highest ADC sum for cluster and shrink
      // This is possible because we check the size, then extend right, then extend left, and repeat.
      while((stopDigitIter - startDigitIter) >= m_clusterMaxWidth) {
        if ( msgLevel( MSG::DEBUG) )
          debug() << "Cluster size exceeded threshold, Shrinking cluster." << endmsg;
        if((*stopDigitIter)->adcCount() > (*startDigitIter)->adcCount())   startDigitIter++;
        else stopDigitIter--;
      }

      lastStopDigitIter = stopDigitIter; // update the 'previous cluster last channel'



      //
      // calculate the cluster charge / mean position
      if ( msgLevel( MSG::DEBUG) )
        debug() << " ---> Done with cluster finding, now calculating charge / frac.pos" << endmsg;

      int      totalCharge  = 0;
      double   meanPosition = 0;

      //std::vector<LHCb::MCFTDigit*> digitVec; //TODO

      for(MCFTDigits::const_iterator iterDigit = startDigitIter; iterDigit <(stopDigitIter+1); ++iterDigit) {
        // Loop over digits in to-be cluster

        MCFTDigit* digit = *iterDigit;
        //digitVec.push_back(digit); //TODO

        // calculate channel ADC as seen by the hardware thresholds (2bits implementation: ADC linked to the 3 thresholds)
        int ChannelWeight = 0;
        if( digit->adcCount() >= m_adcThreshold )      { ChannelWeight = m_adcThreshold; }
        if( digit->adcCount() >= m_clusterMinADCPeak ) { ChannelWeight = m_clusterMinADCPeak; }
        if( digit->adcCount() >= m_clusterMinCharge )  { ChannelWeight = m_clusterMinCharge; }

        totalCharge += ChannelWeight;

        // meanPosition will be [ (rel. pos. from left) * charge ] / totalCharge
        meanPosition += (iterDigit-startDigitIter) * (ChannelWeight);


        // check contributions from MCHits, for monitoring
        const MCFTDeposit* mcDeposit = digit->deposit();
        if( mcDeposit != 0 ) { 
          for (unsigned int idx = 0; idx<mcDeposit->mcHitVec().size(); ++idx) {
            // loop over all mcHits contributing to the deposit associated with the digit

            if(mcDeposit -> mcHitVec()[idx] != 0) {
              // add dir+ref energy to total energy from MC
              totalEnergyFromMC += mcDeposit->energyVec()[idx] + mcDeposit->energyRefVec()[idx];
              // add the MCParticle that deposited the energy to the map
              mcContributionMap[ mcDeposit->mcHitVec()[idx]->mcParticle() ]  +=  (mcDeposit->energyVec()[idx] + mcDeposit->energyRefVec()[idx]);
              // add the MCHit that contributed energy to the map
              mcHitContributionMap[ mcDeposit->mcHitVec()[idx] ]  +=  (mcDeposit->energyVec()[idx] + mcDeposit->energyRefVec()[idx]);
              // add MCHit to clusterHitDistribution list
              clusterHitDistribution.push_back(mcDeposit->mcHitVec()[idx]);
            }
          }
        }
      } // end of loop over digits in 'cluster'
      

      // Update the hitBoolMap of MCHits contributing to clusters
      for(std::vector<const LHCb::MCHit*>::const_iterator hitIter = clusterHitDistribution.begin(); 
        hitIter != clusterHitDistribution.end(); ++hitIter){
        hitBoolMap[*hitIter]=true;     
      }


      if( 0 < totalCharge) { 
        // add the offset of the cluster channelID to the meanPosition
        meanPosition =(*startDigitIter)->channelID() + meanPosition/totalCharge;
      } else {
        // THIS SHOULD NEVER BE REACHED!
        info() << "Warning: Cluster charge < 0! Cluster mean position not estimated correctly." << endmsg;
      }

        


      //
      // Before Cluster is recorded, check that total ADC > threshold and size > minSize

      //if( ( totalCharge >= m_clusterMinCharge) &&
      //    ((stopDigitIter-startDigitIter+1) >= m_clusterMinWidth) ){
      unsigned int width = (stopDigitIter-startDigitIter+1);
      if(
         ( (width==1 && totalCharge >= m_clusterMinCharge) ||
           (width> 1 && totalCharge >= m_clusterMinADCPeak + m_adcThreshold) )
         && (width >= m_clusterMinWidth)
        ) {
        
        // update counters for clusters per SiPM
        if( ReferenceSiPMID == 0 ) ReferenceSiPMID = (*startDigitIter)->channelID().sipmId();
        LastSiPMID = (*stopDigitIter)->channelID().sipmId();
        if (ReferenceSiPMID == LastSiPMID) ++ClusterPerSiPMCounter; // add 1 'cluster' to current SiPM
        else {
          // if we're in the next SiPM, plot the clusters per SiPM of the previous SiPM
          plot(ClusterPerSiPMCounter, "ClusterPerSiPM", "Cluster per SiPM distribution; Nb of clusters" , 0. , 50., 50);
          ReferenceSiPMID = LastSiPMID;
          ClusterPerSiPMCounter = 1;
        }


        // Calculate the fractional position
        // Due to max.size of 4 and thresholds 3,5,8, the extremes of meanPosition are 
        //  clus. size 4: 1.05 - 1.95
        //  clus. size 3: 0.64 - 1.36
        //  clus. size 2: 0.27 - 0.73
        //  clus. size 1: always 0.
        //   (+ the startDigit->channelID)
        uint meanChanPosition = std::floor(meanPosition); // will be 0 or 1  +  startDigit->channelID
        double fractionChanPosition = (meanPosition-std::floor(meanPosition)); // will be the decimals 
        
        // -- This is mostly a choice
        // -- In the future, a range [-.5,.5]
        // -- is desirable, however until all things are understood
        // -- we revert to [0,1[
        /*
        if(fractionChanPosition > .5) {
          // keep fractional part within [-.5,.5] range
          meanChanPosition += 1;
          fractionChanPosition -= 1.; // can range (-0.49,0.5]
        }
        */


        // Check to fix certain properties for tracking eff. studies
        int newClusSize = (stopDigitIter-startDigitIter+1);
        if(m_fixClusterWidth != 0)  { newClusSize = m_fixClusterWidth;  }
        if(m_fixClusterCharge != 0) { totalCharge = m_fixClusterCharge; }
        if(m_fixFracPos != 0.) { fractionChanPosition = m_fixFracPos; }

        
        // Define new cluster
        //  FTCluster::FTCluster( LHCb::FTChannelID &id, double fraction, int size, int charge )
        //FTCluster *newCluster = new FTCluster(meanChanPosition,fractionChanPosition,newClusSize,totalCharge,digitVec); // TODO
        FTCluster *newCluster = new FTCluster(meanChanPosition,fractionChanPosition,newClusSize,totalCharge);
        ++m_nCluster;
        m_sumCharge += totalCharge;
 
        bool acceptCluster = 1;

        // TEMPORARY FIX! The calculation of the meanChanPos creates module=12 when on an edge channel!!
        //  Should be taken into account in the calculation of meanChanPos.
        if( newCluster->channelID().module() >= 12 ) { 
          info() << "Warning: cluster module is out of allowed bounds. Ignoring Cluster. [ " << newCluster->channelID() << " ] " << endmsg;
          acceptCluster = 0; 
        }


        // Get largest MCHit object contributing to cluster
        if( mcHitContributionMap.size() > 0 ) {
          // NOT a noise cluster
          plot( (mcHitContributionMap.size()) , "Cluster_NberOfHits", "Number of Hits in Cluster ; Number of Hits ; Number of clusters",0.,50.,50); 

          double largestDeposit = -999;
          const LHCb::MCHit* largestHit = 0; 
          for(std::map<const LHCb::MCHit*,double>::iterator i = mcHitContributionMap.begin(); i != mcHitContributionMap.end(); ++i) {

            if(largestDeposit < (i->second)) {
              largestDeposit = (i->second);
              largestHit = (i->first);
            }
          }

          


          // TESTS: Check if we should remove cluster
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
            }
          } // end of if removeITRegion




          if ( acceptCluster ) {
            clusterCont -> insert(newCluster); // add cluster to container
        
            //implement counters
            if(newCluster->size() == 1) counter("NberOfClusterSize1")++;
            else if (newCluster->size() == 2) counter("NberOfClusterSize2")++;
            else if (newCluster->size() == 3) counter("NberOfClusterSize3")++;
            else if (newCluster->size() == 4) counter("NberOfClusterSize4")++;

            // draw cluster channelID
            plot(newCluster->channelID(), "ClusChannelID", "Cluster ChannelID; Cluster ChannelID" , 0. , 589824. ,4608);
            plot(newCluster->channelID().sipmCell(), "ClusSiPMCell", "Cluster SiPM Cell; Cluster SiPM Cell", 0. , 130. ,130);
            plot(newCluster->channelID().sipmId(), "ClusSiPMID", "Cluster SiPMID; Cluster SiPMID" , 0. , 96. ,96);
            plot(newCluster->channelID().module(), "ClusModule", "Cluster Module; Cluster Module" , 0. , 16. ,16);
            plot(newCluster->channelID().mat(), "ClusMat", "Cluster Mat; Cluster Mat" , 0. , 2. ,2);
            plot(newCluster->channelID().quarter(), "ClusQuarter", "Cluster Quarter; Cluster Quarter" , 0. , 4. ,4);
            plot(newCluster->channelID().layer(), "ClusLayer", "Cluster Layer; Cluster layer" , 0. , 13. ,13);

            // draw cluster details
            plot(newCluster->size(),"ClusSize","Cluster Size Distribution;Cluster Size;Nber of events" , 0. , 10., 10);
            plot(newCluster->charge(),"ClusCharge","Cluster Charge Distribution;Cluster Charge;Nber of events" , 0., 50., 50);
            plot(newCluster->fraction(), "ClusFractionOld", "Cluster Fraction Pos in old [-.5,.5] range;Cluster Fraction; Nber of events" , -.5, .5 , 100);
            plot(newCluster->fraction(), "ClusFraction", "Cluster Fraction Pos;Cluster Fraction; Nber of events" , 0., 1. , 100);
            plot2D(newCluster->size(), newCluster->charge(), "ClusChargevsSize",
                   "Cluster charge vs. size;Cluster size [Nb of Channels];Cluster Charge [ADC counts]",
                   0. , 16. , 0. , 100.,16, 100);
            plot2D(newCluster->size(),newCluster->fraction(), "ClusterSizeVSFraction",  "Cluster Size VS Cluster Fraction ; Cluster Size; Cluster Fraction" , 0. , 5., 0., 1.);

            // draw largest contributing MCHit properties
            plot( largestHit -> midPoint().x(), "Cluster_MCHit_xPosition", "Cluster x position (from MCHit) ; x [mm] ; Number of clusters",-3500,3500,200 ); 
            plot( largestHit -> midPoint().y(), "Cluster_MCHit_yPosition", "Cluster y position (from MCHit) ; y [mm] ; Number of clusters",-3000,3000,200 ); 
            plot2D( largestHit -> midPoint().x(), largestHit -> midPoint().y(), 
                    "Cluster_MCHit_xyPosition", "Cluster xy position (from MCHit); x [mm]; y[mm]",
                    -3500,3500,200, -3000,3000,200);
 

            // draw current spill + no noise plots
            if( largestHit -> parent() == mcHits ) {
              // is no spillover
              plot(largestHit -> midPoint().x(), "okspillCluster_x_position", "MC Cluster x position (from MCHit) ; x [mm] ; Number of clusters",-3000,3000,200 ); 
              plot(largestHit -> midPoint().y(), "okspillCluster_y_position", "MC Cluster y position (from MCHit) ; y [mm] ; Number of clusters",-2500,2500,200 ); 
              plot2D(largestHit -> midPoint().x(), largestHit -> midPoint().y(), "MCCluster_xy_position", 
                  "MC Cluster xy position ; x [mm]; y[mm]", -3000,3000,-2500,2500,200,200);
              plot(newCluster->size(),"okspillClusSize","MC Cluster Size Distribution;Cluster Size;Nber of events" , 0. , 70., 70);
              plot(newCluster->charge(),"okspillClusCharge", "MC Cluster Charge Distribution;Cluster Charge;Nber of events" , 0 , 100);
              plot(newCluster->channelID().sipmId(), "okspillClusSiPMID", "MC Cluster SiPMID; Cluster SiPMID" , 0. , 96. ,96);
            } else {
              if(m_makeSpillPlots) {
                if ( mcHitsNext == 0 || mcHitsPrev == 0 ) {
                  info() << "WARNING: MakeSpillPlots is set to 1 but no spillover locations found! Ignoring monitoring." << endmsg;
                  break;
                }
                // is spillover
                plot(largestHit -> midPoint().x(), "SpilloverCluster_x_position", "Spillover Cluster x position (from MCHit) ; x [mm] ; Number of clusters",-3000,3000,200 ); 
                plot(largestHit -> midPoint().y(), "SpilloverCluster_y_position", "Spillover Cluster y position (from MCHit) ; y [mm] ; Number of clusters",-2500,2500,200 ); 
                plot2D(largestHit -> midPoint().x(), largestHit -> midPoint().y(),"SpilloverCluster_xy_position", 
                    "Spillover Cluster xy position ; x [mm]; y[mm]", -3000,3000,-2500,2500,200,200);
                plot(newCluster->size(),"SpilloverClusSize", "Spillover Cluster Size Distribution;Cluster Size;Nber of events" , 0. , 70., 70);
                plot(newCluster->charge(),"SpilloverClusCharge", "Spillover Cluster Charge Distribution;Cluster Charge;Nber of events" , 0 , 100);
                plot(newCluster->channelID().sipmId(), "SpilloverClusSiPMID", "Spillover Cluster SiPMID; Cluster SiPMID" , 0. , 96. ,96);

                if ( largestHit -> parent() == mcHitsNext ) {
                  plot(largestHit -> midPoint().x(), "SpillNextCluster_x_position", "SpillNext Cluster x position (from MCHit) ; x [mm] ; Number of clusters",-3000,3000,200 ); 
                  plot(largestHit -> midPoint().y(), "SpillNextCluster_y_position", "SpillNext Cluster y position (from MCHit) ; y [mm] ; Number of clusters",-2500,2500,200 ); 
                  plot2D(largestHit -> midPoint().x(), largestHit -> midPoint().y(), "SpillNextCluster_xy_position", 
                      "SpillNext Cluster xy position ; x [mm]; y[mm]", -3000,3000, -2500,2500,200,200);
                  plot(newCluster->size(),"SpillNextClusSize", "SpillNext Cluster Size Distribution;Cluster Size;Nber of events" , 0. , 70., 70);
                  plot(newCluster->charge(),"SpillNextClusCharge", "SpillNext Cluster Charge Distribution;Cluster Charge;Nber of events" , 0 , 100);
                  plot(newCluster->channelID().sipmId(), "SpillNextClusSiPMID", "SpillNext Cluster SiPMID; Cluster SiPMID" , 0. , 96. ,96);
                }
                if ( largestHit -> parent() == mcHitsPrev ) {
                  plot(largestHit -> midPoint().x(), "SpillPrevCluster_x_position", "SpillPrev Cluster x position (from MCHit) ; x [mm] ; Number of clusters",-3000,3000,200 ); 
                  plot(largestHit -> midPoint().y(), "SpillPrevCluster_y_position", "SpillPrev Cluster y position (from MCHit) ; y [mm] ; Number of clusters",-2500,2500,200 ); 
                  plot2D(largestHit -> midPoint().x(), largestHit -> midPoint().y(), "SpillPrevCluster_xy_position", 
                      "SpillPrev Cluster xy position ; x [mm]; y[mm]", -3000,3000, -2500,2500,200,200);
                  plot(newCluster->size(),"SpillPrevClusSize", "SpillPrev Cluster Size Distribution;Cluster Size;Nber of events" , 0. , 70., 70);
                  plot(newCluster->charge(),"SpillPrevClusCharge", "SpillPrev Cluster Charge Distribution;Cluster Charge;Nber of events" , 0 , 100);
                  plot(newCluster->channelID().sipmId(), "SpillPrevClusSiPMID", "SpillPrev Cluster SiPMID; Cluster SiPMID" , 0. , 96. ,96);
                }
              }
            } // end of IS spillover

          } else {
            // do not accept cluster
            plot2D( largestHit->midPoint().x(), largestHit->midPoint().y(),
                    "Cluster_rejected","Rejected Clusters; x [mm]; y [mm]",
                    -3000, 3000, -2500, 2500, 200, 200 );
          }
        
          // Setup MCParticle to cluster link (also for spillover / not accepted clusters, as long as it's not noise)
          if ( acceptCluster ) {
            for(std::map<const LHCb::MCParticle*,double>::iterator i = mcContributionMap.begin(); i != mcContributionMap.end(); ++i) {
              mcToClusterLink.link(newCluster, (i->first), (i->second)/totalEnergyFromMC ) ;
            }
            
            // Setup MCHit to cluster link
            for(std::map<const LHCb::MCHit*,double>::iterator i = mcHitContributionMap.begin(); i != mcHitContributionMap.end(); ++i) {
              hitToClusterLink.link(newCluster, (i->first), (i->second)/totalEnergyFromMC ) ;
            }
          }
          
        } else { 
          // Cluster has no contributing MCHits --> is a noise cluster

          if( acceptCluster ) {
            clusterCont -> insert(newCluster);

            plot(newCluster->size(),"NoiseClusSize","Noise Cluster Size Distribution;Cluster Size;Nber of events" , 0. , 10., 10);
            plot(newCluster->charge(),"NoiseClusCharge","Noise Cluster Charge Distribution;Cluster Charge;Nber of events" , 0., 50., 50);
            plot(newCluster->fraction(), "NoiseClusFraction", "Noise Cluster Fraction Pos;Cluster Fraction; Nber of events" , -.5, .5 , 100);
            plot(newCluster->channelID(), "NoiseClusChannelID", "Noise Cluster ChannelID; Cluster ChannelID" , 0. , 589824. ,4608);
            plot(newCluster->channelID().sipmCell(), "NoiseClusSiPMCell", "Noise Cluster SiPM Cell; Cluster SiPM Cell", 0. , 130. ,130);
            plot(newCluster->channelID().sipmId(), "NoiseClusSiPMID", "Noise Cluster SiPMID; Cluster SiPMID" , 0. , 96. ,96);
            plot(newCluster->channelID().module(), "NoiseClusModule", "Noise Cluster Module; Cluster Module" , 0. , 16. ,16);
            plot(newCluster->channelID().mat(), "NoiseClusMat", "Noise Cluster Mat; Cluster Mat" , 0. , 2. ,2);
            plot(newCluster->channelID().quarter(), "NoiseClusQuarter", "Noise Cluster Quarter; Cluster Quarter" , 0. , 4. ,4);
            plot(newCluster->channelID().layer(), "NoiseClusLayer", "Noise Cluster Layer; Cluster layer" , 0. , 13. ,13);

            plot2D(newCluster->size(), newCluster->charge(), "NoiseClusChargevsSize",
                   "Noise Cluster charge vs. size;Cluster size [Nb of Channels];Cluster Charge [ADC counts]",
                   0. , 16. , 0. , 100.,16, 100);
            plot2D(newCluster->size(),newCluster->fraction(), "NoiseClusterSizeVSFraction",  
                   "Noise Cluster Size VS Cluster Fraction ; Cluster Size; Cluster Fraction" , 
                   0. , 5., 0., 1.);

            /* TODO 
            if(msgLevel(MSG::DEBUG)) debug() << " >> accessing Digits" << endmsg;
            auto digits = newCluster -> digits();
            assert(digits);
            for(auto digiter = digits.begin(); digiter!= digits.end(); ++digiter) {
              LHCb::MCFTDigit* digit = *digiter;
              const LHCb::MCFTDeposit* deposit = digit -> deposit();
              if(msgLevel(MSG::DEBUG)) debug() << "Noise: " << deposit->thNoiseCont() << " " << deposit->afterplCont() << " " << deposit->xtalkCont() << endmsg;
              plot(deposit->thNoiseCont(), "NoiseClusThermalPEs","NoiseClusThermalPEs;PE;", 0., 20., 20);
              plot(deposit->afterplCont(), "NoiseClusAfterplPEs","NoiseClusAfterplPEs;PE;", 0., 20., 20);
              plot(deposit->xtalkCont(), "NoiseClusXtalkPEs","NoiseClusXtalkPEs;PE;", 0., 20., 20);
            }
            if(msgLevel(MSG::DEBUG)) debug() << " << done accessing Digits" << endmsg;
            */
          }
        }




      } // end of Cluster satisfies charge / size requirements

      // Set the loop iterator to the end digit, to continue looking for next cluster without overlap.
      // Will get +1 at end of loop.
      seedDigitIter = stopDigitIter;
            
    } // END of clustering ( if(seedDigit->adcCount() > m_adcThreshold) )
    
    // Prepare for next cluster
    ++seedDigitIter;

  } // END of loop over Digits

  


  // plot the KeptMCHits / LostMCHits by the clustering
  for(std::map<const LHCb::MCHit*, bool>::const_iterator hitboolMapiter = hitBoolMap.begin(); hitboolMapiter != hitBoolMap.end(); ++hitboolMapiter){
    if(hitboolMapiter->second == true){
      ++m_nberOfKeptHitFromMap;
      counter("NberOfKeptHitFromMap")++;
      plot2D( hitboolMapiter->first->entry().x(), hitboolMapiter->first->entry().y(), "KeptHitEntryPosition",
          "Entry position of Hits kept by Clusterisation ; x [mm]; y [mm]", -500., 500., -500.,500., 100, 100);  
      plot(hitboolMapiter->first->energy(),"KeptHitEnergy", "Energy of Hits kept by Clusterisation; Energy [MeV];Number of hits" , 0 , 1 );
      plot(hitboolMapiter->first->mcParticle()->particleID().pid(),"KeptHitPDGId", "PDGId of Hits kept by Clusterisation; Energy [MeV];Number of hits" , 0. , 100., 100);
    }
    else{
      ++m_nberOfLostHitFromMap;
      counter("NberOfLostHitFromMap")++;
      plot2D( hitboolMapiter->first->entry().x(), hitboolMapiter->first->entry().y(), "LostHitEntryPosition",
          "Entry position of Hits lost in Clusterisation; x [mm]; y [mm]", -500., 500., -500.,500., 100, 100);  
      plot(hitboolMapiter->first->energy(),"LostHitEnergy", "Energy of Hits lost in Clusterisation; Energy [MeV];Number of hits" , 0 , 1 );
      plot(hitboolMapiter->first->mcParticle()->particleID().pid(),"LostHitPDGId", "PDGId of Hits lost in Clusterisation; Energy [MeV];Number of hits" , 0. , 100., 100 );
    }
  }



  if(msgLevel(MSG::DEBUG)) debug() << "done with clustering!" << endmsg;
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







