// $Id: LbAppInit.cpp,v 1.9 2005-03-18 16:23:45 cattanem Exp $

// Include files
#include "LbAppInit.h"
#include "GaudiKernel/Memory.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "AIDA/IHistogram1D.h"
#include "Event/EventHeader.h"
#include "Event/ProcStatus.h"
#include "Tools/INormalizeTool.h"
#include "DetDesc/DataStoreLoadAgent.h"

#include "boost/format.hpp"

//----------------------------------------------------------------------------
// Implementation of class :  LbAppInit
//-----------------------------------------------------------------------------

static const AlgFactory<LbAppInit>  s_factory;
const IAlgFactory& LbAppInitFactory = s_factory;

//-----------------------------------------------------------------------------
LbAppInit::LbAppInit( const std::string& name, ISvcLocator* pSvcLocator )
                                    : GaudiAlgorithm( name, pSvcLocator ) { 
  m_eventCounter = 0;
  m_engine       = 0;
  m_hMemVirtual  = 0;
  
  declareProperty( "FirstEventNumber", m_firstEvent = 1     );
  declareProperty( "RunNumber",        m_runNumber  = 1     );
  declareProperty( "doHistos",         m_doHistos   = true  );
  declareProperty( "SkipFactor",       m_skipFactor = 0     );
  declareProperty( "SingleSeed",       m_singleSeed = false );
  declareProperty( "PreloadGeometry",  m_preloadGeom= false );
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
LbAppInit::~LbAppInit() { }
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
StatusCode LbAppInit::initialize() { 
//-----------------------------------------------------------------------------

  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) return Error( "Failed to initialize base class", sc );
  
  const char* pver = getenv("APPVERSION");
  if( NULL == pver ) {
    return Error( "APPVERSION environment variable not defined" );
  }

  // Get the current time
  time_t t;
  std::time( &t );
  tm* localt = std::localtime( &t );
  
  always() 
    << std::endl
    << "=================================================================="
    << "=================================================================="
    << std::endl
    << "                                "
    << "                   Welcome to " << name() << " version " << pver
    << std::endl
    << "                                "
    << "          running on " << System::hostName()
    << " on " << std::asctime( localt )
    << std::endl
    << "=================================================================="
    << "=================================================================="
    << endmsg;
  
  // Get the random number engine
  m_engine = randSvc()->engine();
  if( 0 == m_engine ) {
    return Error( "Random number engine not found!" );
  }
  else {
    std::vector<long> seeds;
    seeds.push_back( 12345678 );
    seeds.push_back( 0 );
    m_engine->setSeeds( seeds );
  }

  if( m_singleSeed ) {
    warning() << "Using only one 24 bit random number seed" << endmsg;
  }
  
  if( 0 != m_skipFactor ) {
    info() << "Skipping " << m_skipFactor 
           << " random numbers before each event" << endmsg;
  }
  
  // Book the monitoring histograms
  if( m_doHistos ) {
    IProperty* appMgrP = svc<IProperty>( "ApplicationMgr" );
    IntegerProperty numBins;
    numBins.assign( appMgrP->getProperty( "EvtMax" ) );
    release( appMgrP );

    if( -1 == numBins ) numBins = 500;
    m_hMemVirtual = histoSvc()->book( name(), 2, "Virtual memory (kB)",
                                      numBins, 0.5, numBins+0.5 );
  }

  if( m_preloadGeom ) {
    DataStoreLoadAgent *loadAgent = new DataStoreLoadAgent();
    IDataManagerSvc *dataMgr = svc<IDataManagerSvc>("DetectorDataSvc", true);
    info() << "Preloading detector geometry..." << endmsg;
    dataMgr->traverseTree(loadAgent);
    delete loadAgent;
  }
  
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
StatusCode LbAppInit::execute() {
//-----------------------------------------------------------------------------

  // Debug mode: print memory at each event
  debug() << "At event " << m_eventCounter+1 << ", memory usage is " 
          << System::virtualMemory() << endreq;  

  // Fill the memory usage histograms before the next event is loaded
  if( m_doHistos ) {
    long mem = System::virtualMemory();
    m_hMemVirtual->fill( m_eventCounter+1, (double)mem );
  }

  // Retrieve the event header if it exists, otherwise create it
  unsigned int eventNumber;
  if( exist<EventHeader>( EventHeaderLocation::Default ) )
  {
    EventHeader* evt = get<EventHeader>( EventHeaderLocation::Default );
    eventNumber = evt->evtNum();
    m_runNumber = evt->runNum();
  }
  else {
    eventNumber = m_firstEvent + m_eventCounter;
    EventHeader* newEvt = new EventHeader;
    newEvt->setEvtNum( eventNumber );
    newEvt->setRunNum( m_runNumber );
    put( newEvt, EventHeaderLocation::Default );
  }
  ++m_eventCounter;

  info() << "Evt " << eventNumber << ",  Run " << m_runNumber 
         << ",  Nr. in job = " << m_eventCounter << endmsg;

  // Initialise the random number seed  
  initRndmNum( eventNumber, m_runNumber );

  // Create a ProcStatus object if it does not exist
  this->createProcStatus();

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
StatusCode LbAppInit::finalize() { 
//-----------------------------------------------------------------------------

  always()
    << std::endl
    << "=================================================================="
    << "=================================================================="
    << std::endl
    << "                                                 "
    << m_eventCounter << " events processed" << std::endl
    << "=================================================================="
    << "=================================================================="
    << endmsg;

  return GaudiAlgorithm::finalize();
}

//-----------------------------------------------------------------------------
void LbAppInit::initRndmNum( unsigned long evt, unsigned long run ) {
//-----------------------------------------------------------------------------

  // Set the random number seeds
  std::vector<long> seeds;

  // First two seeds are run and event numbers. Skip if only one seed wanted
  if( !m_singleSeed ) {
    seeds.push_back( run );
    seeds.push_back( evt );
  }
  
  // Get last seed by hashing string containing event & run nos.

  std::string s = name() + boost::io::str( boost::format( "_evt_%1%_run_%2%" )
    % boost::io::group( std::setfill('0'), std::hex, std::setw(8), evt )
    % boost::io::group( std::setfill('0'), std::hex, std::setw(8), run ) );
  
  //--> Hash32 algorithm from Pere Mato
  long hash = 0;
  for( std::string::const_iterator iC = s.begin(); s.end() != iC; ++iC ) {
    hash += *iC; hash += (hash << 10); hash ^= (hash >> 6);
  }
  hash += (hash << 3); hash ^= (hash >> 11); hash += (hash << 15);
  //<--

  // CLHEP uses the last seed as a seed (only 24 bits used) but also to generate
  // more pseudorandom seeds to populate the "seeds" vector to its capacity of 24
  // seeds. For this generation, 31 bits are used
  seeds.push_back( abs(hash) );
  seeds.push_back( 0 );
  m_engine->setSeeds( seeds );

  // Optionally skip some random numbers
  if( 0 < m_skipFactor ) {
    int shots  = m_skipFactor;
    double sum = 0.;
    Rndm::Numbers gauss;
    gauss.initialize( randSvc() , Rndm::Gauss(0.,1.0) );
    while( 0 < --shots ) { sum += gauss() * sum ; }
  }
  
  return;
}

//-----------------------------------------------------------------------------
StatusCode LbAppInit::createProcStatus() {
//-----------------------------------------------------------------------------

  // Create a ProcStatus if it does not already exist
  if( !exist<ProcStatus>( ProcStatusLocation::Default ) ) {
    ProcStatus* procStat = new ProcStatus();
    put( procStat, ProcStatusLocation::Default );
  }

  return StatusCode::SUCCESS;
}

