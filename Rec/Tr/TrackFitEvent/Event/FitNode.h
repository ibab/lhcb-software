// $Id: FitNode.h,v 1.12 2006-05-19 11:04:51 erodrigu Exp $
#ifndef TRACKFITEVENT_FITNODE_H
#define TRACKFITEVENT_FITNODE_H 1

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

namespace LHCb
{
  
  class FitNode: public Node {
  public:

    /// Default constructor
    FitNode();
    
    /// Constructor from a z position
    FitNode( double zPos );
    
    /// Constructor from a Measurement
    FitNode(Measurement& meas );
    
    /// Copy constructor
    FitNode( const FitNode& rhs );
    
    /// Destructor
    virtual ~FitNode();
    
    /// retrieve transport matrix
    const Gaudi::TrackMatrix& transportMatrix() const
    { return m_transportMatrix; }
    
    /// retrieve transport vector
    const Gaudi::TrackVector& transportVector() const
    { return m_transportVector; }
    
    /// retrieve noise matrix
    const Gaudi::TrackSymMatrix& noiseMatrix() const { return m_noiseMatrix; }
    
    /// set transport matrix
    void setTransportMatrix( const Gaudi::TrackMatrix& transportMatrix ) {
      m_transportMatrix = transportMatrix;
    }
    
    /// set transport vector
    void setTransportVector( const Gaudi::TrackVector& transportVector ) {
      m_transportVector = transportVector;
    }
    
    /// set noise matrix
    void setNoiseMatrix( const Gaudi::TrackSymMatrix& noiseMatrix ) {
      m_noiseMatrix = noiseMatrix;
    }  
    
    /// set the transport flag
    void setTransportIsSet( bool transportIsSet ) {
      m_transportIsSet = transportIsSet;
    }  
    
    /// Check if the transport information is set correctly
    bool transportIsSet( )
    {  
      return m_transportIsSet;
    }
    
    /// retrieve state predicted by the kalman filter step
    State& predictedState()
    { return m_predictedState; }
    
    /// retrieve state predicted by the kalman filter step
    const State& predictedState() const             
    { return m_predictedState; }
    
    /// set state predicted by the kalman filter
    void setPredictedState( const State& predictedState );
    
  private:
    
    Gaudi::TrackMatrix    m_transportMatrix; ///< transport matrix
    Gaudi::TrackVector    m_transportVector; ///< transport vector
    Gaudi::TrackSymMatrix m_noiseMatrix;     ///< noise matrix
    bool            m_transportIsSet;  ///< Flag for transport parameters
    State           m_predictedState;  ///< predicted state from filter step
  };

} // namespace LHCb

#endif // TRACKFITEVENT_FITNODE_H
