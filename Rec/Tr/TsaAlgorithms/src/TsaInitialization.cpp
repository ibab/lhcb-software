// $Id: TsaInitialization.cpp,v 1.6 2007-01-17 17:47:55 cattanem Exp $
//
// This File contains the implementation of the OTClusterCreator
// C++ code for 'LHCb Tracking package(s)'
//

// Gaudo
#include "GaudiKernel/AlgFactory.h"

#include "TsaKernel/IITDataSvc.h"
#include "TsaKernel/IOTDataSvc.h"
#include "TsaKernel/ITTDataSvc.h"

#include "TsaInitialization.h"

DECLARE_ALGORITHM_FACTORY( TsaInitialization );

//_________________________________________________
// OTPattRecClusterCreator
//
// An TrClusterCreator is a Gaudi sub algorithm that creates Inner tracker Cluster 
// from the digitizations and stores them in the correct layer
// 
TsaInitialization::TsaInitialization(const std::string& name,
                                     ISvcLocator* pSvcLocator):
  TsaBaseAlg(name, pSvcLocator)
{
  // TsaInitialization constructor
  declareProperty("otDataSvcType", m_otDataSvcType = "OTDataSvc");
  declareProperty("itDataSvcType", m_itDataSvcType = "ITDataSvc");
  declareProperty("ttDataSvcType", m_ttDataSvcType = "TTDataSvc");
  declareProperty("otDataSvcName", m_otDataSvcName = "OTDataSvc");
  declareProperty("itDataSvcName", m_itDataSvcName = "ITDataSvc");
  declareProperty("ttDataSvcName", m_ttDataSvcName = "TTDataSvc");
  declareProperty("initIT", m_initIT = true);
  declareProperty("initOT", m_initOT = true);
  declareProperty("initTT", m_initTT = false);

}

TsaInitialization::~TsaInitialization()
{
  // TsaInitialization destructor
}

StatusCode TsaInitialization::initialize()
{
  // Initializes TsaInitialization at the begin of program execution.
  
  StatusCode sc = TsaBaseAlg::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize");
  }

  m_itDataSvc = tool<IITDataSvc>(m_itDataSvcType ,m_itDataSvcName);
  m_otDataSvc = tool<IOTDataSvc>(m_otDataSvcType,m_otDataSvcName);
  m_ttDataSvc = tool<ITTDataSvc>(m_ttDataSvcType,m_ttDataSvcName);

  return StatusCode::SUCCESS;
}

StatusCode TsaInitialization::execute()
{

  //  startTimer();

  StatusCode scIT = StatusCode::SUCCESS;
  StatusCode scOT = StatusCode::SUCCESS;
  StatusCode scTT = StatusCode::SUCCESS;

  // init the tools
  if (m_initIT == true) scIT = m_itDataSvc->initializeEvent();
  if (m_initOT == true) scOT = m_otDataSvc->initializeEvent();
  if (m_initTT == true) scTT = m_ttDataSvc->initializeEvent();

  //  stopTimer();

  if( scIT.isSuccess() && scOT.isSuccess() && scTT.isSuccess() ) {
    return StatusCode::SUCCESS;
  }
  else {
    return StatusCode::FAILURE;
  }
}

StatusCode  TsaInitialization::finalize(){
   
  return TsaBaseAlg::finalize();
}

