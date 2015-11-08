// Include files 

// from Gaudi
#include "Event/Track.h"
#include "Event/State.h"
#include "GaudiKernel/SystemOfUnits.h"

// Interface
#include "MuonDet/DeMuonDetector.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

#include "Event/MuonCoord.h"

// Tools for histogramming
#include "AIDA/IHistogram1D.h"

// local
#include "TrackMuonMatchMonitor.h"
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TrackMuonMatchMonitor
//
// 2010-02-09 : Stefania Vecchi
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackMuonMatchMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackMuonMatchMonitor::TrackMuonMatchMonitor( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator ), m_muonDet(0), m_extrapolator(0)
{
  declareProperty( "TracksLocation"     , m_tTracksLocation      = TrackLocation::Default     );
  declareProperty( "Extrapolator"       , m_nExtrapolator        = "TrackLinearExtrapolator"  );
  declareProperty( "WhichStation"       , m_iMS =0);
  declareProperty( "MaxErrX", m_maxErrX = 5 * Gaudi::Units::mm) ;
  declareProperty( "MaxErrY", m_maxErrY = 20 * Gaudi::Units::mm) ;
}
//=============================================================================
// Destructor
//=============================================================================
TrackMuonMatchMonitor::~TrackMuonMatchMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackMuonMatchMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; 

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  m_extrapolator   = tool<ITrackExtrapolator>(  m_nExtrapolator , "Extrapolator"  , this );
  m_muonDet=getDet<DeMuonDetector>(DeMuonLocation::Default); 
  m_zM1 = m_muonDet->getStationZ(m_iMS);
  
  m_MAXsizeX = m_muonDet->getOuterX(m_iMS);
  m_MAXsizeY = m_muonDet->getOuterY(m_iMS);
  
  std::string name;
  setHistoTopDir("Track/") ;
  for (int iR=0; iR<nREGIONS; ++iR){
    // in the "signal" region +/- 6 error units respect the track extrapolation point
    unsigned int nbin=100;
    double max = 80. + 40.*float(iR);
    double min = -max;
    name = "resX_ASide_M1R"+ std::to_string(iR+1);
    m_resx_a[iR] = book1D( name, name, min, max, nbin );
    name = "resY_ASide_M1R"+ std::to_string(iR+1);
    m_resy_a[iR] = book1D( name, name, min, max, nbin );
    name = "resX_CSide_M1R"+ std::to_string(iR+1);
    m_resx_c[iR] = book1D( name, name, min, max, nbin );
    name = "resY_CSide_M1R"+ std::to_string(iR+1);
    m_resy_c[iR] = book1D( name, name, min, max, nbin );
    m_hisxmax[iR] = max ;
  }
  

  return StatusCode::SUCCESS;
}

namespace {
  struct MuonHit {
    const LHCb::MuonCoord* coord;
    double x,y,z ;
    double dx,dy,dz ;
  } ;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackMuonMatchMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  LHCb::Track::Range tTracks = get<LHCb::Track::Range>( m_tTracksLocation );
  if (tTracks.size()==0)   return StatusCode::SUCCESS;
  
  const LHCb::MuonCoords* coords = getIfExists<LHCb::MuonCoords>("Raw/Muon/Coords");
  if( NULL == coords ) {
    if(msgLevel(MSG::DEBUG)) debug()<<" Container Raw/Muon/Coords doesn't exist"<<endmsg;
    return StatusCode::SUCCESS;
  }
  if( coords->size() == 0 ) {
    if(msgLevel(MSG::DEBUG)) debug() << " No hits retrieved , skip event" << endmsg;
    return StatusCode::SUCCESS;    
  }

  // cache the position of the hits. that saves a lot of time.
  std::vector< MuonHit > muonhits ;
  MuonHit muonhit ;
  muonhits.reserve( coords->size() ) ;
  for(const auto& coord : *coords ) {
    muonhit.coord = coord; 
    if ( m_iMS == int(muonhit.coord->key().station()) ) { // only the Chosen station
      StatusCode sc = m_muonDet->Tile2XYZ(muonhit.coord->key(),
					  muonhit.x,muonhit.dx,
					  muonhit.y,muonhit.dy,
					  muonhit.z,muonhit.dz) ;
      if(sc.isSuccess()) muonhits.push_back( muonhit ) ;
    }
  }
  
  if(msgLevel(MSG::DEBUG)) debug() << " Found " << tTracks.size() << " tracks in the container " << endmsg;
  for( const LHCb::Track* track: tTracks) {
    if( track->hasT() &&
	track->chi2PerDoF() <5 &&
	track->p() > 1 * Gaudi::Units::GeV ) {

      State stateAtM1 ;
      StatusCode sc = m_extrapolator->propagate( *track, m_zM1, stateAtM1 ) ;
      
      if(sc.isSuccess() &&
	 std::abs(stateAtM1.x()) < m_MAXsizeX && std::abs(stateAtM1.y()) < m_MAXsizeY &&
	 std::sqrt(stateAtM1.errX2()) < m_maxErrX && std::sqrt(stateAtM1.errY2()) < m_maxErrY ) {
	
	for( const MuonHit& hit: muonhits ) {
	    
	  int region = hit.coord->key().region() ;
	  double deltaZ = hit.z - stateAtM1.z() ;
	  double deltaX = hit.x - (stateAtM1.x() + stateAtM1.tx() * deltaZ) ;
	  double deltaY = hit.y - (stateAtM1.y() + stateAtM1.ty() * deltaZ) ;
	      
	  if( std::abs(deltaX) < m_hisxmax[region] &&
	      std::abs(deltaY) < m_hisxmax[region] ) {
	    
	    AIDA::IHistogram1D *tempx, *tempy;
	    
	    tempx = hit.x > 0 ? m_resx_a[ region ] : m_resx_c[ region ];
	    tempy = hit.x > 0 ? m_resy_a[ region ] : m_resy_c[ region ];	  	  
	    
	    tempx->fill( deltaX );  // X residuals on the same Z as the hit
	    tempy->fill( deltaY );  // Y residuals on the same Z as the hit
	    
	  } 
	}
      }
    }
  }
  
  return StatusCode::SUCCESS;
}
//=============================================================================

