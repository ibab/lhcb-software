// $Id: FitNode.cpp,v 1.8 2006-02-03 09:17:21 ebos Exp $
// Include files

// local
#include "Event/FitNode.h"

// Event/TrackEvent
#include "Event/SHacks.h"

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
FitNode::FitNode()
{
  // FitNode default constructor
  m_transportMatrix = TransportMatrix();
  m_transportVector = Vector5();
  m_noiseMatrix     = TransportMatrix();
  m_transportDeltaZ = 0.0;
}

/// Constructor from a z position
FitNode::FitNode( double zPos )
{
  //FitNode constructer
  m_transportMatrix = TransportMatrix();
  m_transportVector = Vector5();
  m_noiseMatrix     = TransportMatrix();
  m_transportDeltaZ = 0.0;
  State tempState = State();
  tempState.setZ( zPos );
  m_state = tempState.clone();
}

/// Constructor from a Measurement
FitNode::FitNode(Measurement& aMeas)
{
  //FitNode constructer
  m_transportMatrix = TransportMatrix();
  m_transportVector = Vector5();
  m_noiseMatrix     = TransportMatrix();
  m_transportDeltaZ = 0.0;
  m_measurement     = &aMeas;
  State tempState = State();
  tempState.setZ( aMeas.z() );
  m_state = tempState.clone();
}

FitNode::FitNode( const FitNode& rhs ) : Node()
{
  //FitNode copy constructer
  m_transportMatrix = rhs.m_transportMatrix;
  m_transportVector = rhs.m_transportVector;
  m_noiseMatrix     = rhs.m_noiseMatrix;
  m_transportDeltaZ = rhs.m_transportDeltaZ ;
  m_measurement     = rhs.m_measurement;
  m_predictedState  = rhs.m_predictedState;
  m_residual        = rhs.m_residual;         
  m_errResidual     = rhs.m_errResidual;      
  m_projectionMatrix= rhs.m_projectionMatrix;
  // Clone the state !
  m_state = rhs.state().clone();
}

/// Destructor
FitNode::~FitNode()
{
  if (m_state != 0 ) delete m_state;
}

/// Update the State predicted by the Kalman filter
void FitNode::setPredictedState( const State& predictedState )
{  
  m_predictedState = predictedState ;
}

/// Add the transport transformation of prevNode to this node
void FitNode::updateTransport( const FitNode& prevNode )
{
  // add the transport transformation of prevNode to this node
  m_transportVector += m_transportMatrix * prevNode.transportVector() ;
  m_noiseMatrix += SHacks::Similarity( prevNode.noiseMatrix(), m_transportMatrix );
  m_transportMatrix = m_transportMatrix * prevNode.transportMatrix();  
  m_transportDeltaZ += prevNode.transportDeltaZ();
}
