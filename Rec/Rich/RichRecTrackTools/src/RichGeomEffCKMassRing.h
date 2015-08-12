
//-----------------------------------------------------------------------------
/** @file RichGeomEffCKMassRing.h
 *
 *  Header file for tool : Rich::Rec::GeomEffCKMassRing
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichGeomEffCKMassRing_H
#define RICHRECTOOLS_RichGeomEffCKMassRing_H 1

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichGeomEff.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichRecBase/IRichMassHypothesisRingCreator.h"
#include "RichKernel/IRichParticleProperties.h"

// GSL
#include "gsl/gsl_math.h"

// RichKernel
#include "RichKernel/RichGeomFunctions.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class GeomEffCKMassRing RichGeomEffCKMassRing.h
     *
     *  Tool to perform a full detailed calculation of the geometrical
     *  efficiency for a given RichRecSegment and mass hypothesis.
     *
     *  Uses the mass CK ring objects.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     *
     *  @todo  Remove the use of random numbers if at all possible
     */
    //-----------------------------------------------------------------------------

    class GeomEffCKMassRing : public Rich::Rec::ToolBase,
                              virtual public IGeomEff
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      GeomEffCKMassRing( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

      /// Destructor
      virtual ~GeomEffCKMassRing() {};

      // Initialize method
      StatusCode initialize();

    public: // methods (and doxygen comments) inherited from public interface

      // Obtain geometrical efficiency for this track and hypothesis
      double geomEfficiency ( LHCb::RichRecSegment * segment,
                              const Rich::ParticleIDType id ) const;

      // Obtain scattered geometrical efficiency for this track and hypothesis
      double geomEfficiencyScat ( LHCb::RichRecSegment * segment,
                                  const Rich::ParticleIDType id ) const;

    private: // Private data

      // Pointers to tool instances
      const ICherenkovAngle * m_ckAngle; ///< Cherenkov angle tool

      /// Pointer to ring creator
      const IMassHypothesisRingCreator * m_massHypoRings;

      /// Pointer to RichParticleProperties interface
      const IParticleProperties * m_richPartProp;

      /// Particle ID types to consider in the photon creation checks
      Rich::Particles m_pidTypes;

      /// Use first ring to set all mas hypos
      bool m_useFirstRingForAll;

    };

  }
}

#endif // RICHRECTOOLS_RichGeomEffCKMassRing_H
