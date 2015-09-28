// DigiEvent
#include "Event/STCluster.h"
#include "Event/STSummary.h"

// STDet
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

// from LHCbKernel

#include "Kernel/STDetSwitch.h"
#include "TsaKernel/Line.h"
// local
#include "STCosmicFilter.h"
#include "Kernel/ITNames.h"

// xml geometry
#include "STDet/DeSTDetector.h"
#include "Kernel/Trajectory.h"
#include "Event/RawEvent.h"
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STCosmicFilter )

//--------------------------------------------------------------------
//
//  STCosmicFilter : Monitoring class for the STClusters
//
//--------------------------------------------------------------------

STCosmicFilter::STCosmicFilter( const std::string& name, 
                                    ISvcLocator* pSvcLocator ) :
  ST::HistoAlgBase(name, pSvcLocator)
{
  // constructer

  declareSTConfigProperty("InputData", m_clusterLocation , STClusterLocation::TTClusters);
  declareSTConfigProperty("summaryLocation", m_summaryLocation , STSummaryLocation::TTSummary);
  declareProperty("chargeThreshold", m_chargeThreshold = 12.0);
  declareProperty("highThreshold", m_highChargeThreshold = 15.0);
  declareProperty("maxClusters", m_maxClusters = 1000);
  declareProperty("nClus1", m_nClus1 = 50);
  declareProperty("nClus2", m_nClus2 = 7);
  declareProperty("xWindow", m_xWindow = 3.0);
}

STCosmicFilter::~STCosmicFilter() { }

StatusCode STCosmicFilter::initialize()
{
  if( "" == histoTopDir() ) setHistoTopDir(detType()+"/");

  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  return StatusCode::SUCCESS;
}

StatusCode STCosmicFilter::execute()
{

  const STClusters* clusterCont = get<STClusters>(m_clusterLocation);

  
  // get only the good quality clusters.
  std::vector<STCluster*> goodClusters;  goodClusters.reserve(100);
  
  // number of digits
  plot((double)clusterCont->size(),1,"Number of clusters", 0.,5000., 500);

  // histograms per cluster
  STClusters::const_iterator iterClus = clusterCont->begin();
  for( ; iterClus != clusterCont->end(); ++iterClus){
    if ((*iterClus)->totalCharge() > m_chargeThreshold){
      goodClusters.push_back(*iterClus); 
    }
  } // loop clusters

  // number of digits
  plot((double)goodClusters.size(),2,"Number of clusters (high)", 0.,500., 500);

  setFilterPassed(false);
  if (goodClusters.size() == 0 || goodClusters.size() > m_maxClusters) return StatusCode::SUCCESS;
  
  // count number of clusters per station
  unsigned int boxes[3][4][4][50];
  for (unsigned int is = 0 ; is < 3 ; ++is){
    for (unsigned int ib = 0 ; ib < 4; ++ib) {
      for (unsigned int il = 0; il < 4; ++il){
        for (unsigned int t = 0; t < 50; ++t){
          boxes[is][ib][il][t] = 0;
	} // spills
      } // layers
    } // boxes
  } // stations


  for (std::vector<STCluster*>::const_iterator iter = goodClusters.begin();
    iter != goodClusters.end(); ++iter){
    const STChannelID chan = (*iter)->channelID();
    ++boxes[chan.station() - 1][chan.detRegion() - 1][chan.layer() - 1][(*iter)->spill() + 20];  
  }


  bool hasT[3] = {false ,false, false};
  for (unsigned int iS = 0  ; iS < 3; ++iS ){
    for (unsigned int ib = 0; ib < 4; ++ib){
      for (unsigned int t = 0; t < 50; ++t) {
        unsigned int nlayer = 0;
        for (unsigned int il = 0; il < 4; ++il){
  	  if (boxes[iS][ib][il][t] > 0) ++nlayer;
	} // il
        if (nlayer > 2) hasT[iS] = true;
      } // t
    }
  }

  bool select = false; 
  if (hasT[0] && hasT[1]) select = true;
  if (hasT[0] && hasT[2]) select = true;
  if (hasT[1] && hasT[2]) select = true;
  if (hasT[1] || hasT[0] || hasT[2]) select = true;


  if (select || goodClusters.size() > m_nClus1){    
    if (goodClusters.size() > m_nClus2) {
      setFilterPassed(true);
      ++counter("selected");
      plot((double)goodClusters.size(),3,"Number of clusters (high)", 0.,500., 500);  
    }
    else {
      bool select2 = reselect(goodClusters);
      setFilterPassed(select2);
      if (select2) {
        ++counter("selected");
        plot((double)goodClusters.size(),3,"Number of clusters (high)", 0.,500., 500);  
      }
    }
  } 
 
  return StatusCode::SUCCESS;
}


bool STCosmicFilter::reselect(const std::vector<STCluster*>& clusters) const {

  // get the x and y
  typedef std::pair<double, double> xInfo;
  std::vector<xInfo> hits;

  for (std::vector<STCluster*>::const_iterator iter = clusters.begin();
      iter != clusters.end(); ++iter){
    if ((*iter)->totalCharge() < m_highChargeThreshold ) continue;
    const DeSTSector* aSector = tracker()->findSector((*iter)->channelID());
    auto tmpTraj = aSector->trajectory((*iter)->channelID() , (*iter)->interStripFraction() );
    hits.push_back(std::make_pair(tmpTraj->beginPoint().x(), tmpTraj->beginPoint().z()));
  }

  unsigned int bestHits = 0u;
  for (std::vector<xInfo>::const_iterator iter1 = hits.begin(); iter1 != hits.end(); ++iter1){
    std::vector<xInfo>::const_iterator iter2 = iter1; ++iter2;
    for (; iter2 != hits.end(); ++iter2){ 
      Tf::Tsa::Line xline = Tf::Tsa::Line(iter1->first, iter2->first, iter1->second, iter2->second); 
      unsigned int nWindow = 0u;
       for (std::vector<xInfo>::const_iterator iter3 = hits.begin(); iter3 != hits.end(); ++iter3){
	 const double xDiff = xline.value(iter3->second) - iter3->first;
         if (fabs(xDiff) < m_xWindow) ++nWindow;
       }

       if (nWindow > bestHits ){
	 bestHits = nWindow; 
       } 
    } // iter2
  } // iter1 


  return bestHits > 2 ? true: false ;

}




