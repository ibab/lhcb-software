// $Id: STDecodingBaseAlg.cpp,v 1.5 2008-05-06 14:40:39 mneedham Exp $

#include <algorithm>


// local
#include "STDecodingBaseAlg.h"

// Event
#include "Event/RawEvent.h"
#include "Event/ByteStream.h"
#include "Event/STCluster.h"
#include "Event/STSummary.h"
#include "Kernel/STDataFunctor.h"

#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STTell1Board.h"
#include "Kernel/STTell1ID.h"

#include "SiDAQ/SiADCWord.h"
#include "STDAQGeneral.h"
#include "SiDAQ/SiHeaderWord.h"

#include "Kernel/STDetSwitch.h"

#include "STDet/DeSTDetector.h"


using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : RawBufferToSTClusterAlg
//
// 2004-01-07 : Matthew Needham   
//-----------------------------------------------------------------------------


STDecodingBaseAlg::STDecodingBaseAlg( const std::string& name,
                                           ISvcLocator* pSvcLocator ):
GaudiAlgorithm (name , pSvcLocator){
 
 // Standard constructor, initializes variables
 declareProperty("readoutTool", m_readoutToolName = "TTReadoutTool");     
 declareProperty("detType", m_detType = "TT" );
 declareProperty("summaryLocation", m_summaryLocation = STSummaryLocation::TTSummary);
}

STDecodingBaseAlg::~STDecodingBaseAlg() {
  // Destructor
};

StatusCode STDecodingBaseAlg::initialize() {

  // Initialization
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  STDetSwitch::flip(m_detType,m_readoutToolName);
  STDetSwitch::flip(m_detType,m_summaryLocation);
  
  // readout tool
  m_readoutTool = tool<ISTReadoutTool>(m_readoutToolName,m_readoutToolName);
   
  // bank type
  m_bankType =  STDAQGeneral::stringToRawBankType(m_detType);

  // tracker
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));


  return StatusCode::SUCCESS;
}
    

void STDecodingBaseAlg::createSummaryBlock(const unsigned int nclus, const unsigned int pcn, 
                                           const bool pcnsync, const std::vector<unsigned int>& bankList) const{

  STSummary* sum = new STSummary(nclus,pcn,pcnsync,bankList);   
  put(sum, m_summaryLocation);
}
