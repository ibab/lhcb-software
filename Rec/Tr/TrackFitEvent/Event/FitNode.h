// $Id: FitNode.h,v 1.8 2006-02-08 17:35:48 erodrigu Exp $
#ifndef TRACKFITEVENT_FITNODE_H
#define TRACKFITEVENT_FITNODE_H 1

// from TrackEvent
#include "Event/Node.h"
#include "Event/Measurement.h"

using namespace Gaudi;
using namespace LHCb;

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
    const TransportMatrix& transportMatrix() const { return m_transportMatrix; }
    
    /// retrieve transport vector
    const TrackVector& transportVector() const { return m_transportVector; }
    
    /// retrieve noise matrix
    const TrackMatrix& noiseMatrix() const { return m_noiseMatrix; }
    
    /// retrieve delta z transport
    double transportDeltaZ() const { return m_transportDeltaZ; }
    
    /// set transport matrix
    void setTransportMatrix( const TransportMatrix& transportMatrix ) {
      m_transportMatrix = transportMatrix;
    }
    
    /// set transport vector
    void setTransportVector( const TrackVector& transportVector ) {
      m_transportVector = transportVector;
    }
    
    /// set noise matrix
    void setNoiseMatrix( const TrackMatrix& noiseMatrix ) {
      m_noiseMatrix = noiseMatrix;
    }  
    
    /// set the transport delta z
    void setTransportDeltaZ( double transportDeltaZ ) {
      m_transportDeltaZ = transportDeltaZ;
    }  
    
    /// Check if the transport information is set correctly
    bool transportIsSet( double deltaZ )
    {  
      return fabs(m_transportDeltaZ - deltaZ) < TrackParameters::lowTolerance;
    }
    
    /// retrieve state predicted by the kalman filter step
    State& predictedState()
    { return m_predictedState; }
    
    /// retrieve state predicted by the kalman filter step
    const State& predictedState() const             
    { return m_predictedState; }
    
    /// set state predicted by the kalman filter
    void setPredictedState( const State& predictedState );
    
    /// add the transport transformation of prevNode to this node
    void updateTransport( const FitNode& node );
    
  private:
    
    TransportMatrix m_transportMatrix; ///< transport matrix
    TrackVector     m_transportVector; ///< transport vector
    TrackMatrix     m_noiseMatrix;     ///< noise matrix
    double          m_transportDeltaZ; ///< transport delta z
    State           m_predictedState;  ///< predicted state from filter step
  };

} // namespace LHCb

#endif // TRACKFITEVENT_FITNODE_H
