// $Id: KalmanFilter.cpp,v 1.1.1.1 2005-04-18 11:55:37 hernando Exp $
//
//  Implementation of the KalmanFilter tool
//
//  Original Author: Rutger van der Eijk
//  Created: 07-04-1999
//  Adapted: 21-03-2002  Olivier Callot
//  Adapted: 15-04-2005  Jose A. Hernando

//gaudi
#include "GaudiKernel/ToolFactory.h"

// TrEvent
#include "KalmanFilter.h"

// CLHEP
#include "CLHEP/Matrix/DiagMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Units/PhysicalConstants.h"

static const ToolFactory<KalmanFilter>  s_factory;
const IToolFactory& KalmanFilterFactory = s_factory;

KalmanFilter::KalmanFilter( const std::string& type,
                            const std::string& name,
                            const IInterface* parent) :
  GaudiTool( type, name, parent)
{
  declareInterface<IKalmanFilter>( this );

  declareProperty("Extrapolator",m_extrapolatorName = "MasterExtrapolator");

  declareProperty("Projector",m_projectorName = "MasterProjector");

  m_state = NULL;
  m_nodes.clear();
  m_K = HepVector(5,0);
}

KalmanFilter::~KalmanFilter() {
}

StatusCode KalmanFilter::initialize() 
{
  m_extrapolator = get<ITrackExtrapolator>(m_extrapolatorName);

  m_projector = get<ITrackProjector>(m_projectorName);

  return StatusCode::SUCCESS;
}

StatusCode KalmanFilter::failure(const std::string& comment){
  info() << " Kalman Filter failure: " << comment << endreq;
  return StatusCode::FAILURE;
}

//
// perform filter /smoothing
//

StatusCode KalmanFilter::fit(Track& track, const State& state)
{
  StatusCode sc = filter(track,state);
  if (sc.isFailure()) return failure(" No able to filter the track");
  
  return sc = smoother(track);
  if (sc.isFailure()) return failure(" No able to smooth the track");
  
  return sc;
}

