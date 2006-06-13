// $Id: FitNode.cpp,v 1.13 2006-06-13 06:54:55 mneedham Exp $
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
Node()
{
  // FitNode default constructor
  m_transportMatrix = TrackMatrix();
  m_transportVector = TrackVector();
  m_noiseMatrix     = TrackSymMatrix();
  m_transportIsSet  = false;
}

/// Constructor from a z position
FitNode::FitNode( double zPos ):
Node()
{
  //FitNode constructer
  m_transportMatrix = TrackMatrix();
  m_transportVector = TrackVector();
  m_noiseMatrix     = TrackSymMatrix();
  m_transportIsSet  = false;
  State tempState = State();
  tempState.setZ( zPos );
  m_state = tempState.clone();
}

/// Constructor from a Measurement
FitNode::FitNode(Measurement& aMeas):
Node(){
  //FitNode constructer
  m_transportMatrix = TrackMatrix();
  m_transportVector = TrackVector();
  m_noiseMatrix     = TrackSymMatrix();
  m_transportIsSet  = false;
  m_measurement     = &aMeas;
  State tempState = State();
  tempState.setZ( aMeas.z() );
  m_state = tempState.clone();
}

FitNode::FitNode( const FitNode& rhs ) : Node(rhs){
  //FitNode copy constructer
  m_transportMatrix = rhs.m_transportMatrix;
  m_transportVector = rhs.m_transportVector;
  m_noiseMatrix     = rhs.m_noiseMatrix;
  m_transportIsSet  = rhs.m_transportIsSet ;
}

/// Destructor
FitNode::~FitNode()
{
}

/// Update the State predicted by the Kalman filter
void FitNode::setPredictedState( const State& predictedState )
{  
  m_predictedState = predictedState ;
}

