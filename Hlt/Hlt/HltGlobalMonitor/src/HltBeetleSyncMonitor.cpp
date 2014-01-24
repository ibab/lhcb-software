// $Id: HltBeetleSyncMonitor.cpp,v 1.76 2010-08-26 08:45:29 graven Exp $
// ============================================================================
// Include files
// ============================================================================
// STD&STL
// ============================================================================
#include <iostream>
#include <string>
#include <map>
// ============================================================================
// AIDA
// ============================================================================
#include <AIDA/IHistogram2D.h>
// ============================================================================
// GaudiKernel
// ============================================================================
#include <GaudiKernel/AlgFactory.h>
// ============================================================================
// GaudiUtils
// ============================================================================
#include <GaudiUtils/HistoStats.h>
#include <GaudiUtils/HistoLabels.h>
// ============================================================================
// Event
// ============================================================================
#include <Event/HltDecReports.h>
#include <Event/STLiteCluster.h>
#include <Event/VeloLiteCluster.h>
#include <Event/L0DUReport.h>
#include <Event/ODIN.h>
// ============================================================================
// local
// ============================================================================
#include "HltBeetleSyncMonitor.h"
// ============================================================================

//-----------------------------------------------------------------------------
// Implementation file for class : HltBeetleSyncMonitor
//
// 2010-10-13 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltBeetleSyncMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltBeetleSyncMonitor::HltBeetleSyncMonitor( const std::string& name,
                                            ISvcLocator* pSvcLocator )
    : HltBaseAlg( name, pSvcLocator )
{
    declareProperty( "L0DUReportLocation",
                     m_L0DUReportLocation = LHCb::L0DUReportLocation::Default );
    declareProperty( "ODINLocation", m_ODINLocation = LHCb::ODINLocation::Default );
    declareProperty( "VeloClusterLocation",
                     m_VeloLiteClusterLocation =
                         LHCb::VeloLiteClusterLocation::Default );
    declareProperty( "ITClusterLocation",
                     m_ITLiteClusterLocation =
                         LHCb::STLiteClusterLocation::ITClusters );
}
//=============================================================================
// Destructor
//=============================================================================
HltBeetleSyncMonitor::~HltBeetleSyncMonitor()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltBeetleSyncMonitor::initialize()
{
    StatusCode sc = HltBaseAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    m_SPDIT = book2D( "SPDvsIT", "SPD multiplicity. vs. IT hits", 0., 1200., 120, 0.,
                      5000., 100 );
    declareInfo( "SPDvsIT", m_SPDIT, "SPD multiplicity vs IT hits." );
    m_SPDVelo = book2D( "SPDvsVelo", "SPD multiplicity vs. Velo hits", 0., 1200.,
                        120, 0., 10000., 100 );
    declareInfo( "SPDvsVelo", m_SPDVelo, "SPD multiplicity vs Velo hits." );

    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltBeetleSyncMonitor::execute()
{
    monitor();
    ++counter( "#events" );
    setFilterPassed( true );
    return StatusCode::SUCCESS;
}

//=============================================================================
void HltBeetleSyncMonitor::monitor()
{
    auto odin = fetch<LHCb::ODIN>( m_ODINLocation );
    auto l0du = fetch<LHCb::L0DUReport>( m_L0DUReportLocation );
    auto itClusters =
        fetch<LHCb::STLiteCluster::FastContainer>( m_ITLiteClusterLocation );
    auto veloClusters =
        fetch<LHCb::VeloLiteCluster::FastContainer>( m_VeloLiteClusterLocation );

    if ( !odin || !l0du || !itClusters || !veloClusters ) {
        return;
    }

    // Check for presence of SPD in L0DU
    const LHCb::L0DUConfig* config = l0du->configuration();
    if ( !l0du->valid() || !config ) return;

    const auto& data = config->data();
    auto it = data.find( "Spd(Mult)" );
    if ( it == data.end() ) return;

    double nSPD = it->second->value();
    double nIT = itClusters->size();
    double nVelo = veloClusters->size();

    m_SPDIT->fill( nSPD, nIT );
    m_SPDVelo->fill( nSPD, nVelo );
}
