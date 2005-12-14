// $Id: InitRndmTool.cpp,v 1.1 2005-12-14 13:22:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

#include "boost/format.hpp"

// local
#include "InitRndmTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : InitRndmTool
//
// 2005-12-14 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( InitRndmTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
InitRndmTool::InitRndmTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent ), m_engine(0), m_randSvc(0)
{
  declareProperty( "SkipFactor", m_skipFactor = 0     );
  declareProperty( "SingleSeed", m_singleSeed = false );

  declareInterface<IRndmTool>(this);
}

//=============================================================================
// Destructor
//=============================================================================
InitRndmTool::~InitRndmTool() {}; 

//=============================================================================
// Tool Initialization
//=============================================================================
StatusCode InitRndmTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // Initialize the base class
  if( sc.isFailure() ) return sc;          // Error message printed by base class
  
  // Get the random number engine
  m_randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );
  m_engine  = m_randSvc->engine();
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
           << " random numbers after each seed initialization" << endmsg;
  }

  return sc;
}; 

//=============================================================================
// Initialize random number
//=============================================================================
void InitRndmTool::initRndm( unsigned int seed1, ulonglong seed2 ) {

  // Interface requires long int
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
  m_engine->setSeeds( seeds );

  // Optionally skip some random numbers
  if( 0 < m_skipFactor ) {
    int shots  = m_skipFactor;
    double sum = 0.;
    Rndm::Numbers gauss;
    gauss.initialize( m_randSvc , Rndm::Gauss(0.,1.0) );
    while( 0 < --shots ) { sum += gauss() * sum ; }
  }

  debug() << "using seeds " << seeds << endmsg;

  return;
}; 

//=============================================================================
