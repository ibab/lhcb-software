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
#include <AIDA/IHistogram1D.h>
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
#include <Event/Track.h>
// ============================================================================
// local
// ============================================================================
#include "Hlt1TrackMonitor.h"
// ============================================================================

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt1TrackMonitor
//
// 2015-05-21 : Sascha Stahl
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Hlt1TrackMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt1TrackMonitor::Hlt1TrackMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator )
: HltBaseAlg( name, pSvcLocator )
{
  declareProperty( "VeloTrackLocation",
                   m_VeloTrackLocation = "");
  declareProperty( "VeloTTTrackLocation",
                   m_VeloTTTrackLocation = "");
  declareProperty( "ForwardTrackLocation",
                   m_ForwardTrackLocation = "");
  declareProperty( "FittedTrackLocation",
                   m_FittedTrackLocation = "");
  

}
//=============================================================================
// Destructor
//=============================================================================
Hlt1TrackMonitor::~Hlt1TrackMonitor()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode Hlt1TrackMonitor::initialize()
{
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

  m_VeloTrackMult = book1D( "VeloTrackMult", "Velo track multiplicity", 0., 250., 251 );
  declareInfo( "VeloTrackMult", m_VeloTrackMult, "Velo track multiplicity" );
  m_VeloTTTrackMult = book1D( "VeloTTTrackMult", "VeloTT track multiplicity", 0., 200., 201 );
  declareInfo( "VeloTTTrackMult", m_VeloTTTrackMult, "VeloTT track multiplicity" );
  m_ForwardTrackMult = book1D( "ForwardTrackMult", "Forward track multiplicity", 0., 100., 101 );
  declareInfo( "ForwardTrackMult", m_ForwardTrackMult, "Forward track multiplicity" );  
  m_nVeloHits = book1D("VeloHitsPerLongTrack" , "# Velo hits per long track",  0., 40.5 ,41);
  declareInfo("VeloHitsPerLongTrack",  m_nVeloHits , "# Velo hits per long track");
  m_nTTHits = book1D("TTHitsPerLongTrack",  "# TT hits per long track",  0., 10.5 ,11);
  declareInfo("TTHitsPerLongTrack",  m_nTTHits , "# TT hits per long track");
  m_nITHits = book1D("ITHitsPerLongTrack" , "# IT hits per long track",  0., 40.5 ,41);
  declareInfo("ITHitsPerLongTrack",  m_nITHits , "# IT hits per long track");
  m_nOTHits = book1D("OTHitsPerLongTrack" , "# OT hits per long track",  0., 40.5 ,41);
  declareInfo("OTHitsPerLongTrack",  m_nOTHits , "# OT hits per long track");
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Hlt1TrackMonitor::execute()
{
  monitor();
  ++counter( "#events" );
  setFilterPassed( true );
  return StatusCode::SUCCESS;
}

//=============================================================================
void Hlt1TrackMonitor::monitor()
{

  auto veloTracks    = get<LHCb::Track::Range>( m_VeloTrackLocation    );
  auto veloTTTracks  = get<LHCb::Track::Range>( m_VeloTTTrackLocation  );
  auto forwardTracks = get<LHCb::Track::Range>( m_ForwardTrackLocation );

  m_VeloTrackMult->fill( veloTracks.size() );
  m_VeloTTTrackMult->fill( veloTTTracks.size() );
  m_ForwardTrackMult->fill( forwardTracks.size() );
  
  for (auto track : forwardTracks ){
    const auto ids = track->lhcbIDs();
    const auto nVeloHits = std::count_if(ids.begin(), ids.end(),[](LHCb::LHCbID id){return id.isVelo();});
    const auto nTTHits = std::count_if(ids.begin(), ids.end(),[](LHCb::LHCbID id){return id.isTT();});
    const auto nITHits = std::count_if(ids.begin(), ids.end(),[](LHCb::LHCbID id){return id.isIT();});
    const auto nOTHits = std::count_if(ids.begin(), ids.end(),[](LHCb::LHCbID id){return id.isOT();});
    m_nVeloHits->fill(nVeloHits);
    m_nTTHits->fill(nTTHits);
    m_nITHits->fill(nITHits);
    m_nOTHits->fill(nOTHits);
    
    
  }
  
}