StatusCode KalmanFilter::iniKalman(Track & track) 
{
  // destroy the previous state if not null
  if (m_state != NULL) delete m_state;
  // Kalman has private copy of the nodes to acelerate the computations
  // remember that the track owns the Nodes, so it is responsability of the
  // Track destructor to delete them!
  m_nodes.clear();
  std::vector<Measurement*>& measures = track.measurements();
  std::vector<Node*>& nodes = track.nodes();
  if (nodes.size() > 0) 
    for (std::vector<Node*>::iterator it = nodes.begin(); it != nodes.end();
         it++) delete *it;  
  nodes.clear();
  for (std::vector<Measurement*>::iterator it = measures.begin(); 
       it != measures.end(); it++) {
    Measurement& meas = *(*it);
    FitNode* node = new FitNode(meas);
    m_nodes.push_back(node);
    track.addToNodes(node);
  }

  if (msgLevel(MSG::DEBUG)) {
    debug() << " seed state vector \t" << m_state->state() << endreq;
    debug() << " seed state cov \t" << m_state->covariance() << endreq;
    debug() << " track nodes size \t" << m_nodes.size() << endreq;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode KalmanFilter::filter(Track& track, const State& state)
{
  StatusCode sc = StatusCode::SUCCESS;
  sc = iniKalman(track);
  if (sc.isFailure()) return failure(" Not able to create Nodes");

  m_state = state.clone();

  std::vector<FitNode*>::iterator iNode = m_nodes.begin();
  double z = (*iNode)->measurement().z();
  // sc = m_extrapolator->propagate(*m_state,z,m_particleID);
  sc = m_extrapolator->propagate(*m_state,z);
  if (sc.isFailure()) 
    return failure(" Not able to extrapolate to 1st measurement");

  // TODO: handle different iterations
  for (iNode = m_nodes.begin(); iNode != m_nodes.end(); ++iNode) {
    FitNode& node = *(*iNode);
    sc = predict(node,*m_state);
    if (sc.isFailure()) return failure(" not able to predict at node");
    sc = filter(node);
    if (sc.isFailure()) return failure(" not able to filter node ");
  }  

  computeChiSq(track);
  return sc;
}

StatusCode KalmanFilter::smoother(Track& track) 
{
  StatusCode sc = StatusCode::SUCCESS;
  // smoother loop - runs in opposite direction to filter
  std::vector<FitNode*>::reverse_iterator riNode = m_nodes.rbegin();
  FitNode* oldNode = *riNode;
  ++riNode;
  
  while (riNode != m_nodes.rend()) {
    sc = smooth( *(*riNode), *oldNode );
    if (sc.isFailure()) return failure(" not able to smooth node");      
    oldNode = *riNode;
    ++riNode;
  }

  computeChiSq(track);

  return StatusCode::SUCCESS;
}

void KalmanFilter::computeChiSq(Track& track) 
{
  double chi2 = 0.;
  for (std::vector<FitNode*>::iterator it = m_nodes.begin();
       it != m_nodes.end(); it++) 
    chi2 += (*it)->chi2();
  int ndof = -(m_state->nParameters());
  ndof += m_nodes.size();
  double chi2Norma = 0.;
  if (ndof != 0) chi2Norma = chi2 /((float) (ndof));
  track.setChi2PerDoF(chi2Norma);
  track.setNDoF(ndof);

  if (msgLevel(MSG::DEBUG)) {
    debug() << " track ndof \t" << track.chi2() << endreq;
    debug() << " track chi2, chi2/ndof \t " << track.chi2()  << ", "
            << track.chi2PerDoF() << endreq;
  }
  
}

//
// perform Kalman filter step on a node ie predict-update
//
StatusCode KalmanFilter::predict(FitNode& aNode, State& aState) {

  Measurement& thisMeasure = aNode.measurement();

  // only use extrapolator in first iteration; else use stored parameters
  HepVector prevStateVec = aState.state();
  HepSymMatrix prevStateCov = aState.covariance();
  double z = thisMeasure.z();
  // StatusCode sc = m_extrapolator->propagate(state,z,m_particleID);
  StatusCode sc = m_extrapolator->propagate(aState,z);
  if (sc.isFailure()) 
    return failure(" No able to predit state at next measurement");

  const HepMatrix& F = m_extrapolator->transportMatrix();
  aNode.setTransportMatrix( F );
  aNode.setTransportVector( aState.state() - F * prevStateVec );
  aNode.setNoiseMatrix( aState.covariance() - prevStateCov.similarity(F) );

  // save predicted state
  aNode.setPredictedState(aState);

  if (msgLevel(MSG::DEBUG)) {
    debug() << " predicted  state at z \t" << z << endreq;
    debug() << " predicted  state vector  \t" << aState.state() << endreq;
    debug() << " predicted  state cov \t" << aState.covariance() << endreq;
    debug() << " transport matrix \t" << aNode.transportMatrix() << endreq;
    debug() << " transport vector \t" << aNode.transportVector() << endreq;
    debug() << " noise matrix \t" << aNode.noiseMatrix() << endreq;
  } 

  return StatusCode::SUCCESS;  
}

StatusCode KalmanFilter::filter(FitNode& node) 
{
  
  Measurement& meas = node.measurement();
  State& state = node.state();

  StatusCode sc = filter(state, meas);
  if (sc.isFailure()) return failure("Not able to filter the node!");

  double tMeasureError = meas.errMeasure();
  double res = m_projector->residual();  
  const HepVector& H = m_projector->projectionMatrix();

  double HK = dot ( H, m_K);
  res     *= (1 - HK);
  double errorRes = (1 - HK) * pow(tMeasureError, 2.0);
  
  node.setResidual(res);
  node.setErrResidual(sqrt(errorRes));
  
  // save predicted state
  node.setFilteredState(state);

  if (msgLevel(MSG::DEBUG)) {
    debug() << " filtered residual and error \t" << node.residual() << ", "
            <<node.errResidual() << endreq;;
  } 
 
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------
// Update the current track 'state' with the TrMeasurement 'measure'.
// (i.e. filter the measurement into the state)
//  the  measurement ch-sq is updated
//-----------------------------------------------------------------
StatusCode KalmanFilter::filter(State& state, Measurement& meas)  {
  
  // check z position
  if ( fabs(meas.z() - state.z()) > 1e-6) {
    warning() << "z position of TrState (=" << state.z() 
              << ") and TrStateMeasurement (= " << meas.z() 
              << ") are not equal." << endreq;
  }

  // get reference to the state vector and cov
  HepVector&    tX = state.state();
  HepSymMatrix& tC = state.covariance();

  // project the state into the measurement 
  StatusCode sc = m_projector->project(state,meas);
  if (sc.isFailure()) 
    return failure(" Not able to project a state into a measurement");
  
  // calculate predicted residuals
  double errorMeas = meas.errMeasure();
  double res = m_projector->residual();
  double errorRes = m_projector->errResidual();
  double errorRes2 = (errorRes*errorRes);
  const HepVector& H = m_projector->projectionMatrix();

  // calculate gain matrix K
  m_K = (tC * H)/errorRes2;

  // update state vector 
  tX += m_K * res;

  HepMatrix B  = HepDiagMatrix(tC.num_row(), 1) - ( m_K * H.T());
  tC.assign( B * tC * B.T() + ( m_K * pow(errorMeas, 2.0) * m_K.T()));

 if (msgLevel(MSG::DEBUG)) {
    debug() << " measure and error \t" << meas.measure() << ", " 
            << errorMeas << endreq;
    debug() << " residual and error \t" << res << ", " 
            << errorRes << endreq;
    debug() << " projection matrix \t" << H << endreq;
    debug() << " gain matrix \t" << m_K << endreq;
    debug() << " filter state vector \t" << tX << endreq;
    debug() << " filter state covariance \t"<< tC << endreq;
  }
  
  return sc;
}

//----------------------------------------------------------------
// smooth a node on a track
// Input: node to be smoothed (k) + last node (k+1)
// remember we are now running in the opposite direction to the filter !!
// M. Needham 9/11/99
//----------------------------------------------------------------
StatusCode KalmanFilter::smooth(FitNode& thisNode, FitNode& prevNode){

  // preliminaries, first we need to invert the _predicted_ covariance
  // matrix at k+1
  HepVector&    prevNodeX = prevNode.predictedState().state();
  HepSymMatrix& prevNodeC = prevNode.predictedState().covariance();

  // check that the elements are not too large else dsinv will crash
  StatusCode sc = checkInvertMatrix(prevNodeC);; 
  if ( sc.isFailure() ) return failure(" not valid matrix in smoother"); 

  //invert the covariance matrix
  HepSymMatrix  invPrevNodeC = prevNodeC;
  sc = invertMatrix(invPrevNodeC);
  if ( sc.isFailure() ) return failure(" inverting matrix in smoother");

  //references to _predicted_ state + cov of this node from the first step
  HepVector&    thisNodeX = thisNode.state().state();
  HepSymMatrix& thisNodeC = thisNode.state().covariance();

  //transport
  const HepMatrix& F = prevNode.transportMatrix();

  //calculate gain matrix A
  HepMatrix A = thisNodeC * F.T() * invPrevNodeC;

  // best = smoothed state at prev Node
  HepVector&    prevNodeSmoothedX = prevNode.state().state();
  HepSymMatrix& prevNodeSmoothedC = prevNode.state().covariance();

  //smooth state
  thisNodeX += A * (prevNodeSmoothedX - prevNodeX);

  // smooth covariance  matrix
  HepSymMatrix covDiff = prevNodeSmoothedC- prevNodeC;
  HepSymMatrix covUpDate = covDiff.similarity(A);
  thisNodeC += covUpDate;

  // check that the cov matrix is possitive
  sc = checkPositiveMatrix(thisNodeC);
  if (sc.isFailure()) return Warning(" not postive Matrix in smoother");

  // update = smooth the residuals
  const HepVector& H = thisNode.projectionMatrix();
  sc = m_projector->project(thisNode.state(),thisNode.measurement());
  if (sc.isFailure()) Warning(" unable to project the smooth node");
  double res = m_projector->residual();
  thisNode.setResidual(res);
  double errRes = thisNode.errResidual();
  double errRes2 = errRes*errRes -  covUpDate.similarity(H);
  if (errRes2 >0.) errRes = sqrt(errRes2);
  else return Warning(" negative error residuals in smoother");
  thisNode.setErrResidual(sqrt(errRes));

  if (msgLevel(MSG::DEBUG)) {
    debug() << " A matrix \t" << A << endreq;;
    debug() << " smooth state vector \t" << thisNodeX << endreq;
    debug() << " smooth state covariance \t"<< thisNodeC << endreq;
    debug() << " smooth residual, error and chi2 \t" << res << ", "
            << errRes << ", " << thisNode.chi2() << endreq;      
  }

  return StatusCode::SUCCESS;
}

StatusCode KalmanFilter::checkInvertMatrix(HepSymMatrix& mat) 
{
  int nParam = (int) mat.num_row();
  for (int i=0; i<nParam; ++i){
    for (int j=0; j<nParam; ++j){
      if (mat[i][j] > 1e20)
        return Warning("covariance errors too big to invert");
    } // j
  }   // i
  return StatusCode::SUCCESS;
}

StatusCode KalmanFilter::checkPositiveMatrix(HepSymMatrix& mat) 
{
  int nParam = (int) mat.num_row();
  for (int i=0; i<nParam; ++i){
    if (mat[i][i] <= 0.)
      return Warning("covariance errors too big to convert");
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
StatusCode KalmanFilter::invertMatrix(HepSymMatrix& invPrevNodeC){

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
StatusCode KalmanFilter::cToG3(HepSymMatrix& C){

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
StatusCode KalmanFilter::cToG4(HepSymMatrix& invC){

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












