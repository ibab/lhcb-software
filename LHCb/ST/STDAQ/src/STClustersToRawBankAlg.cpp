// STDAQ
#include "STClustersToRawBankAlg.h"
#include "Kernel/STRawBankMap.h"
#include "Kernel/STTell1ID.h"

#include "Kernel/STTell1Board.h"
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STClusterWord.h"
#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiADCWord.h"
#include "STDAQFunctor.h"

#include "Kernel/ISTReadoutTool.h"

// Event
#include "Event/STCluster.h"
#include "Event/RawEvent.h"
#include "Event/BankWriter.h"
#include "Event/STSummary.h"

// Kernel
#include "Kernel/STChannelID.h"


#include <algorithm>

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : STClusterToRawBankAlg
//
// 2004-08-01 : M. Needham
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( STClustersToRawBankAlg )

STClustersToRawBankAlg::STClustersToRawBankAlg( const std::string& name,
                                                      ISvcLocator* pSvcLocator )
  :ST::AlgBase( name, pSvcLocator ),
   m_overflow(0),
   m_maxClusterSize(4),
   m_pcn(128)
{
  // constructer

  declareSTConfigProperty("clusterLocation", m_clusterLocation , STClusterLocation::TTClusters);
  declareSTConfigProperty("summaryLocation", m_summaryLocation , STSummaryLocation::TTSummary);
  declareProperty("rawLocation", m_rawLocation = RawEventLocation::Default);
  declareProperty("maxClusters", m_maxClustersPerPPx = 512);


  setForcedInit();
}

// Destructor
STClustersToRawBankAlg::~STClustersToRawBankAlg() {
}

// Finalisation.
StatusCode STClustersToRawBankAlg::finalize() {

  m_clusVectors.clear();
  m_bankMapping.clear();
  return StatusCode::SUCCESS;
}

// Initialisation.
StatusCode STClustersToRawBankAlg::initialize() {

  // initialize
  StatusCode sc = ST::AlgBase::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  // banktype
  if (configureBankType().isFailure()){
    fatal() << "unknown bank type" << endmsg;
    return StatusCode::FAILURE;
  }

  // init the map
  unsigned int nBoard = readoutTool()->nBoard();
  m_clusVectors.reserve(nBoard);
  for (unsigned int iVal = 0; iVal<nBoard; ++iVal ){

    STTell1Board* aBoard = readoutTool()->findByOrder(iVal);
    m_bankMapping.addEntry(aBoard->boardID(),iVal);

    m_clusVectors.emplace_back(m_maxClustersPerPPx);
    m_clusMap[aBoard->boardID()] = &m_clusVectors.back();
  } // iVal

  return StatusCode::SUCCESS;
}

StatusCode STClustersToRawBankAlg::configureBankType(){

  // configure the type of bank to write (TT or IT)
  StatusCode sc = StatusCode::SUCCESS;

  m_bankType = STRawBankMap::stringToType(detType());
  if (m_bankType == RawBank::Velo){
    sc = StatusCode::FAILURE;
  }
  return sc;
}

StatusCode STClustersToRawBankAlg::execute() {

  // Retrieve the RawBank
  RawEvent* tEvent = get<RawEvent>(m_rawLocation);

  // initialize this event
  initEvent();

  // get the data....
  const STClusters* clusCont = get<STClusters>(m_clusterLocation);

  // group the data by banks..
  StatusCode sc = groupByBoard(clusCont);
  if ( sc.isFailure()) {
    return Error("Problems linking offline to DAQ channel",sc);
  }

  // convert to a bank and add to buffer
  const unsigned int nBoard = readoutTool()->nBoard();
  for (unsigned int iBoard = 0u; iBoard < nBoard; ++iBoard){
    // get the data ....
    const STTell1ID aBoardID = m_bankMapping.findBoard(iBoard);
    STClustersOnBoard::ClusterVector boardClusCont = m_clusVectors[iBoard].clusters();

    if ( m_clusVectors[iBoard].inOverflow() ) ++m_overflow;

    // make the a bankwriter....
    BankWriter bWriter(bankSize(boardClusCont));

    writeBank(boardClusCont,bWriter,aBoardID);


    RawBank* tBank = tEvent->createBank(STDAQ::rawInt(aBoardID.id()),
                                        m_bankType ,STDAQ::v4,
                                        bWriter.byteSize(),
                                        &(bWriter.dataBank()[0]));

    tEvent->adoptBank(tBank,true);

  } // iBoard

  // flag overflow
  if (m_overflow>0){
    return Warning("RAWBank overflow some banks truncated", StatusCode::SUCCESS);
  }

  return StatusCode::SUCCESS;

}

