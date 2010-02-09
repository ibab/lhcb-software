// $Id: TrackMuonMatchMonitor.cpp,v 1.1 2010-02-09 16:03:57 svecchi Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"
#include "Event/State.h"

// Interface
#include "MuonDet/DeMuonDetector.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

#include "Event/MuonCoord.h"

// Tools for histogramming
#include "AIDA/IHistogram1D.h"

#include <boost/lexical_cast.hpp>
// local
#include "TrackMuonMatchMonitor.h"
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TrackMuonMatchMonitor
//
// 2010-02-09 : Stefania Vecchi
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackMuonMatchMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackMuonMatchMonitor::TrackMuonMatchMonitor( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty( "TracksLocation"     , m_tTracksLocation      = TrackLocation::Default     );
  declareProperty( "Extrapolator"       , m_nExtrapolator        = "TrackLinearExtrapolator"  );
  declareProperty( "WhichStation"       , m_iMS =0);
  declareProperty( "nFOI"               , m_nFOI =6.);
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

  std::string name;
  setHistoTopDir("Track/") ;
  for (int iR=0; iR<nREGIONS; ++iR){
    // in the "signal" region +/- 6 error units respect the track extrapolation point
    unsigned int nbin=100;
    double max = 80. + 40.*float(iR);
    double min = -max;
    name = "resX_ASide_M1R"+ boost::lexical_cast<std::string>(iR+1);
    m_resx_a[iR] = book1D( name, name, min, max, nbin );
    name = "resY_ASide_M1R"+ boost::lexical_cast<std::string>(iR+1);
    m_resy_a[iR] = book1D( name, name, min, max, nbin );
    name = "resX_CSide_M1R"+ boost::lexical_cast<std::string>(iR+1);
    m_resx_c[iR] = book1D( name, name, min, max, nbin );
    name = "resY_CSide_M1R"+ boost::lexical_cast<std::string>(iR+1);
    m_resy_c[iR] = book1D( name, name, min, max, nbin );
  
  }
  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackMuonMatchMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  const Tracks*       tTracks = NULL; 
  
  if ( ! exist<LHCb::Tracks>( m_tTracksLocation )) { 
    debug()<<" Container "<<m_tTracksLocation<<" doesn't exist"<<endmsg;
    return StatusCode::SUCCESS;
  }
  tTracks = get<Tracks>( m_tTracksLocation );
  if (tTracks->size()==0)   return StatusCode::SUCCESS;

  LHCb::MuonCoords* coords = NULL;
  
  if ( ! exist<LHCb::MuonCoords>("Raw/Muon/Coords")) { 
    debug()<<" Container Raw/Muon/Coords doesn't exist"<<endmsg;
    return StatusCode::SUCCESS;
  }
  coords = get<LHCb::MuonCoords>("Raw/Muon/Coords");
  if( coords == NULL || coords->size() == 0 ) {
    debug() << " No hits retrieved , skip event" << endmsg;
    return StatusCode::SUCCESS;    
  }
  
    
  info()<<" Found "<<tTracks->size() << " tracks in the container "<<endmsg;
  for ( Tracks::const_iterator t = tTracks->begin(), tEnd = tTracks->end(); t != tEnd; ++t ) {
    if((*t)->chi2PerDoF()>10) continue;
  
    /// Get the T state closest to this z
    State* tState = &(*t)->closestState( m_zM1 );
    StatusCode sc = m_extrapolator->propagate( *tState, m_zM1 , LHCb::ParticleID(13)  );
    if ( !sc.isSuccess() ) {
      Warning( "Could not propagate Track state on M1"   , StatusCode::FAILURE, 5 );
      continue;
    }
    
    double x_fit = tState->x();
    double y_fit = tState->y();

    double d2x_fit = tState->errX2();
    double d2y_fit = tState->errY2();

    int noKFOI(0);
    
    for(LHCb::MuonCoords::iterator ihT = coords->begin(); ihT != coords->end() ; ihT++ ) { // loop on all the hits
      if ( m_iMS == int((*ihT)->key().station()) ) { // only the Chosen station
        
        double x_hit,dx_hit,y_hit,dy_hit,z_hit,dz_hit;
        StatusCode sc = m_muonDet->Tile2XYZ((*ihT)->key(),x_hit,dx_hit,y_hit,dy_hit,z_hit,dz_hit);
        if ( !sc.isSuccess() ) continue;
        
        double deltaX = fabs( x_fit - x_hit ) / ( sqrt( d2x_fit + pow(dx_hit,2)) );
        double deltaY = fabs( y_fit - y_hit ) / ( sqrt( d2y_fit + pow(dy_hit,2)) );          
        
        if ( deltaX < m_nFOI && deltaY < m_nFOI ) {
          
          sc = m_extrapolator->propagate( *tState, z_hit,  LHCb::ParticleID(13)  );          
          if( sc.isSuccess() ) {
            
            AIDA::IHistogram1D *tempx, *tempy;
            
            tempx = x_hit > 0 ? m_resx_a[ (*ihT)->key().region() ] : m_resx_c[ (*ihT)->key().region() ];
            tempy = x_hit > 0 ? m_resy_a[ (*ihT)->key().region() ] : m_resy_c[ (*ihT)->key().region() ];	  	  
            
            tempx->fill( tState->x() - x_hit );  // X residuals on the same Z as the hit
            tempy->fill( tState->y() - y_hit );  // Y residuals on the same Z as the hit

            noKFOI ++; // Number of hits in the right FOI
          }            
        } // end select the station
      } // end of loop on all  hits
      
      debug()<<" found a track "<<x_fit<<" "<<y_fit<<" n. Hits in FOI ="<<noKFOI<<endmsg;
    }
    
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackMuonMatchMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================

