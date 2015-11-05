
// Boost
#include "boost/format.hpp"

// from Gaudi
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiAlg/IGenericTool.h"
#include "GaudiKernel/Memory.h"

#ifndef GOD_NOALLOC
#include "GaudiObjDesc/MemoryPoolAllocatorReleaser.h"
#endif

// local
#include "Kernel/LbAppInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LbAppInit
//
// 2005-12-21 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Factory not declared here to avoid it appearing in the LHCbKernel library
// and in all component libraries depending on LHCBKernel. Moved to LHCbApps

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LbAppInit::LbAppInit( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_engine(0),
    m_randSvc(0),
    m_condDBInfo(0),
    m_evtCounter(0),
    m_eventMax(0),
    m_appName(""),
    m_appVersion("")
  , m_lastMem(0)
{
  declareProperty( "SkipFactor",      m_skipFactor = 0     );
  declareProperty( "SingleSeed",      m_singleSeed = false );
  declareProperty( "PreloadGeometry", m_preload    = false );
  declareProperty( "PrintFreq",       m_printFreq  = 1     );
  declareProperty( "PrintEventTime",  m_printTime  = false );
  declareProperty( "EvtCounter",      m_evtCounterName = "EvtCounter" );
  declareProperty( "Increment",       m_increment = 100,
                   "Number of events to measure memory");
  declareProperty( "MemoryPurgeLimit",m_memPurgeLimit = 3400 * 1000, // 3.4GB
                   "The memory threshold at which to trigger a purge of the boost pools");
  declareProperty( "MinMemoryDelta", m_minMemDelta = 16,
                   "The minimum change in memory usage (KB) to trigger a message" );
}
//=============================================================================
// Destructor
//=============================================================================
LbAppInit::~LbAppInit() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode LbAppInit::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  IService* appMgr = svc<IService>( "ApplicationMgr" );
  SmartIF<IProperty> propMgr( appMgr );
  std::string value ;
  sc = propMgr->getProperty( "EvtMax", value );
  if( sc.isFailure() ) {
    return Error( " Fatal error while retrieving Property EvtMax " );

  } else {
    m_eventMax = std::atoi(value.c_str()) ;
    always()
      << "=================================================================="
      << endmsg;
    if( -1 == m_eventMax ) {
      always() << "Requested to process all events on input file(s)" << endmsg;
    }
    else {
      always() << "Requested to process " << m_eventMax << " events" << endmsg;
    }
    always()
      << "=================================================================="
      << endmsg;
  }

  sc = propMgr->getProperty( "AppName", value );
  if( sc.isFailure() ) {
    return Error( " Fatal error while retrieving Property AppName " );
  } else {
    m_appName = value;
  }

  sc = propMgr->getProperty( "AppVersion", value );
  if( sc.isFailure() ) {
    return Error( " Fatal error while retrieving Property AppVersion " );
  } else {
    m_appVersion = value;
  }

  if( m_preload ) {
    IGenericTool* preloadTool = tool<IGenericTool>( "PreloadGeometryTool" );
    preloadTool->execute();
  }

  m_condDBInfo = svc<ICondDBInfo>("CondDBCnvSvc", true );

  // Retrieve event counter tool
  m_evtCounter = tool<IEventCounter>(m_evtCounterName);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LbAppInit::execute()
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> LbAppInit()::execute" << endmsg;

  const unsigned long long nev = eventCounter();

  if ( UNLIKELY( 0 == m_lastMem ) )
  {
    m_lastMem = System::virtualMemory();
  }
  else if ( UNLIKELY( 0 == nev%m_increment && m_increment > 0 ) )
  {
    unsigned long long mem     = System::virtualMemory();
    const    long long memDiff = (long long)(mem-m_lastMem);
    if ( UNLIKELY( std::abs(memDiff) >= m_minMemDelta ) )
    {
      info() << "Memory has changed from " << m_lastMem << " to " << mem << " KB"
             << " (" << memDiff << "KB, " << 100.*memDiff/m_lastMem << "%)"
             << " in last " << m_increment << " events" << endmsg ;
      if ( mem > m_memPurgeLimit )
      {

        Info( "Memory exceeds limit of " + std::to_string(m_memPurgeLimit) 
            + " KB -> Purging pools", StatusCode::SUCCESS, 1 ).ignore();
        releaseMemoryPools();
        mem = System::virtualMemory();
      }
      m_lastMem = mem;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LbAppInit::finalize()
{
  releaseMemoryPools();

  always()
    << "=================================================================="
    << endmsg;;
  always() << eventCounter() << " events processed" << endmsg;
  always()
    << "=================================================================="
    << endmsg;

  if( (-1 != m_eventMax) && (m_eventMax != eventCounter()) ) {
    warning()
      << "Should have processed " << m_eventMax << " events" << endmsg;
    warning()
      << "=================================================================="
      << endmsg;
  }

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
void LbAppInit::printEventRun( longlong event, int run,
                               std::vector<long int>* seeds,
                               Gaudi::Time time ) const
{
  if ( this->okToPrint() )
  {
    info() << "Evt " << event << ",  Run " << run;
    if( m_printTime ) info() << ", UTC time "
                             << time.format(false,"%Y-%m-%d %H:%M:%S")
                             << "." << time.nanoformat(6);
    info() << ",  Nr. in job = " << eventCounter();
    if( 0 != seeds ) info() << " with seeds " << *seeds;
    info() << endmsg;
  }
}

//=============================================================================

StatusCode LbAppInit::initRndm( std::vector<long int>& seeds )
{
  // Get the random number engine if not already done
  if( 0 == m_randSvc ) m_randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );
  if( 0 == m_engine  )
  {
    m_engine  = m_randSvc->engine();
    if( 0 == m_engine ) {
      return Error( "Random number engine not found!" );
    }
  }

  StatusCode sc = m_engine->setSeeds( seeds );
  if( sc.isFailure() ) return Error( "Unable to set random number seeds", sc );

  // Optionally skip some random numbers
  if( 0 < m_skipFactor )
  {
    if ( this->okToPrint() )
    {
      info() << "Skipping " << m_skipFactor << " random numbers" << endmsg;
    }
    int shots  = m_skipFactor;
    double sum = 0.;
    Rndm::Numbers gauss;
    gauss.initialize( m_randSvc , Rndm::Gauss(0.,1.0) );
    while( 0 < --shots ) { sum += gauss() * sum ; }
  }

  if(msgLevel(MSG::DEBUG)) debug() << "using seeds " << seeds << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Get the random number seeds vector to be used for this event
//=============================================================================
std::vector<long int> LbAppInit::getSeeds( unsigned int seed1, ulonglong seed2 )
{

  std::vector<long int> seeds;

  // CLHEP engne requires positive int
  int seed1a = seed1 & 0x7FFFFFFF;

  // Make two 31 bit seeds out of seed2
  int seed2a = (int)(seed2 & 0x7FFFFFFF);
  int seed2b = (int)((seed2 >> 32) & 0x7FFFFFFF);

  if( !m_singleSeed ) {
    if( 0 != seed1a ) seeds.push_back( seed1a );
    if( 0 != seed2a ) seeds.push_back( seed2a );
    if( 0 != seed2b ) seeds.push_back( seed2b );
  }
  else {
    warning() << "Using only one 24 bit random number seed" << endmsg;
  }

  // Get last seed by hashing string containing seed1 and seed2
  const std::string s = name() +
    ( boost::io::str( boost::format( "_%1%_%2%" )
                      % boost::io::group( std::setfill('0'), std::hex, std::setw(8),  seed1 )
                      % boost::io::group( std::setfill('0'), std::hex, std::setw(16), seed2 ) ) );

  //--> Hash32 algorithm from Pere Mato
  int hash = 0;
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

  return seeds;
}

//=============================================================================
// Return vector of condDB tags used by CondDBSvc
//=============================================================================
const std::vector<LHCb::CondDBNameTagPair> LbAppInit::condDBTags() {
  std::vector<LHCb::CondDBNameTagPair> tmp;
  m_condDBInfo->defaultTags(tmp);
  return tmp;
}

//=============================================================================
// Return number of the current event
//=============================================================================
long LbAppInit::eventCounter() const {
  return m_evtCounter->getEventCounter();
}

//=============================================================================
// Release unused memory in Boost memory pools
//=============================================================================
void LbAppInit::releaseMemoryPools() const
{
#ifndef GOD_NOALLOC

  const unsigned long long vmem_b = System::virtualMemory();

  if (msgLevel(MSG::DEBUG)) {
    Gaudi::MemoryPoolAllocatorReleaser::releaseMemory(debug());
  } else {
    Gaudi::MemoryPoolAllocatorReleaser::releaseMemory();
  }

  const unsigned long long vmem_a = System::virtualMemory();

  if ( msgLevel(MSG::DEBUG) && vmem_b != vmem_a )
  {
    debug() << "Memory change after pool release = "
            << (long long)(vmem_a-vmem_b) << " KB" << endmsg;
  }

#endif
}

//=============================================================================
