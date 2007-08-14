
#include "TsaBaseAlg.h"

using namespace Tf::Tsa;

BaseAlg::BaseAlg(const std::string& name,
                 ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  //  declareProperty("time", m_time = true);
}

BaseAlg::~BaseAlg()
{
  // BaseAlg destructor
}

StatusCode BaseAlg::initialize()
{
  // Initializes BaseAlg at the begin of program execution.

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize");
  }

  return StatusCode::SUCCESS;
}


StatusCode BaseAlg::finalize(){

  //  if (m_time == true){
  //   info() << "*** Processed: " << m_nEvent<< endmsg;
  //    info() << "Time per event" << timer().sum()/(m_nEvent-1) << endmsg;
  // }

  return GaudiAlgorithm::finalize();
}

