// Include files
// -------------
// local
#include "Event/FitNode.h"

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

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FitNode::FitNode():
  m_predictedState(0),
  m_filteredState(0)
{
  // FitNode default constructer
  m_transportMatrix = HepMatrix(5, 5, 1);
  m_transportVector = HepVector(5, 0);
  m_noiseMatrix     = HepSymMatrix(5, 0);
  m_state           = 0;
}

//=============================================================================
// Constructor from a Measurement
//=============================================================================
FitNode::FitNode(Measurement* aMeas):
  m_predictedState(0),
  m_filteredState(0)
{
  //FitNode constructer
  m_transportMatrix = HepMatrix(5, 5, 1);
  m_transportVector = HepVector(5, 0);
  m_noiseMatrix     = HepSymMatrix(5, 0);
  m_state           = 0;
  m_measurement     = aMeas;
}

//=============================================================================
// Destructor
//=============================================================================
FitNode::~FitNode()
{
  delete m_predictedState;
  delete m_filteredState;
  delete m_state;
}

//=============================================================================
// Update the State predicted by the Kalman filter
//=============================================================================
void FitNode::updatePredictedState( State* predictedState )
{  
  //if pointer not set clone state - else copy (not great)
  if ( !m_predictedState ) {
    m_predictedState = predictedState -> clone();    
  }
  else{
    // get reference to the state vector and cov
    HepVector& tX    = m_predictedState -> stateVector();
    HepSymMatrix& tC = m_predictedState -> covariance();
   
    //update 
    tX = predictedState -> stateVector();
    tC = predictedState -> covariance();

  }
}

//=============================================================================
// Update the filtered state from the Kalman filter step
//=============================================================================
void FitNode::updateFilteredState( State* filteredState )
{  
  //if pointer not set clone state - else copy (not great)
  if ( !m_filteredState ) {
    m_filteredState = filteredState -> clone();
  }
  else{
    // get reference to the state vector and cov
    HepVector& tX    = m_filteredState -> stateVector();
    HepSymMatrix& tC = m_filteredState -> covariance();
   
    //update 
    tX = filteredState -> stateVector();
    tC = filteredState -> covariance();

  }
}

//=============================================================================
// 
//=============================================================================
void FitNode::updateBestState( State* state )
{
  /// update the best state 
  //if pointer not set clone state - else copy (not great)
  if ( !m_state ) {
    m_state = state -> clone();
  }
  else{
    // get reference to the state vector and cov
    HepVector& tX    = m_state -> stateVector();
    HepSymMatrix& tC = m_state -> covariance();

    //update 
    tX = state -> stateVector();
    tC = state -> covariance();
  }
}

//=============================================================================
// Add the transport transformation of prevNode to this node
//=============================================================================
void FitNode::addNode( FitNode* prevNode )
{
  // add the transport transformation of prevNode to this node
  m_transportVector += m_transportMatrix * prevNode->transportVector() ;
  m_noiseMatrix += (prevNode->noiseMatrix()).similarity( m_transportMatrix );
  m_transportMatrix = m_transportMatrix * prevNode->transportMatrix();  
}

//=============================================================================
