// $Id: FitNode.cpp,v 1.25 2009-09-25 09:04:35 wouter Exp $
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
  m_deltaEnergy(0),
  m_transportIsSet(false),
  m_refResidual(0),
  m_deltaChi2Forward(0),
  m_deltaChi2Backward(0),
  m_doca(0)
{
  // FitNode default constructor
}

/// Constructor from a z position
FitNode::FitNode( double zPos, LHCb::State::Location location ):
  Node(zPos,location),
  m_deltaEnergy(0),
  m_transportIsSet(false),
  m_refResidual(0),
  m_deltaChi2Forward(0),
  m_deltaChi2Backward(0),
  m_doca(0)
{
}

/// Constructor from a Measurement
FitNode::FitNode(Measurement& aMeas):
  Node(&aMeas),
  m_deltaEnergy(0),
  m_transportIsSet(false),
  m_refResidual(0),
  m_deltaChi2Forward(0),
  m_deltaChi2Backward(0),
  m_doca(0)
{
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

/// Forwarddate the State predicted by the upstream filter
void FitNode::setPredictedStateForward( const State& predictedStateForward )
{  
  m_predictedStateForward = predictedStateForward ;
}

/// Forwarddate the predicted State from downstream filter
void FitNode::setPredictedStateBackward( const State& predictedStateBackward )
{  
  m_predictedStateBackward = predictedStateBackward ;
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
  // Forwarddate the state vectors
  const TrackVector&  biasedX = state().stateVector();
  TrackVector unbiasedX=biasedX - K.Col(0) * r;
  // Forwarddate the covariance matrix
  static const TrackSymMatrix unit = TrackSymMatrix( ROOT::Math::SMatrixIdentity());
  TrackSymMatrix unbiasedC ;
  ROOT::Math::AssignSym::Evaluate(unbiasedC, (unit + K*H)*biasedC) ;
  return State( unbiasedX, unbiasedC, z(), state().location()) ;
}
