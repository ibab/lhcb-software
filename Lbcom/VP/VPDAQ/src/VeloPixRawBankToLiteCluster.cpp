// $Id: VeloPixRawBankToLiteCluster.cpp,v 1.2 2010-03-01 10:51:28 cocov Exp $
// Include files:
// GSL
#include "gsl/gsl_math.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Kernel
#include "Kernel/VeloPixChannelID.h"
#include "Kernel/FastClusterContainer.h"
// Event
#include "Event/VeloPixLiteCluster.h"
#include "Event/RawEvent.h"
// VeloPixelDet
#include "VeloPixDet/DeVeloPix.h"
// Local
#include "VeloPixClusterWord.h"
#include "VeloPixRawBankDecoder.h"
#include "VeloPixRawBankToLiteCluster.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VeloPixRawBankToLiteCluster
//
// 2009-12-27 : Marcin Kucharczyk
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VeloPixRawBankToLiteCluster);

//=============================================================================
// Constructor
//=============================================================================
VeloPixRawBankToLiteCluster::VeloPixRawBankToLiteCluster(const std::string& name,
                                                 ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty("RawEventLocation", m_rawEventLocation =
                  LHCb::RawEventLocation::Default);
  declareProperty("ClusterLocation", m_clusterLocation = 
                  VeloPixLiteClusterLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
VeloPixRawBankToLiteCluster::~VeloPixRawBankToLiteCluster(){};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VeloPixRawBankToLiteCluster::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  m_isVerbose = msgLevel(MSG::VERBOSE);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Execution
//=============================================================================
StatusCode VeloPixRawBankToLiteCluster::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;
  // Make new clusters container
  VeloPixLiteCluster::VeloPixLiteClusters* clusCont =
                      new VeloPixLiteCluster::VeloPixLiteClusters();
  //clusCont->reserve()//could be good to eval that...
  // Retrieve the RawEvent
  if(!exist<RawEvent>(m_rawEventLocation)){
    return Warning("Failed to find raw data", StatusCode::SUCCESS,1);
  }
  RawEvent* rawEvt = get<RawEvent>(m_rawEventLocation);
  // Decode RawBanks
  StatusCode sc = decodeRawBanks(rawEvt,clusCont);
  if(sc.isFailure()){
    return Error("Problems in decoding, event skipped", sc);
  }
  std::sort(clusCont->begin(),clusCont->end(),SiDataFunctor::Less_by_Channel< LHCb::VeloPixLiteCluster >());
  put(clusCont, m_clusterLocation);
  return sc;

};


//=============================================================================
// Decode RawBanks
//=============================================================================
StatusCode VeloPixRawBankToLiteCluster::decodeRawBanks(RawEvent* rawEvt,
                     VeloPixLiteCluster::VeloPixLiteClusters* clusCont) const
{
  const std::vector<RawBank*>& tBanks = rawEvt->banks(LHCb::RawBank::VeloPix);
  if(tBanks.size() == 0) {
    Warning("No VeloPix RawBanks found");
    return StatusCode::SUCCESS;
  }
  // VeloPixLiteCluster::VeloPixLiteClusters* clusCont_tmp;
  
  // Loop over VeloPix RawBanks  
  int nrClu = 0;
  std::vector<RawBank*>::const_iterator iterBank;
  for(iterBank = tBanks.begin(); iterBank != tBanks.end(); ++iterBank) {
    nrClu++;
    // Get sensor number
    unsigned int sensor = (*iterBank)->sourceID();
    // Decoder
    VeloPixRawBankDecoder<VeloPixClusterWord> decoder((*iterBank)->data());
    // Get version of the bank
    unsigned int bankVersion = (*iterBank)->version();
    debug() << "Decoding bank version " << bankVersion << endmsg;
    // Decode lite clusters
    VeloPixRawBankDecoder<VeloPixClusterWord>::pos_iterator iterClu =
                                                            decoder.posBegin();
    for(;iterClu != decoder.posEnd(); ++iterClu) {
      createLiteCluster(sensor,*iterClu,clusCont);
    }
  } 
  return StatusCode::SUCCESS;
}


//=============================================================================
// Create liteCluster
//=============================================================================
void VeloPixRawBankToLiteCluster::createLiteCluster(
                     unsigned int sensor,
                     VeloPixClusterWord aWord,
                     VeloPixLiteCluster::VeloPixLiteClusters* clusCont) const 
{
  LHCb::VeloPixChannelID achan;
  achan.setSensor(sensor);
  achan.setPixel(aWord.pixel());
  std::pair<unsigned int,unsigned int> xyFract;
  xyFract.first  = aWord.xFract();
  xyFract.second = aWord.yFract();
  const VeloPixLiteCluster newCluster(achan,aWord.totValue(),xyFract,
                                      aWord.hasIsLong());    
  clusCont->push_back(newCluster);

  return;
}


//============================================================================
StatusCode VeloPixRawBankToLiteCluster::finalize() {

  return GaudiAlgorithm::finalize();

}
