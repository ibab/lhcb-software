#include <algorithm>

// local
#include "STDigitsToSTTELL1Data.h"

// Event
#include "Event/STTELL1Data.h"
#include "Event/STDigit.h"


#include "Kernel/LHCbConstants.h"
#include "Kernel/STTell1Board.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STDAQDefinitions.h"


#include "STDet/DeSTDetector.h"


using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : RawBufferToSTClusterAlg
//
// 2004-01-07 : Matthew Needham   
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( STDigitsToSTTELL1Data )

STDigitsToSTTELL1Data::STDigitsToSTTELL1Data( const std::string& name,
                                           ISvcLocator* pSvcLocator ):
ST::AlgBase (name , pSvcLocator)
{

 // Standard constructor, initializes variables
 declareSTConfigProperty( "outputLocation", m_outputLocation , STTELL1DataLocation::TTSubPeds);
 declareSTConfigProperty( "inputLocation", m_inputLocation , STDigitLocation::TTDigits);
 
}

STDigitsToSTTELL1Data::~STDigitsToSTTELL1Data() {
  // Destructor
}
    
StatusCode STDigitsToSTTELL1Data::execute() {

  // Retrieve the digits
  const STDigits* digitCont = get<STDigits>(m_inputLocation);

  // make a new digits container
  STTELL1Datas* outCont = new STTELL1Datas();
  
  StatusCode sc = createTell1Data(digitCont,outCont);
  
  if (sc.isFailure()) {
    delete outCont;
    return Warning("Problems creating Tell1 data", StatusCode::FAILURE, 1);
  }
  
  put(outCont, m_outputLocation);
  return StatusCode::SUCCESS;
}


StatusCode STDigitsToSTTELL1Data::createTell1Data(const STDigits* digits, STTELL1Datas* outCont) const {

  if (digits->size() != tracker()->nStrip()) {
    return Warning("Digit cont size does not equal number of detector strips",StatusCode::SUCCESS, 1);
  }

  // make correct number of output containers
  for (unsigned int i = 0; i < readoutTool()->nBoard(); ++i){
    STTell1Board* board = readoutTool()->findByOrder(i);
    STTELL1Data::Data dataVec;
    dataVec.resize(STDAQ::noptlinks);
    for(STTELL1Data::Data::iterator j=dataVec.begin(); j!= dataVec.end(); ++j)
      j->resize(LHCbConstants::nStripsInBeetle);
    STTELL1Data* tell1Data = new STTELL1Data(dataVec);
    int key = (int)board->boardID().id();
    outCont->insert(tell1Data,key);
  }  //nBoard

  // then its just one big loop
  for (STDigits::const_iterator iterD = digits->begin(); iterD != digits->end(); ++iterD){
     STDAQ::chanPair aPair = readoutTool()->offlineChanToDAQ((*iterD)->channelID(),0.0);
     STTELL1Data* adcBank = outCont->object(aPair.first.id());
     STTELL1Data::Data& dataVec = adcBank->data();
     const unsigned int beetle = aPair.second/LHCbConstants::nStripsInBeetle;
     const unsigned int strip = aPair.second%LHCbConstants::nStripsInBeetle;
     dataVec[beetle][strip] = int((*iterD)->depositedCharge());
  } // iterD

  return StatusCode::SUCCESS;
}


