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

using namespace LHCb;
using namespace Gaudi::Utils::Histos;

//-----------------------------------------------------------------------------
// Implementation file for class : HltL0GlobalMonitor
//
// 2010-03-24 : Johannes Albrecht
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltL0GlobalMonitor );

// utilities

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltL0GlobalMonitor::HltL0GlobalMonitor( const std::string& name,
                                        ISvcLocator* pSvcLocator )
    : HltBaseAlg( name, pSvcLocator ), m_events( 0 ), m_lastL0TCK( 0 )
{
    declareProperty( "ODIN", m_ODINLocation = LHCb::ODINLocation::Default );
    declareProperty( "HltDecReports", m_HltDecReportsLocation =
                                          LHCb::HltDecReportsLocation::Default );
    declareProperty( "L0DUReport",
                     m_L0DUReportLocation = LHCb::L0DUReportLocation::Default );
    declareProperty( "Hlt1DecName", m_hlt1Decision = "Hlt1Global" );
    declareProperty( "Hlt2DecName", m_hlt2Decision = "Hlt2Global" );
}
//=============================================================================
// Destructor
//=============================================================================
HltL0GlobalMonitor::~HltL0GlobalMonitor() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltL0GlobalMonitor::initialize()
{
    StatusCode sc = HltBaseAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm
    m_nboflabels = 0;

    // #if 0
    //   /**
    //    * 2010-03-29 J.A.
    //    *This is to put labels on the histogram at initialize
    //    *Will be updated with corresponding flags during the run
    //    *I am not sure if the saveset has a "2010 type" L0 TCK
    //    * --> be save and do it by hand
    //    */
    //   std::vector< std::pair<unsigned, std::string> > labels;
    //   labels.push_back(std::make_pair( 0, "HCAL"));
    //   labels.push_back(std::make_pair( 1, "SPD"));
    //   labels.push_back(std::make_pair( 2, "CALO"));
    //   labels.push_back(std::make_pair( 3, "MUON,minbias"));
    //   labels.push_back(std::make_pair( 4, "PU"));
    //   labels.push_back(std::make_pair( 5, "SPD40"));
    //   labels.push_back(std::make_pair( 6, "PU20"));
    //   labels.push_back(std::make_pair( 7, "Electron"));
    //   labels.push_back(std::make_pair( 8, "Photon"));
    //   labels.push_back(std::make_pair( 9, "Hadron"));
    //   labels.push_back(std::make_pair( 10, "Muon"));
    //   labels.push_back(std::make_pair( 11, "DiMuon"));
    //   labels.push_back(std::make_pair( 12, "Muon,lowMult"));
    //   labels.push_back(std::make_pair( 13, "DiMuon,lowMult"));
    //   labels.push_back(std::make_pair( 14, "LocalPi0"));
    //   labels.push_back(std::make_pair( 15, "GlobalPi0"));
    //   labels.push_back(std::make_pair( 16, "B1gas"));
    //   labels.push_back(std::make_pair( 17, "B2gas"));
    //   labels.push_back(std::make_pair( 18, "B1gas * ODIN BE"));
    //   labels.push_back(std::make_pair( 19, "B2gas * ODIN EB"));
    //   labels.push_back(std::make_pair( 20, "L0 Global"));
    //   if (!setBinLabels( m_L0Input, labels )) {
    //     error() << "failed to set binlables on L0 hist" << endmsg;
    //   }

    //   m_L0Input->setTitle("L0 channel summary, L0TCK: _not yet set_");
    // #endif

    declareInfo( "COUNTER_TO_RATE[L0Accept]", counter( "L0Accept" ), "L0Accept" );

    return StatusCode::SUCCESS;
};

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
        std::vector<std::pair<unsigned, std::string>> labels;
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
        m_histL0EnabledHLT1 = book1D( "L0 channel summary, enabled, after HLT1",
                                      xmin, xmax, m_nboflabels );
        m_histL0EnabledHLT2 = book1D( "L0 channel summary, enabled, after HLT2",
                                      xmin, xmax, m_nboflabels );

        setBinLabels( m_L0Input, labels );
        if ( m_histL0Enabled ) setBinLabels( m_histL0Enabled, labels );
        if ( m_histL0Disabled ) setBinLabels( m_histL0Disabled, labels );
        if ( m_histL0EnabledHLT1 ) setBinLabels( m_histL0EnabledHLT1, labels );
        if ( m_histL0EnabledHLT2 ) setBinLabels( m_histL0EnabledHLT2, labels );
        m_lastL0TCK = L0TCK;
        // #if 0
        //       char txt[128];
        //       sprintf(txt,"L0 channel summary, L0TCK: 0x%x",L0TCK);
        //       m_L0Input->setTitle(txt);
        // #endif
    }

    LHCb::ODIN* odin = fetch<LHCb::ODIN>( LHCb::ODINLocation::Default );
    if ( !odin ) return;

    // now check HLT decisions for rejected events after Hlt
    LHCb::HltDecReports* hlt = fetch<LHCb::HltDecReports>( m_HltDecReportsLocation );
    bool hlt1 = false, hlt2 = false;
    if ( hlt ) {
        const LHCb::HltDecReport* report = hlt->decReport( m_hlt1Decision );
        if ( report ) hlt1 = report->decision();
        report = hlt->decReport( m_hlt2Decision );
        if ( report ) hlt2 = report->decision();
    } // end if hlt

    bool odinBGas = false;
    bool l0Physics = false;
    // fill the histogram containing the l0 channels
    for ( const auto& i : channels ) {
        int id = i.second->id();
        bool l0chan = l0du->channelDecision( id );

        fill( m_L0Input, id, l0chan );

        if ( i.second->decisionType() != LHCb::L0DUDecision::Disable ) {
            fill( m_histL0Enabled, id, l0chan );
            fill( m_histL0EnabledHLT1, id, l0chan && hlt1 );
            fill( m_histL0EnabledHLT2, id, l0chan && hlt2 );
        } else {
            fill( m_histL0Disabled, id, l0chan );
        }

        if ( id == 21 && odin->bunchCrossingType() == LHCb::ODIN::Beam1 ) {
            fill( m_L0Input, m_nboflabels - 3, l0chan );
            fill( m_histL0Enabled, m_nboflabels - 3, l0chan );
            fill( m_histL0EnabledHLT1, m_nboflabels - 3, l0chan && hlt1 );
            fill( m_histL0EnabledHLT2, m_nboflabels - 3, l0chan && hlt2 );
            if ( l0chan ) odinBGas = true;
        }

        if ( id == 22 && odin->bunchCrossingType() == LHCb::ODIN::Beam2 ) {
            fill( m_L0Input, m_nboflabels - 2, l0chan );
            fill( m_histL0Enabled, m_nboflabels - 2, l0chan );
            fill( m_histL0EnabledHLT1, m_nboflabels - 2, l0chan && hlt1 );
            fill( m_histL0EnabledHLT2, m_nboflabels - 2, l0chan && hlt2 );
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
        fill( m_histL0EnabledHLT1, m_nboflabels - 1, hlt1 );
        fill( m_histL0EnabledHLT2, m_nboflabels - 1, hlt2 );
    }
};
