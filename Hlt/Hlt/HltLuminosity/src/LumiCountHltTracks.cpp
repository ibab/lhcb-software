// $Id: LumiCountHltTracks.cpp,v 1.12 2010-03-28 12:48:59 graven Exp $
// Include files
#include "GaudiKernel/AlgFactory.h"

#include "Event/Track.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"

#include "HltBase/HltSelection.h"

// local
#include "LumiCountHltTracks.h"

#include "boost/format.hpp"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : LumiCountHltTracks
//
// 2008-07-22 : Jaap panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiCountHltTracks )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiCountHltTracks::LumiCountHltTracks( const std::string& name,
                                        ISvcLocator* pSvcLocator )
    : HltBaseAlg( name, pSvcLocator )
{
    // TODO: support a map of selction <-> counter
    declareProperty( "InputSelection", m_InputSelectionName );
    declareProperty( "CounterName", m_CounterName );
    declareProperty( "OutputContainer",
                     m_OutputContainerName = LHCb::HltLumiSummaryLocation::Default );
};
//=============================================================================
// Destructor
//=============================================================================
LumiCountHltTracks::~LumiCountHltTracks()
{
}

//=============================================================================//
// Initialization
//=============================================================================
StatusCode LumiCountHltTracks::initialize()
{

    Hlt::IRegister::Lock lock( regSvc(), this );
    StatusCode sc = HltBaseAlg::initialize();
    if ( sc.isFailure() ) return sc;
    sc = regSvc()->registerInput( m_InputSelectionName, this );
    if ( sc.isFailure() ) {
        error() << " could not register " << m_InputSelectionName << endmsg;
        return sc;
    }
    m_input = hltSvc()->selection( m_InputSelectionName, this );
    if ( m_input == 0 ) {
        error() << " could not obtain " << m_InputSelectionName << endmsg;
        return StatusCode::FAILURE;
    }

    debug() << "InputSelection: " << boost::format( "%20s" ) % m_InputSelectionName
            << " "
            << "CounterName: " << boost::format( "%20s" ) % m_CounterName << " "
            << "OutputContainer: " << boost::format( "%20s" ) % m_OutputContainerName
            << endmsg;

    // ------------------------------------------
    m_Counter = LHCb::LumiCounters::counterKeyToType( m_CounterName );
    if ( m_Counter == LHCb::LumiCounters::Unknown ) {
        info() << "LumiCounter not found with name: " << m_CounterName << endmsg;
    } else {
        debug() << m_CounterName << " key value: " << m_Counter << endmsg;
    }
    // ------------------------------------------

    return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiCountHltTracks::execute()
{

    // get container, and add track counter
    LHCb::HltLumiSummary* sum =
        getOrCreate<HltLumiSummary, HltLumiSummary>( m_OutputContainerName );
    sum->addInfo( m_Counter, m_input->size() );

    setFilterPassed( true );
    return StatusCode::SUCCESS;
}
