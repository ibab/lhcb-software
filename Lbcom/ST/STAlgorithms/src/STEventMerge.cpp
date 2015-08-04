
// LHCbKernel includes
#include "Kernel/STChannelID.h"
#include "Kernel/ISTReadoutTool.h"
#include "Event/STCluster.h"
#include "STEventMerge.h"
#include "Kernel/STDataFunctor.h"
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STEventMerge )

STEventMerge::STEventMerge( const std::string& name,
                                    ISvcLocator* pSvcLocator):
  ST::AlgBase(name, pSvcLocator),
  m_inputLocation(STClusterLocation::TTClusters){
  
  declareProperty("spills", m_spillsVector);
  declareSTConfigProperty("clusterLocation", m_clusterLocation ,"/Event/Raw/TT/MergedSpills");

  addToFlipList(&m_inputLocation);

}

STEventMerge::~STEventMerge()
{
  // STEventMerge destructor
}

StatusCode STEventMerge::initialize()
{
  StatusCode sc = ST::AlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
 
  std::vector<std::string>::const_iterator iSpillName = m_spillsVector.begin();
  while( iSpillName != m_spillsVector.end()) {
    m_spillPath.push_back("/Event"+(*iSpillName)+m_inputLocation);
    ++iSpillName;
  } // spillnames
  
  return StatusCode::SUCCESS;
}


StatusCode STEventMerge::execute()
{

  
  STClusters* fCont = new STClusters();
  fCont->reserve(5000);  
  put(fCont, m_clusterLocation);

  std::vector<std::string>::const_iterator iterS = m_spillPath.begin();
  for ( ;iterS != m_spillPath.end(); ++iterS){
    STClusters* clusterCont = get<STClusters>(*iterS);
    STClusters::iterator iterC = clusterCont->begin();
    for (; iterC != clusterCont->end() ; ++iterC){

      std::vector<STCluster*> clusters;
      overlappingClusters(fCont,*iterC,clusters);   

      if (clusters.empty() == true){ 
	// not in the container insert     
        fCont->insert((*iterC)->clone(),(*iterC)->channelID());
      }
      else {
        // if this is a better cluster take it...
	// first pick the best in the container
        STCluster* bestCluster = selectBestCluster(clusters,(*iterC));
	if ((*iterC)->totalCharge() > bestCluster->totalCharge()){
	  fCont->remove(bestCluster);
          fCont->insert((*iterC)->clone(),(*iterC)->channelID()); 
	}
      }
    }
  } // spills 
   
  // sort 
  std::sort(fCont->begin(),fCont->end(), STDataFunctor::Less_by_Channel<const STCluster*>());

  return StatusCode::SUCCESS;
}

STCluster* STEventMerge::selectBestCluster(const std::vector<STCluster*>& clusters, const STCluster* testClus) const{

  if (testClus->key() == clusters.front()->key()) return clusters.front();

  STCluster* bestC = 0; double bestCharge = -100;
  for (std::vector<STCluster*>::const_iterator iterC = clusters.begin(); iterC != clusters.end(); ++iterC){
    if ((*iterC)->totalCharge() > bestCharge) {
      bestCharge = (*iterC)->totalCharge();
      bestC = *iterC;
    }
  } // iterC

  return bestC;
}

void STEventMerge::overlappingClusters(const STClusters* inCont, 
                                       const STCluster* testClus,
                                       std::vector<STCluster*>& outCont) const{

  // test if a cluster with the same key exists
  STCluster* aCluster = inCont->object(testClus->key());
  if (aCluster != 0){
    outCont.push_back(aCluster);
  }

  // get list of channels to test
  std::vector<STChannelID> chans = testClus->channels();
  for (std::vector<STChannelID>::const_iterator iterC = chans.begin(); 
       iterC != chans.end(); ++iterC ){
    STCluster* aCluster2 = inCont->object(*iterC);
    if (aCluster2 != 0){ 
      if (spillDifference(testClus,aCluster2) == 1){
      // overlapping clusters from 'spillover'
        outCont.push_back(aCluster2);
      }
    }
  } // iterC

  //  std::unique(outCont.begin(), outCont.end());
  
}

unsigned int STEventMerge::spillDifference(const STCluster* clus1, const STCluster* clus2 ) const{ 
  return abs(clus1->spill() - clus2->spill());
} 

