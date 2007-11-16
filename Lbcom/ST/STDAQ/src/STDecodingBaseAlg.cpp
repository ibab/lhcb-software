// $Id: STDecodingBaseAlg.cpp,v 1.4 2007-11-16 16:43:36 mneedham Exp $

#include <algorithm>


// local
#include "STDecodingBaseAlg.h"

// Event
#include "Event/RawEvent.h"
#include "Event/ByteStream.h"
#include "Event/STCluster.h"
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
  
  // readout tool
  m_readoutTool = tool<ISTReadoutTool>(m_readoutToolName,m_readoutToolName);
   
  // bank type
  m_bankType =  STDAQGeneral::stringToRawBankType(m_detType);

  // tracker
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  return StatusCode::SUCCESS;
}
    


