// linker
#include "Linker/LinkerTool.h"

// Event
#include "Event/STCluster.h"


// local
#include "STCluster2MCHitLinker.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STCluster2MCHitLinker )

STCluster2MCHitLinker::STCluster2MCHitLinker( const std::string& name,
                                              ISvcLocator* pSvcLocator) : 
  ST::AlgBase (name,pSvcLocator),
  m_hitLocation(MCHitLocation::TT)
{

  declareSTConfigProperty("DigitASCTlocation", m_asctLocation , STDigitLocation::TTDigits + "2MCHits" );
  declareSTConfigProperty("InputData", m_inputData , STClusterLocation::TTClusters );
  declareSTConfigProperty("OutputData", m_outputData , STClusterLocation::TTClusters 
                  + "2MCHits" );
  declareProperty("AddSpillOverHits",m_addSpillOverHits = false); 
  declareProperty("Minfrac", m_minFrac = 0.2);
  declareProperty("OneRef",m_oneRef = false);
  declareSTConfigProperty("DigitLocation", m_digitLocation , STDigitLocation::TTDigits);

  // register to be flipped IT/TT
  addToFlipList(&m_hitLocation);
  addToFlipList(&m_asctLocation);

}

STCluster2MCHitLinker::~STCluster2MCHitLinker() 
{
  // destructer
}

StatusCode STCluster2MCHitLinker::execute() 
{
  // get STClusters
  const STClusters* clusterCont = get<STClusters>(m_inputData);

  // get the digits
  m_digitCont = get<STDigits>(m_digitLocation);

  // get MCParticles
  MCHits* mcHits = get<MCHits>("/Event/"+m_hitLocation);

  // create a linker
  LinkerWithKey<MCHit,STCluster> myLink( evtSvc(), msgSvc(), outputData() );

  // loop and link STClusters to MC truth
  STClusters::const_iterator iterClus = clusterCont->begin();
  for( ; iterClus != clusterCont->end(); ++iterClus){

    // find all hits
    HitMap hitMap;
    StatusCode sc = associateToTruth(*iterClus,hitMap);
    if( sc.isFailure() ) return sc;  // error message printed in failed method

    // select references to add to table
    std::vector<HitPair> selectedRefs;
    refsToRelate(selectedRefs,hitMap,mcHits);

    if (selectedRefs.empty() == false){
      if (m_oneRef == false ){
        std::vector<HitPair>::iterator iterPair = selectedRefs.begin();
        while (iterPair != selectedRefs.end()){ 
          myLink.link(*iterClus,iterPair->first,iterPair->second);
          ++iterPair;
        } //iterPair
      }
      else{
        HitPair bestPair = selectedRefs.back();
        myLink.link(*iterClus,bestPair.first,bestPair.second);
      }
    } // refsToRelate ! empty
  } // loop iterClus

  return StatusCode::SUCCESS;
}

void STCluster2MCHitLinker::refsToRelate( std::vector<HitPair>& selRefs,
                                          const HitMap& hitMap,
                                          MCHits* hits ) const
{
  // iterate over map
  HitMap::const_iterator iterMap = hitMap.begin();
  while (iterMap != hitMap.end()){
    const MCHit* aHit = iterMap->first;
    if ( (0 != aHit) && (iterMap->second > m_minFrac) ) {
      if ((m_addSpillOverHits == true)||(hits == aHit->parent())){
        selRefs.push_back(std::make_pair(aHit,iterMap->second));
      }
    }
    ++iterMap;
  } // iterMap
  return;
}

StatusCode STCluster2MCHitLinker::associateToTruth(const STCluster* aCluster,
                                                   HitMap& hitMap)
{
  // make link to truth  to MCHit from cluster
  typedef LinkerTool<STDigit, MCHit> AsctTool;
  typedef AsctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  // Use the STDigit to MCHit association
  AsctTool associator(evtSvc(), m_asctLocation);
  const Table* aTable = associator.direct();
  if (!aTable) return Error( "Failed to find " + m_asctLocation + " table", 
                             StatusCode::FAILURE);

  std::vector<STChannelID> chanVector = aCluster->channels();
  std::vector<STChannelID>::iterator iterChan = chanVector.begin();
  while (iterChan != chanVector.end()){
    STDigit* aDigit = m_digitCont->object(*iterChan);
    double foundCharge = 0.;
    if (aDigit !=0){
      Range hitsCont = aTable->relations(aDigit);
      iterator iterHit = hitsCont.begin();   
      for ( ; iterHit != hitsCont.end(); ++iterHit){
        const MCHit* aHit = iterHit->to();
        hitMap[aHit] += iterHit->weight() * aDigit->depositedCharge() ;
        foundCharge += iterHit->weight() * aDigit->depositedCharge();
      } // iterHit
      hitMap[0] += aDigit->depositedCharge() - foundCharge;
    } // aDigit
    ++iterChan;
  } // Digit

  // renormalize to 1
  HitMap::iterator iterMap = hitMap.begin();
  for ( ; iterMap != hitMap.end() ; ++iterMap){
    iterMap->second /= aCluster->totalCharge();   
  }

  return StatusCode::SUCCESS;
}
