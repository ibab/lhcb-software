// $Id: TrackKalmanFilter.cpp,v 1.4 2005-10-25 12:50:05 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from CLHEP
#include "CLHEP/Matrix/DiagMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Units/PhysicalConstants.h"

// from TrackEvent
#include "Event/TrackFunctor.h"

// local
#include "TrackKalmanFilter.h"

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

  // Define the (default) fixed z-positions
  m_zPositions.push_back(   990.0*mm );
  m_zPositions.push_back(  2165.0*mm );
  m_zPositions.push_back(  9450.0*mm );
  m_zPositions.push_back( 11900.0*mm );

  declareProperty( "Extrapolator"        , m_extrapolatorName =
                                           "TrackMasterExtrapolator" );
  declareProperty( "Projector"           , m_projectorName =
                                           "TrackMasterProjector" );
  declareProperty( "ZPositions"          , m_zPositions               );
  declareProperty( "StatesAtMeasZPos"    , m_statesAtMeasZPos = false );
  declareProperty( "StateAtBeamLine"     , m_stateAtBeamLine  = true  );
  declareProperty( "NumberFitIterations" , m_numFitIter       = 5     );
  declareProperty( "Chi2Outliers"        , m_chi2Outliers     = 9.0   );
  declareProperty( "StoreTransport"      , m_storeTransport   = true  );

  m_nodes.clear();
  m_K = HepVector(5,0);
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
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName );

  m_projector    = tool<ITrackProjector>( m_projectorName );

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
// 
//=========================================================================
void TrackKalmanFilter::iniKalman(Track & track) 
{
  // the Kalman filter has a private copy of the nodes to accelerate
  // the computations. The track owns the Nodes, so it is responsability
  // of the Track destructor to delete them!
  m_nodes.clear();
  std::vector<Node*>& nodes = track.nodes();
  if ( !nodes.empty() )
    for (std::vector<Node*>::iterator it = nodes.begin(); it != nodes.end();
         ++it) delete *it;  
  nodes.clear();

}

//=========================================================================
// Fit the track upstream with a seed state (filter and smooth)
//=========================================================================
StatusCode TrackKalmanFilter::fitUpstream( Track& track, State& state )
{
  StatusCode sc;

  iniKalman( track );

  // TODO: improve this! This is common to fitUpstream/fitDownstream
  // reserve some space in node vector
  std::vector<Measurement*>& measures = track.measurements();
  m_nodes.reserve( measures.size() );

  // Create the nodes and add them to the private copy
  for ( std::vector<Measurement*>::reverse_iterator it = measures.rbegin(); 
        it != measures.rend(); ++it ) {
    Measurement& meas = *(*it);
    FitNode* node = new FitNode( meas );
    m_nodes.push_back( node );
    ////track.addToNodes( node );
  }

  std::vector<FitNode*>::iterator iNode = m_nodes.begin();
  double z = (*iNode) -> measurement().z();

  sc = m_extrapolator -> propagate( state, z );
  if ( sc.isFailure() ) 
    return failure("unable to extrapolate to measurement");

  // Keep the seed state so we can use it in another iteration
  State* seedState = state.clone();

  // Iterations of filtering-smoothing sequence
  bool doNextIteration = true;
  int iter = 0;
  do {
    // fit iteration
    ++iter;

    //debug() << "ITERATION # " << iter << endreq;

    // Call the filter sequence
    sc = filter( track, state );
    if ( sc.isFailure() ) {
      delete seedState;
      return failure( "unable to filter the track" );
    }

    if ( m_debugLevel ) debug() << "### Nodes after filtering:" << endreq;
    std::vector<FitNode*>::iterator iNode;
    for ( iNode = m_nodes.begin(); iNode != m_nodes.end(); ++iNode ) {
      FitNode& node = *(*iNode);
      if ( m_debugLevel )
        debug() << "-- at z = " << node.z()
                << "best state = " << node.state().stateVector() << endreq
                << "predicted state = " << node.predictedState().stateVector() << endreq
                << "filtered state = " << node.filteredState().stateVector() << endreq
                << endreq;
    }
    
    // Call the smoother sequence
    sc = smoother( track );
    if ( sc.isFailure() ) {
      clearNodes(); // clear the nodes vector
      return failure("unable to smooth the track");
    }

    if ( m_debugLevel ) debug() << "### Nodes after smoothing:" << endreq;
    for ( iNode = m_nodes.begin(); iNode != m_nodes.end(); ++iNode ) {
      FitNode& node = *(*iNode);
      if ( m_debugLevel )
        debug() << "-- at z = " << node.z()
                << " best state = " << node.state().stateVector() << endreq
                << "predicted state = " << node.predictedState().stateVector() << endreq
                << "filtered state = " << node.filteredState().stateVector() << endreq
                << endreq;
    }

    // Prepare for next iteration
    if ( iter < m_numFitIter ) {
      // Outlier removal
      if ( m_nodes.size() > state.nParameters() ) {
        doNextIteration = outlierRemoved( track );
      } else {
        doNextIteration = false;
      }

      // Update best state at end of smoothing
      iNode = m_nodes.begin();
      updateCurrentState( state, *(*iNode) );

      // Re-seeding for next iteration
      reSeed( *seedState, state );

      // Update the reference trajectories in the measurements
      updateMeasurements();
    }

  } while ( iter < m_numFitIter && doNextIteration );

  // update the best state
  std::vector<FitNode*>::reverse_iterator riNode = m_nodes.rbegin();
  updateCurrentState( state, *(*riNode) );

  //determine the track states at user defined z positions
  sc = determineStates( track );
  if ( sc.isFailure() ) {
    warning() << "Failed to determine the states at the requested z-positions" << endreq;
    //clear the node vector
    clearNodes();
    return sc;
  }

  delete seedState;
  // clear the node vector
  clearNodes();

  // set the HistoryFit flag to "Kalman fit done"
  track.setHistoryFit( Track::Kalman );

  return sc;
}

