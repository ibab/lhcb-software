// $Id: MooreInitSvc.cpp,v 1.3 2010-08-13 12:04:03 graven Exp $
// Include files

// from Gaudi
#include "GaudiKernel/SvcFactory.h"
#include "Event/ODIN.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "boost/format.hpp"

// local
#include "MooreInitSvc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MooreInitSvc
//
// 2010-??-?? : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_SERVICE_FACTORY( MooreInitSvc )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MooreInitSvc::MooreInitSvc( const std::string& name, ISvcLocator* pSvcLocator )
    : extends1<Service, IIncidentListener>( name, pSvcLocator )
    , m_odinTool( "ODINDecodeTool", this )
    , m_evtSvc( 0 )
{
}
//=============================================================================
// Destructor
//=============================================================================
MooreInitSvc::~MooreInitSvc() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode MooreInitSvc::initialize()
{
    // Tool to decode ODIN object to RawEvent
    StatusCode sc = m_odinTool.retrieve();
    if ( !sc.isSuccess() ) return sc;

    IIncidentSvc* svc( 0 );
    if ( !service( "IncidentSvc", svc ).isSuccess() ) return StatusCode::FAILURE;
    // register with beginRun.... make sure this is called
    // 'early' before eg. the rate limiters calls the rndm svc...
    // so we subscribe to the same incidents as $LOKICOREROOT/src/Scalers.cpp
    // but with a higher priority..

    bool rethrow = false;
    bool oneShot = false;
    svc->addListener( this, IncidentType::BeginRun, std::numeric_limits<long>::max(),
                      rethrow, oneShot );
    svc->addListener( this, "RunChange", std::numeric_limits<long>::max(), rethrow,
                      oneShot );
    svc->release();

    if ( !service( "EventDataSvc", m_evtSvc ).isSuccess() )
        return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
};

//=============================================================================
// Finalization
//=============================================================================
StatusCode MooreInitSvc::finalize()
{
    m_evtSvc->release();
    m_evtSvc = 0;
    return StatusCode::SUCCESS;
}
//=============================================================================
// Get the random number seeds vector to be used for this event
//=============================================================================
std::vector<long int> MooreInitSvc::getSeeds( unsigned int seed1, ulonglong seed2 )
{

    std::vector<long int> seeds;

    // CLHEP engne requires positive int
    int seed1a = seed1 & 0x7FFFFFFF;

    // Make two 31 bit seeds out of seed2
    int seed2a = seed2 & 0x7FFFFFFF;
    int seed2b = ( seed2 >> 32 ) & 0x7FFFFFFF;

    if ( 0 != seed1a ) seeds.push_back( seed1a );
    if ( 0 != seed2a ) seeds.push_back( seed2a );
    if ( 0 != seed2b ) seeds.push_back( seed2b );

    // Get last seed by hashing string containing seed1 and seed2

    std::string s =
        name() + boost::io::str( boost::format( "_%1%_%2%" ) %
                                 boost::io::group( std::setfill( '0' ), std::hex,
                                                   std::setw( 8 ), seed1 ) %
                                 boost::io::group( std::setfill( '0' ), std::hex,
                                                   std::setw( 16 ), seed2 ) );

    //--> Hash32 algorithm from Pere Mato
    int hash = 0;
    for ( std::string::const_iterator iC = s.begin(); s.end() != iC; ++iC ) {
        hash += *iC;
        hash += ( hash << 10 );
        hash ^= ( hash >> 6 );
    }
    hash += ( hash << 3 );
    hash ^= ( hash >> 11 );
    hash += ( hash << 15 );
    //<--

    // CLHEP uses the last seed as a seed (only 24 bits used) but also to generate
    // more pseudorandom seeds to populate the "seeds" vector to its capacity of 24
    // seeds. For this generation, 31 bits are used
    seeds.push_back( abs( hash ) );
    seeds.push_back( 0 );

    return seeds;
};

StatusCode MooreInitSvc::initRndm( std::vector<long int>& seeds )
{
    // Get the random number engine if not already done
    IRndmGenSvc* rndSvc( 0 );
    if ( !service( "RndmGenSvc", rndSvc ).isSuccess() ) return StatusCode::FAILURE;
    IRndmEngine* engine = rndSvc->engine();
    if ( 0 == engine ) return StatusCode::FAILURE;
    StatusCode sc = engine->setSeeds( seeds );
    if ( sc.isFailure() ) return sc;
    debug() << "using seeds " << seeds << endmsg;
    return StatusCode::SUCCESS;
}

void MooreInitSvc::handle( const Incident& /*incident*/ )
{
    SmartDataPtr<LHCb::ODIN> odin( m_evtSvc, LHCb::ODINLocation::Default );
    if ( !odin ) m_odinTool->execute();
    if ( !odin ) {
        error() << " Could not obtain ODIN...  " << endmsg;
    } else {
        std::vector<long int> seeds =
            getSeeds( odin->runNumber(), odin->eventNumber() );
        initRndm( seeds ).ignore();
    }
}
