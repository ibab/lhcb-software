// Include files
// -------------
// from Gaudi
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"

// From LHCbMath
#include "LHCbMath/MatrixManip.h"

// from TrackEvent
#include "Event/TrackUnitsConverters.h"
#include "Event/FitNode.h"
#include "Event/KalmanFitResult.h"

// local
#include "TrackInterpolator.h"

// Disable warning on windows about using 'this' in constructors
#ifdef _WIN32
#pragma warning ( disable:4355 )
#endif

using namespace Gaudi;
using namespace Gaudi::Math;
using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : TrackInterpolator
//
// 2006-10-06 : Jeroen van Tilburg
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackInterpolator )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackInterpolator::TrackInterpolator( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_extrapolator("TrackMasterExtrapolator",this)
{
  // interfaces
  declareInterface<ITrackInterpolator>(this);

  declareProperty( "Extrapolator", m_extrapolator );
}

//=============================================================================
// Destructor
//=============================================================================
TrackInterpolator::~TrackInterpolator() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackInterpolator::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class

  // extrapolator
  sc = m_extrapolator.retrieve() ;
  
  return sc;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode TrackInterpolator::finalize() {
  m_extrapolator.release().ignore() ;
  return GaudiTool::finalize() ;
}

//=============================================================================
// Interpolate between the nearest nodes
//=============================================================================
StatusCode TrackInterpolator::interpolate( const Track& track,
                                           double z,
                                           State& state )
{
  // Check that track was actally fitted. Otherwise quietly call
  // extrapolator.
  const LHCb::KalmanFitResult* fr = 
    dynamic_cast<const LHCb::KalmanFitResult*>(track.fitResult()) ;
  if( fr==0 || fr->nodes().empty() )
    return m_extrapolator->propagate( track, z, state ) ;
  
  StatusCode sc = StatusCode::SUCCESS ;
  typedef std::vector<LHCb::Node*> NodeContainer ;
  const NodeContainer& nodes = fr->nodes() ;
  
  // If we are between the first and last node with a measurement, we
  // interpolate. If not, we extrapolate from the closest 'inside'
  // node. (That's more stable than interpolation.) In the end this
  // needs to work both for upstream and downstream fits. I am not
  // sure that it works for either now.

  // first find the pair of iterators such that z is between 'prevnode' and 'nextnode'
  NodeContainer::const_iterator nextnode = nodes.begin() ;
  if(  nodes.front()->z() < nodes.back()->z() ) {
    while( nextnode != nodes.end() && (*nextnode)->z() < z ) ++nextnode ;
  } else {
    while( nextnode != nodes.end() && (*nextnode)->z() > z ) ++nextnode ;
  }
  
  // determine where we are wrt to nodes with (active) measurements
  bool foundprecedingmeasurement(false) ; // is there measurement in nodes < nextnode?
  bool foundprocedingmeasurement(false) ; // is there a measurement in nodes >= nextnode?
  // any measurement nodes between begin and nextnode?
  for( NodeContainer::const_iterator inode = nodes.begin() ;
       inode != nextnode && !foundprecedingmeasurement; ++inode)
    foundprecedingmeasurement = (*inode)->type() == LHCb::Node::HitOnTrack ;
  for( NodeContainer::const_iterator inode = nextnode ;
       inode != nodes.end() && !foundprocedingmeasurement; ++inode)
    foundprocedingmeasurement = (*inode)->type() == LHCb::Node::HitOnTrack ;
  
  // we must find either of the two (there must be measurement nodes!)
  if( !foundprecedingmeasurement && !foundprocedingmeasurement)
    return Error("Did not find any measurement nodes on track!") ;

  // This is not necessarily a valid iterator, but that should be
  // caught by the logic later on.
  NodeContainer::const_iterator prevnode = nextnode ; --prevnode ;
  
  // interpolate only if we have measurements on both sides
  if( !foundprecedingmeasurement || !foundprocedingmeasurement) {
    const LHCb::Node* extrapolationnode = foundprocedingmeasurement ? *nextnode : *prevnode ;
    state = extrapolationnode->state() ;
    sc = m_extrapolator -> propagate( state, z ) ;
    if( !sc.isSuccess() ) {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "Failure with normal extrapolator: z_target = " << z 
                << " track type = " << track.type() << std::endl
                << "state = " << extrapolationnode->state() << endmsg ;
      return Warning("Failure extrapolating outside measurement range",StatusCode::FAILURE,0) ;
    }
    return sc ;
  } 
  
  // so, we interpolate. Get the nodes:
  const LHCb::FitNode* nodeNext = dynamic_cast<const LHCb::FitNode*>(*nextnode) ;
  const LHCb::FitNode* nodePrev = dynamic_cast<const LHCb::FitNode*>(*prevnode) ;
  
  if( (z-nodeNext->z()) * (z-nodePrev->z()) > 0 ) {
    error() << "logic failure in locating nodes: " 
	    << z << ", " << nodePrev->z() << "," << nodeNext->z() << endmsg ;
    return StatusCode::FAILURE ;
  }
  
  
  // bail out if we have actually reached our destination
  if( std::abs(nodeNext->z() - z) < TrackParameters::propagationTolerance ) {
    state = nodeNext->state() ;
    return StatusCode::SUCCESS ;
  }
  if( std::abs(nodePrev->z() - z) < TrackParameters::propagationTolerance ) {
    state = nodePrev->state() ;
    return StatusCode::SUCCESS ;
  }

  // Get the filtered states
  State stateDown = nodePrev->filteredStateForward();
  State stateUp   = nodeNext->filteredStateBackward();
  
  // extrapolate the upstream and downstream states
  sc = m_extrapolator -> propagate( stateDown, z );  
  if( sc.isFailure() ) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "Error propagating downstream state to z = " << z << std::endl
              << "state = " << stateDown << endmsg ;
    return Warning("Failure propagating downstream state",StatusCode::FAILURE,0) ;
  }
  
  sc = m_extrapolator -> propagate( stateUp  , z );
  if( sc.isFailure() ) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "Error propagating upstream state to z = " << z 
              << " tracktype = " << track.type() << std::endl 
              << "state = " << stateUp << endmsg ;
    return Warning("Failure propagating upstream state",StatusCode::FAILURE,0) ;    
  }
  
  // Get the predicted downstream state and invert the covariance matrix
  const TrackVector& stateDownX = stateDown.stateVector();
  TrackSymMatrix invStateDownC = stateDown.covariance();
  if ( !invStateDownC.InvertChol() )
    return Warning( "Failure inverting matrix in smoother",StatusCode::FAILURE,0 );

  // Get the predicted upstream state and invert the covariance matrix
  const TrackVector& stateUpX = stateUp.stateVector();
  TrackSymMatrix invStateUpC = stateUp.covariance();
  if ( !invStateUpC.InvertChol() )
    return Warning( "Failure inverting matrix in smoother",StatusCode::FAILURE,0 );

  // Add the inverted matrices
  TrackSymMatrix& stateC = state.covariance();
  stateC = invStateDownC + invStateUpC;
  if ( !stateC.InvertChol() )
    return Warning( "Failure inverting matrix in smoother",StatusCode::FAILURE,0 );
  
  // Get the state by calculating the weighted mean
  TrackVector& stateX = state.stateVector();
  stateX = stateC * ((invStateDownC * stateDownX) + (invStateUpC * stateUpX)) ;
  state.setZ( z );   

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "filteredstate A: "
  	    << stateUpX << std::endl
  	    << "filteredstate B: "
  	    << stateDownX << std::endl
  	    << "smoothed state A: "
  	    << nodePrev->state()
  	    << "smoothed state B: "
  	    << nodeNext->state() 
  	    << "interpolated state: "
  	    << state << endmsg ;

  return StatusCode::SUCCESS;
}
