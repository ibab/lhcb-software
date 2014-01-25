// $Id: LumiPrepareResult.cpp,v 1.1 2008-08-29 07:52:45 panmanj Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "Event/HltLumiResult.h"
// local
#include "LumiPrepareResult.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : LumiPrepareResult
//
// 2008-08-27 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiPrepareResult )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiPrepareResult::LumiPrepareResult( const std::string& name,
                                      ISvcLocator* pSvcLocator )
    : GaudiAlgorithm( name, pSvcLocator )
{
    declareProperty( "OutputContainer",
                     m_OutputContainerName = LHCb::HltLumiResultLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
LumiPrepareResult::~LumiPrepareResult()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiPrepareResult::initialize()
{
    StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm
    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Initialize" << endmsg;
    info() << "OutputContainer        " << m_OutputContainerName << endmsg;

    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiPrepareResult::execute()
{

    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;
    // get container
    getOrCreate<HltLumiResult, HltLumiResult>( m_OutputContainerName );
    setFilterPassed( true );

    return StatusCode::SUCCESS;
}

