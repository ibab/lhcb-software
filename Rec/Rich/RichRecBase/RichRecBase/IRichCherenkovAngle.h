
//-----------------------------------------------------------------------------
/** @file IRichCherenkovAngle.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::ICherenkovAngle
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_IRICHCHERENKOVANGLE_H
#define RICHRECBASE_IRICHCHERENKOVANGLE_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// LHCbKernel
#include "Kernel/RichParticleIDType.h"
#include "Kernel/RichRadiatorType.h"

// Event model
namespace LHCb
{
  class RichRecSegment;
}

/// Static Interface Identification
static const InterfaceID IID_IRichCherenkovAngle( "Rich::Rec::ICherenkovAngle", 1, 0 );

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class ICherenkovAngle IRichCherenkovAngle.h
     *
     *  Interface for tool calculating the expected Cherenkov angle
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class ICherenkovAngle : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichCherenkovAngle; }

      /** Computes the average Cherenkov angle for given particle hypothesis
       *
       *  @param segment The RichRecSegment for which to coumpute the expected CK angle
       *  @param id      The mass hypothesis for which the angle should be calculated
       *  @param useEmittedSpectrum Flag to turn on (true) the use of the emitted photon
       *                            spectrum instead of the detected (false).
       *
       *  @return The expected average cherenkov angle
       */
      virtual double avgCherenkovTheta( LHCb::RichRecSegment * segment,
                                        const Rich::ParticleIDType id,
                                        const bool useEmittedSpectrum = false ) const = 0;

      /** Computes the average Cherenkov angle for given particle hypothesis
       *
       *  @param segment The RichRecSegment for which to coumpute the expected CK angle
       *  @param id      The mass hypothesis for which the angle should be calculated
       *  @param useEmittedSpectrum Flag to turn on (true) the use of the emitted photon
       *                            spectrum instead of the detected (false).
       *
       *  @return The expected average cherenkov angle
       */
      inline double avgCherenkovTheta( const LHCb::RichRecSegment * segment,
                                       const Rich::ParticleIDType id,
                                       const bool useEmittedSpectrum = false ) const
      {
        return avgCherenkovTheta( const_cast<LHCb::RichRecSegment*>(segment), 
                                  id, useEmittedSpectrum );
      }
      
      /** Computes the average Cherenkov angle for the current mass hypothesis
       *  assigned to the given segment
       *
       *  @param segment The RichRecSegment for which to coumpute the expected CK angle
       *
       *  @return The expected average cherenkov angle
       */
      virtual double avgCherenkovTheta( LHCb::RichRecSegment * segment ) const = 0;

      /** Computes the average Cherenkov angle for the current mass hypothesis
       *  assigned to the given segment.
       *
       *  @param segment The RichRecSegment for which to coumpute the expected CK angle
       *
       *  @return The expected average cherenkov angle
       */
      inline double avgCherenkovTheta( const LHCb::RichRecSegment * segment ) const
      {
        return avgCherenkovTheta( const_cast<LHCb::RichRecSegment*>(segment) );
      }

      /** Computes the nominal saturated Cherenkov angle for a given radiator medium
       *
       *  @param rad The radiator medium
       *
       *  @return The nominal saturated Cherenkov angle
       */
      virtual double nominalSaturatedCherenkovTheta( const Rich::RadiatorType rad ) const = 0;

      /** Computes the saturated Cherenkov angle for the radiator associated to the given track segment
       *
       *  @param segment The RichRecSegment for which to coumpute the expected CK angle
       *
       *  @return The saturated Cherenkov angle
       */
      virtual double saturatedCherenkovTheta( const LHCb::RichRecSegment * segment ) const = 0;

      /** Computes the average ring radius on the detector plane, in local HPD coordinates,
       *  for the given mass hypothesis.
       *
       *  Computes the radius by ray tracing a set number of photons at the given angle
       *  taking the average radius, defined by the distance to the track segment projected
       *  centre point
       *
       *  @param segment  Pointer to the RichRecSegment
       *  @param id       The mass hypothesis
       *  @param nSamples Number of samples to make to compute the radius
       *
       *  @return The Cherenkov averge ring radius
       */
      virtual double avCKRingRadiusLocal( LHCb::RichRecSegment * segment,
                                          const Rich::ParticleIDType id,
                                          const unsigned int nSamples = 6 ) const = 0;

      /** Computes the average ring radius on the detector plane, in local HPD coordinates,
       *  for the Cherenkov angle theta.
       *
       *  Computes the radius by ray tracing a set number of photons at the given angle
       *  taking the average radius, defined by the distance to the track segment projected
       *  centre point
       *
       *  @param segment  Pointer to the RichRecSegment
       *  @param ckTheta  The Cherenkov angle theta for which to calculate the ring radius
       *  @param nSamples Number of samples to make to compute the radius
       *
       *  @return The Cherenkov averge ring radius
       */
      virtual double avCKRingRadiusLocal( LHCb::RichRecSegment * segment,
                                          const double ckTheta,
                                          const unsigned int nSamples = 6 ) const = 0;

      /** Computes the average ring radius on the detector plane, in local HPD coordinates,
       *  for the given track segment under the assumption it is saturated.
       *
       *  Computes the radius by ray tracing a set number of photons at the given angle
       *  taking the average radius, defined by the distance to the track segment projected
       *  centre point
       *
       *  @param segment  Pointer to the RichRecSegment
       *  @param nSamples Number of samples to make to compute the radius
       *
       *  @return The Cherenkov averge ring radius
       */
      virtual double satCKRingRadiusLocal( LHCb::RichRecSegment * segment,
                                           const unsigned int nSamples = 6 ) const = 0;

    };

  }
}

#endif // RICHRECTOOLS_IRICHCHERENKOVANGLE_H
