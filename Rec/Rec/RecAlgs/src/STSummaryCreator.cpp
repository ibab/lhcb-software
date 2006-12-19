// ST SummaryCreator block creator

// Gaudi headers
#include "GaudiKernel/AlgFactory.h"

// STAlgorithms
#include "STSummaryCreator.h"
#include "Kernel/STDetSwitch.h"

// Event
#include "Event/STDigit.h"
#include "Event/STCluster.h"
#include "Event/STSummary.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STSummaryCreator );

STSummaryCreator::STSummaryCreator(const std::string& name, ISvcLocator* pSvcLocator) :
GaudiAlgorithm(name, pSvcLocator)
{
  // constructer
  this->declareProperty("detType", m_detType = "TT");

  m_inputClusterLocation = STClusterLocation::TTClusters;
  m_inputDigitLocation = STDigitLocation::TTDigits;
  m_outputLocation = STSummaryLocation::TTSummary;

}

STSummaryCreator::~STSummaryCreator() {
  // destructer
}

StatusCode STSummaryCreator::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  STDetSwitch::flip(m_detType,m_inputClusterLocation);
  STDetSwitch::flip(m_detType,m_inputDigitLocation);
  STDetSwitch::flip(m_detType,m_outputLocation);

  return StatusCode::SUCCESS;
}

StatusCode STSummaryCreator::execute() {

  //make summary block

  // Retrieve digit 
  STDigits* digitCont = get<STDigits>(m_inputDigitLocation);
 
  // retrieve clusters
  STClusters* clusterCont = get<STClusters>(m_inputClusterLocation);
  
  // make object
  STSummary* summaryBlock = new STSummary((int)digitCont->size()
                                         ,(int)clusterCont->size());

  // register in store
  put(summaryBlock,m_outputLocation);

  return StatusCode::SUCCESS;
}


