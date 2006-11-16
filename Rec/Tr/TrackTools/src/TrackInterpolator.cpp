// Include files
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// From LHCbMath
#include "LHCbMath/MatrixManip.h"

// from LHCbDefinitions
#include "Kernel/GenericVectorTypes.h"
#include "Kernel/SymmetricMatrixTypes.h"

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
StatusCode TrackInterpolator::interpolate( Track& track,
                                           double z,
                                           State& state )
{
  // Check if there are nodes on the track
  std::vector<Node*> nodes = track.nodes();
  if ( nodes.empty() ) return Error("No nodes on track found.");

  // Find the first node > z
  std::vector<Node*>::iterator iter = nodes.begin();
  while ( iter != nodes.end() && (*iter)->z() < z ) ++iter;

  // If not found delegate to extrapolator
  if ( iter == nodes.begin() || iter == nodes.end() )
    m_extrapolator -> propagate( track, z, state );

  FitNode* node2 = dynamic_cast<FitNode*>(*iter);
  --iter;
  FitNode* node1 = dynamic_cast<FitNode*>(*iter);

  // Get the predicted states (assumes an upstream fit !!!)
  State state1 = node1->predictedStateRev();
  State state2 = node2->predictedState();

  filter( *node1, state1 );
  filter( *node2, state2 );
  
  const TrackVector& state1X = state1.stateVector();
  TrackSymMatrix invState1C = state1.covariance();
  StatusCode sc = invertMatrix( invState1C );
  if ( sc.isFailure() ) return Error( "inverting matrix in smoother" );

  const TrackVector& state2X = state2.stateVector();
  TrackSymMatrix invState2C = state2.covariance();
  sc = invertMatrix( invState2C );
  if ( sc.isFailure() ) return Error( "inverting matrix in smoother" );

  // Add the inverted matrices
  TrackSymMatrix& stateC = state.covariance();
  stateC = invState1C + invState2C;
  sc = invertMatrix( stateC );
  if ( sc.isFailure() ) return Error( "inverting matrix in smoother" );

  // Get the state by calculating the weighted mean
  TrackVector& stateX = state.stateVector();
  stateX = stateC * ((invState1C * state1X) + (invState2C * state2X)) ;

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

  // get reference to the state vector and cov
  TrackVector&    X = state.stateVector();
  TrackSymMatrix& C = state.covariance();

  // get the predicted residuals
  double res        = node.residual();
  double errorRes2  = node.errResidual2();
  double errorMeas2 = node.errMeasure2();

  // calculate gain matrix K
  const TrackProjectionMatrix& H = node.projectionMatrix();
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
StatusCode TrackInterpolator::invertMatrix( TrackSymMatrix& m )
{
  TrackUnitsConverters::convertToG3( m );
  bool OK = m.Invert();
  TrackUnitsConverters::convertToG4( m );

  if ( !OK ) {
    return Warning("Failed to invert covariance matrix", StatusCode::FAILURE,1);
  }

  return StatusCode::SUCCESS;
}
