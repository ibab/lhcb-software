// $Id: TrackProjector.h,v 1.2 2005-04-08 15:45:46 erodrigu Exp $
#ifndef TRACKPROJECTORS_TRACKPROJECTOR_H 
#define TRACKPROJECTORS_TRACKPROJECTOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Tools/ITrackProjector.h"            // Interface

#include "Event/State.h"
#include "Event/Measurement.h"

/** @class TrackProjector TrackProjector.h TrackProjectors/TrackProjector.h
 *
 *  A TrackProjector is a base class implementing methods
 *  from the ITrackProjector interface.
 *
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-03-10
 */
class TrackProjector : public GaudiTool,
                       virtual public ITrackProjector {
public:
  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual double project( const State& state,
                          Measurement& meas );
  

  /// Retrieve the projection matrix H of the (last) projection
  virtual const HepVector& projectionMatrix() const;

  /// Retrieve the chi squared of the (last) projection
  double chi2() const;

  /// Retrieve the residual of the (last) projection
  double residual() const;

  /// Retrieve the error on the residual of the (last) projection
  double errResidual() const;

  /// Standard constructor
  TrackProjector( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent );

  virtual ~TrackProjector( ); ///< Destructor

protected:

  void computeResidual(const State& state, const Measurement& meas);
  void computeErrorResidual(const State& state, const Measurement& meas);

protected:
  double    m_residual;
  double    m_errResidual;
  HepVector m_H;

private:

};
#endif // TRACKPROJECTORS_TRACKPROJECTOR_H
