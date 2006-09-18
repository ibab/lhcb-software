// $Id: TsaInitialization.cpp,v 1.4 2006-09-18 13:42:59 mneedham Exp $
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

  // startTimer();

  // init the tools
  if (m_initIT == true) m_itDataSvc->initializeEvent();
  if (m_initOT == true) m_otDataSvc->initializeEvent();
  if (m_initTT == true) m_ttDataSvc->initializeEvent();

  //  stopTimer();

  return StatusCode::SUCCESS;
}

StatusCode  TsaInitialization::finalize(){
   
  return TsaBaseAlg::finalize();
}

