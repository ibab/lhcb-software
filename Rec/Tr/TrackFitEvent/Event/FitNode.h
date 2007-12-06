// $Id: FitNode.h,v 1.23 2007-12-06 14:43:32 wouter Exp $
#ifndef TRACKFITEVENT_FITNODE_H
#define TRACKFITEVENT_FITNODE_H 1

// from TrackEvent
#include "Event/Node.h"
#include "Event/Measurement.h"

namespace LHCb
{

  /** @class FitNode FitNode.h Event/FitNode.h
   *
   *  This File contains the declaration of the FitNode.
   *
   *  A FitNode is a basket of objects at a given z position.
   *  The information inside the FitNode has to be sufficient
   *  to allow smoothing and refitting.
   *
   *  At the moment a FitNode contains or allows you to access
   *  info on the the (kth) measurement,
   *  transport from k --> k + 1 , predicted state at k+1
   *  (predicted from filter step)  and the best state at k
   *  (notation note filter procedes from k -> k + 1 -> k + 2 ......)
   *
   *  @author Eduardo Rodrigues 
   *  @date   2005-04-15 (adaptations to the new track event model)
   *
   *  @author Matthew Needham
   *  @date   19-08-1999
   */

  class FitNode: public Node {
  public:

    /// Default constructor
    FitNode();

    /// Constructor from a z position
    FitNode( double zPos );

    /// Constructor from a Measurement
    FitNode(Measurement& meas );

    /// Destructor
    virtual ~FitNode();

    /// Clone the Node
    virtual Node* clone() const;

    /// retrieve transport matrix
    const Gaudi::TrackMatrix& transportMatrix() const
    { return m_transportMatrix; }

    /// retrieve transport vector
    const Gaudi::TrackVector& transportVector() const
    { return m_transportVector; }

    /// retrieve noise matrix
    const Gaudi::TrackSymMatrix& noiseMatrix() const { return m_noiseMatrix; }

    /// retrieve noise matrix
    Gaudi::TrackSymMatrix& noiseMatrix() { return m_noiseMatrix; }

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
    bool transportIsSet() const
    {
      return m_transportIsSet;
    }
    
    /// Retrieve the projection term (obsolete)
    double projectionTerm() const { return m_refResidual + (projectionMatrix()*refVector().parameters())(0) ; }
    
    /// retrieve state predicted by the kalman filter step
    State& predictedStateUp()
    { return m_predictedStateUp; }

    /// retrieve state predicted by the kalman filter step
    const State& predictedStateUp() const
    { return m_predictedStateUp; }

    /// set state predicted by the kalman filter
    void setPredictedStateUp( const State& predictedStateUp );

    /// retrieve predicted state from backward filter
    State& predictedStateDown()
    { return m_predictedStateDown; }

    /// retrieve predicted state from backward filter
    const State& predictedStateDown() const
    { return m_predictedStateDown; }

    /// set predicted state from backward filter
    void setPredictedStateDown( const State& predictedStateDown );

    /// retrieve unbiased residual
    double unbiasedResidual() const 
    { return residual() * errMeasure2() / errResidual2() ; }

    /// retrieve error on unbiased residual
    double errUnbiasedResidual() const 
    { return errMeasure2()/errResidual() ; }

    /// retrieve the unbiased smoothed state at this position
    State unbiasedState() const ;

    /// set chisq contribution in upstream filter
    void setDeltaChi2Upstream(double dchi2) { m_deltaChi2Up = dchi2 ; }

    /// retrieve chisq contribution in upstream filter
    double deltaChi2Upstream() const { return m_deltaChi2Up ; }

    /// set chisq contribution in downstream filter
    void setDeltaChi2Downstream(double dchi2) { m_deltaChi2Down = dchi2 ; }

    /// retrieve chisq contribution in downstream filter
    double deltaChi2Downstream() const { return m_deltaChi2Down ; }

    /// set the residual of the reference
    void setRefResidual( double res ) { m_refResidual = res ; }
    
    /// retrieve the residual of the reference
    double refResidual() const { return m_refResidual ; }
    
    /// set the delta-energy
    void setDeltaEnergy( double e) { m_deltaEnergy = e; }

    /// get the delta-energy
    double deltaEnergy() const { return m_deltaEnergy ; }
    
  private:

    Gaudi::TrackMatrix    m_transportMatrix;    ///< transport matrix for propagation from previous node to this one
    Gaudi::TrackVector    m_transportVector;    ///< transport vector for propagation from previous node to this one
    Gaudi::TrackSymMatrix m_noiseMatrix;        ///< noise in propagation from previous node to this one
    double                m_deltaEnergy;        ///< change in energy in propagation from previous node to this one
    bool                  m_transportIsSet;     ///< Flag for transport params
    double                m_refResidual;        ///< residual of the reference    
    State                 m_predictedStateUp;   ///< predicted state upstream
    State                 m_predictedStateDown; ///< predicted state downstream
    double                m_deltaChi2Up;        ///< chisq contribution in upstream filter
    double                m_deltaChi2Down;      ///< chisq contribution in downstream filter
  };

} // namespace LHCb

#endif // TRACKFITEVENT_FITNODE_H
