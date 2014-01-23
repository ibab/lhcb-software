// $Id: LumiFlagMethod.cpp,v 1.5 2010-02-17 15:43:17 graven Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"
#include "Event/LumiMethods.h"

// local
#include "LumiFlagMethod.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : LumiFlagMethod
// Puts a flag value to tag for which lumi method this event should be used
// Since we just put the value and not add bits, the "Random" method should
// just be the default (i.e. not use the counter) and the L0Rate method
// should put its flag.
//
// 2008-07-21 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiFlagMethod );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiFlagMethod::LumiFlagMethod( const std::string& name, ISvcLocator* pSvcLocator )
    : GaudiAlgorithm( name, pSvcLocator )
{
    declareProperty( "CounterName", m_CounterName = "Method" );
    declareProperty( "ValueName", m_ValueName );
    declareProperty( "OutputContainer",
                     m_OutputContainerName = LHCb::HltLumiSummaryLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
LumiFlagMethod::~LumiFlagMethod()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiFlagMethod::initialize()
{
    StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Initialize" << endmsg;

    debug() << "CounterName:       " << m_CounterName << " "
            << "OutputContainer:   " << m_OutputContainerName << " "
            << "ValueName:         " << m_ValueName << endmsg;

    // ------------------------------------------
    m_Counter = LHCb::LumiCounters::counterKeyToType( m_CounterName );
    if ( m_Counter == LHCb::LumiCounters::Unknown ) {
        warning() << "LumiCounter not found with name: " << m_CounterName << endmsg;
    } else {
        debug() << "ExtraInfo key value: " << m_Counter << endmsg;
    }
    // ------------------------------------------
    m_Value = LHCb::LumiMethods::methodKeyToType( m_ValueName );
    if ( m_Value == LHCb::LumiMethods::Unknown ) {
        warning() << "LumiMethod not found with name: " << m_ValueName << endmsg;
    } else {
        debug() << "ExtraInfo method key value: " << m_Value << endmsg;
    }
    // ------------------------------------------

    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiFlagMethod::execute()
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
StatusCode LumiFlagMethod::finalize()
{

    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Finalize" << endmsg;

    return GaudiAlgorithm::finalize(); // must be called after all other actions
}

//=============================================================================
