// $Id: TrackParabolicExtrapolator.h,v 1.1 2005-03-16 14:10:05 hernando Exp $
#ifndef TRACKPARABOLICEXTRAPOLATOR_H
#define TRACKPARABOLICEXTRAPOLATOR_H 1

// Include files
#include "TrackExtrapolators/TrackExtrapolator.h"

// Forward declaration
class IMagneticFieldSvc;

/** @class TrackParabolicExtrapolator TrackParabolicExtrapolator.h \
 *         "TrackParabolicExtrapolator.h"
 *
 *  A TrackParabolicExtrapolator is a ITrackExtrapolator that does a transport 
 *  using a parabolic expansion of the trajectory. It doesn't take into
 *  account Multiple Scattering.
 *
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
  
  /// initialize and finalize
  virtual StatusCode initialize();
  

  /// Propagate a state to a given z-position
  virtual StatusCode propagate( State& state,
                                double z,
                                ParticleID pid = ParticleID(211) );
protected:

  /// update transport matrix
  virtual void updateTransportMatrix( const double dz, State& pState);

protected:

  double             m_ax;
  double             m_ay;
  HepVector3D        m_B;
  IMagneticFieldSvc* m_pIMF; ///< Pointer to the magnetic field service

};


#endif // TRACKPARABOLICEXTRAPOLATOR_H
