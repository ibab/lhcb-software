
#include "TsaBaseAlg.h"


TsaBaseAlg::TsaBaseAlg(const std::string& name,
                                     ISvcLocator* pSvcLocator):
  GaudiHistoAlg(name, pSvcLocator), 
  m_stopWatch(),
  m_nEvent(0)
{
  declareProperty("time", m_time = true);
}

TsaBaseAlg::~TsaBaseAlg()
{
  // TsaBaseAlg destructor
}

StatusCode TsaBaseAlg::initialize()
{
  // Initializes TsaBaseAlg at the begin of program execution.
  
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize");
  }



  return StatusCode::SUCCESS;
}


StatusCode  TsaBaseAlg::finalize(){
   
  if (m_time == true){
    info() << "*** Processed: " << m_nEvent<< endmsg;
    info() << "Time per event" << timer().sum()/(m_nEvent-1) << endmsg;
  }
 
  return GaudiHistoAlg::finalize();
}

