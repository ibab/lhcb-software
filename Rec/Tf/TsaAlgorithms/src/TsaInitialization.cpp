// $Id: TsaInitialization.cpp,v 1.1.1.1 2007-08-14 13:50:47 jonrob Exp $
//
// This File contains the implementation of the OTClusterCreator
// C++ code for 'LHCb Tracking package(s)'
//

// Gaudo
#include "GaudiKernel/AlgFactory.h"

#include "TsaInitialization.h"

using namespace Tf::Tsa;

DECLARE_ALGORITHM_FACTORY( Initialization );

//_________________________________________________
// OTPattRecClusterCreator
//
// An TrClusterCreator is a Gaudi sub algorithm that creates Inner tracker Cluster
// from the digitizations and stores them in the correct layer
//
Initialization::Initialization(const std::string& name,
                               ISvcLocator* pSvcLocator)
  : BaseAlg(name, pSvcLocator),
    m_otDataSvc(NULL),
    m_itDataSvc(NULL),
    m_ttDataSvc(NULL)
{
  // Initialization constructor
  declareProperty("otDataSvcType", m_otDataSvcType = "Tf::OTStationHitManager<Tf::Tsa::SeedingHit>");
  declareProperty("itDataSvcType", m_itDataSvcType = "Tf::ITStationHitManager<Tf::Tsa::SeedingHit>");
  declareProperty("ttDataSvcType", m_ttDataSvcType = "Tf::TTStationHitManager<Tf::Tsa::SeedingHit>");
  declareProperty("otDataSvcName", m_otDataSvcName = "TsaOTDataManager");
  declareProperty("itDataSvcName", m_itDataSvcName = "TsaITDataManager");
  declareProperty("ttDataSvcName", m_ttDataSvcName = "TsaTTDataManager");
  declareProperty("initIT", m_initIT = true);
  declareProperty("initOT", m_initOT = true);
  declareProperty("initTT", m_initTT = false);
}

Initialization::~Initialization()
{
  // Initialization destructor
}

StatusCode Initialization::initialize()
{
  // Initializes Initialization at the begin of program execution.
  const StatusCode sc = BaseAlg::initialize();
  if (sc.isFailure())
  {
    return Error("Failed to initialize",sc);
  }

  // trick to make sure tools are created and initialised before first event
  if ( m_initIT ) itHitMan();
  if ( m_initOT ) otHitMan();
  if ( m_initTT ) ttHitMan();

  return sc;
}

StatusCode Initialization::execute()
{

  //  startTimer();

  if ( m_initIT ) itHitMan()->prepareHits();
  if ( m_initOT ) otHitMan()->prepareHits();
  if ( m_initTT ) ttHitMan()->prepareHits();

  //  stopTimer();

  return StatusCode::SUCCESS;
}
