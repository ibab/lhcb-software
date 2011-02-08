// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

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
                                      const IInterface* parent) :
  GaudiTool( type, name, parent)
{
  declareInterface<ITrackKalmanFilter>( this );

  declareProperty( "BiDirectionalFit" , m_biDirectionalFit  = true   );
  declareProperty( "Smooth", m_smooth = true ) ;
  declareProperty( "ForceBiDirectional" , m_forceBidirectional  = true   );
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
  
  LHCb::KalmanFitResult::NodeContainer& nodes = kalfit->nodes() ;
  if( nodes.empty() ) return Warning( "Fit failure: track has no nodes", StatusCode::FAILURE,0 );
  
  // get the total chisquare of the forward filter
  double chisq(0) ;
  int ndof(0) ;
  double chisqbw(0) ;
  const LHCb::State* laststate(0) ;

  // This is set up with the aim to trigger the cache such that there
  // will be no nested calls. That makes it easier to profile the
  // fit. Eventually, we could do without all of this, since
  // everything is anyway computed on demand.
  for( LHCb::TrackFitResult::NodeContainer::iterator inode = kalfit->nodes().begin() ;
       inode != kalfit->nodes().end(); ++inode) {
    LHCb::FitNode* fitnode = static_cast<LHCb::FitNode*>(*inode) ;
    fitnode->predictedState(LHCb::FitNode::Forward) ;
    fitnode->filteredState(LHCb::FitNode::Forward) ;
  }
  if(m_forceBidirectional){
    for( LHCb::TrackFitResult::NodeContainer::reverse_iterator inode = kalfit->nodes().rbegin() ;
	 inode != kalfit->nodes().rend(); ++inode) {
      LHCb::FitNode* fitnode = static_cast<LHCb::FitNode*>(*inode) ;
      fitnode->predictedState(LHCb::FitNode::Backward) ;
      fitnode->filteredState(LHCb::FitNode::Backward) ;
    }
  }
  // This is the only thing we KalmanFilter stll for: set the total
  // chisquare of the track. This could also be done from
  // TrackMasterFitter.
  BOOST_FOREACH( LHCb::Node* node, nodes) {
    LHCb::FitNode* fitnode = static_cast<LHCb::FitNode*>(node) ;
    if( node->type()==LHCb::Node::HitOnTrack ) {
      chisq   += fitnode->deltaChi2Forward() ;
      chisqbw += fitnode->deltaChi2Backward() ;
      laststate = &(fitnode->filteredState(LHCb::FitNode::Forward)) ;
      ++ndof ;
    }
  }
  // force the smoothing. 
  if (m_forceSmooth){
    for( LHCb::TrackFitResult::NodeContainer::iterator inode = kalfit->nodes().begin() ;
	 inode != kalfit->nodes().end(); ++inode)
      static_cast<LHCb::FitNode*>(*inode)->state() ; 
  }

  if (kalfit->inError()){
    sc = Warning(kalfit->getError(), StatusCode::FAILURE, 0 ) ;
  }

  // Count the number of active track parameters. For now, just look at the momentum.
  if(laststate) {
    const double threshold = 0.1 ;
    size_t npar = m_DoF != 5u ? m_DoF :
      (laststate->covariance()(4,4) / kalfit->seedCovariance()(4,4) < threshold ? 5 : 4) ;
    dynamic_cast<LHCb::KalmanFitResult*>(track.fitResult())->setNTrackParameters( npar ) ;
    ndof -= npar ;
  }
  track.setChi2AndDoF( std::min(chisq,chisqbw), ndof );
  return sc ;
}
