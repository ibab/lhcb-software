// $Id: TrackKalmanFilter.cpp,v 1.1 2005-06-29 15:35:02 erodrigu Exp $
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

  declareProperty( "Extrapolator"     , m_extrapolatorName = "TrackFirstCleverExtrapolator" );
  declareProperty( "Projector"        , m_projectorName = "TrackMasterProjector" );
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
  m_extrapolator = tool<ITrackExtrapolator>(m_extrapolatorName);

  m_projector    = tool<ITrackProjector>(m_projectorName);

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
  if (m_state != NULL) { delete m_state; m_state = 0; }

  // Kalman has private copy of the nodes to acelerate the computations
  // remember that the track owns the Nodes, so it is responsability of the
  // Track destructor to delete them!
  m_nodes.clear();
  debug() << "- iniKalman: getting track measurements ..." << endreq;
  std::vector<Measurement*>& measures = track.measurements();
  debug() << "- iniKalman: getting track nodes ..." << endreq;
  std::vector<Node*>& nodes = track.nodes();
  if (nodes.size() > 0) 
    for (std::vector<Node*>::iterator it = nodes.begin(); it != nodes.end();
         it++) delete *it;  
  nodes.clear();
  debug() << "- iniKalman: private nodes vector cleared ..." << endreq;
  // reserve some space in node vector
  m_nodes.reserve(measures.size());
  for (std::vector<Measurement*>::iterator it = measures.begin(); 
       it != measures.end(); it++) {
    Measurement& meas = *(*it);
    FitNode* node = new FitNode(meas);
    m_nodes.push_back(node);
    track.addToNodes(node);
  }
  debug() << "- iniKalman: private nodes vector rebuilt ..." << endreq;

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
  if (sc.isFailure()) return failure(" unable to filter the track");

  debug() << "-> calling smoother(track) ..." << endreq;
  sc = smoother(track);
  if (sc.isFailure()) return failure(" unable to smooth the track");
  
  return sc;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::filter(Track& track, const State& state)
{
  StatusCode sc = StatusCode::SUCCESS;
  sc = iniKalman(track);
  if (sc.isFailure()) return failure(" not able to create Nodes");

  m_state = state.clone();

  if ( m_debugLevel ) {
    debug() << " seed state at z \t " << m_state->z() << endreq
            << " seed state vector \t" << m_state->stateVector() << endreq    
            << " seed state cov \t" << m_state->covariance() << endreq
            << " track nodes size \t" << m_nodes.size() << endreq;
  }

  std::vector<FitNode*>::iterator iNode = m_nodes.begin();
  double z = (*iNode)->measurement().z();

  debug() << "Z-positions of seed state/meas.begin = "
          << state.z() << " / " << z << endreq;

  debug() << "- propagating the state ..." << endreq;
  // sc = m_extrapolator->propagate(*m_state,z,m_particleID);
  sc = m_extrapolator->propagate(*m_state,z);
  if (sc.isFailure()) 
    return failure(" unable to extrapolate to 1st measurement");

  // TODO: handle different iterations
  unsigned int nNodes = 0;
  debug() << "- looping over nodes ..." << endreq;
  for (iNode = m_nodes.begin(); iNode != m_nodes.end(); ++iNode) {
    FitNode& node = *(*iNode);
    debug() << "-> predict(...) for node # " << nNodes << endreq;
    sc = predict(node,*m_state);
    if (sc.isFailure()) return failure(" unable to predict at node");
    debug() << "-> filter(...) for node # " << nNodes++ << endreq;
    sc = filter(node);
    if (sc.isFailure()) return failure(" unable to filter node ");
  }  

  computeChiSq(track);

  return sc;
}