//=========================================================================
// Fit the track downstream with a seed state (filter and smooth)
//=========================================================================
StatusCode TrackKalmanFilter::fitDownstream( Track& track, State& state )
{
  StatusCode sc;

  iniKalman( track );

  // reserve some space in node vector
  std::vector<Measurement*>& measures = track.measurements();
  m_nodes.reserve( measures.size() );

  // Create the nodes and add them to the private copy
  for ( std::vector<Measurement*>::iterator it = measures.begin(); 
        it != measures.end(); ++it ) {
    Measurement& meas = *(*it);
    FitNode* node = new FitNode( meas );
    m_nodes.push_back( node );
    ////track.addToNodes( node );
  }

  std::vector<FitNode*>::iterator iNode = m_nodes.begin();
  double z = (*iNode) -> measurement().z();

  sc = m_extrapolator -> propagate( state, z );
  if (sc.isFailure()) 
    return failure("unable to extrapolate to measurement");

  // Keep the seed state so we can use it in another iteration
  State* seedState = state.clone();

  // Iterations of filtering-smoothing sequence
  bool doNextIteration = true;
  int iter = 0;
  do {
    // fit iteration
    ++iter;

    //debug() << "ITERATION # " << iter << endreq;

    // Call the filter sequence
    sc = filter(track,state);
    if ( sc.isFailure() ) {
      delete seedState;
      return failure( "unable to filter the track" );
    }

    if ( m_debugLevel ) debug() << "### Nodes after filtering:" << endreq;
    std::vector<FitNode*>::iterator iNode;
    for (iNode = m_nodes.begin(); iNode != m_nodes.end(); ++iNode) {
      FitNode& node = *(*iNode);
      if ( m_debugLevel )
        debug() << "-- at z = " << node.z()
                << "best state = " << node.state().stateVector() << endreq
                << "predicted state = " << node.predictedState().stateVector() << endreq
                << "filtered state = " << node.filteredState().stateVector() << endreq
                << endreq;
    }
    
    // Call the smoother sequence
    sc = smoother(track);
    if ( sc.isFailure() ) {
      clearNodes(); // clear the nodes vector
      return failure("unable to smooth the track");
    }

    if ( m_debugLevel ) debug() << "### Nodes after smoothing:" << endreq;
    for (iNode = m_nodes.begin(); iNode != m_nodes.end(); ++iNode) {
      FitNode& node = *(*iNode);
      if ( m_debugLevel )
        debug() << "-- at z = " << node.z()
                << " best state = " << node.state().stateVector() << endreq
                << "predicted state = " << node.predictedState().stateVector() << endreq
                << "filtered state = " << node.filteredState().stateVector() << endreq
                << endreq;
    }

    // Prepare for next iteration
    if ( iter < m_numFitIter ) {
      // Outlier removal
      if ( m_nodes.size() > state.nParameters() ) {
        doNextIteration = outlierRemoved( track );
      } else {
        doNextIteration = false;
      }

      // Update best state at end of smoothing
      iNode = m_nodes.begin();
      updateCurrentState( state, *(*iNode) );

      // Re-seeding for next iteration
      reSeed( *seedState, state );

      // Update the reference trajectories in the measurements
      updateMeasurements();
    }

  } while ( iter < m_numFitIter && doNextIteration );

  // update the best state
  iNode = m_nodes.begin();
  updateCurrentState( state, *(*iNode) );
  
  //determine the track states at user defined z positions
  sc = determineStates( track );
  if ( sc.isFailure() ) {
    warning() << "Failed to determine the states at the requested z-positions" << endreq;
    //clear the node vector
    clearNodes();
    return sc;
  }

  delete seedState;
  // clear the node vector
  clearNodes();

  // set the HistoryFit flag to "Kalman fit done"
  track.setHistoryFit( Track::Kalman );

  return sc;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::filter(Track& track, State& state)
{
  StatusCode sc = StatusCode::SUCCESS;

  //if ( m_debugLevel ) {
  //  debug() << " seed state at z \t " << state.z() << endreq
  //          << " seed state vector \t" << state.stateVector() << endreq
  //          << " seed state cov \t" << state.covariance() << endreq
  //          << " track nodes size \t" << m_nodes.size() << endreq;
  //}

  std::vector<FitNode*>::iterator iNode = m_nodes.begin();
  //unsigned int nNodes = 0;
  for (iNode = m_nodes.begin(); iNode != m_nodes.end(); ++iNode) {
    FitNode& node = *(*iNode);
      //if ( m_debugLevel ) debug() << "-> predict(...) for node # " << nNodes
      //                            << " at z = " << node.measurement().z() << endreq;
    sc = predict( node, state );
    if ( sc.isFailure() ) return failure( "unable to predict at node" );
    //debug() << "just after predict: state at z = " << state.z() << endreq
    //        << " state vector: " << state.stateVector() << endreq
    //        << " state cov:" << state.covariance() << endreq;
    //if ( m_verboseLevel )
      //verbose() << "-> filter(...) for node # " << nNodes++ << endreq
      //          << "-- just before calling filter(node): z = " << node.z() << endreq
      //          << "best state = " << node.state().stateVector() << endreq
      //          << "predicted state = " << node.predictedState().stateVector() << endreq
      //          << "filtered state = " << node.filteredState().stateVector() << endreq
      //          << endreq;
    sc = filter( node, state );
    if ( sc.isFailure() ) return failure( "unable to filter node " );
    //if ( m_debugLevel )
    //  debug() << "-- at end of cycle: z = " << node.z()
    //          << "best state = " << node.state().stateVector() << endreq
    //          << "predicted state = " << node.predictedState().stateVector() << endreq
    //          << "filtered state = " << node.filteredState().stateVector() << endreq
    //          << endreq;
  }
  
  computeChiSq( track );

  return sc;
}

//=========================================================================
// Predict the state to this node
//=========================================================================
StatusCode TrackKalmanFilter::predict(FitNode& aNode, State& aState) {

  //if ( m_debugLevel )
  //  debug() << "predict: state at z = " << aState.z() << endreq
  //          << " state vector: " << aState.stateVector() << endreq
  //          << " state cov:" << aState.covariance() << endreq;

  Measurement& thisMeasure = aNode.measurement();
  
  // only use extrapolator in first iteration; else use stored parameters
  HepVector prevStateVec = aState.stateVector();
  HepSymMatrix prevStateCov = aState.covariance();
  double z = thisMeasure.z();
  if ( aNode.transportVector()[0] == 0. ) {
    //debug() << "transportVector empty" << endreq;
  // TODO: how to translate this into the new TEM!!!?
  ////if ( !(aNode.predictedState()) || !m_storeTransport ) { 
    // first iteration only: need to call extrapolator to get 
    // the predicted state at measurement
    //debug() << " PROPAGATE state at z \t" << z << endreq
    //        << "           state vector  \t" << aState.stateVector() << endreq
    //        << "           state cov \t" << aState.covariance() << endreq
    //        << "           to z = " << z << endreq;
    StatusCode sc = m_extrapolator -> propagate( aState, z );
    if ( sc.isFailure() )
      return failure( "unable to predict state at next measurement" );
      //debug() << "... extrapolation done." << endreq
      //        << "-> state at z = " << aState.z() << endreq
      //        << " state vector: " << aState.stateVector() << endreq
      //        << " state cov:" << aState.covariance() << endreq
      //        << " and transport matrix "
      //        << m_extrapolator->transportMatrix() << endreq;
    // store transport matrix F, noise matrix and vector for next iterations
    const HepMatrix& F = m_extrapolator->transportMatrix();
    aNode.setTransportMatrix( F );
    aNode.setTransportVector( aState.stateVector() - F * prevStateVec );
    aNode.setNoiseMatrix( aState.covariance() - prevStateCov.similarity(F) );
  }
  else{ // next iterations
    // update node with information from 1st iteration
    //debug() << "transportVector non-empty, = " << endreq
    //          << aNode.transportVector() << endreq;
    const HepMatrix& F = aNode.transportMatrix();
    HepVector& stateVec = aState.stateVector();
    HepSymMatrix& stateCov = aState.covariance();
    stateVec = F * stateVec + aNode.transportVector();
    stateCov = stateCov.similarity(F) + aNode.noiseMatrix();
    aState.setZ( z );
  }

  // save predicted state
  aNode.setPredictedState(aState);

  //if ( m_debugLevel ) {
  //  debug() << " predicted  state at z \t" << z << endreq
  //          << " predicted  state vector  \t" << aState.stateVector() << endreq
  //          << " predicted  state cov \t" << aState.covariance() << endreq
  //          << " transport matrix \t" << aNode.transportMatrix() << endreq
  //          << " transport vector \t" << aNode.transportVector() << endreq
  //          << " noise matrix \t" << aNode.noiseMatrix() << endreq
  //          << "-- at end of predict(...): z = " << aNode.z() << endreq
  //    //<< "best state = " << aNode.state().stateVector() << endreq
  //          << "predicted state = " << aNode.predictedState().stateVector() << endreq
  //    //<< "filtered state = " << aNode.filteredState().stateVector() << endreq
  //          << endreq;
  //} 

  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::filter(FitNode& node, State& state) 
{
  Measurement& meas = node.measurement();
  // set a temporary "value" for the filtered state
  ////node.setFilteredState( node.predictedState() );
  //State& state      = node.state();
  //State& state      = node.predictedState();
  ////State& state = node.filteredState();

  //if ( m_verboseLevel )
  //  verbose() << "filter(node): predicted  state at z \t" << state.z() << endreq
  //            << " predicted state vector  \t" << state.stateVector() << endreq;

  // keep a copy - needed!
  double stateX = state.x();
  double stateY = state.y();
  //HepSymMatrix& tmpC = state.covariance();

  //debug() << "z-position of State = " << state.z() << endreq;
  //debug() << "z-position of Meas  = " << meas.z() << endreq;
  //debug() << "-> calling filter(state,meas) ..." << endreq;
  StatusCode sc = filter( state, meas );
  if ( sc.isFailure() ) return failure( "unable to filter the node!" );

  //debug() << "z-position of State = " << state.z() << endreq;
  //debug() << "z-position of Meas  = " << meas.z() << endreq;

  double tMeasureError = meas.errMeasure();
  //TODO: find a less ugly way to implement this!
  if ( meas.type() == Measurement::VeloPhi )
    tMeasureError *= sqrt( stateX*stateX + stateY*stateY );
  const HepVector& H = m_projector->projectionMatrix();
  double res = m_projector -> residual();  // projection just made in filter(,)

  // save it for later use
  node.setProjectionMatrix( H );

  double HK = dot ( H, m_K );
  res     *= (1 - HK);
  double errorRes2 = (1 - HK) * ( tMeasureError*tMeasureError );

  node.setResidual( res );
  node.setErrResidual( sqrt(errorRes2) );
  //node.setErrResidual( errorRes );

  // save predicted state
  node.setFilteredState( state );
  node.setState( state );

  //if ( m_debugLevel )
  //  debug() << " bef print" << endreq 
  //          << " filtered residual and error \t" << node.residual() << ", "
  //          <<node.errResidual() << endreq
  //          << " aft print" << endreq;  
 
  return sc;
}

//-----------------------------------------------------------------
// Update the current track 'state' with the TrMeasurement 'measure'.
// (i.e. filter the measurement into the state)
//  the  measurement ch-sq is updated
//-----------------------------------------------------------------
StatusCode TrackKalmanFilter::filter( State& state, Measurement& meas )
{
  //debug() << "In filter(state,meas)" << endreq;
  //debug() << "z-position of State = " << state.z() << endreq;
  //debug() << "z-position of Meas  = " << meas.z() << endreq;
  // check z position
  if ( fabs(meas.z() - state.z()) > 1e-6) {
    warning() << "z-position of State (=" << state.z() 
              << ") and Measurement (= " << meas.z() 
              << ") are not equal." << endreq;
  }

  // get reference to the state vector and cov
  HepVector&    tX = state.stateVector();
  HepSymMatrix& tC = state.covariance();

  //debug() << "filter(state,meas): projecting ..."
  //        << " of type" << meas.type() << endreq;
  // project the state into the measurement 
  StatusCode sc = m_projector -> project( state, meas );
  if ( sc.isFailure() ) 
    return failure( "not able to project a state into a measurement" );

  // calculate predicted residuals
  double errorMeas = meas.errMeasure();
  //TODO: find a less ugly way to implement this!
  if ( meas.type() == Measurement::VeloPhi )
    errorMeas *= sqrt( state.x()*state.x() + state.y()*state.y() );
  const HepVector& H = m_projector->projectionMatrix();
  double res       = m_projector->residual();
  double errorRes  = m_projector->errResidual();
  double errorRes2 = (errorRes*errorRes);
  if ( meas.type() == Measurement::VeloPhi ) {
    errorRes2 = errorMeas*errorMeas + tC.similarity( H );
  }

  // calculate gain matrix K
  m_K = (tC * H) / errorRes2;

  // update state vector 
  tX += ( m_K * res );

  HepMatrix B  = HepDiagMatrix(tC.num_row(), 1) - ( m_K * H.T());
  tC.assign( B * tC * B.T() + ( m_K * errorMeas * errorMeas * m_K.T()));

  //if ( m_debugLevel )
  //  debug() << " measure and error \t" << meas.measure() << ", " 
  //          << errorMeas << endreq
  //          << " residual and error \t" << res << ", " 
  //          << errorRes << endreq
  //          << " projection matrix \t" << H << endreq
  //          << " gain matrix \t" << m_K << endreq
  //          << " filter state vector \t" << tX << endreq
  //          << " filter state covariance \t"<< tC << endreq
  //          << "state.stateVector()" << state.stateVector() << endreq;
  
  return sc;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::smoother( Track& track )
{
  StatusCode sc = StatusCode::SUCCESS;

  // smoother loop - runs in opposite direction to filter
  std::vector<FitNode*>::reverse_iterator riNode = m_nodes.rbegin();
  FitNode* oldNode = *riNode;
  ++riNode;

  while (riNode != m_nodes.rend()) {
    sc = smooth( *(*riNode), *oldNode );
    if ( sc.isFailure() ) return failure( "Unable to smooth node!" );      
    oldNode = *riNode;
    ++riNode;
  }

  computeChiSq(track);

  return sc;
}

//----------------------------------------------------------------
// smooth a node on a track
// Input: node to be smoothed (k) + last node (k+1)
// remember we are now running in the opposite direction to the filter !!
// M. Needham 9/11/99
//----------------------------------------------------------------
StatusCode TrackKalmanFilter::smooth(FitNode& thisNode,
                                     const FitNode& prevNode)
{
  //if ( m_debugLevel )
  //  debug() << "just entered smooth(node0,node1)" << endreq
  //          << "thisNode: z = "<< thisNode.z()
  //          << " predicted state = "
  //          << thisNode.predictedState().stateVector() << endreq
  //          << "prevNode: z = "<< prevNode.z()
  //          << " predicted state = "
  //          << prevNode.predictedState().stateVector() << endreq;

  // preliminaries, first we need to invert the _predicted_ covariance
  // matrix at k+1
  const HepVector&    prevNodeX = prevNode.predictedState().stateVector();
  const HepSymMatrix& prevNodeC = prevNode.predictedState().covariance();

  //debug() << "calling checkInvertMatrix ..." << endreq;  
  // check that the elements are not too large else dsinv will crash
  StatusCode sc = checkInvertMatrix(prevNodeC);
  if ( sc.isFailure() ) return failure("not valid matrix in smoother"); 

  //invert the covariance matrix
  HepSymMatrix invPrevNodeC = prevNodeC;
  sc = invertMatrix(invPrevNodeC);
  if ( sc.isFailure() ) return failure("inverting matrix in smoother");

  //references to _predicted_ state + cov of this node from the first step
  HepVector&    thisNodeX = thisNode.state().stateVector();
  HepSymMatrix& thisNodeC = thisNode.state().covariance();

  //transport
  const HepMatrix& F = prevNode.transportMatrix();

  //calculate gain matrix A
  HepMatrix A = thisNodeC * F.T() * invPrevNodeC;

  // best = smoothed state at prev Node
  const HepVector&    prevNodeSmoothedX = prevNode.state().stateVector();
  const HepSymMatrix& prevNodeSmoothedC = prevNode.state().covariance();

  //smooth state
  thisNodeX += A * (prevNodeSmoothedX - prevNodeX);

  // smooth covariance  matrix
  HepSymMatrix covDiff = prevNodeSmoothedC- prevNodeC;
  HepSymMatrix covUpDate = covDiff.similarity(A);
  thisNodeC += covUpDate;

  // check that the cov matrix is positive
  sc = checkPositiveMatrix(thisNodeC);
  if (sc.isFailure()) return Warning("non-positive Matrix in smoother");

  // update = smooth the residuals
  const HepVector& H = thisNode.projectionMatrix();
  sc = m_projector -> project( thisNode.state(), thisNode.measurement() );
  if ( sc.isFailure() ) Warning( " unable to project the smooth node" );
  double res = m_projector -> residual();
  thisNode.setResidual( res );
  //double errRes = thisNode.errResidual();
  //double errRes2 = errRes*errRes -  covUpDate.similarity(H);
  double errRes2 = thisNode.errResidual2() -  covUpDate.similarity(H);
  if ( errRes2 < 0.)
    return Warning( "Negative residual error in smoother!" );
  thisNode.setErrResidual( sqrt(errRes2) );
  //thisNode.setErrResidual( sqrt(errRes) );

  //if ( m_debugLevel )
  //  debug() << " A matrix \t" << A << endreq
  //          << " smooth state vector \t" << thisNodeX << endreq
  //          << " smooth state covariance \t"<< thisNodeC << endreq
  //          << " smooth residual, error and chi2 \t" << res << ", "
  //          << errRes << ", " << thisNode.chi2() << endreq;      

  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
void TrackKalmanFilter::computeChiSq(Track& track) 
{
  double chi2 = 0.;
  for ( std::vector<FitNode*>::const_iterator it = m_nodes.begin();
       it != m_nodes.end(); ++it )
    chi2 += (*it)->chi2();
  int ndof = -( track.firstState().nParameters() );
  ndof += m_nodes.size();

  double chi2Norma = 0.;
  if (ndof != 0) chi2Norma = chi2 / ( (double) (ndof) );
  track.setChi2PerDoF( chi2Norma );
  track.setNDoF( ndof );
}

//=============================================================================
// 
//=============================================================================
StatusCode TrackKalmanFilter::determineStates( Track& track )
{
  // clean the non-fitted states in the track!
  const std::vector<State*> allstates = track.states();
  for ( std::vector<State*>::const_iterator it = allstates.begin();
        it != allstates.end(); ++it) track.removeFromStates( *it );

  // use copy of z pos vector
  std::vector<double> tZPositions = m_zPositions;
  std::sort( tZPositions.begin(), tZPositions.end() );
  
  if ( m_statesAtMeasZPos ) {
    // just copy the "best state" properties from the vector of nodes
    // into the track's vector of states
    std::vector<FitNode*>::const_iterator it;
    for ( it = m_nodes.begin(); it < m_nodes.end(); ++it ) {
      //State& newState = (*it) -> state();
      //track.addToStates( newState );
      track.addToStates( (*it) -> state() );
    }
  }
  else {
    // determine states
    std::vector<double>::const_iterator zPos;
    for ( zPos = tZPositions.begin(); zPos != tZPositions.end(); ++zPos ) {

      // get predicted state
      State& newState = predictState( *zPos );

      //add state to container
      track.addToStates( newState );
    }
  }

  // extrapolate current state to position closest to the beam line
  if ( m_stateAtBeamLine ) {
    State& closeState = track.closestState(0.0);
    double z = closestToBeamLine( closeState );
    // check if this z position is reasonable and if it is smaller than 
    //  the z position of the state at the first measurement.
    if ( fabs(z) < 200.0  &&  z < closeState.z() ) {      
      State* stateAtFirstMeas = closeState.clone();
      //StatusCode sc = closeState->extrapolate( m_extrapolator, z, m_particleID);
      StatusCode sc = m_extrapolator -> propagate( closeState, z );
      if ( sc.isFailure() ) {
        warning() << "Extrapolation of state to z = " << z << " failed." << endreq;
        // Set the original values back
        closeState.setZ( stateAtFirstMeas->z() );
        HepVector& vec = closeState.stateVector();
        HepSymMatrix& cov = closeState.covariance();
        vec = stateAtFirstMeas -> stateVector();
        cov = stateAtFirstMeas -> covariance();
        delete stateAtFirstMeas;
      }
    }
  }

  if ( m_debugLevel ) {
    debug() << "Determined state for track # " << track.key() << endreq
            << "  # of states after fit:" << track.nStates() << endreq
            << "  States are: ";
    const std::vector<State*>& allstates = track.states();
    for ( unsigned int it2 = 0; it2 < allstates.size(); it2++ ) {
      debug() << "-- at z = " << allstates[it2]->z() << endreq
              << "state vector:" << allstates[it2]->stateVector() << endreq
              << "covariance:" << allstates[it2]->covariance() << endreq;
    }
    debug() << "  # of measurement after fit:" << track.nMeasurements() << endreq;
  }

  return StatusCode::SUCCESS;

}

//=============================================================================
// 
//=============================================================================
State& TrackKalmanFilter::predictState( const double zPos )
{
  // loop to find the closest node
  //TODO: use the "closest method" in the track functor to do this!!!
  std::vector<FitNode*>::iterator iNode;
  double diffZ = 99999.*mm;
  FitNode* closestNode = 0;
  for ( iNode = m_nodes.begin(); iNode != m_nodes.end(); ++iNode){
    if ( fabs(zPos-(*iNode)->z()) < diffZ ) {
      diffZ = fabs(zPos-(*iNode)->z());
      closestNode = *iNode;
    }
  } // loop nodes
  //std::vector<FitNode*>::iterator it =
    //std::max_element( m_nodes.begin(), m_nodes.end(),
                      //TrackFunctor::closestToZ<FitNode>(zPos) );
  //if ( it == m_nodes.end() )
    //throw GaudiException( "No node closest to z","TrackKalmanFilter.cpp",
                          //StatusCode::FAILURE );
  
  State* closeState = closestNode -> state().clone();
  
  StatusCode sc = m_extrapolator -> propagate( *closeState, zPos );
  if ( sc.isFailure() ) {
    warning() << " Unable to extrapolate to z = " << zPos << endreq;
    closeState -> setZ( zPos );
  }

  //if ( m_debugLevel )
  //  debug() << "node.state: z = " << closestNode->state().z() << endreq
  //          << "state vector" << closestNode->state().stateVector() << endreq
  //          << "covariance" << closestNode->state().covariance() << endreq
  //          << "-> track state: z = " << closeState->z() << endreq
  //          << "state vector" << closeState->stateVector() << endreq
  //          << "covariance" << closeState->covariance() << endreq;
  
  return (*closeState);
}

//=========================================================================
//
//=========================================================================
void TrackKalmanFilter::updateCurrentState( State& state,
                                            FitNode& lastNode )
{
  // get reference to the state vector and cov
  HepVector& tX    = state.stateVector();
  HepSymMatrix& tC = state.covariance();

  // update
  tX = lastNode.state().stateVector();
  tC = lastNode.state().covariance();
  state.setZ( lastNode.z() );
}

//=========================================================================
//
//=========================================================================
bool TrackKalmanFilter::outlierRemoved( Track& track )
{
  // some debugging
  //std::vector<Measurement*>& measures = track.measurements();
  //for ( std::vector<Measurement*>::iterator it = measures.begin(); 
  //      it != measures.end(); ++it ) {
  //  Measurement& meas = *(*it);
  //  debug() <<"in OUTLIERS...: measurement of type " << meas.type()
  //          << " at z = " << meas.z() << endreq;
  //}

  // return true if outlier chi2 cut < 0
  if ( m_chi2Outliers < 0.0 ) return true;

  // flag (true if outliers are removed)
  bool outlierWasRemoved = false;

  // loop over the nodes and find the one
  // with the highest chi2 > m_chi2Outliers
  std::vector<FitNode*>::iterator iNode;
  std::vector<FitNode*>::iterator iWorstNode = m_nodes.end();
  double worstChi2 = m_chi2Outliers;
  for ( iNode = m_nodes.begin(); iNode != m_nodes.end(); ++iNode ) {
    //Measurement* meas = (*iNode) -> measurement();
    //double chi2 = meas->lastChiSqContr();
    double chi2 = (*iNode) -> chi2();
    if ( chi2 > worstChi2 ) {
      worstChi2 = chi2;
      iWorstNode = iNode;
    }
  }

  // if a node is found: remove it and it measurement from the track
  if ( iWorstNode != m_nodes.end() ) {
    FitNode* aNode = *(iWorstNode);
    // copy worst node's transport transformation to next node (if exists)
    std::vector<FitNode*>::iterator iNextNode = iWorstNode;
    ++iNextNode;
    // if it is not the last node, copy transport information to next node
    if ( iNextNode != m_nodes.end() ) {
      (*iNextNode) -> updateTransport( *aNode );
    }
    // delete the node and measurement from the track
    //TrTrack::measure_iterator iBadMeas = std::find(track->beginMeas(),
    //                                               track->endMeas(),
    //                                               aNode->measurementOnTrack());
    //track->deleteMeasurement( iBadMeas );
    //debug() << " -> deleted from track measurement at z = "
    //        << aNode->measurement().z() << endreq;
    track.removeFromMeasurements( &(aNode->measurement()) );
    // some debugging
    //std::vector<Measurement*>& measures2 = track.measurements();
    //for ( std::vector<Measurement*>::iterator it = measures2.begin(); 
    //      it != measures2.end(); ++it ) {
    //  Measurement& meas = *(*it);
    //  debug() <<"in OUTLIERS...2: measurement of type " << meas.type()
    //          << " at z = " << meas.z() << endreq;
    //}
    delete aNode;
    m_nodes.erase( iWorstNode );
    outlierWasRemoved = true;

    // some debugging
    //std::vector<Measurement*>& measures3 = track.measurements();
    //for ( std::vector<Measurement*>::iterator it = measures3.begin(); 
    //      it != measures3.end(); ++it ) {
    //  Measurement& meas = *(*it);
    //  debug() <<"in OUTLIERS...3: measurement of type " << meas.type()
    //          << " at z = " << meas.z() << endreq;
    //}

  }

  return outlierWasRemoved;
}

//=========================================================================
//
//=========================================================================
StatusCode TrackKalmanFilter::reSeed( State& seedState, State& state )
{
  // First extrapolate the previous seedState to the z position of 
  // the current state
  //StatusCode sc = seedState->extrapolate( m_extrapolator, state->z(), m_particleID);
  StatusCode sc = m_extrapolator->propagate( seedState, state.z() );
  if ( sc.isFailure() ) { 
    warning() << "extrapolation of state to z=" << state.z()
              << " failed" << endreq;
    return sc;
  }

  // use the large covariance matrix of the previous seedState for new seedState
  HepSymMatrix& tC = state.covariance();
  tC = seedState.covariance();
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
void TrackKalmanFilter::updateMeasurements() 
{
  //debug() << "just entered updateMeasurements()" << endreq;
  std::vector<FitNode*>::iterator iNode;
  for ( iNode = m_nodes.begin(); iNode != m_nodes.end(); ++iNode ) {
    Measurement& meas = (*iNode) -> measurement();
    //debug() << "updating meas of type " << meas.type()
    //        << " at z = " << meas.z() << endreq;
    meas.update( (*iNode)->state().stateVector() );
    //debug() << "done." << endreq;
  }
  //debug() << "exiting updateMeasurements() ..." << endreq;
}

//=========================================================================
// 
//=========================================================================
void TrackKalmanFilter::clearNodes() 
{
  if ( !m_nodes.empty() ) {
    std::vector<FitNode*>::iterator iNode = m_nodes.begin();
    while ( iNode != m_nodes.end() ) {
      FitNode* aNode = *iNode;
      m_nodes.erase( iNode );
      delete aNode;
    }
  }
}

//=========================================================================
// Determine the z-position of the closest approach to the beam line
//  by linear extrapolation.
//=========================================================================
double TrackKalmanFilter::closestToBeamLine( State& state )
{
  HepVector& vec = state.stateVector();
  double z = state.z();
  // check on division by zero (track parallel to beam line!)
  if ( vec[2] != 0 || vec[3] != 0 ) {
    z -= ( vec[0]*vec[2] + vec[1]*vec[3] ) / ( vec[2]*vec[2] + vec[3]*vec[3] );
  }
  return z;
}


//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::checkInvertMatrix( const HepSymMatrix& mat ) 
{
  unsigned int nParam = (unsigned int) mat.num_row();
  for ( unsigned int i = 0; i < nParam; ++i ) {
    for ( unsigned int j = 0; j < nParam; ++j ) {
      if ( mat[i][j] > 1e20 )
        return Warning( "Covariance errors too big to invert!" );
    }
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::checkPositiveMatrix( HepSymMatrix& mat ) 
{
  unsigned int nParam = (unsigned int) mat.num_row();
  for ( unsigned int i=0; i < nParam; ++i ) {
    if ( mat[i][i] <= 0. )
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
StatusCode TrackKalmanFilter::invertMatrix( HepSymMatrix& invPrevNodeC )
{
  // Geant3 units
  cToG3(invPrevNodeC);

  HepSymMatrix tempMatrix = invPrevNodeC;

  int ifail;
  invPrevNodeC.invert(ifail);

  // Geant4 units
  cToG4(invPrevNodeC);

  if ( ifail != 0 ) {
    warning() << "Failed to invert covariance matrix, failure code "
              << ifail << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Change some units for better matrix invertability
//=========================================================================
void TrackKalmanFilter::cToG3( HepSymMatrix& C )
{
  // cov matrix
  C[0][0] /= cm2;
  C[0][1] /= cm2;
  C[0][2] /= cm;
  C[0][3] /= cm;
  C[0][4]  = C[0][4]*GeV/cm;

  C[1][1] /= cm2;
  C[1][2] /= cm;
  C[1][3] /= cm;
  C[1][4]  = C[1][4]*GeV/cm;

  C[2][4]  = C[2][4]*GeV;

  C[3][4]  = C[3][4]*GeV;

  C[4][4]  = C[4][4]*GeV*GeV;
}

//=========================================================================
//  Restore units after inversion. same as cToG3 !
//=========================================================================
void TrackKalmanFilter::cToG4( HepSymMatrix& invC )
{
  // cov matrix
  invC[0][0] /= cm2;
  invC[0][1] /= cm2;
  invC[0][2] /= cm;
  invC[0][3] /= cm;
  invC[0][4]  = invC[0][4]*GeV/cm;

  invC[1][1] /= cm2;
  invC[1][2] /= cm;
  invC[1][3] /= cm;
  invC[1][4]  = invC[1][4]*GeV/cm;

  invC[2][4]  = invC[2][4]*GeV;

  invC[3][4]  = invC[3][4]*GeV;

  invC[4][4]  = invC[4][4]*GeV*GeV;
}

//=========================================================================
