// $Id: TrackKalmanFilter.cpp,v 1.17 2006-05-16 08:42:47 mneedham Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from LHCbDefinitions
#include "Kernel/PhysicalConstants.h"
#include "Kernel/MatrixManip.h"

// from TrackEvent
#include "Event/TrackFunctor.h"
#include "Event/TrackUnitsConverters.h"

// local
#include "TrackKalmanFilter.h"

using namespace Gaudi;
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TrackKalmanFilter
//
//  Original Author: Rutger van der Eijk
//  Created: 07-04-1999
//  Adapted: 21-03-2002  Olivier Callot
//  Adapted: 15-04-2005  Jose A. Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const   ToolFactory<TrackKalmanFilter>  s_factory;
const IToolFactory& TrackKalmanFilterFactory = s_factory;

//=========================================================================
// Standard Constructor, initializes variables
//=========================================================================
TrackKalmanFilter::TrackKalmanFilter( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent) :
  GaudiTool( type, name, parent)
{
  declareInterface<ITrackFitter>( this );

  declareProperty( "Extrapolator"  , m_extrapolatorName =
                                           "TrackMasterExtrapolator" );
  declareProperty( "Projector"     , m_projectorName =
                                           "TrackMasterProjector" );
  declareProperty( "StoreTransport", m_storeTransport   = true  );

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

  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName,
                                             "Extrapolator", this );
  m_projector    = tool<ITrackProjector>( m_projectorName,
                                          "Projector", this );
  m_debugLevel   = msgLevel( MSG::DEBUG );
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// Helper to print a failure comment
//=========================================================================
StatusCode TrackKalmanFilter::failure(const std::string& comment) {
  info() << "TrackKalmanFilter failure: " << comment << endreq;
  return StatusCode::FAILURE;
}

//=========================================================================
// Fit the track
//=========================================================================
StatusCode TrackKalmanFilter::fit( Track& track )
{
  StatusCode sc; 
 
  // First node must have a seed state
  std::vector<Node*>& nodes = track.nodes();
  std::vector<Node*>::iterator iNode = nodes.begin();
  State state = (*iNode)->state() ;

  // Loop over the nodes in the current order (should be sorted)
  // ==> prediction and filter
  for ( ; iNode != nodes.end(); ++iNode) {
    FitNode& node = *(dynamic_cast<FitNode*>(*iNode));

    // Prediction step
    sc = predict( node, state );
    if ( sc.isFailure() ) return failure( "Unable to predict at a node" );

    // Filter step
    if ( node.hasMeasurement() ) { 
      sc = filter( node, state );
      if ( sc.isFailure() ) return failure( "Unable to filter node " );
    }

    // save filtered state
    node.setState( state );
  }
  
  // Loop in opposite direction for the smoother
  std::vector<Node*>::reverse_iterator iPrevNode = nodes.rbegin();
  std::vector<Node*>::reverse_iterator ithisNode = iPrevNode;
  ++ithisNode;
  while ( iPrevNode != nodes.rend() && ithisNode != nodes.rend() ) {
    FitNode& prevNode = *(dynamic_cast<FitNode*>(*iPrevNode));
    FitNode& thisNode = *(dynamic_cast<FitNode*>(*ithisNode));
    // Smoother step
    sc = smooth( thisNode, prevNode );
    if ( sc.isFailure() ) return failure( "Unable to smooth node!" );  
    ++ithisNode;
    ++iPrevNode;
  }

  // Compute the chi2 and degrees of freedom
  computeChi2( track );

  // set the HistoryFit flag to "Kalman fit done"
  track.setHistoryFit( Track::Kalman );

  return sc;
}


