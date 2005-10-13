
//-----------------------------------------------------------------------------
/** @file RichCherenkovAngle.h
 *
 *  Header file for tool : RichCherenkovAngle
 *
 *  CVS Log :-
 *  $Id: RichCherenkovAngle.h,v 1.8 2005-10-13 16:01:55 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHCHERENKOVANGLE_H
#define RICHRECTOOLS_RICHCHERENKOVANGLE_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// Rich Kernel
#include "RichKernel/BoostArray.h"

// interfaces
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichKernel/IRichRefractiveIndex.h"
#include "RichKernel/IRichParticleProperties.h"

// GSL
#include "gsl/gsl_math.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

//-----------------------------------------------------------------------------
/** @class RichCherenkovAngle RichCherenkovAngle.h
 *
 *  Tool calculating the expected Cherenkov angle
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichCherenkovAngle : public RichRecToolBase,
                           virtual public IRichCherenkovAngle {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichCherenkovAngle( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  /// Destructor
  virtual ~RichCherenkovAngle() {};

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Returns average Cherenkov angle for given particle hypothesis
  double avgCherenkovTheta( RichRecSegment * segment,
                            const Rich::ParticleIDType id ) const;

  // Returns average Cherenkov angle for the current mass hypothesis
  // Assigned to the segment
  double avgCherenkovTheta( RichRecSegment * segment ) const;

  // Computes the nominal saturated Cherenkov angle for a given radiator medium
  double nominalSaturatedCherenkovTheta( const Rich::RadiatorType rad ) const;

  // Computes the average ring radius on the detector plane, in local HPD coordinates,
  // for the given mass hypothesis.
  double avCKRingRadiusLocal( RichRecSegment * segment,
                              const Rich::ParticleIDType id,
                              const unsigned int nSamples = 6 ) const;

  // Computes the average ring radius on the detector plane, in local HPD coordinates,
  // for the given cherenkov angle
  double avCKRingRadiusLocal( RichRecSegment * segment,
                              const double ckTheta,
                              const unsigned int nSamples = 6 ) const;

  // Computes the average ring radius on the detector plane, in local HPD coordinates,
  double satCKRingRadiusLocal( RichRecSegment * segment,
                               const unsigned int nSamples = 6 ) const;

private: // methods

  /// Compute and store the average radii values for the mass hypotheses
  void computeRadii( RichRecSegment * segment,
                     const unsigned int nSamples ) const;

private: // Private data

  /// Pointer to RichExpectedTrackSignal interface
  const IRichExpectedTrackSignal * m_signal;

  /// Pointer to RichParticleProperties interface
  const IRichParticleProperties * m_richPartProp;

  /// Pointer to refractive index tool
  const IRichRefractiveIndex * m_refIndex;

  /// Pointer to the RichSmartID tool
  const IRichSmartIDTool * m_smartIDTool;

  /// Pointer to the ray tracking tool
  const IRichRayTracing * m_rayTrace;

  /// Storage for nominal saturated Cherenkov angles
  boost::array< double, Rich::NRadiatorTypes > m_nomCK;

};

#endif // RICHRECTOOLS_RICHCHERENKOVANGLE_H
