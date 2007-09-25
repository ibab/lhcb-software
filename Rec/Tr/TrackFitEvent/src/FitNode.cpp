// $Id: FitNode.cpp,v 1.19 2007-09-25 11:48:09 wouter Exp $
// Include files

// local
#include "Event/FitNode.h"

using namespace Gaudi;
using namespace LHCb;

/** @file FitNode.cpp
 *
 *  This File contains the implementation of the FitNode.
 *  A FitNode is a basket of objects at a given z position.
 *  The information inside the FitNode has to be sufficient 
 *  to allow smoothing and refitting.
 *  At the moment a FitNode contains or allows you to access
 *  info on the the (kth) measurement, 
 *  transport from k --> k + 1 , predicted state at k+1  
 *  (predicted from filter step)  and the best state at k  
 *  (notation note filter procedes from k -> k + 1 -> k + 2 ......)
 *  
 *  @author Eduardo Rodrigues (adaptations to the new track event model)
 *  @date   2005-04-15
 *
 *  @author Matt Needham
 *  @date   11-11-1999
 */

/// Standard constructor, initializes variables
FitNode::FitNode():
  Node(),
  m_transportIsSet(false),
  m_refResidual(0),
  m_deltaChi2Up(0),
  m_deltaChi2Down(0)
{
  // FitNode default constructor
}

/// Constructor from a z position
FitNode::FitNode( double zPos ):
  Node(),
  m_transportIsSet(false),
  m_refResidual(0),
  m_deltaChi2Up(0),
  m_deltaChi2Down(0)
{
  //FitNode constructer
  State tempState = State();
  tempState.setZ( zPos );
  m_state = tempState.clone();
}

/// Constructor from a Measurement
FitNode::FitNode(Measurement& aMeas):
  Node(),
  m_transportIsSet(false),
  m_refResidual(0),
  m_deltaChi2Up(0),
  m_deltaChi2Down(0)
{
  //FitNode constructer
  m_measurement   = &aMeas;
  State tempState = State();
  tempState.setZ( aMeas.z() );
  m_state = tempState.clone();
}

FitNode::FitNode( const FitNode& rhs ) : 
  Node(rhs),
  m_transportMatrix   (rhs.m_transportMatrix),
  m_transportVector   (rhs.m_transportVector),
  m_noiseMatrix       (rhs.m_noiseMatrix),
  m_transportIsSet    (rhs.m_transportIsSet),
  m_refResidual       (rhs.m_refResidual),
  m_predictedStateUp  (rhs. m_predictedStateUp),
  m_predictedStateDown(rhs. m_predictedStateDown),
  m_deltaChi2Up       (rhs.m_deltaChi2Up),
  m_deltaChi2Down     (rhs.m_deltaChi2Down)
{
  //FitNode copy constructer
}

/// Destructor
FitNode::~FitNode()
{
}

// Clone the node
LHCb::Node* FitNode::clone() const
{
  return new LHCb::FitNode(*this) ;
}

/// Update the State predicted by the upstream filter
void FitNode::setPredictedStateUp( const State& predictedStateUp )
{  
  m_predictedStateUp = predictedStateUp ;
}

/// Update the predicted State from downstream filter
void FitNode::setPredictedStateDown( const State& predictedStateDown )
{  
  m_predictedStateDown = predictedStateDown ;
}

/// Calculate an unbiased state
LHCb::State FitNode::unbiasedState() const
{
  // This performs an inverse kalman filter step.
  // First calculate the gain matrix
  const TrackProjectionMatrix& H = projectionMatrix();
  double r = residual();
  double R = errResidual2() ;
  const TrackSymMatrix& biasedC = state().covariance() ;
  ROOT::Math::SMatrix<double,5,1> K = (biasedC * Transpose(H)) / R;
  // Update the state vectors
  const TrackVector&  biasedX = state().stateVector();
  TrackVector unbiasedX=biasedX - K.Col(0) * r;
  // Update the covariance matrix
  static const TrackSymMatrix unit = TrackSymMatrix( ROOT::Math::SMatrixIdentity());
  TrackSymMatrix unbiasedC ;
  ROOT::Math::AssignSym::Evaluate(unbiasedC, (unit + K*H)*biasedC) ;
  return State( unbiasedX, unbiasedC, z(), state().location()) ;
}