//=========================================================================
// Predict the state to this node
//=========================================================================
StatusCode TrackKalmanFilter::predict(FitNode& aNode, State& aState)
{
  TrackVector prevStateVec = aState.stateVector();
  TrackSymMatrix prevStateCov = aState.covariance();
  double z = aNode.z();

  if ( (z - aState.z()) != 0.0 ) { // only extrapolate when needed
    // first iteration only: need to call extrapolator to get 
    // the predicted state at measurement
    if ( !(aNode.transportIsSet()) || !m_storeTransport ) {
      StatusCode sc = m_extrapolator -> propagate( aState, z );
      if ( sc.isFailure() )
        return failure( "unable to predict state at next measurement" );
      aState.setLocation( (aNode.state()).location() );

      // store transport matrix F, noise matrix and vector for next iterations
      if ( m_storeTransport ) {
        const TrackMatrix& F = m_extrapolator -> transportMatrix();
        aNode.setTransportMatrix( F );
        aNode.setTransportVector( aState.stateVector() - F * prevStateVec );
        aNode.setNoiseMatrix( aState.covariance() -
                              ROOT::Math::Similarity<double,5,5>
                              ( F, prevStateCov ) );
        aNode.setTransportIsSet( true );
      }
    }
    // next iterations: update node with transport information 
    // from 1st iteration to save CPU time
    else { 
      const TrackMatrix& F = aNode.transportMatrix();
      TrackVector& stateVec = aState.stateVector();
      TrackSymMatrix& stateCov = aState.covariance();
      TrackVector tempVec( stateVec );
      stateVec = F * tempVec + aNode.transportVector();
      stateCov = ROOT::Math::Similarity<double,5,5>( F, stateCov )
                 + aNode.noiseMatrix();
      aState.setZ( z );
      aState.setLocation( (aNode.state()).location() );
    }
  }

  // save predicted state
  aNode.setPredictedState( aState );

  // project the state into the measurement
  if ( aNode.hasMeasurement() ) {
    Measurement& meas = aNode.measurement();
    StatusCode sc = m_projector -> project( aState, meas );
    if ( sc.isFailure() ) 
      return failure( "not able to project a state into a measurement" );
    const TrackVector& H = m_projector -> projectionMatrix();

    // calculate predicted residuals
    double res       = m_projector -> residual();
    double errorRes  = m_projector -> errResidual();
    double errorMeas = m_projector -> errMeasure();

    aNode.setProjectionMatrix( H );
    aNode.setResidual( res ) ;
    aNode.setErrResidual( errorRes ) ;
    aNode.setErrMeasure( errorMeas ) ;
  }
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::filter(FitNode& node, State& state) 
{
  Measurement& meas = node.measurement();

  // check z position
  if ( fabs(meas.z() - state.z()) > 1e-6) {
    warning() << "z-position of State (=" << state.z() 
              << ") and Measurement (= " << meas.z() 
              << ") are not equal." << endreq;
  }

  // get reference to the state vector and cov
  TrackVector& tX = state.stateVector();
  TrackSymMatrix& tC = state.covariance();

  // get the predicted residuals
  double res        = node.residual();
  double errorRes2  = node.errResidual2();
  double errorMeas2 = node.errMeasure2();

  // calculate gain matrix K
  const TrackVector& H = node.projectionMatrix();
  TrackVector mK = (tC * H) / errorRes2;

  // update the state vector
  tX += ( mK * res );

  // update the covariance matrix
  TrackMatrix uniDiagMat = TrackMatrix( ROOT::Math::SMatrixIdentity());
  TrackMatrix B = uniDiagMat - TrackVectorProd( mK, H );
  TrackMatrix bTimesC = B * tC;
  tC = LHCb::MatrixManip::Symmetrize<TrackMatrix>( bTimesC );

  // update the residual and the error on the residual
  double HK = ROOT::Math::Dot( H, mK );
  res      *= (1 - HK);
  errorRes2 = (1 - HK) * ( errorMeas2 );

  node.setResidual( res );
  node.setErrResidual( sqrt(errorRes2) );

  if ( m_debugLevel ) verbose() << "filter z = " << node.z()
                                << " chi2 = " << node.chi2() << endmsg;

  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
TrackMatrix TrackKalmanFilter::TrackVectorProd( TrackVector& vec1,
                                                    const TrackVector& vec2 )
{
  // hack to do the product (5x1) * (1x5) of TrackVectors
  // this is really an ugly hack!
  TrackMatrix result = TrackSymMatrix();
  for ( unsigned int i = 0; i < vec1.Dim(); ++i ) {
    for ( unsigned int j = 0; j < vec2.Dim(); ++j ) {
      result(i,j) = vec1[i] * vec2[j];
    }
  }
  return result;
}

//----------------------------------------------------------------
// smooth a node on a track
// Input: node to be smoothed (k) + last node (k+1)
// remember we are now running in the opposite direction to the filter !!
// M. Needham 9/11/99
//----------------------------------------------------------------
StatusCode TrackKalmanFilter::smooth( FitNode& thisNode,
                                      const FitNode& prevNode )
{
  // preliminaries, first we need to invert the _predicted_ covariance
  // matrix at k+1
  const TrackVector& prevNodeX = prevNode.predictedState().stateVector();
  const TrackSymMatrix& prevNodeC = prevNode.predictedState().covariance();

  // check that the elements are not too large else dsinv will crash
  StatusCode sc = checkInvertMatrix( prevNodeC );
  if ( sc.isFailure() ) return failure("not valid matrix in smoother"); 

  // invert the covariance matrix
  TrackSymMatrix invPrevNodeC = prevNodeC;
  sc = invertMatrix( invPrevNodeC );
  if ( sc.isFailure() ) {
    debug() << "invPrevNodeC = " << invPrevNodeC << endreq;
    return failure( "inverting matrix in smoother" );
  }
  

  // references to _predicted_ state + cov of this node from the first step
  TrackVector& thisNodeX = thisNode.state().stateVector();
  TrackSymMatrix& thisNodeC = thisNode.state().covariance();

  // Save filtered state vector
  TrackVector oldNodeX = thisNodeX ;

  // transport
  const TrackMatrix& F = prevNode.transportMatrix();

  // calculate gain matrix A
  TrackMatrix A = thisNodeC * ROOT::Math::Transpose( F ) * invPrevNodeC;

  // best = smoothed state at prev Node
  const TrackVector& prevNodeSmoothedX = prevNode.state().stateVector();
  const TrackSymMatrix& prevNodeSmoothedC = prevNode.state().covariance();

  // smooth state
  thisNodeX += A * (prevNodeSmoothedX - prevNodeX);

  // smooth covariance  matrix
  TrackSymMatrix covDiff = prevNodeSmoothedC - prevNodeC;
  TrackSymMatrix covUpDate =
    ROOT::Math::Similarity<double,5,5>( A, covDiff );

  thisNodeC += covUpDate;

  // check that the cov matrix is positive
  sc = checkPositiveMatrix( thisNodeC ) ;
  if ( sc.isFailure() ) return Warning("non-positive Matrix in smoother");

  // No need to update residuals for node w/o measurement
  if ( thisNode.hasMeasurement() ) {
    // update = smooth the residuals
    const TrackVector& H = thisNode.projectionMatrix();
    double res = thisNode.residual()
                 - ROOT::Math::Dot( H, thisNodeX - oldNodeX ) ;
    thisNode.setResidual( res );
    double errRes2 =
      thisNode.errResidual2()
      - ROOT::Math::Similarity<double,5>( H, covUpDate );
    if ( errRes2 < 0.) {
      return Warning( "Negative residual error in smoother!" );
    }
    thisNode.setErrResidual( sqrt(errRes2) );    

    if ( m_debugLevel ) verbose() << "smoother z = " << thisNode.z()
                                  << " chi2 = " << thisNode.chi2() << endmsg;
  }
       
  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
void TrackKalmanFilter::computeChi2( Track& track ) 
{
  double chi2 = 0.;
  int ndof = -( track.firstState().nParameters() );

  // Loop over the node to calculate total chi2 and # measurements
  const std::vector<Node*>& nodes = track.nodes();
  std::vector<Node*>::const_iterator it = nodes.begin();
  for ( ; it != nodes.end(); ++it ) {
    // a node without a measurement has chi2 = 0 ;-)
    chi2 += (*it)->chi2();
    if ( (*it)->hasMeasurement() ) ++ndof;
  }

  double chi2Norma = ( ndof != 0 ) ? chi2/((double) (ndof)) : 0.0;
  track.setChi2PerDoF( chi2Norma );
  track.setNDoF( ndof );
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::checkInvertMatrix( const TrackSymMatrix& mat ) 
{
  unsigned int nParams = (unsigned int) mat.kRows;
  for ( unsigned int i = 0; i < nParams; ++i ) {
    for ( unsigned int j = 0; j < nParams; ++j ) {
      if ( mat(i,j) > 1e20 )
        return Warning( "Covariance errors too big to invert!" );
    }
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::checkPositiveMatrix( TrackSymMatrix& mat ) 
{
  unsigned int nParams = (unsigned int) mat.kRows;
  for ( unsigned int i=0; i < nParams; ++i ) {
    if ( mat(i,i) <= 0. )
      return Warning( "Covariance matrix has non-positive elements!" );
  }

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
StatusCode TrackKalmanFilter::invertMatrix( TrackSymMatrix& invPrevNodeC )
{
  // Geant3 units
  TrackUnitsConverters::convertToG3( invPrevNodeC );

  bool OK = invPrevNodeC.Invert();

  // Geant4 units
  TrackUnitsConverters::convertToG4( invPrevNodeC );

  if ( !OK ) {
    warning() << "Failed to invert covariance matrix, failure code "
              << OK << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
