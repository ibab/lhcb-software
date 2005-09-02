// $Id: TrackKalmanFilter.cpp,v 1.3 2005-09-02 17:05:03 erodrigu Exp $
//
//  Implementation of the TrackKalmanFilter tool
//
//  Original Author: Rutger van der Eijk
//  Created: 07-04-1999
//  Adapted: 21-03-2002  Olivier Callot
//  Adapted: 15-04-2005  Jose A. Hernando

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from CLHEP
#include "CLHEP/Matrix/DiagMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Units/PhysicalConstants.h"

// from TrackEvent
#include "Event/TrackKeys.h"
#include "Event/TrackFunctor.h"

// local
#include "TrackKalmanFilter.h"

static const ToolFactory<TrackKalmanFilter>  s_factory;
const IToolFactory& TrackKalmanFilterFactory = s_factory;

//=========================================================================
// 
//=========================================================================
TrackKalmanFilter::TrackKalmanFilter( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent) :
  GaudiTool( type, name, parent)
{
  declareInterface<ITrackFitter>( this );

  // Define the fixed z-positions
  m_zPositions.push_back(   990.0*mm );
  m_zPositions.push_back(  2165.0*mm );
  m_zPositions.push_back(  9450.0*mm );
  m_zPositions.push_back( 11900.0*mm );

  declareProperty( "Extrapolator"     , m_extrapolatorName =
                                        "TrackMasterExtrapolator" );
  declareProperty( "Projector"        , m_projectorName =
                                        "TrackMasterProjector" );
  declareProperty( "ZPositions"       , m_zPositions               );
  declareProperty( "StatesAtMeasZPos" , m_statesAtMeasZPos = false );
  declareProperty( "StateAtBeamLine"  , m_stateAtBeamLine  = true  );
  declareProperty( "NumberFitIter"    , m_numFitIter       = 5     );
  declareProperty( "Chi2Outliers"     , m_chi2Outliers     = 9.0   );
  declareProperty( "StoreTransport"   , m_storeTransport   = true  );

  m_state = NULL;
  m_nodes.clear();
  m_K = HepVector(5,0);
}

