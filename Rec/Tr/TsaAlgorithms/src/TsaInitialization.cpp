// $Id: TsaInitialization.cpp,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
//
// This File contains the implementation of the OTClusterCreator
// C++ code for 'LHCb Tracking package(s)'
//

// Gaudo
#include "GaudiKernel/AlgFactory.h"

#include "TsaKernel/IITDataSvc.h"
#include "TsaKernel/IOTDataSvc.h"

#include "TsaInitialization.h"

// factory defs
static const AlgFactory<TsaInitialization> s_factory;
const IAlgFactory& TsaInitializationFactory = s_factory;
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
  declareProperty("otDataSvc", m_otDataSvcName = "OTDataSvc");
  declareProperty("itDataSvc", m_itDataSvcName = "ITDataSvc");
  declareProperty("initIT", m_initIT = true);
  declareProperty("initOT", m_initOT = true);
  declareProperty("maxClusters", m_maxITClusters = 5000);
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

  m_itDataSvc = tool<IITDataSvc>(m_itDataSvcName);
  m_otDataSvc = tool<IOTDataSvc>(m_otDataSvcName);

  return StatusCode::SUCCESS;
}

StatusCode TsaInitialization::execute()
{

  startTimer();

  Tsa::STClusters* clusCont = get<Tsa::STClusters>(Tsa::STClusterLocation::IT);
  if (clusCont->size() > m_maxITClusters) {  
    setFilterPassed(false);
    return Warning("Too many clusters",StatusCode::SUCCESS);
  }

  // init the tools
  if (m_initIT == true) m_itDataSvc->initializeEvent();
  if (m_initOT == true) m_otDataSvc->initializeEvent();

  stopTimer();

  return StatusCode::SUCCESS;
}

StatusCode  TsaInitialization::finalize(){
   
  return TsaBaseAlg::finalize();
}

