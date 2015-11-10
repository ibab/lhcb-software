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
#include <Event/Node.h>

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

  m_VeloTrackMult = book1D( "VeloTrackMult", "Velo track multiplicity", 0., 600., 300 );
  declareInfo( "VeloTrackMult", m_VeloTrackMult, "Velo track multiplicity" );
  m_VeloTTTrackMult = book1D( "VeloTTTrackMult", "VeloTT track multiplicity", 0., 400., 200 );
  declareInfo( "VeloTTTrackMult", m_VeloTTTrackMult, "VeloTT track multiplicity" );
  m_ForwardTrackMult = book1D( "ForwardTrackMult", "Forward track multiplicity", 0., 400., 200 );
  declareInfo( "ForwardTrackMult", m_ForwardTrackMult, "Forward track multiplicity" );  
  m_nVeloHits = book1D("VeloHitsPerLongTrack" , "# Velo hits per long track",  0., 40.5 ,41);
  declareInfo("VeloHitsPerLongTrack",  m_nVeloHits , "# Velo hits per long track");
  m_nTTHits = book1D("TTHitsPerLongTrack",  "# TT hits per long track",  0., 10.5 ,11);
  declareInfo("TTHitsPerLongTrack",  m_nTTHits , "# TT hits per long track");
  m_nITHits = book1D("ITHitsPerLongTrack" , "# IT hits per long track",  0., 30.5 ,31);
  declareInfo("ITHitsPerLongTrack",  m_nITHits , "# IT hits per long track");
  m_nOTHits = book1D("OTHitsPerLongTrack" , "# OT hits per long track",  0., 40.5 ,41);
  declareInfo("OTHitsPerLongTrack",  m_nOTHits , "# OT hits per long track");
  m_trackChi2DoF = book1D("TrackChi2DoF" , "Track #chi^{2}/DoF",  0., 10. ,100);
  declareInfo("TrackChi2DoF", m_trackChi2DoF, "Track #chi^{2}/DoF");
  m_hitResidual = book1D("HitResidual" , "hit residual",  -6., 6. ,100);
  declareInfo("HitResidual", m_hitResidual, "hit residual");
  m_hitResidualPull = book1D("HitResidualPull" , "hit residual pull",  -5., 5. ,100);
  declareInfo("HitResidualPull", m_hitResidualPull, "hit residual pull");
  for (auto type : { LHCb::Measurement::VeloLiteR, 
                     LHCb::Measurement::VeloLitePhi, 
                     LHCb::Measurement::TTLite, 
                     LHCb::Measurement::ITLite, 
                     LHCb::Measurement::OT  }){
    auto typeName = LHCb::Measurement::TypeToString(type);
    if (type==LHCb::Measurement::OT)
      m_hitResidualPerDet[type] = book1D("HitResidual"+typeName , typeName + " hit residual",  -6., 6. ,200);
    else if (type==LHCb::Measurement::ITLite || type==LHCb::Measurement::TTLite)
      m_hitResidualPerDet[type] = book1D("HitResidual"+typeName , typeName + " hit residual",  -1.5, 1.5 ,200);
    else
      m_hitResidualPerDet[type] = book1D("HitResidual"+typeName , typeName + " hit residual",  -0.2, 0.2 ,200);
    declareInfo("HitResidual"+typeName,  m_hitResidualPerDet[type], typeName + " hit residual");
    m_hitResidualPullPerDet[type] = book1D("HitResidualPull"+typeName , typeName + " hit residual pull",  -5., 5. ,100);
    declareInfo("HitResidualPull"+typeName,  m_hitResidualPullPerDet[type], typeName + " hit residual pull");
  }

  m_VeloPhi = book1D( "VeloPhi", "Velo track phi", -M_PI, M_PI, 100 );
  declareInfo( "VeloPhi", m_VeloPhi, "Velo track phi" );
  m_VeloEta = book1D( "VeloEta", "Velo track eta", 1., 6.5, 100 );
  declareInfo( "VeloEta", m_VeloEta, "Velo track eta" );
  m_forwardPhi = book1D( "forwardPhi", "forward track phi", -M_PI, M_PI, 100 );
  declareInfo( "forwardPhi", m_forwardPhi, "forward track phi" );
  m_forwardEta = book1D( "forwardEta", "forward track eta", 1., 6.5, 100 );
  declareInfo( "forwardEta", m_forwardEta, "forward track eta" );
  m_forwardPt = book1D( "forwardPt", "forward track pt", 0., 10000., 100 );
  declareInfo( "forwardPt", m_forwardPt, "forward track pt" );

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
void Hlt1TrackMonitor::monitorTracks(LHCb::Track::Range tracks){
  for ( const auto& track : tracks ) {
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

//=============================================================================
void Hlt1TrackMonitor::monitorFittedTracks(LHCb::Track::Range tracks){
  for ( const auto& track : tracks ){
    m_trackChi2DoF->fill(track->chi2PerDoF());
    if (track->nodes().empty()) continue;
    LHCb::Track::ConstNodeRange nodes = track->nodes() ;
    for ( const auto& node : nodes ){
      const double f = std::sqrt( node->errMeasure2()/node->errResidual2()) ;
      m_hitResidual->fill(f*node->residual());
      m_hitResidualPull->fill(node->residual()/node->errResidual());
      if (node->hasMeasurement()){
        auto type = node->measurement().type();
        if (m_hitResidualPerDet.find(type)!=m_hitResidualPerDet.end()){
          m_hitResidualPerDet[type]->fill(f*node->residual());
          m_hitResidualPullPerDet[type]->fill(node->residual()/node->errResidual());
        }
      }
    }
  }
}

void Hlt1TrackMonitor::monitorKinematics(LHCb::Track::Range tracksVelo, LHCb::Track::Range tracksForward){
  for (const auto& track : tracksVelo){
    m_VeloPhi->fill(track->phi());
    m_VeloEta->fill(track->pseudoRapidity());
  }
  for (const auto& track : tracksForward){
    m_forwardPhi->fill(track->phi());
    m_forwardEta->fill(track->pseudoRapidity());
    m_forwardPt->fill(track->pt());
  }
}


//=============================================================================
void Hlt1TrackMonitor::monitor()
{
  auto veloTracks    = getIfExists<LHCb::Track::Range>( m_VeloTrackLocation    );
  auto veloTTTracks  = getIfExists<LHCb::Track::Range>( m_VeloTTTrackLocation  );
  auto forwardTracks = getIfExists<LHCb::Track::Range>( m_ForwardTrackLocation );
  auto fittedTracks  = getIfExists<LHCb::Track::Range>( m_FittedTrackLocation  );
  
  m_VeloTrackMult->fill( veloTracks.size() );
  m_VeloTTTrackMult->fill( veloTTTracks.size() );
  m_ForwardTrackMult->fill( forwardTracks.size() );
  
  monitorTracks(forwardTracks);
  monitorFittedTracks(fittedTracks);
  monitorKinematics(veloTracks,fittedTracks);
}
