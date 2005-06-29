#ifndef TRACKFITEVENT_FITNODE_H
#define TRACKFITEVENT_FITNODE_H 1

// from CLHEP
#include "CLHEP/Matrix/Matrix.h"

// from TrackEvent
#include "Event/Node.h"
#include "Event/Measurement.h"

/** @class FitNode FitNode.h
 *
 *  This File contains the declaration of the FitNode.
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
 *  @author Matthew Needham
 *  @date   19-08-1999
 */

class FitNode: public Node {
public:
  /// Default constructor
  FitNode();

  /// Constructor from a Measurement
  FitNode(Measurement& meas );

  /// Destructor
  virtual ~FitNode();

  /// retrieve transport matrix
  const HepMatrix& transportMatrix() const  { return m_transportMatrix; }

  /// retrieve transport vector
  const HepVector& transportVector() const { return m_transportVector; }

  /// retrieve noise matrix
  const HepSymMatrix& noiseMatrix() const { return m_noiseMatrix; }

  /// set transport matrix
  void setTransportMatrix( const HepMatrix& transportMatrix ) {
    m_transportMatrix = transportMatrix;
  }
  
  /// set transport vector
  void setTransportVector( const HepVector& transportVector ) {
    m_transportVector = transportVector;
  }
  
  /// set noise matrix
  void setNoiseMatrix( const HepSymMatrix& noiseMatrix ) {
    m_noiseMatrix = noiseMatrix;
  }  

  /// retrieve state predicted by the kalman filter step
  State& predictedState()
  { return *m_predictedState; }

  /// retrieve state predicted by the kalman filter step
  const State& predictedState() const             
  { return *m_predictedState; }

  /// set state predicted by the kalman filter
  void setPredictedState( const State& predictedState );

  /// retrieve filtered state from the kalman filter step
  State& filteredState()
  {return *m_filteredState; }

  /// retrieve filtered state from the kalman filter step
  const State& filteredState() const 
  {return *m_filteredState; }

  /// set filtered state from the kalman filter step
  void setFilteredState( const State& filteredState );

  /// add the transport transformation of prevNode to this node
  void addNode( const FitNode& node );

  /// z position of Node
  double z() const   { return m_measurement->z(); };

private:

  HepMatrix      m_transportMatrix;  ///< transport matrix
  HepVector      m_transportVector;  ///< transport vector
  HepSymMatrix   m_noiseMatrix;      ///< noise matrix
  State*         m_predictedState;   ///< predicted state from filter step
  State*         m_filteredState;    ///< filtered State at this Node
};


#endif // TRACKFITEVENT_FITNODE_H
