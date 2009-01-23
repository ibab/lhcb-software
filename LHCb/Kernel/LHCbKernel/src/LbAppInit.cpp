// $Id: LbAppInit.cpp,v 1.12 2009-01-23 10:30:08 cattanem Exp $
// Include files
#include <string>
#include <vector>
#include "boost/format.hpp"

// from Gaudi
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiAlg/IGenericTool.h"

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
    m_eventCounter(0),
    m_eventMax(0),
    m_appName(""),
    m_appVersion("")
{
  declareProperty( "SkipFactor",      m_skipFactor = 0     );
  declareProperty( "SingleSeed",      m_singleSeed = false );
  declareProperty( "PreloadGeometry", m_preload    = false );
  declareProperty( "PrintFreq",       m_printFreq  = 1     );
}
//=============================================================================
// Destructor
//=============================================================================
LbAppInit::~LbAppInit() {};

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

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode LbAppInit::execute() {

  debug() << "==> Execute" << endmsg;
  ++m_eventCounter;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LbAppInit::finalize() {

  always()
    << "=================================================================="
    << endmsg;;
  always() << m_eventCounter << " events processed" << endmsg;
  always()
    << "=================================================================="
    << endmsg;

  if( (-1 != m_eventMax) && (m_eventMax != m_eventCounter) ) {
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
                               std::vector<long int>* seeds ) const
{
  if ( this->okToPrint() )
  {
    info() << "Evt " << event << ",  Run " << run
           << ",  Nr. in job = " << m_eventCounter;
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

  debug() << "using seeds " << seeds << endmsg;

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
  int seed2a = seed2 & 0x7FFFFFFF;
  int seed2b = (seed2 >> 32) & 0x7FFFFFFF;

  if( !m_singleSeed ) {
    if( 0 != seed1a ) seeds.push_back( seed1a );
    if( 0 != seed2a ) seeds.push_back( seed2a );
    if( 0 != seed2b ) seeds.push_back( seed2b );
  }
  else {
    warning() << "Using only one 24 bit random number seed" << endmsg;
  }

  // Get last seed by hashing string containing seed1 and seed2

  std::string s = name() + boost::io::str( boost::format( "_%1%_%2%" )
                                           % boost::io::group( std::setfill('0'), std::hex, std::setw(8),  seed1 )
                                           % boost::io::group( std::setfill('0'), std::hex, std::setw(16), seed2 ) );

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
};

//=============================================================================
// Return vector of condDB tags used by CondDBSvc
//=============================================================================
const std::vector<LHCb::CondDBNameTagPair> LbAppInit::condDBTags() {
  std::vector<LHCb::CondDBNameTagPair> tmp;
  m_condDBInfo->defaultTags(tmp);
  return tmp;
};
