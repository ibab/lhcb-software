// $Id: LumiPutResult.cpp,v 1.2 2009-02-18 13:11:13 panmanj Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "Event/LumiCounters.h"
#include "Event/HltLumiResult.h"

// local
#include "LumiPutResult.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : LumiPutResult
//
// 2008-08-27 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiPutResult )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiPutResult::LumiPutResult( const std::string &name, ISvcLocator *pSvcLocator )
    : GaudiAlgorithm( name, pSvcLocator ), m_size( 0 ) {
    declareProperty( "InputContainer",
                     m_InputContainerName = LHCb::HltLumiResultLocation::Default );
}

//=============================================================================
// Destructor
//=============================================================================
LumiPutResult::~LumiPutResult() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiPutResult::initialize() {
    StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm
    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Initialize" << endmsg;

    info() << "InputContainer   " << m_InputContainerName << endmsg;

    // first count number of valid keys
    m_size = 0;
    for ( int iKey = 0; iKey != LHCb::LumiCounters::LastGlobal; iKey++ ) {
        if ( iKey != LHCb::LumiCounters::Unknown ) ++m_size;
    }

    // get arrays
    m_means.reset(
        new double[m_size] ); // create a fixed location for DIM to look at
    m_thresholds.reset(
        new double[m_size] ); // create a fixed location for DIM to look at
    m_infoKeys.reset( new unsigned int[m_size] ); // corresponding key in the info

    int i = 0;
    for ( int iKey = 0; iKey != LHCb::LumiCounters::LastGlobal; iKey++ ) {
        if ( iKey == LHCb::LumiCounters::Unknown ) continue;
        // declare all possible counters
        std::string name = LHCb::LumiCounters::counterKeyToString( iKey );
        m_infoKeys[i] = iKey;
        // announce the values
        declareInfo( "COUNTER_TO_RATE[" + name + "_mean]", m_means[i],
                     "mean of " + name );
        declareInfo( "COUNTER_TO_RATE[" + name + "_threshold]", m_thresholds[i],
                     "fraction over threshold of " + name );
        info() << "counter " << name << " declared at " << i << " with key "
               << m_infoKeys[i] << endmsg;
        ++i;
    }

    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiPutResult::execute() {

    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;
    // get container
    LHCb::HltLumiResult *result =
        getOrCreate<HltLumiResult, HltLumiResult>( m_InputContainerName );

    // read the contents and publish the values which exist
    for ( unsigned int i = 0; i < m_size; i++ ) {
        unsigned key = m_infoKeys[i];
        double mean = result->info( key + 100, -10000. );
        if ( mean != -10000. ) {
            m_means[i] = mean;
            m_thresholds[i] = result->info( key + 200, -10000. );
            if ( msgLevel( MSG::DEBUG ) )
                debug() << "Key: " << key << " mean: " << mean << " threshold "
                        << m_thresholds[i] << endmsg;
        }
    }

    setFilterPassed( true );

    return StatusCode::SUCCESS;
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode LumiPutResult::finalize() {

    IMonitorSvc *mS = monitorSvc().get();
    if ( mS ) {
        for ( int iKey = 0; iKey != LHCb::LumiCounters::LastGlobal; iKey++ ) {
            if ( iKey == LHCb::LumiCounters::Unknown ) continue;
            // undeclare all possible counters
            std::string name = LHCb::LumiCounters::counterKeyToString( iKey );
            mS->undeclareInfo( "COUNTER_TO_RATE[" + name + "_mean]", this );
            mS->undeclareInfo( "COUNTER_TO_RATE[" + name + "_threshold]", this );
            info() << "counter " << name << " undeclared" << endmsg;
        }
    }

    // release storage
    m_means.reset();
    m_thresholds.reset();
    m_infoKeys.reset();

    return GaudiAlgorithm::finalize();
}
