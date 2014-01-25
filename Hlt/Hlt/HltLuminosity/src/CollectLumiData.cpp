// $Id: CollectLumiData.cpp,v 1.8 2010-05-23 19:36:57 gligorov Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// from LHCB
#include "Event/Track.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"

// local
#include "CollectLumiData.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CollectLumiData
//
// 2008-07-11 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CollectLumiData );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CollectLumiData::CollectLumiData( std::string name, ISvcLocator* pSvcLocator )
    : GaudiAlgorithm( std::move( name ), pSvcLocator )
    , m_iRZVelo{LHCb::LumiCounters::Unknown}
    , m_iPV3D{LHCb::LumiCounters::Unknown}
{
    declareProperty( "RZVeloContainer", m_RZVeloContainerName = "Hlt/Track/RZVelo" );
    declareProperty( "PV3DContainer", m_PV3DContainerName = "Hlt/Vertex/PV3D" );
    declareProperty( "OutputContainer",
                     m_OutputContainerName = LHCb::HltLumiSummaryLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
CollectLumiData::~CollectLumiData()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode CollectLumiData::initialize()
{
    StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Initialize" << endmsg;

    // ------------------------------------------
    m_iPV3D = LHCb::LumiCounters::counterKeyToType( "PV3D" );
    if ( m_iPV3D == LHCb::LumiCounters::Unknown ) {
        info() << "LumiCounters not found: "
               << "PV3D" << endmsg;
    } else {
        info() << "LumiCounters::PV3D key value: " << m_iPV3D << endmsg;
    }

    m_iRZVelo = LHCb::LumiCounters::counterKeyToType( "RZVelo" );
    if ( m_iRZVelo == LHCb::LumiCounters::Unknown ) {
        info() << "LumiCounters not found: "
               << "RZVelo" << endmsg;
    } else {
        info() << "LumiCounters::RZVelo key value: " << m_iRZVelo << endmsg;
    }
    // ------------------------------------------

    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CollectLumiData::execute()
{

    // printing
    bool m_printing_verbose = msgLevel( MSG::VERBOSE );
    bool m_printing_debug = msgLevel( MSG::DEBUG );
    bool m_printing_info = msgLevel( MSG::INFO );

    // ------------------------------------------
    // create output container on the TES
    LHCb::HltLumiSummary* sum =
        getOrCreate<LHCb::HltLumiSummary, LHCb::HltLumiSummary>(
            m_OutputContainerName );

    // ------------------------------------------
    // load the track objects
    // ------------------------------------------
    const LHCb::Tracks* RZVelo = getIfExists<LHCb::Tracks>( m_RZVeloContainerName );
    int n_RZVelo = RZVelo ? RZVelo->size() : -1;
    sum->addInfo( m_iRZVelo, n_RZVelo );

    if ( m_printing_info && n_RZVelo < 0 )
        info() << m_RZVeloContainerName << " not found" << endmsg;
    if ( m_printing_verbose )
        verbose() << "found " << n_RZVelo << " RZVelo tracks." << endmsg;
    if ( m_printing_debug )
        debug() << "There are " << n_RZVelo << " tracks in " << m_RZVeloContainerName
                << endmsg;

    // ------------------------------------------
    // load the vertex objects
    // ------------------------------------------
    const LHCb::RecVertices* PV3D = get<LHCb::RecVertices>( m_PV3DContainerName );
    int n_PV3D = PV3D->size();
    sum->addInfo( m_iPV3D, n_PV3D );

    if ( n_PV3D < 0 && m_printing_info )
        info() << m_PV3DContainerName << " not found" << endmsg;
    if ( m_printing_verbose )
        verbose() << "found " << n_PV3D << " PV3D vertices." << endmsg;
    if ( m_printing_debug )
        debug() << "There are " << n_PV3D << " vertices in " << m_PV3DContainerName
                << endmsg;

    setFilterPassed( true );

    return StatusCode::SUCCESS;
}
