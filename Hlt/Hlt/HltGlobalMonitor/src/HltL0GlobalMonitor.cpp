// $Id: HltL0GlobalMonitor.cpp,v 1.11 2010-08-26 08:45:29 graven Exp $
// ============================================================================
// Include files
// ============================================================================
// STD&STL
// ============================================================================
#include <string>
#include <vector>
// ============================================================================
// BOOST
// ============================================================================
#include "boost/algorithm/string/erase.hpp"

// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IAxis.h"

// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
// ============================================================================
// GaudiUtils
// ============================================================================
#include "GaudiUtils/HistoStats.h"
#include "GaudiUtils/HistoLabels.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/HltDecReports.h"
// ============================================================================
// Memory Usage
// ============================================================================
#include "GaudiKernel/Timing.h"
#include "GaudiKernel/Memory.h"
// ============================================================================
// local
// ============================================================================
#include "HltL0GlobalMonitor.h"
// ============================================================================

namespace {
   using namespace LHCb;
   using namespace Gaudi::Utils::Histos;
   using std::string;
}

//-----------------------------------------------------------------------------
// Implementation file for class : HltL0GlobalMonitor
//
// 2010-03-24 : Johannes Albrecht
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltL0GlobalMonitor )

// utilities

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltL0GlobalMonitor::HltL0GlobalMonitor( const string& name,
                                        ISvcLocator* pSvcLocator )
    : HltBaseAlg( name, pSvcLocator ), m_events( 0 ), m_lastL0TCK( 0 )
{
    declareProperty( "Stage", m_stage = "Hlt1" );
    declareProperty( "ODIN", m_ODINLocation = LHCb::ODINLocation::Default );
    declareProperty( "DecReports", m_decReportsLocation = LHCb::HltDecReportsLocation::Default );
    declareProperty( "L0DUReport",
                     m_L0DUReportLocation = LHCb::L0DUReportLocation::Default );
    declareProperty( "HltDecName", m_hltDecision = "Hlt1Global" );
}
//=============================================================================
// Destructor
//=============================================================================
HltL0GlobalMonitor::~HltL0GlobalMonitor() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltL0GlobalMonitor::initialize()
{
    StatusCode sc = HltBaseAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm
    m_nboflabels = 0;

    declareInfo( "COUNTER_TO_RATE[L0Accept]", counter( "L0Accept" ), "L0Accept" );

    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltL0GlobalMonitor::execute()
{

    LHCb::L0DUReport* l0du = fetch<LHCb::L0DUReport>( m_L0DUReportLocation );

    monitorL0DU( l0du );

    counter( "#events" )++;
    m_events++;

    return StatusCode::SUCCESS;
}

//==============================================================================
void HltL0GlobalMonitor::monitorL0DU( const LHCb::L0DUReport* l0du )
{
    if ( l0du == 0 ) return;

    if ( !l0du->valid() ) {
        Warning( "L0DUReport Invalid... skipping L0 monitoring" ).ignore();
        return;
    }

    counter( "L0Accept" ) += l0du->decision();
    counter( "L0Forced" ) += l0du->forceBit();

    const LHCb::L0DUConfig* config = l0du->configuration();
    if ( !config ) {
        Error(
            "Failed to obtain valid L0DU configuration... skipping L0 monitoring" )
            .ignore();
        return;
    }
    LHCb::L0DUChannel::Map channels = config->channels();

    // define the bin labels
    unsigned int L0TCK = l0du->tck();
    if ( L0TCK != m_lastL0TCK ) {
        std::vector<std::pair<unsigned, string>> labels;
        for ( const auto& i : channels ) {
            labels.emplace_back( i.second->id(), i.first );
        }
        labels.emplace_back( m_nboflabels, "B1gas * ODIN BE" );
        labels.emplace_back( m_nboflabels, "B2gas * ODIN EB" );
        labels.emplace_back( m_nboflabels, "L0 Global" );
        m_nboflabels += labels.size();
        double xmin = -0.5;
        double xmax = (double)m_nboflabels - 0.5;
        m_L0Input = book1D( "L0 channel summary", xmin, xmax, m_nboflabels );
        m_histL0Enabled =
            book1D( "L0 channel summary, enabled", xmin, xmax, m_nboflabels );
        m_histL0Disabled =
            book1D( "L0 channel summary, disabled", xmin, xmax, m_nboflabels );
        m_histL0EnabledHLT = book1D( string{"L0 channel summary, enabled, after "} + m_stage,
                                     xmin, xmax, m_nboflabels );

        setBinLabels( m_L0Input, labels );
        if ( m_histL0Enabled ) setBinLabels( m_histL0Enabled, labels );
        if ( m_histL0Disabled ) setBinLabels( m_histL0Disabled, labels );
        if ( m_histL0EnabledHLT ) setBinLabels( m_histL0EnabledHLT, labels );
        m_lastL0TCK = L0TCK;
    }

    LHCb::ODIN* odin = fetch<LHCb::ODIN>( LHCb::ODINLocation::Default );
    if ( !odin ) return;

    // now check HLT decisions for rejected events after Hlt
    bool hlt = false;
    LHCb::HltDecReports* decReps = fetch<LHCb::HltDecReports>( m_decReportsLocation );
    if ( decReps ) {
        const LHCb::HltDecReport* report = decReps->decReport( m_hltDecision );
        if ( report ) hlt = report->decision();
    }

    bool odinBGas = false;
    bool l0Physics = false;
    // fill the histogram containing the l0 channels
    for ( const auto& i : channels ) {
        int id = i.second->id();
        bool l0chan = l0du->channelDecision( id );

        fill( m_L0Input, id, l0chan );

        if ( i.second->decisionType() != LHCb::L0DUDecision::Disable ) {
            fill( m_histL0Enabled, id, l0chan );
            fill( m_histL0EnabledHLT, id, l0chan && hlt );
        } else {
            fill( m_histL0Disabled, id, l0chan );
        }

        if ( id == 21 && odin->bunchCrossingType() == LHCb::ODIN::Beam1 ) {
            fill( m_L0Input, m_nboflabels - 3, l0chan );
            fill( m_histL0Enabled, m_nboflabels - 3, l0chan );
            fill( m_histL0EnabledHLT, m_nboflabels - 3, l0chan && hlt );
            if ( l0chan ) odinBGas = true;
        }

        if ( id == 22 && odin->bunchCrossingType() == LHCb::ODIN::Beam2 ) {
            fill( m_L0Input, m_nboflabels - 2, l0chan );
            fill( m_histL0Enabled, m_nboflabels - 2, l0chan );
            fill( m_histL0EnabledHLT, m_nboflabels - 2, l0chan && hlt );
            if ( l0chan ) odinBGas = true;
        }

        // get the global L0 physics decision
        // enabled, non-beam gas channel which fired..
        if ( i.second->decisionType() != LHCb::L0DUDecision::Disable && id < 20 &&
             l0chan )
            l0Physics = true;
    }
    // fill the global L0 decision
    if ( odinBGas || l0Physics ) {
        fill( m_L0Input, m_nboflabels - 1, 1 );
        fill( m_histL0Enabled, m_nboflabels - 1, 1 );
        fill( m_histL0EnabledHLT, m_nboflabels - 1, hlt );
    }
}