void STClustersToRawBankAlg::initEvent(){

  // intialize temp bank structure each event
  std::for_each( m_clusVectors.begin(),m_clusVectors.end(),
                 [](STClustersOnBoard& i) { i.clear(); } );
  m_overflow = 0;

  // locate and set the pcn from the summary block if it exists
  // in the case there is no summary block write 128
  const LHCb::STSummary* sum = getIfExists<LHCb::STSummary>(m_summaryLocation);
  if (sum) m_pcn = sum->pcn();

}

StatusCode STClustersToRawBankAlg::groupByBoard(const STClusters* clusCont){

  // divide up the clusters by readout board
  for (const auto& clus : *clusCont ) {

    // find the online channel and board
    auto iterMap = m_clusMap.find(STTell1ID(clus->sourceID(), detType()=="UT"));
    if (iterMap != m_clusMap.end() ){
      STClustersOnBoard* tVec = iterMap->second;
      tVec->addCluster(clus);
    } else {
     return Warning("Failed to find board in map ", StatusCode::SUCCESS);
    }
  } // clusCont
  return StatusCode::SUCCESS;
}

unsigned int STClustersToRawBankAlg::bankSize(STClustersOnBoard::ClusterVector& clusCont) const{
  // bank size in 32 bit words
  // 1 short (header)
  // + n short (clusters)
  // + n char (neighbour sum)
  // + n adc * n cluster (char)
  unsigned int nClus = clusCont.size();
  unsigned int nADC = std::accumulate( clusCont.begin(), clusCont.end(), 0u,
                                       [](unsigned n, const STClustersOnBoard::boardPair& p ) {
                                           return n + p.first->size();
                                       });

  unsigned int nByte = sizeof(short)
    +(nClus*sizeof(short))
    +(nClus*sizeof(char))
    +(nADC* sizeof(char));

 return (unsigned int)ceil(nByte/(double)sizeof(int));
}

void STClustersToRawBankAlg::writeBank(const STClustersOnBoard::ClusterVector&
                                       clusCont,
                                       LHCb::BankWriter& bWriter,
                                       const STTell1ID aBoardID)
{
  auto nClus = clusCont.size();
  // make a bank header
  SiHeaderWord aHeader = SiHeaderWord(nClus,getPCN());
  bWriter << aHeader.value();

  // pick up the data and write first half of the bank into temp container...
  for (const auto& clus : clusCont ) {
    STCluster* aClus = clus.first;
    STChannelID aChan = aClus->channelID();

    double isf = readoutTool()->interStripToDAQ(aChan,
                                                aBoardID,
                                                aClus->interStripFraction());
    bWriter << STClusterWord(clus.second,
                                        isf,
                                        aClus->size(),
                                        aClus->highThreshold());
  } // clusCont

  if ( nClus & 1 ) { // add padding if odd number of clusters
    short padding = 0;
    bWriter << padding;
  }

  // now the second half neighbour sum and ADC
  for (const auto& clus : clusCont ) {
    STCluster* aCluster = clus.first;
    // implicit double->char conversion!
    char neighbourSum = std::min(std::max(aCluster->neighbourSum(),-16.),15.);
    bWriter << neighbourSum;
    STCluster::ADCVector adcs = aCluster->stripValues();

    //flip ADC values for rotated modules
    STChannelID channelID = aCluster->channelID();
    readoutTool()->ADCOfflineToDAQ(channelID,aBoardID,adcs);

    unsigned int nToWrite = std::min(aCluster->size(), m_maxClusterSize);
    for (unsigned int i = 0; i < nToWrite; ++i){
      bool last = ( i == nToWrite-1 );
      bWriter << SiADCWord(adcs[i].second, last);
    } //iter   

  } // clusCont
}

unsigned int STClustersToRawBankAlg::getPCN() const{
  return m_pcn;
}
