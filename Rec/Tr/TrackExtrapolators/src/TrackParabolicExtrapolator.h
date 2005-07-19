#ifndef TRACKPARABOLICEXTRAPOLATOR_H
#define TRACKPARABOLICEXTRAPOLATOR_H 1

// Include files
#include "TrackExtrapolator.h"

// Forward declaration
class IMagneticFieldSvc;

/** @class TrackParabolicExtrapolator TrackParabolicExtrapolator.h \
 *         "TrackParabolicExtrapolator.h"
 *
 *  A TrackParabolicExtrapolator is a ITrackExtrapolator that does a transport 
 *  using a parabolic expansion of the trajectory. It doesn't take into
 *  account Multiple Scattering.
 *
 *  @author Edwin Bos (added extrapolation methods)
 *  @date   05/07/2005
 *  @author Jose A. Hernando (13-03-2005)
 *  @author Matt Needham
 *  @date   22-04-2000
 */

class TrackParabolicExtrapolator: public TrackExtrapolator 
{

public:
  /// Constructor
  TrackParabolicExtrapolator( const std::string& type, 
                              const std::string& name, 
                              const IInterface* parent);
  
  /// Destructor
  virtual ~TrackParabolicExtrapolator();
  
  /// initialize
  virtual StatusCode initialize();
  
  // Predicts the distance in Z from the state to the plane
  StatusCode predict( const State& state,
                      const HepPlane3D& plane,
                      double& dZ );

  /// Propagate a state to a given z-position
  virtual StatusCode propagate( State& state,
                                double z,
                                ParticleID pid = ParticleID(211) );

  
  // Propagate a state to the closest position to the specified point
  StatusCode propagate( State& state,
                        const HepPoint3D& point,
                        ParticleID pid = ParticleID(211) );

protected:

  /// update transport matrix
  virtual void updateTransportMatrix( const double dz,
				      State& State );

protected:

  double             m_ax;
  double             m_ay;
  HepVector3D        m_B;
  IMagneticFieldSvc* m_pIMF; ///< Pointer to the magnetic field service

};

#endif // TRACKPARABOLICEXTRAPOLATOR_H
