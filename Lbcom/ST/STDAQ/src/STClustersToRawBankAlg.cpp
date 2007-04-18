// $Id: STClustersToRawBankAlg.cpp,v 1.7 2007-04-18 12:10:07 csalzman Exp $

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// STDAQ
#include "STClustersToRawBankAlg.h"
#include "STBoardToBankMap.h"
#include "STDAQGeneral.h"
#include "ISTReadoutTool.h"
#include "STTell1ID.h"

#include "STTell1Board.h"
#include "STDAQDefinitions.h"
#include "STClusterWord.h" 
#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiADCWord.h"
#include "STDAQFunctor.h"

#include "Kernel/STDetSwitch.h"

// Event
#include "Event/STCluster.h"
#include "Event/RawEvent.h"
#include "Event/BankWriter.h"

// Kernel
#include "Kernel/STChannelID.h"

#include "gsl/gsl_math.h"

#include <algorithm>

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : STClusterToRawBankAlg
//
// 2004-08-01 : M. Needham
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( STClustersToRawBankAlg );

STClustersToRawBankAlg::STClustersToRawBankAlg( const std::string& name,
                                                      ISvcLocator* pSvcLocator )
  :GaudiAlgorithm( name, pSvcLocator ),
   m_readoutTool(0),
   m_bankMapping(0),
   m_overflow(0),
   m_maxClusterSize(4)
{
  // constructer
  declareProperty("type", m_detType = "TT");
  declareProperty("readoutTool", m_readoutToolName = "TTReadoutTool");
 
  declareProperty("clusterLocation", m_clusterLocation = STClusterLocation::TTClusters);
  declareProperty("maxClusters", m_maxClustersPerPPx = 512);

  m_bankMapping = new STBoardToBankMap();
}

// Destructor
STClustersToRawBankAlg::~STClustersToRawBankAlg() {
  // Destructor
  delete m_bankMapping;
}
 
// Finalisation.
StatusCode STClustersToRawBankAlg::finalize() {
 
  std::vector<STClustersOnBoard*>::iterator iClusVector = m_clusVectors.begin();
  while (iClusVector != m_clusVectors.end()) {
    STClustersOnBoard* tmpCont = *iClusVector;
    delete tmpCont;
    m_clusVectors.erase(iClusVector);
  } // iClusVector

  return StatusCode::SUCCESS;
};

// Initialisation.
StatusCode STClustersToRawBankAlg::initialize() {

  // initialize
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  STDetSwitch::flip(m_detType,m_clusterLocation); 
  STDetSwitch::flip(m_detType,m_readoutToolName);
  
  // tools
  m_readoutTool = tool<ISTReadoutTool>(m_readoutToolName,m_readoutToolName);
    
  // banktype
  if (configureBankType().isFailure()){
    fatal() << "unknown bank type" << endreq;
    return StatusCode::FAILURE;
  }

  // init the map
  unsigned int nBoard = m_readoutTool->nBoard();
  for (unsigned int iVal = 0; iVal<nBoard; ++iVal ){

    STTell1Board* aBoard = m_readoutTool->findByOrder(iVal);
    m_bankMapping->addEntry(aBoard->boardID(),iVal);

    STClustersOnBoard* tVec = new STClustersOnBoard(m_maxClustersPerPPx); 
    m_clusVectors.push_back(tVec);
    m_clusMap[aBoard->boardID()] = tVec;       
  } // iVal

  return StatusCode::SUCCESS;
};

StatusCode STClustersToRawBankAlg::configureBankType(){

  // configure the type of bank to write (TT or IT)
  StatusCode sc = StatusCode::SUCCESS;

  m_bankType = STDAQGeneral::stringToRawBankType(m_detType);
  if (m_bankType == RawBank::Velo){
    sc = StatusCode::FAILURE;
  }
  return sc;
}

