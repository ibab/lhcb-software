// $Id: TrackInterpolator.cpp,v 1.5 2007-11-30 14:49:53 wouter Exp $
// Include files
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"

// From LHCbMath
#include "LHCbMath/MatrixManip.h"

// from TrackEvent
#include "Event/TrackUnitsConverters.h"
#include "Event/FitNode.h"

// local
#include "TrackInterpolator.h"

using namespace Gaudi;
using namespace Gaudi::Math;
using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : TrackInterpolator
//
// 2006-10-06 : Jeroen van Tilburg
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackInterpolator );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackInterpolator::TrackInterpolator( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  // interfaces
  declareInterface<ITrackInterpolator>(this);

  declareProperty( "Extrapolator", m_extrapolatorName =
                   "TrackMasterExtrapolator" );
};

//=============================================================================
// Destructor
//=============================================================================
TrackInterpolator::~TrackInterpolator() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackInterpolator::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class

  // extrapolator
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName,
                                             "Extrapolator", this );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Interpolate between the nearest nodes
//=============================================================================
StatusCode TrackInterpolator::interpolate( const Track& track,
                                           double z,
                                           State& state )
{
  // Check if there are nodes on the track
  typedef std::vector<Node*> NodeContainer ;
  const NodeContainer& nodes = track.nodes();
  if ( nodes.empty() ) return Error("No nodes on track found.");

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
  
  // determine where we are wrt to nodes with measurements
  bool foundprecedingmeasurement(false) ; // is there measurement in nodes < nextnode?
  bool foundprocedingmeasurement(false) ; // is there a measurement in nodes >= nextnode?
  // any measurement nodes between begin and nextnode?
  for( NodeContainer::const_iterator inode = nodes.begin() ;
       inode != nextnode && !foundprecedingmeasurement; ++inode)
    foundprecedingmeasurement = (*inode)->hasMeasurement() ;
  for( NodeContainer::const_iterator inode = nextnode ;
       inode != nodes.end() && !foundprocedingmeasurement; ++inode)
    foundprocedingmeasurement = (*inode)->hasMeasurement() ;
  
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
    return m_extrapolator -> propagate( state, z ) ;
  } 
  
  // so, we interpolate. Get the nodes:
  const LHCb::FitNode* nodeUp   = dynamic_cast<LHCb::FitNode*>(*nextnode) ;
  const LHCb::FitNode* nodeDown = dynamic_cast<LHCb::FitNode*>(*prevnode) ;
  if( nodeUp->z() > nodeDown->z() ) std::swap(nodeUp,nodeDown) ;
  
  // bail out if we have actually reached our destination
  if( fabs(nodeUp->z() - z) < TrackParameters::propagationTolerance ) {
    state = nodeUp->state() ;
    return StatusCode::SUCCESS ;
  }
  if( fabs(nodeDown->z() - z) < TrackParameters::propagationTolerance ) {
    state = nodeDown->state() ;
    return StatusCode::SUCCESS ;
  }

  // Get the predicted states
  State stateDown = nodeDown->predictedStateDown();
  State stateUp   = nodeUp->predictedStateUp();

  if ( nodeDown->hasMeasurement() ) filter( *nodeDown, stateDown );
  if ( nodeUp->hasMeasurement()   ) filter( *nodeUp, stateUp );

  // extrapolate the upstream and downstream states
  m_extrapolator -> propagate( stateDown, z );  
  m_extrapolator -> propagate( stateUp  , z );

  // Get the predicted downstream state and invert the covariance matrix
  const TrackVector& stateDownX = stateDown.stateVector();
  TrackSymMatrix invStateDownC = stateDown.covariance();
  StatusCode sc = invertMatrix( invStateDownC );
  if ( sc.isFailure() ) return Error( "inverting matrix in smoother" );

  // Get the predicted upstream state and invert the covariance matrix
  const TrackVector& stateUpX = stateUp.stateVector();
  TrackSymMatrix invStateUpC = stateUp.covariance();
  sc = invertMatrix( invStateUpC );
  if ( sc.isFailure() ) return Error( "inverting matrix in smoother" );

  // Add the inverted matrices
  TrackSymMatrix& stateC = state.covariance();
  stateC = invStateDownC + invStateUpC;
  sc = invertMatrix( stateC );
  if ( sc.isFailure() ) return Error( "inverting matrix in smoother" );

  // Get the state by calculating the weighted mean
  TrackVector& stateX = state.stateVector();
  stateX = stateC * ((invStateDownC * stateDownX) + (invStateUpC * stateUpX)) ;
  state.setZ( z );

  return StatusCode::SUCCESS;
};


//=============================================================================
// Filter step (needed as the result of filter step is not stored in the Node)
//=============================================================================
StatusCode TrackInterpolator::filter(const FitNode& node, State& state) 
{
  const Measurement& meas = node.measurement();

  // check z position
  if ( fabs(meas.z() - state.z()) > 1e-6) {
    Warning( "Z positions of State and Measurement are not equal", 0, 1 );
    debug() << "State at z=" << state.z() 
            << ", Measurement at z=" << meas.z() << endmsg;
  }

  // get the state vector and cov
  TrackVector&    X = state.stateVector();
  TrackSymMatrix& C = state.covariance();

  // Get the projected residual
  const TrackProjectionMatrix& H = node.projectionMatrix();
  double res = node.projectionTerm() - Vector1(H*X)(0) ;
  double errorMeas2 = node.errMeasure2();
  double errorRes2 = errorMeas2 + Similarity( H, C )(0,0) ;

  // calculate gain matrix K
  SMatrix<double,5,1> K = (C * Transpose(H)) / errorRes2;

  // update the state vector
  X += K.Col(0) * res ;

  // update the covariance matrix
  static const TrackSymMatrix unit = TrackSymMatrix( SMatrixIdentity());
  C = Symmetrize( Similarity( unit - ( K*H ), C ) 
                  +(errorMeas2*K)*Transpose(K) );

  return StatusCode::SUCCESS;
}

//=========================================================================
// Invert prev node covariance matrix
// What follows may seem strange - trust me it works - you
// are strongly recommended NOT to change it. It turns out that
// the choice of MeV, mm as units is BAD - the inversion simply fails
// for numerical reasons. Therefore it is necessary to change back to G3
// units, invert then go back to G4 units
// M. Needham 13/6/2000
// J.A. Hernando (we trust you) 15/05/05
//=========================================================================
StatusCode TrackInterpolator::invertMatrix( Gaudi::TrackSymMatrix& m )
{
  TrackUnitsConverters::convertToG3( m );
  bool OK = m.Invert();
  TrackUnitsConverters::convertToG4( m );

  if ( !OK ) {
    return Warning("Failed to invert covariance matrix", StatusCode::FAILURE,1);
  }

  return StatusCode::SUCCESS;
}