//=========================================================================
// 
//=========================================================================
TrackKalmanFilter::~TrackKalmanFilter() {
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::initialize() 
{
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName );

  m_projector    = tool<ITrackProjector>( m_projectorName );

  m_debugLevel   = msgLevel( MSG::DEBUG );
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::failure(const std::string& comment) {
  info() << "TrackKalmanFilter failure: " << comment << endreq;
  return StatusCode::FAILURE;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::iniKalman(Track & track) 
{
  // destroy the previous state if not null
  if ( m_state != NULL ) { delete m_state; m_state = 0; }

  // the Kalman filter has a private copy of the nodes to accelerate
  // the computations. The track owns the Nodes, so it is responsability
  // of the Track destructor to delete them!
  m_nodes.clear();
  debug() << "- iniKalman: getting the "
          << track.nMeasurements() << "track measurements ..." << endreq;
  std::vector<Measurement*>& measures = track.measurements();
  //debug() << "- iniKalman: getting track nodes ..." << endreq;
  std::vector<Node*>& nodes = track.nodes();
  if (nodes.size() > 0) 
    for (std::vector<Node*>::iterator it = nodes.begin(); it != nodes.end();
         it++) delete *it;  
  nodes.clear();
  //debug() << "- iniKalman: private nodes vector cleared ..." << endreq;
  // reserve some space in node vector
  m_nodes.reserve(measures.size());
  for (std::vector<Measurement*>::iterator it = measures.begin(); 
       it != measures.end(); it++) {
    Measurement& meas = *(*it);
    FitNode* node = new FitNode(meas);
    m_nodes.push_back(node);
    track.addToNodes(node);
  }
  //debug() << "- iniKalman: private nodes vector rebuilt ..." << endreq;

  if ( m_debugLevel )
    debug() << " track nodes size \t" << m_nodes.size() << endreq;

  return StatusCode::SUCCESS;
}

//=========================================================================
// Fit the track (filter and smoother)
//=========================================================================
StatusCode TrackKalmanFilter::fit(Track& track, const State& state)
{
  debug() << "-> calling filter(track,state) ..." << endreq;
  StatusCode sc = filter(track,state);
  if (sc.isFailure()) return failure("unable to filter the track");

  debug() << "### Nodes after filtering:" << endreq;
  std::vector<FitNode*>::iterator iNode;
  for (iNode = m_nodes.begin(); iNode != m_nodes.end(); ++iNode) {
    FitNode& node = *(*iNode);
    debug() << "-- at z = " << node.z()
            << "best state = " << node.state().stateVector() << endreq
            << "predicted state = " << node.predictedState().stateVector() << endreq
            << "filtered state = " << node.filteredState().stateVector() << endreq
            << endreq;
  }
  
  debug() << "-> calling smoother(track) ..." << endreq;
  sc = smoother(track);
  if (sc.isFailure()) return failure("unable to smooth the track");

  debug() << "### Nodes after smoothing:" << endreq;
  //std::vector<FitNode*>::iterator iNode;
  for (iNode = m_nodes.begin(); iNode != m_nodes.end(); ++iNode) {
    FitNode& node = *(*iNode);
    debug() << "-- at z = " << node.z()
            << "best state = " << node.state().stateVector() << endreq
            << "predicted state = " << node.predictedState().stateVector() << endreq
            << "filtered state = " << node.filteredState().stateVector() << endreq
            << endreq;
  }

  //update the best state
  //std::vector<FitNode*>::iterator iNode = m_nodes.begin();
  //updateCurrentState( state, *(*iNode) );

  debug() << "-> calling determineStates(track) ..." << endreq;
  //determine the track states at user defined z positions
  sc = determineStates( track );
  if ( sc.isFailure() ) {
    warning()
        << "Failed determining states at z positions" << endmsg;
    //clear the node Vector
    //cleanUp();
    return sc;
  }

  return sc;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::filter(Track& track, const State& state)
{
  StatusCode sc = StatusCode::SUCCESS;
  sc = iniKalman(track);
  if (sc.isFailure()) return failure("not able to create Nodes");

  if ( m_debugLevel ) {
    debug() << " seed state at z \t " << state.z() << endreq
            << " seed state vector \t" << state.stateVector() << endreq
            << " seed state cov \t" << state.covariance() << endreq
            << " track nodes size \t" << m_nodes.size() << endreq;
  }

  std::vector<FitNode*>::iterator iNode = m_nodes.begin();
  double z = (*iNode)->measurement().z();

  debug() << "Z-positions of seed state/meas.begin = "
          << state.z() << " / " << z << endreq;

  // keep seed state so we can use it in another iteration
  m_state = state.clone();

  debug() << "- propagating the state ..." << endreq;
  // sc = m_extrapolator->propagate(*m_state,z,m_particleID);
  sc = m_extrapolator->propagate(*m_state,z);
  if (sc.isFailure()) 
    return failure("unable to extrapolate to measurement");

  debug() << " -after extrapolation, seed state at z \t " << m_state->z() << endreq
          << " seed state vector \t" << m_state->stateVector() << endreq ;

  // TODO: handle different iterations
  unsigned int nNodes = 0;
  debug() << "- looping over nodes ..." << endreq;
  for (iNode = m_nodes.begin(); iNode != m_nodes.end(); ++iNode) {
    FitNode& node = *(*iNode);
    debug() << "-> predict(...) for node # " << nNodes
            << " at z = " << node.measurement().z() << endreq;
    sc = predict(node,*m_state);
    if (sc.isFailure()) return failure("unable to predict at node");
    debug() << "-> filter(...) for node # " << nNodes++ << endreq;
    debug() << "-- just before calling filter(node): z = " << node.z() << endreq
            //<< "best state = " << node.state().stateVector() << endreq
            << "predicted state = " << node.predictedState().stateVector() << endreq
            //<< "filtered state = " << node.filteredState().stateVector() << endreq
            << endreq;
    sc = filter(node);
    if (sc.isFailure()) return failure("unable to filter node ");
    debug() << "-- at end of cycle: z = " << node.z()
            << "best state = " << node.state().stateVector() << endreq
            << "predicted state = " << node.predictedState().stateVector() << endreq
            << "filtered state = " << node.filteredState().stateVector() << endreq
            << endreq;
  }
  
  computeChiSq(track);
  
  return sc;
}

//=========================================================================
// Predict the state to this node
//=========================================================================
StatusCode TrackKalmanFilter::predict(FitNode& aNode, State& aState) {

  debug() << "predict: state at z = " << aState.z() << endreq
          << " state vector: " << aState.stateVector() << endreq
          << " state cov:" << aState.covariance() << endreq;

  Measurement& thisMeasure = aNode.measurement();
  
  // only use extrapolator in first iteration; else use stored parameters
  HepVector prevStateVec = aState.stateVector();
  HepSymMatrix prevStateCov = aState.covariance();
  double z = thisMeasure.z();
  if ( aNode.transportVector()[0] == 0. ) {
    debug() << "transportVector empty" << endreq;
  // TODO: how to translate this into the new TEM!!!?
  ////if ( !(aNode.predictedState()) || !m_storeTransport ) { 
    // first iteration only: need to call extrapolator to get 
    // the predicted state at measurement
    StatusCode sc = m_extrapolator->propagate(aState,z);
    if (sc.isFailure()) 
      return failure("unable to predict state at next measurement");
    debug() << "... extrapolation done." << endreq
            << "-> state at z = " << aState.z() << endreq
            << " state vector: " << aState.stateVector() << endreq
            << " state cov:" << aState.covariance() << endreq;
    // store transport matrix F, noise matrix and vector for next iterations
    const HepMatrix& F = m_extrapolator->transportMatrix();
    aNode.setTransportMatrix( F );
    aNode.setTransportVector( aState.stateVector() - F * prevStateVec );
    aNode.setNoiseMatrix( aState.covariance() - prevStateCov.similarity(F) );
  }
  else{ // next iterations
    // update node with information from 1st iteration
    debug() << "transportVector non-empty, = " << endreq
            << aNode.transportVector() << endreq;
    const HepMatrix& F = aNode.transportMatrix();
    HepVector& stateVec = aState.stateVector();
    HepSymMatrix& stateCov = aState.covariance();
    stateVec = F * stateVec + aNode.transportVector();
    stateCov = stateCov.similarity(F) + aNode.noiseMatrix();
    aState.setZ( z );
  }

  // save predicted state
  aNode.setPredictedState(aState);

  if ( m_debugLevel ) {
    debug() << " predicted  state at z \t" << z << endreq
            << " predicted  state vector  \t" << aState.stateVector() << endreq
            << " predicted  state cov \t" << aState.covariance() << endreq
            << " transport matrix \t" << aNode.transportMatrix() << endreq
            << " transport vector \t" << aNode.transportVector() << endreq
            << " noise matrix \t" << aNode.noiseMatrix() << endreq;
    debug() << "-- at end of predict(...): z = " << aNode.z() << endreq
            //<< "best state = " << aNode.state().stateVector() << endreq
            << "predicted state = " << aNode.predictedState().stateVector() << endreq
            //<< "filtered state = " << aNode.filteredState().stateVector() << endreq
            << endreq;
  } 

  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::filter(FitNode& node) 
{
  
  Measurement& meas = node.measurement();
  // set a temporary "value" for the filtered state
  node.setFilteredState( node.predictedState() );
  //State& state      = node.state();
  //State& state      = node.predictedState();
  State& state = node.filteredState();

  debug() << "filter(node): predicted  state at z \t" << state.z() << endreq
          << " predicted state vector  \t" << state.stateVector() << endreq;

  // keep a copy - needed!
  double stateX = state.x();
  double stateY = state.y();
  //HepSymMatrix& tmpC = state.covariance();

  //debug() << "z-position of State = " << state.z() << endreq;
  //debug() << "z-position of Meas  = " << meas.z() << endreq;
  //debug() << "-> calling filter(state,meas) ..." << endreq;
  StatusCode sc = filter(state, meas);
  if (sc.isFailure()) return failure("unable to filter the node!");

  //debug() << "z-position of State = " << state.z() << endreq;
  //debug() << "z-position of Meas  = " << meas.z() << endreq;

  double tMeasureError = meas.errMeasure();
  //TODO: find a less ugly way to implement this!
  if ( meas.type() == Measurement::VeloPhi )
    tMeasureError *= sqrt( stateX*stateX + stateY*stateY );
  const HepVector& H = m_projector->projectionMatrix();
  debug() << "getting info from projection ..." << endreq;
  double res = m_projector->residual();  // projection just made in filter(,)

  // save it for later use
  node.setProjectionMatrix( H );

  double HK = dot ( H, m_K);
  res     *= (1 - HK);
  double errorRes = (1 - HK) * (tMeasureError*tMeasureError);

  debug() << "res/errRes = " << res << " / " << errorRes << endreq;

  debug() << "setting info in node ..." << endreq;
  node.setResidual(res);
  //node.setErrResidual(sqrt(errorRes));
  node.setErrResidual(errorRes);

  // save predicted state
  debug() << "setting filteredState" << endreq;
  // not needed as I had retrieved a reference to it!
  //node.setFilteredState(state);
  debug() << "just set filteredState" << endreq;
  node.setState(state);

  if ( m_debugLevel ) {
    debug() << " bef print" << endreq;  
    debug() << " filtered residual and error \t" << node.residual() << ", "
            <<node.errResidual() << endreq;;
    debug() << " aft print" << endreq;  
  }
 
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------
// Update the current track 'state' with the TrMeasurement 'measure'.
// (i.e. filter the measurement into the state)
//  the  measurement ch-sq is updated
//-----------------------------------------------------------------
StatusCode TrackKalmanFilter::filter(State& state, Measurement& meas)  {
  
  debug() << "In filter(state,meas)" << endreq;
  debug() << "z-position of State = " << state.z() << endreq;
  debug() << "z-position of Meas  = " << meas.z() << endreq;
  // check z position
  if ( fabs(meas.z() - state.z()) > 1e-6) {
    warning() << "z-position of State (=" << state.z() 
              << ") and Measurement (= " << meas.z() 
              << ") are not equal." << endreq;
  }

  // get reference to the state vector and cov
  HepVector&    tX = state.stateVector();
  HepSymMatrix& tC = state.covariance();

  debug() << "filter(state,meas): projecting ..."
          << " of type" << meas.type() << endreq;
  // project the state into the measurement 
  StatusCode sc = m_projector->project(state,meas);
  if (sc.isFailure()) 
    return failure("not able to project a state into a measurement");
  debug() << "filter(state,meas): projection done" << endreq;

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
  
  debug() << "m_errorRes2 = " << errorRes2 << endreq
          << "and cov" << tC << endreq;

  // calculate gain matrix K
  m_K = (tC * H)/errorRes2;

  // update state vector 
  tX += m_K * res;

  HepMatrix B  = HepDiagMatrix(tC.num_row(), 1) - ( m_K * H.T());
  tC.assign( B * tC * B.T() + ( m_K * pow(errorMeas, 2.0) * m_K.T()));

  m_state -> setState( state.stateVector() );
  m_state -> setCovariance( state.covariance() );

 if ( m_debugLevel ) {
    debug() << " measure and error \t" << meas.measure() << ", " 
            << errorMeas << endreq
            << " residual and error \t" << res << ", " 
            << errorRes << endreq
            << " projection matrix \t" << H << endreq
            << " gain matrix \t" << m_K << endreq
            << " filter state vector \t" << tX << endreq
            << " filter state covariance \t"<< tC << endreq
            << "state.stateVector()" << state.stateVector() << endreq;
  }
  
  return sc;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::smoother(Track& track) 
{
  StatusCode sc = StatusCode::SUCCESS;
  // smoother loop - runs in opposite direction to filter
  std::vector<FitNode*>::reverse_iterator riNode = m_nodes.rbegin();
  FitNode* oldNode = *riNode;
  ++riNode;

  debug() << "starting loop over nodes..." << endreq;  
  while (riNode != m_nodes.rend()) {
    debug() << " call to smooth(node0,node1)..." << endreq;  
    sc = smooth( *(*riNode), *oldNode );
    if ( sc.isFailure() ) return failure("Unable to smooth node!");      
    oldNode = *riNode;
    ++riNode;
  }

  computeChiSq(track);

  return StatusCode::SUCCESS;
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
  debug() << "just entered smooth(node0,node1)" << endreq;

  debug() << "thisNode: z = "<< thisNode.z()
          << " predicted state = "
          << thisNode.predictedState().stateVector() << endreq;

  debug() << "prevNode: z = "<< prevNode.z()
          << " predicted state = "
          << prevNode.predictedState().stateVector() << endreq;

  // preliminaries, first we need to invert the _predicted_ covariance
  // matrix at k+1
  const HepVector&    prevNodeX = prevNode.predictedState().stateVector();
  const HepSymMatrix& prevNodeC = prevNode.predictedState().covariance();

  debug() << "calling checkInvertMatrix ..." << endreq;  
  // check that the elements are not too large else dsinv will crash
  StatusCode sc = checkInvertMatrix(prevNodeC);
  if ( sc.isFailure() ) return failure("not valid matrix in smoother"); 

  debug() << "inverting matrix ..." << endreq;
  //invert the covariance matrix
  HepSymMatrix invPrevNodeC = prevNodeC;
  sc = invertMatrix(invPrevNodeC);
  if ( sc.isFailure() ) return failure("inverting matrix in smoother");
  debug() << "matrix inverted ..." << endreq;  

  //references to _predicted_ state + cov of this node from the first step
  HepVector&    thisNodeX = thisNode.state().stateVector();
  HepSymMatrix& thisNodeC = thisNode.state().covariance();

  //transport
  const HepMatrix& F = prevNode.transportMatrix();

  debug() << "calculation of gain matrix A ..." << endreq;  
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

  debug() << "calling checkPositiveMatrix ..." << endreq;  
  // check that the cov matrix is positive
  sc = checkPositiveMatrix(thisNodeC);
  if (sc.isFailure()) return Warning("non-positive Matrix in smoother");
  debug() << "checkPositiveMatrix done" << endreq;  

  // update = smooth the residuals
  debug() << "dims of H is" 
          << thisNode.projectionMatrix().num_row() << "x"
          << thisNode.projectionMatrix().num_col() << endreq;
  const HepVector& H = thisNode.projectionMatrix();
  debug() << "projecting ..." << endreq;
  sc = m_projector->project(thisNode.state(),thisNode.measurement());
  if (sc.isFailure()) Warning(" unable to project the smooth node");
  double res = m_projector->residual();
  thisNode.setResidual(res);
  double errRes = thisNode.errResidual();
  double errRes2 = errRes*errRes -  covUpDate.similarity(H);
  debug() << "errRes2 = " << errRes2 << endreq;
  if (errRes2 >0.) errRes = sqrt(errRes2);
  else return Warning( "Negative residual error in smoother!" );
  thisNode.setErrResidual(sqrt(errRes));

  if ( m_debugLevel ) {
    debug() << " A matrix \t" << A << endreq
            << " smooth state vector \t" << thisNodeX << endreq
            << " smooth state covariance \t"<< thisNodeC << endreq
            << " smooth residual, error and chi2 \t" << res << ", "
            << errRes << ", " << thisNode.chi2() << endreq;      
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
void TrackKalmanFilter::computeChiSq(Track& track) 
{
  double chi2 = 0.;
  for (std::vector<FitNode*>::iterator it = m_nodes.begin();
       it != m_nodes.end(); it++) 
    chi2 += (*it)->chi2();
  int ndof = -(m_state->nParameters());
  ndof += m_nodes.size();

  debug() << "NDoF = " << m_nodes.size() << "-" << m_state->nParameters()
          << " = " << ndof << endreq;

  double chi2Norma = 0.;
  if (ndof != 0) chi2Norma = chi2 /((double) (ndof));
  track.setChi2PerDoF(chi2Norma);
  track.setNDoF(ndof);

  if ( m_debugLevel ) {
    debug() << " track ndof \t" << track.nDoF() << endreq
            << " track chi2, chi2/ndof \t " << track.chi2()  << ", "
            << track.chi2PerDoF() << endreq;
  }
  
}

//=============================================================================
// 
//=============================================================================
StatusCode TrackKalmanFilter::determineStates( Track& track )
{
  debug() << "In determineStates" << endreq;

  // clean the non-fitted states in the track!
  debug() << "beg: # states = " << track.nStates() << endreq;
  const std::vector<State*> allstates = track.states();
  for ( std::vector<State*>::const_iterator it = allstates.begin();
        it != allstates.end(); it++) track.removeFromStates( *it );
  debug() << "aft: # states = " << track.nStates() << endreq;

  // use copy of z pos vector
  std::vector<double> tZPositions = m_zPositions;
  std::sort( tZPositions.begin(), tZPositions.end() );
  
  if ( m_statesAtMeasZPos ) {
    // just copy the "best state" properties from the vector of nodes
    // into the track's vector of states
    std::vector<FitNode*>::const_iterator it;
    for ( it = m_nodes.begin(); it < m_nodes.end(); it++ ) {
      State& newState = (*it) -> state();
      track.addToStates( newState );
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
    //TODO: implement this part ...
  }

  return StatusCode::SUCCESS;

}

//=============================================================================
// 
//=============================================================================
State& TrackKalmanFilter::predictState( const double zPos )
{
  debug() << " - predict state at z = " << zPos << endreq;
  // loop to find the closest node
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

  debug() << "node.state: z = " << closestNode->state().z() << endreq
          << "state vector" << closestNode->state().stateVector() << endreq
          << "covariance" << closestNode->state().covariance() << endreq
          << "-> track state: z = " << closeState->z() << endreq
          << "state vector" << closeState->stateVector() << endreq
          << "covariance" << closeState->covariance() << endreq;
  
  return (*closeState);
}


//=========================================================================
//
//=========================================================================
StatusCode TrackKalmanFilter::updateCurrentState( State& state,
                                                  FitNode& lastNode )
{
  // get reference to the state vector and cov
  HepVector& tX = state.stateVector();
  HepSymMatrix& tC = state.covariance();

  //update
  tX = lastNode.state().stateVector();
  tC = lastNode.state().covariance();
  state.setZ( lastNode.z() );

  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::checkInvertMatrix(const HepSymMatrix& mat) 
{
  int nParam = (int) mat.num_row();
  for (int i=0; i<nParam; ++i){
    for (int j=0; j<nParam; ++j){
      if (mat[i][j] > 1e20)
        return Warning( "Covariance errors too big to invert!");
    } // j
  }   // i
  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::checkPositiveMatrix(HepSymMatrix& mat) 
{
  int nParam = (int) mat.num_row();
  for (int i=0; i<nParam; ++i){
    if (mat[i][i] <= 0.)
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
StatusCode TrackKalmanFilter::invertMatrix(HepSymMatrix& invPrevNodeC){

  StatusCode sc = StatusCode::SUCCESS; 

  // G3 units
  cToG3(invPrevNodeC);

  HepSymMatrix tempMatrix = invPrevNodeC;

  int ifail;
  invPrevNodeC.invert(ifail);

  //G4 units
  cToG4(invPrevNodeC);

  if (ifail !=0){
    warning()
       <<"failed to invert covariance matrix, failure code " <<ifail<<endreq;
    sc = StatusCode::FAILURE;
  }
  return sc;
}

//=========================================================================
//  Change some units for better matrix invertability
//=========================================================================
StatusCode TrackKalmanFilter::cToG3(HepSymMatrix& C){

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

  return StatusCode::SUCCESS;

}

//=========================================================================
//  Restore units after inversion. same as cToG3 !
//=========================================================================
StatusCode TrackKalmanFilter::cToG4(HepSymMatrix& invC){

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

  return StatusCode::SUCCESS;
}

//=========================================================================