StatusCode STClustersToRawBankAlg::execute() {

  // Retrieve the RawBank
  RawEvent* tEvent = get<RawEvent>(RawEventLocation::Default);

  // initialize this event
  initEvent();

  // get the data....
  STClusters* clusCont = get<STClusters>(m_clusterLocation);

  // group the data by banks..
  StatusCode sc = groupByBoard(clusCont);
  if ( sc.isFailure()) {
    return Error("Problems linking offline to DAQ channel",sc);
  }

  // convert to a bank and add to buffer
  unsigned int nBoard = m_readoutTool->nBoard();
  for (unsigned int iBoard = 0u; iBoard < nBoard; ++iBoard){   
    // get the data ....
    STTell1ID aBoardID = m_bankMapping->findBoard(iBoard);
    STClustersOnBoard::ClusterVector clusCont = m_clusVectors[iBoard]->clusters();

    if ( m_clusVectors[iBoard]->inOverflow() == true) ++m_overflow;

    // make the a bankwriter....
    BankWriter bWriter(bankSize(clusCont)); 

    writeBank(clusCont,bWriter,aBoardID);

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

};

void STClustersToRawBankAlg::initEvent(){

  // intialize temp bank structure each event
  for (std::vector<STClustersOnBoard*>::iterator iter = m_clusVectors.begin(); iter != m_clusVectors.end(); ++iter){

    (*iter)->clear();
  } // iter
 
  m_overflow = 0; 
}

StatusCode STClustersToRawBankAlg::groupByBoard(const STClusters* clusCont){

  // divide up the clusters by readout board
  for (STClusters::const_iterator iterClus = clusCont->begin(); 
       iterClus != clusCont->end(); ++iterClus ){

    // find the online channel and board
    STChannelID chan = (*iterClus)->channelID();
    double isf = (*iterClus)->interStripFraction();
    STDAQ::chanPair aPair = m_readoutTool->offlineChanToDAQ(chan,isf);

    if (aPair.first.id() == STTell1ID::nullBoard) {
      // screwed
      err()  << "failed to link "   
             << chan.station() 
             << " " << chan.layer()
             << " " << chan.detRegion()  
             << " " << chan.sector() << 
             endreq;
      return StatusCode::FAILURE;
    }
    else {
      ClusterMap::iterator iterMap = m_clusMap.find(aPair.first);
      if (iterMap != m_clusMap.end() ){
        STClustersOnBoard* tVec = iterMap->second;
        tVec->addCluster(*iterClus,aPair.second);

        // look for consistancy...
        //STTell1Board* board =  m_readoutTool->findByBoardID(aPair.first);
        //STChannelID chan2 = board->DAQToOffline(aPair.second);
      }
      else {
        warning() << "Failed to find board in map " << endreq;
      }
    }
    
     
  } // iterClus

  return StatusCode::SUCCESS;
}

unsigned int STClustersToRawBankAlg::bankSize(STClustersOnBoard::ClusterVector& clusCont) const{
  // bank size in 32 bit words
  // 1 short (header)
  // + n short (clusters)
  // + n char (neighbour sum)
  // + n adc * n cluster (char)
  unsigned int nClus = clusCont.size();
  unsigned int nADC = 0;
  for (unsigned int iCluster = 0; iCluster < nClus; ++iCluster){
    STCluster* aCluster = clusCont[iCluster].first;
    nADC += aCluster->size();
  }
  
  unsigned int nByte = sizeof(short) 
    +(nClus*sizeof(short))
    +(nClus*sizeof(char))
    +(nADC* sizeof(char));

 return (unsigned int)ceil(nByte/(double)sizeof(int)); 
}
 
void STClustersToRawBankAlg::writeBank(STClustersOnBoard::ClusterVector& 
                                       clusCont,
                                       LHCb::BankWriter& bWriter,
                                       const STTell1ID aBoardID)
{
  unsigned int nClus = clusCont.size();

  // make a bank header
  unsigned int pcn = 128;
  SiHeaderWord aHeader = SiHeaderWord(nClus,pcn);
  bWriter << aHeader.value();

  // pick up the data and write first half of the bank into temp container...
  for (unsigned int iCluster = 0; iCluster < nClus; ++iCluster){
    STCluster* aClus = clusCont[iCluster].first;
    STChannelID aChan = aClus->channelID();

    double isf = m_readoutTool->interStripToDAQ(aChan,
                                                aBoardID,
                                                aClus->interStripFraction());
    STClusterWord aWord = STClusterWord(clusCont[iCluster].second,
                                        isf,
                                        aClus->size(),
                                        aClus->highThreshold());
    bWriter << aWord;
  } // iCluster

  // padding
  if (nClus % 2 == 1 ){
    short padding = 0;
    bWriter << padding;
  }

  // now the second half neighbour sum and ADC
  for (unsigned int iCluster = 0; iCluster < nClus; ++iCluster){
    STCluster* aCluster = clusCont[iCluster].first;
    double tADC = GSL_MIN(GSL_MAX(aCluster->neighbourSum(),-16),15);
    char neighbourSum = (char)tADC;
    bWriter << neighbourSum;
    STCluster::ADCVector adcs = aCluster->stripValues();
    unsigned int nToWrite = GSL_MIN(aCluster->size(), m_maxClusterSize);

    //flip ADC values for rotated modules
    STChannelID channelID = aCluster->channelID();
    m_readoutTool->ADCOfflineToDAQ(channelID,aBoardID,adcs);
    
    for (unsigned int i = 0; i < nToWrite; ++i){
      bool last;
      i == nToWrite-1 ? last = true: last = false;
      SiADCWord adcWord = SiADCWord(adcs[i].second, last);
      bWriter << adcWord;  
    } //iter   
    
  } // iCluster

  return;
}
