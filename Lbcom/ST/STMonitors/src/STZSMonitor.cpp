// Include files 


// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "STZSMonitor.h"
#include "GaudiKernel/Algorithm.h" 
#include "Event/STCluster.h"
#include "Kernel/STDetSwitch.h"
#include "Kernel/STTell1Board.h"
#include "Kernel/STTell1ID.h"

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "Kernel/Trajectory.h"




using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : STZSMonitor
//
// 2008-May-6: Mathias Knecht
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STZSMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STZSMonitor::STZSMonitor( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
     
  declareProperty("InputLocation",m_inputLocation=STClusterLocation::ITClusters );
  declareProperty("DetType", m_detType = "IT");
  declareProperty("TAEtag", m_TAEtag = "");
  declareProperty("SkippedEvents", m_skippedEvents = 1000);
  declareProperty("MaskFirstChannel", m_maskFirstChannel = false);
  declareProperty("MaskLastChannel", m_maskLastChannel = false);
  declareProperty("readoutTool", m_readoutToolName = "ITReadoutTool");     
 
  
  

    
}
//=============================================================================
// Destructor
//=============================================================================
STZSMonitor::~STZSMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode STZSMonitor::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
 
  if(m_detType == "IT"){
    info() << "You are using an Inner Tracker-style algorithm"<< endmsg;
  }
  else if(m_detType == "TT"){
    info() << "You are using a Trigger Turicensis-style algorithm"<< endmsg;
  }else{
    fatal() << "Algorithm only for IT or TT!! Either change your code, either change your sub-detector."<< endmsg;
    return StatusCode::FAILURE;
  }
 
 
  m_readoutTool = tool<ISTReadoutTool>(m_readoutToolName,m_readoutToolName);
  sc = service("HistogramDataSvc", m_histosvc, true );
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

 
  // output location
  STDetSwitch::flip(m_detType,m_inputLocation);
 
  m_evtCounter=0;
      
  
  // general histograms for full detector 
  
  evt_number = m_histosvc->book(m_detType+"/evt_number"+m_TAEtag, "evt_number"+m_TAEtag, 2, 0.5, 2.5 );
  declareInfo( "evt_number"+m_TAEtag, evt_number , "evt_number"+m_TAEtag);
   
  std::ostringstream nameClusterCount; 
  nameClusterCount << m_detType  <<"-Cluster_Number" << m_TAEtag;  
  CLUSTERCOUNT = m_histosvc->book( nameClusterCount.str(), nameClusterCount.str(), 500, 0,  5000);
  declareInfo( nameClusterCount.str(),  CLUSTERCOUNT , nameClusterCount.str());

  
  std::ostringstream nameClusterChargeSum; 
  nameClusterChargeSum <<  m_detType  <<"-Cluster_Charge_Sum" << m_TAEtag;   
  CLUSTERCHARGESUM = m_histosvc->book( nameClusterChargeSum.str(), nameClusterChargeSum.str(), 100,0,150);
  declareInfo( nameClusterChargeSum.str(),  CLUSTERCHARGESUM , nameClusterChargeSum.str());


  
  
  // specific histograms, for full detector

  std::ostringstream nameClusterSize; 
  nameClusterSize <<  m_detType  <<"-Cluster_Size" << m_TAEtag;
  CLUSTERSIZE = m_histosvc->book( nameClusterSize.str(), nameClusterSize.str(), 7, -0.5,  6.5);
  declareInfo( nameClusterSize.str(),  CLUSTERSIZE , nameClusterSize.str());

  std::ostringstream nameTotalCharge; 
  nameTotalCharge <<  m_detType  <<"-Total_Charge"<< m_TAEtag; 
  TOTALCHARGE = m_histosvc->book( nameTotalCharge.str(), nameTotalCharge.str(), 300, 0,  300);
  declareInfo( nameTotalCharge.str(),  TOTALCHARGE , nameTotalCharge.str());

  std::ostringstream nameHighThreshold; 
  nameHighThreshold <<  m_detType  <<"-High_Threshold" << m_TAEtag;    
  HIGHTHRESHOLD = m_histosvc->book( nameHighThreshold.str(), nameHighThreshold.str(), 2, -0.5,  1.5);
  declareInfo( nameHighThreshold.str(),  HIGHTHRESHOLD , nameHighThreshold.str());
 
  std::ostringstream nameNeighbourSum; 
  nameNeighbourSum <<  m_detType  <<"-Neighbour_Sum" << m_TAEtag;    
  NEIGHBOURSUM = m_histosvc->book( nameNeighbourSum.str(), nameNeighbourSum.str(), 43, -16.5,  26.5);
  declareInfo( nameNeighbourSum.str(),  NEIGHBOURSUM , nameNeighbourSum.str());
  
  std::ostringstream nameRelNeighbourSum; 
  nameRelNeighbourSum <<  m_detType  <<"-Rel_Neighbour_Sum" << m_TAEtag;    
  RELNEIGHBOURSUM = m_histosvc->book( nameRelNeighbourSum.str(), nameRelNeighbourSum.str(), 51, -1.02,  1.02);
  declareInfo( nameRelNeighbourSum.str(),  RELNEIGHBOURSUM , nameRelNeighbourSum.str());

  
    
 
 
  int m_stationMax=0;
  int m_regionMax=0;
  int m_layerMax=0;
  int m_sectorMax=0;
  std::string m_detRegion="Invalid";
  std::string m_station="Invalid";
  std::string m_layer="Invalid";   
   
  if(m_detType=="IT"){
    m_stationMax=3;
    m_regionMax=4;
    m_layerMax=4;
  }
  if(m_detType=="TT"){
    m_stationMax=2;
    m_regionMax=3;
    m_layerMax=2;
  }
 
  for(int k=0; k<m_stationMax; k++){
  
    if(m_detType=="IT"){
      if(k==0) m_station="IT1";
      if(k==1) m_station="IT2";
      if(k==2) m_station="IT3";  
    }
    if(m_detType=="TT"){
      if(k==0) m_station="TTa";
      if(k==1) m_station="TTb";
    }
 
 
    // Per station
    std::ostringstream nameClusterSizeSTA; 
    nameClusterSizeSTA <<  m_detType  <<"-Cluster_Size/" << m_station << m_TAEtag; 
    CLUSTERSIZESTA[k] = m_histosvc->book( nameClusterSizeSTA.str(), nameClusterSizeSTA.str(), 7, -0.5,  6.5);
    declareInfo( nameClusterSizeSTA.str(),  CLUSTERSIZESTA[k] , nameClusterSizeSTA.str());
    
    
    std::ostringstream nameTotalChargeSTA; 
    nameTotalChargeSTA <<  m_detType  <<"-Total_Charge/" << m_station << m_TAEtag;    
    TOTALCHARGESTA[k] = m_histosvc->book( nameTotalChargeSTA.str(), nameTotalChargeSTA.str(), 300, 0,  300);
    declareInfo( nameTotalChargeSTA.str(),  TOTALCHARGESTA[k] , nameTotalChargeSTA.str());

 
    std::ostringstream nameHighThresholdSTA; 
    nameHighThresholdSTA <<  m_detType  <<"-High_Threshold/" << m_station << m_TAEtag;     
    HIGHTHRESHOLDSTA[k] = m_histosvc->book( nameHighThresholdSTA.str(), nameHighThresholdSTA.str(), 2, -0.5,  1.5);
    declareInfo( nameHighThresholdSTA.str(),  HIGHTHRESHOLDSTA[k] , nameHighThresholdSTA.str());

 
    std::ostringstream nameNeighbourSumSTA; 
    nameNeighbourSumSTA <<  m_detType  <<"-Neighbour_Sum/" << m_station  << m_TAEtag;       
    NEIGHBOURSUMSTA[k] = m_histosvc->book( nameNeighbourSumSTA.str(), nameNeighbourSumSTA.str(), 43, -16.5,  26.5);
    declareInfo( nameNeighbourSumSTA.str(),  NEIGHBOURSUMSTA[k] , nameNeighbourSumSTA.str());
 
 
    std::ostringstream nameRelNeighbourSumSTA; 
    nameRelNeighbourSumSTA <<  m_detType  <<"-Rel_Neighbour_Sum/" << m_station  << m_TAEtag;  
    RELNEIGHBOURSUMSTA[k] = m_histosvc->book( nameRelNeighbourSumSTA.str(), nameRelNeighbourSumSTA.str(), 51, -1.02,  1.02);
    declareInfo( nameRelNeighbourSumSTA.str(),  RELNEIGHBOURSUMSTA[k] , nameRelNeighbourSumSTA.str());
    
  }
  
  
  for(int p=0; p<m_stationMax; p++){
    for(int k=0; k<m_regionMax; k++){
   
      if(m_detType=="IT"){
	if(p==0) m_station="IT1";
	if(p==1) m_station="IT2";
	if(p==2) m_station="IT3"; 
  
	if(k==0) m_detRegion="Cryo";
	if(k==1) m_detRegion="Access";
	if(k==2) m_detRegion="Bottom"; 
	if(k==3) m_detRegion="Top"; 
   
      }
      if(m_detType=="TT"){
	if(p==0) m_station="TTa";
	if(p==1) m_station="TTb";
  
	if(k==0) m_detRegion="Cryo";
	if(k==1) m_detRegion="BeamPipe";
	if(k==2) m_detRegion="Access"; 
 
      }
      
      
      
      // Per box for IT, and detector region for TT
      std::ostringstream nameClusterSizeBOX; 
      nameClusterSizeBOX <<  m_detType  <<"-Cluster_Size/" << m_station  << "/" << m_detRegion << m_TAEtag;       
      CLUSTERSIZEBOX[p][k] = m_histosvc->book( nameClusterSizeBOX.str(), nameClusterSizeBOX.str(), 7, -0.5,  6.5);
      declareInfo( nameClusterSizeBOX.str(),  CLUSTERSIZEBOX[p][k] , nameClusterSizeBOX.str());
      
      
      std::ostringstream nameTotalChargeBOX; 
      nameTotalChargeBOX <<  m_detType  <<"-Total_Charge/" << m_station  << "/" << m_detRegion  << m_TAEtag;      
      TOTALCHARGEBOX[p][k] = m_histosvc->book( nameTotalChargeBOX.str(), nameTotalChargeBOX.str(), 300, 0,  300);
      declareInfo( nameTotalChargeBOX.str(),  TOTALCHARGEBOX[p][k] , nameTotalChargeBOX.str());

 
 
      std::ostringstream nameHighThresholdBOX; 
      nameHighThresholdBOX <<  m_detType  <<"-High_Threshold/" << m_station  << "/" << m_detRegion  << m_TAEtag;      
      HIGHTHRESHOLDBOX[p][k] = m_histosvc->book( nameHighThresholdBOX.str(), nameHighThresholdBOX.str(), 2, -0.5,  1.5);
      declareInfo( nameHighThresholdBOX.str(),  HIGHTHRESHOLDBOX[p][k] , nameHighThresholdBOX.str());      
      
 
 
      std::ostringstream nameNeighbourSumBOX; 
      nameNeighbourSumBOX <<  m_detType  <<"-Neighbour_Sum/" << m_station  << "/" << m_detRegion << m_TAEtag;        
      NEIGHBOURSUMBOX[p][k] = m_histosvc->book( nameNeighbourSumBOX.str(), nameNeighbourSumBOX.str(), 43, -16.5,  26.5);
      declareInfo( nameNeighbourSumBOX.str(),  NEIGHBOURSUMBOX[p][k] , nameNeighbourSumBOX.str());
 
 
      std::ostringstream nameRelNeighbourSumBOX; 
      nameRelNeighbourSumBOX <<  m_detType  <<"-Rel_Neighbour_Sum/" << m_station  << "/" << m_detRegion  << m_TAEtag;  
      RELNEIGHBOURSUMBOX[p][k] = m_histosvc->book( nameRelNeighbourSumBOX.str(), nameRelNeighbourSumBOX.str(), 51, -1.02,  1.02);
      declareInfo( nameRelNeighbourSumBOX.str(),  RELNEIGHBOURSUMBOX[p][k] , nameRelNeighbourSumBOX.str());
      
      
      // Occupancies
      
      for(int r=0; r<m_layerMax;r++){

	if(m_detType=="IT"){
	  if(r==0) m_layer="x1";
	  if(r==1) m_layer="u";
	  if(r==2) m_layer="v";
	  if(r==3) m_layer="x2";
	  
	  std::ostringstream nameOccupancyBOX; 
    	  nameOccupancyBOX <<  m_detType  <<"-Occupancy/" << m_station  << "/" << m_detRegion << "/Layer_" << m_layer << m_TAEtag;   
	  OCCUPANCYIT[p][k][r] = m_histosvc->book( nameOccupancyBOX.str(), nameOccupancyBOX.str(),2688,1,2689);
	  declareInfo( nameOccupancyBOX.str(),  OCCUPANCYIT[p][k][r] , nameOccupancyBOX.str());
     
	}
 
	if(m_detType=="TT"){
	  if(p==0 && r==0) m_layer="x1";
	  if(p==0 && r==1) m_layer="u";
	  if(p==1 && r==0) m_layer="v";
	  if(p==1 && r==1) m_layer="x2";
	  if((p==0 || p==1) && (k==0 || k==2)) m_sectorMax=24;
	  if(p==0 && k==1) m_sectorMax=18;
	  if(p==1 && k==1) m_sectorMax=26; 
	    
	    
	  for(int kSector=0; kSector<m_sectorMax;kSector++){  
	
	    std::ostringstream nameOccupancyBOX; 
	    nameOccupancyBOX <<  m_detType  <<"-Occupancy/" << m_station  << "/" << m_detRegion << "/Layer_" << m_layer <<
	      "/Sector" << kSector+1 << m_TAEtag; 
  
	    OCCUPANCYTT[p][k][r][kSector] = m_histosvc->book( nameOccupancyBOX.str(), nameOccupancyBOX.str(),512,1,513);
	    declareInfo( nameOccupancyBOX.str(),  OCCUPANCYTT[p][k][r][kSector] , nameOccupancyBOX.str());
    
	  }	
	}  
      } 
    }
  }
  
  
  
 
  // Charge per service box (IT only)
 
  if(m_detType=="IT"){
  
  
    const std::vector<std::string>& boxes = m_readoutTool->serviceBoxes();
 
    for(unsigned int i=0; i<boxes.size(); i++){
   
 
      std::ostringstream nameTotChargeSvcBox; 
      nameTotChargeSvcBox << m_detType  <<"-Tot_Charge_SvceBox/" << boxes[i] << m_TAEtag;   
      TOTALCHARGESVCEBOX[i] = m_histosvc->book( nameTotChargeSvcBox.str(), nameTotChargeSvcBox.str(), 300, 0,  300);
      declareInfo( nameTotChargeSvcBox.str(), TOTALCHARGESVCEBOX[i] , nameTotChargeSvcBox.str());
   
      m_SvceBoxMap[boxes[i]]=i;
   
    }
  }


  
  // Cluster maps
 
  std::ostringstream nameClusterMap; 
  nameClusterMap <<  m_detType  <<"-Cluster_Map" << m_TAEtag;
   
  std::ostringstream nameClusterMapPerLayerTT11; 
  nameClusterMapPerLayerTT11 <<  m_detType  <<"-Cluster_Map_perLayer/TTaX1" << m_TAEtag; 
  std::ostringstream nameClusterMapPerLayerTT12; 
  nameClusterMapPerLayerTT12 <<  m_detType  <<"-Cluster_Map_perLayer/TTaU" << m_TAEtag; 
  std::ostringstream nameClusterMapPerLayerTT21; 
  nameClusterMapPerLayerTT21 <<  m_detType  <<"-Cluster_Map_perLayer/TTbV" << m_TAEtag; 
  std::ostringstream nameClusterMapPerLayerTT22; 
  nameClusterMapPerLayerTT22 <<  m_detType  <<"-Cluster_Map_perLayer/TTbX2" << m_TAEtag; 
 
   
   
   
  if(m_detType=="IT"){
  
    CLUSTERMAPIT = m_histosvc->book( nameClusterMap.str(), nameClusterMap.str(), 252, 1,  253, 38,1,39);
    declareInfo( nameClusterMap.str(),  CLUSTERMAPIT , nameClusterMap.str());
    
  }
  if(m_detType=="TT"){
    CLUSTERMAPTT = m_histosvc->book( nameClusterMap.str(), nameClusterMap.str(),100,-1000.,1000., 100,-1000.,1000.);
    declareInfo( nameClusterMap.str(),  CLUSTERMAPTT , nameClusterMap.str());
     
    CLUSTERMAPTTSta1Lay1 = m_histosvc->book( nameClusterMapPerLayerTT11.str(), nameClusterMapPerLayerTT11.str(),100,-1000.,1000., 100,-1000.,1000.);
    declareInfo( nameClusterMapPerLayerTT11.str(),  CLUSTERMAPTTSta1Lay1 , nameClusterMapPerLayerTT11.str());
    CLUSTERMAPTTSta1Lay2 = m_histosvc->book( nameClusterMapPerLayerTT12.str(), nameClusterMapPerLayerTT12.str(),100,-1000.,1000., 100,-1000.,1000.);
    declareInfo( nameClusterMapPerLayerTT12.str(),  CLUSTERMAPTTSta1Lay2 , nameClusterMapPerLayerTT12.str());
    CLUSTERMAPTTSta2Lay1 = m_histosvc->book( nameClusterMapPerLayerTT21.str(), nameClusterMapPerLayerTT21.str(),100,-1000.,1000., 100,-1000.,1000.);
    declareInfo( nameClusterMapPerLayerTT21.str(),  CLUSTERMAPTTSta2Lay1 , nameClusterMapPerLayerTT21.str());
    CLUSTERMAPTTSta2Lay2 = m_histosvc->book( nameClusterMapPerLayerTT22.str(), nameClusterMapPerLayerTT22.str(),100,-1000.,1000., 100,-1000.,1000.);
    declareInfo( nameClusterMapPerLayerTT22.str(),  CLUSTERMAPTTSta2Lay2 , nameClusterMapPerLayerTT22.str());
 
  }
 
 
  

  
  
  
  return StatusCode::SUCCESS;
 
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode STZSMonitor::execute() {

  debug() << "==> Execute " << endmsg;
  evt_number->fill(1);
 
  ++m_evtCounter;
  
  debug() << "Event Counter: " << m_evtCounter << endmsg;
  
  if(m_evtCounter<=m_skippedEvents){
    debug() << "Event number: " << m_evtCounter << ", skip event for pedestal convergence" << endmsg;
    return StatusCode::SUCCESS;
  }
  
 
  
  
  const STClusters* ClusterContainer = getIfExists< STClusters>(m_inputLocation);
  if( NULL != ClusterContainer ){
 
    STClusters::const_iterator Begin(ClusterContainer -> begin()), End(ClusterContainer -> end()); 
  
    m_TotalCharge=0;
  
  
    if(ClusterContainer->size()==0)   CLUSTERCHARGESUM->fill(0);
    else CLUSTERCHARGESUM->fill(m_TotalCharge/float(ClusterContainer->size()));
  
    CLUSTERCOUNT->fill(ClusterContainer->size());

  
    for(STClusters::const_iterator it(Begin); it != End; it++){
  
  
      // Exclusion of first and last strip of every module (very noisy)
      
      // first strip
      if(m_maskFirstChannel && (((*it)->channelID().strip()-1) % 32==0)) continue; 
      // last strip for IT
      if(m_maskLastChannel && m_detType=="IT" &&  (((*it)->channelID().strip()-1) % 384==383))	continue;
      // last strip for TT
      if(m_maskLastChannel && m_detType=="TT" &&  (((*it)->channelID().strip()-1) % 512==511))	continue;

    
      const int iStation = (*it)->channelID().station();
      const int iBox = (*it)->channelID().detRegion(); 
      const int iLayer = (*it)->channelID().layer();
      const int iSector = (*it)->channelID().sector();
      const int iStrip = (*it)->channelID().strip();
    
      // Cluster maps for TT
      if(m_detType=="TT"){
    
	const DeSTSector* aSector = m_tracker->findSector((*it)->channelID());
	std::auto_ptr<Trajectory> tmpTraj = aSector->trajectory((*it)->channelID() , (*it)->interStripFraction() ); 
       
	Gaudi::XYZPoint center = tmpTraj->position( 0.5*( tmpTraj->beginRange()+tmpTraj->endRange())) ;
      
        // normal cluster map
	CLUSTERMAPTT->fill(center.x(),center.y());
      
        // Cluster map per layer
	if(iStation==1 && iLayer == 1)	  CLUSTERMAPTTSta1Lay1->fill(center.x(),center.y());
	if(iStation==1 && iLayer == 2)	  CLUSTERMAPTTSta1Lay2->fill(center.x(),center.y());
	if(iStation==2 && iLayer == 1)	  CLUSTERMAPTTSta2Lay1->fill(center.x(),center.y());
	if(iStation==2 && iLayer == 2)	  CLUSTERMAPTTSta2Lay2->fill(center.x(),center.y());   
    
      }
 
 
      // cluster map for IT
      if(m_detType=="IT"){
	int originx=0;
	int originy=0;
	int distToOriginx=0;
        
	// origin points calculation (lower left corner of each box)
	if(iBox==1){// box c-side 
	  originx=168; 
	  originy=(iStation-1)*13+4;
	}
	if(iBox==3 ){// box bottom
	  originx=84;
	  originy=(iStation-1)*13;
	}
	if(iBox==4 ){// box top
	  originx=84;
	  originy=(iStation-1)*13+8;
	}
	if(iBox==2){// box a-side
	  originx=0;
	  originy=(iStation-1)*13+4;
	}
 
 
     
	// Calculation of distance of port hit by cluster from origin point
      
	distToOriginx=84-((iSector-1)*384+iStrip-1)/32;
      

	const int iXcoord = originx+distToOriginx;
	const int iYcoord= iLayer+originy;
 

  
	CLUSTERMAPIT->fill(iXcoord,iYcoord,(*it)->totalCharge());
      
      }
    
 
  
      // specific histograms for full detector
      CLUSTERSIZE->fill((*it)->size());
      HIGHTHRESHOLD->fill((*it)->highThreshold());
      TOTALCHARGE->fill((*it)->totalCharge());
      if ((*it)->size() < 3) {
	NEIGHBOURSUM->fill((*it)->neighbourSum());
	RELNEIGHBOURSUM->fill((*it)->neighbourSum()/(*it)->totalCharge());
      }
 
 
      // specific histograms per station
      CLUSTERSIZESTA[iStation-1]->fill((*it)->size());
      HIGHTHRESHOLDSTA[iStation-1]->fill((*it)->highThreshold());
      TOTALCHARGESTA[iStation-1]->fill((*it)->totalCharge());
      if ((*it)->size() < 3) {
	NEIGHBOURSUMSTA[iStation-1]->fill((*it)->neighbourSum());
	RELNEIGHBOURSUMSTA[iStation-1]->fill((*it)->neighbourSum()/(*it)->totalCharge());
      }
 
      // specific histograms per box (IT) and detector region (TT)
      CLUSTERSIZEBOX[iStation-1][iBox-1]->fill((*it)->size());
      HIGHTHRESHOLDBOX[iStation-1][iBox-1]->fill((*it)->highThreshold());
      TOTALCHARGEBOX[iStation-1][iBox-1]->fill((*it)->totalCharge()); 
      if ((*it)->size() < 3) {
	NEIGHBOURSUMBOX[iStation-1][iBox-1]->fill((*it)->neighbourSum());
	RELNEIGHBOURSUMBOX[iStation-1][iBox-1]->fill((*it)->neighbourSum()/(*it)->totalCharge());
      }
 
 
      // Occupancies
      if(m_detType=="IT")	OCCUPANCYIT[iStation-1][iBox-1][iLayer-1]->fill((iSector-1)*384+iStrip);
      if(m_detType=="TT"){
        if(iBox == 1)	OCCUPANCYTT[iStation-1][iBox-1][iLayer-1][iSector-1]->fill(iStrip);
        else warning() << "Invalid box for TT: " << iBox << endmsg; 
      }


     // Charge per service box (IT only)
     if(m_detType=="IT"){
	STTell1Board* board = m_readoutTool->findByBoardID(STTell1ID((*it)->sourceID()));
	std::string SvceBox = board->serviceBox((*it)->channelID());//(*it)->channelID()
	TOTALCHARGESVCEBOX[m_SvceBoxMap[SvceBox]]->fill((*it)->totalCharge());
      }
 
    
  
    }
  
  
 
 
  }

  return StatusCode::SUCCESS;
} 

//=============================================================================
