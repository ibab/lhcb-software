#include "GaudiKernel/AlgFactory.h"

#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"

#include "LumiStoreConstant.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : LumiStoreConstant
// Puts a constant value for a counter.
//
// 2015-11-09 : Rosen Matev
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiStoreConstant )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiStoreConstant::LumiStoreConstant( const std::string& name, ISvcLocator* pSvcLocator )
    : GaudiAlgorithm( name, pSvcLocator )
{
    declareProperty( "CounterName", m_CounterName = "Method" );
    declareProperty( "Value", m_Value );
    declareProperty( "OutputContainer",
                     m_OutputContainerName = LHCb::HltLumiSummaryLocation::Default );
}
//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiStoreConstant::initialize()
{
    StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Initialize" << endmsg;

    debug() << "CounterName:       " << m_CounterName << " "
            << "OutputContainer:   " << m_OutputContainerName << " "
            << "Value:             " << m_Value << endmsg;

    m_Counter = LHCb::LumiCounters::counterKeyToType( m_CounterName );
    if ( m_Counter == LHCb::LumiCounters::Unknown ) {
        warning() << "LumiCounter not found with name: " << m_CounterName << endmsg;
    } else {
        debug() << "ExtraInfo key value: " << m_Counter << endmsg;
    }

    if ( m_Value < 0 || m_Value >= 65536 ) {
        warning() << "Value outside range [0,65535], will be truncated: " << m_Value << endmsg;
    }

    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiStoreConstant::execute()
{

    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;

    // get container
    LHCb::HltLumiSummary* sums =
        getOrCreate<HltLumiSummary, HltLumiSummary>( m_OutputContainerName );
    // add counter
    sums->addInfo( m_Counter, m_Value );

    setFilterPassed( true );

    return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LumiStoreConstant::finalize()
{

    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Finalize" << endmsg;

    return GaudiAlgorithm::finalize(); // must be called after all other actions
}

//=============================================================================
