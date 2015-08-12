// Include files 
// -------------
// from TrackEvent
#include "Event/KalmanFitResult.h"
#include "Event/FitNode.h"

// local
#include "TrackKalmanFilter.h"
#include <boost/foreach.hpp>


//-----------------------------------------------------------------------------
// Implementation file for class : TrackKalmanFilter
//
//  Original Author: Rutger van der Eijk
//  Created: 07-04-1999
//  Adapted: 21-03-2002  Olivier Callot
//  Adapted: 15-04-2005  Jose A. Hernando, Eduardo Rodrigues
//  Adapted: 20-10-2006  J. van Tilburg
//-----------------------------------------------------------------------------


DECLARE_TOOL_FACTORY( TrackKalmanFilter )

//=========================================================================
// Standard Constructor, initializes variables
//=========================================================================
TrackKalmanFilter::TrackKalmanFilter( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : GaudiTool( type, name, parent)
  , m_debugLevel(false)
{
  declareInterface<ITrackKalmanFilter>( this );
  declareProperty( "ForceBiDirectionalFit" , m_forceBiDirectionalFit  = true   );
  declareProperty( "ForceSmooth" , m_forceSmooth  = false   );
  declareProperty( "DoF", m_DoF = 5u);
}

//=========================================================================
// Destructor
//=========================================================================
TrackKalmanFilter::~TrackKalmanFilter() {
}

//=========================================================================
// Initialize
//=========================================================================
StatusCode TrackKalmanFilter::initialize() 
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;
  m_debugLevel   = msgLevel( MSG::DEBUG );
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// Fit the nodes
//=========================================================================
StatusCode TrackKalmanFilter::fit( LHCb::Track& track ) const
{
  if( m_debugLevel ) debug() << "TrackKalmanFilter::fit" << endreq ;
  
  StatusCode sc(StatusCode::SUCCESS, true); 

  // The seed covariance comes from the KalmanFitResult
  LHCb::KalmanFitResult* kalfit = dynamic_cast<LHCb::KalmanFitResult*>(track.fitResult()) ;
  if( !kalfit) return Warning("No kalfit on track",StatusCode::FAILURE,0) ;
  
  LHCb::KalmanFitResult::FitNodeRange nodes = kalfit->fitNodes() ;
  if( nodes.empty() ) return Warning( "Fit failure: track has no nodes", StatusCode::FAILURE,0 );
  
  // This is set up with the aim to trigger the cache such that there
  // will be no nested calls. That makes it easier to profile the
  // fit. Eventually, we could do without all of this, since
  // everything is anyway computed on demand.
  for( LHCb::KalmanFitResult::FitNodeRange::iterator inode = nodes.begin() ;
       inode != nodes.end(); ++inode) {
    (*inode)->predictedState(LHCb::FitNode::Forward) ;
    (*inode)->filteredState(LHCb::FitNode::Forward) ;
  }
  if(m_forceBiDirectionalFit){
    for( LHCb::KalmanFitResult::FitNodeRange::reverse_iterator inode = nodes.rbegin() ;
	 inode != nodes.rend(); ++inode) {
      (*inode)->predictedState(LHCb::FitNode::Backward) ;
      (*inode)->filteredState(LHCb::FitNode::Backward) ;
    }
  }
  // force the smoothing. 
  if (m_forceSmooth){
    for( LHCb::KalmanFitResult::FitNodeRange::iterator inode = nodes.begin() ;
	 inode != nodes.end(); ++inode) 
      (*inode)->state() ; 
  }

  if (kalfit->inError()){
    sc = Warning(kalfit->getError(), StatusCode::FAILURE, 0 ) ;
  }

  // This is the only thing we need KalmanFilter still for: set the
  // total chisquare of the track. This could also be done from
  // TrackMasterFitter.
  
  // Count the number of active track parameters. For now, just look at the momentum.
  size_t npar = m_DoF ;
  if( npar == 5u ) {
    const LHCb::State* laststate(0) ;
    for( LHCb::KalmanFitResult::FitNodeRange::iterator inode = nodes.begin() ;
	 inode != nodes.end(); ++inode) {
      if( (*inode)->type()==LHCb::Node::HitOnTrack ) 
	laststate = &((*inode)->filteredState(LHCb::FitNode::Forward)) ;
    }
    if(laststate) {
      const double threshold = 0.1 ;
      npar = (laststate->covariance()(4,4) / kalfit->seedCovariance()(4,4) < threshold ? 5 : 4) ;
      dynamic_cast<LHCb::KalmanFitResult*>(track.fitResult())->setNTrackParameters( npar ) ;
    }
  }
  
  LHCb::ChiSquare chisq = nodes.back()->totalChi2(LHCb::FitNode::Forward) ;
  int ndof = chisq.nDoF() - (npar - kalfit->nTrackParameters() ) ;
  if( m_forceBiDirectionalFit ) {
    // FIXME: why don't we take the maximum, like in KalmanFitResult?
    LHCb::ChiSquare chisqbw =  nodes.front()->totalChi2(LHCb::FitNode::Backward) ;
    if( chisqbw.chi2() < chisq.chi2() ) chisq = chisqbw ;
  }
  track.setChi2AndDoF( chisq.chi2(), ndof );
  return sc ;
}