//=========================================================================
// Predict the state to this node
//=========================================================================
StatusCode TrackKalmanFilter::predict(FitNode& aNode, State& aState) {

  Measurement& thisMeasure = aNode.measurement();

  // only use extrapolator in first iteration; else use stored parameters
  HepVector prevStateVec = aState.stateVector();
  HepSymMatrix prevStateCov = aState.covariance();
  double z = thisMeasure.z();
  // StatusCode sc = m_extrapolator->propagate(state,z,m_particleID);
  StatusCode sc = m_extrapolator->propagate(aState,z);
  if (sc.isFailure()) 
    return failure(" Unable to predit state at next measurement");

  const HepMatrix& F = m_extrapolator->transportMatrix();
  aNode.setTransportMatrix( F );
  aNode.setTransportVector( aState.stateVector() - F * prevStateVec );
  aNode.setNoiseMatrix( aState.covariance() - prevStateCov.similarity(F) );

  // save predicted state
  aNode.setPredictedState(aState);

  if ( m_debugLevel ) {
    debug() << " predicted  state at z \t" << z << endreq;
    debug() << " predicted  state vector  \t" << aState.stateVector() << endreq;
    debug() << " predicted  state cov \t" << aState.covariance() << endreq;
    debug() << " transport matrix \t" << aNode.transportMatrix() << endreq;
    debug() << " transport vector \t" << aNode.transportVector() << endreq;
    debug() << " noise matrix \t" << aNode.noiseMatrix() << endreq;
  } 

  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::filter(FitNode& node) 
{
  
  Measurement& meas = node.measurement();
  State& state      = node.state();

  debug() << "z-position of State = " << state.z() << endreq;
  debug() << "z-position of Meas  = " << meas.z() << endreq;
  debug() << "-> calling filter(state,meas) ..." << endreq;
  StatusCode sc = filter(state, meas);
  if (sc.isFailure()) return failure("Unable to filter the node!");

  debug() << "z-position of State = " << state.z() << endreq;
  debug() << "z-position of Meas  = " << meas.z() << endreq;

  double tMeasureError = meas.errMeasure();
  debug() << "getting info from projection ..." << endreq;
  double res = m_projector->residual();  // projection just made in filter(,)
  const HepVector& H = m_projector->projectionMatrix();

  // save it for later use
  node.setProjectionMatrix( H );

  double HK = dot ( H, m_K);
  res     *= (1 - HK);
  double errorRes = (1 - HK) * (tMeasureError*tMeasureError);

  debug() << "res/errRes = " << res << " / " << errorRes << endreq;

  debug() << "setting info in node ..." << endreq;
  node.setResidual(res);
  node.setErrResidual(sqrt(errorRes));

  // save predicted state
  debug() << "setting filteredState" << endreq;
  node.setFilteredState(state);
  debug() << "just set filteredState" << endreq;

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

  debug() << "filter(state,meas): projecting ..." << endreq;
  // project the state into the measurement 
  StatusCode sc = m_projector->project(state,meas);
  if (sc.isFailure()) 
    return failure(" Not able to project a state into a measurement");
  debug() << "filter(state,meas): projection done" << endreq;

  // calculate predicted residuals
  double errorMeas = meas.errMeasure();
  double res       = m_projector->residual();
  double errorRes  = m_projector->errResidual();
  double errorRes2 = (errorRes*errorRes);
  const HepVector& H = m_projector->projectionMatrix();

  // calculate gain matrix K
  m_K = (tC * H)/errorRes2;

  // update state vector 
  tX += m_K * res;

  HepMatrix B  = HepDiagMatrix(tC.num_row(), 1) - ( m_K * H.T());
  tC.assign( B * tC * B.T() + ( m_K * pow(errorMeas, 2.0) * m_K.T()));

 if ( m_debugLevel ) {
    debug() << " measure and error \t" << meas.measure() << ", " 
            << errorMeas << endreq
            << " residual and error \t" << res << ", " 
            << errorRes << endreq
            << " projection matrix \t" << H << endreq
            << " gain matrix \t" << m_K << endreq
            << " filter state vector \t" << tX << endreq
            << " filter state covariance \t"<< tC << endreq;
  }
  
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
  debug() << "just entered smooth(node0,node1)" << endreq;  
  // preliminaries, first we need to invert the _predicted_ covariance
  // matrix at k+1
  const HepVector&    prevNodeX = prevNode.predictedState().stateVector();
  const HepSymMatrix& prevNodeC = prevNode.predictedState().covariance();

  debug() << "calling checkInvertMatrix ..." << endreq;  
  // check that the elements are not too large else dsinv will crash
  StatusCode sc = checkInvertMatrix(prevNodeC);
  if ( sc.isFailure() ) return failure(" not valid matrix in smoother"); 

  debug() << "inverting matrix ..." << endreq;  
  //invert the covariance matrix
  HepSymMatrix invPrevNodeC = prevNodeC;
  sc = invertMatrix(invPrevNodeC);
  if ( sc.isFailure() ) return failure(" inverting matrix in smoother");
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
