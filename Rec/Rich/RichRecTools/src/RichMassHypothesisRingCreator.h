
//-----------------------------------------------------------------------------
/** @file RichMassHypothesisRingCreator.h
 *
 *  Header file for tool : Rich::Rec::MassHypothesisRingCreator
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHMASSHYPOTHESISRINGCREATOR_H
#define RICHRECTOOLS_RICHMASSHYPOTHESISRINGCREATOR_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichMassHypothesisRingCreator.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichRayTraceCherenkovCone.h"
#include "RichKernel/IRichParticleProperties.h"

// Event
#include "Event/RichRecRing.h"
#include "Event/RichRecSegment.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class MassHypothesisRingCreator RichMassHypothesisRingCreator.h
     *
     *  Tool to create RichRecRings that represent the rings a given
     *  segment would produce under a given mass hypothesis
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     *
     *  @todo See if newMassHypoRing and saveMassHypoRing methods can be removed
     *        from public interface ...
     */
    //-----------------------------------------------------------------------------

    class MassHypothesisRingCreator : public Rich::Rec::ToolBase,
                                      virtual public IMassHypothesisRingCreator,
                                      virtual public IIncidentListener
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      MassHypothesisRingCreator( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

      /// Destructor
      virtual ~MassHypothesisRingCreator(){}

      // Initialize method
      StatusCode initialize();

    public: // methods (and doxygen comments) inherited from public interface

      // Implement the handle method for the Incident service.
      // This is used to inform the tool of software incidents.
      void handle( const Incident& incident );

      // Returns the mas hypothesis ring for a given segment and mass hypothesis
      LHCb::RichRecRing * massHypoRing( LHCb::RichRecSegment * segment,
                                        const Rich::ParticleIDType id ) const;

      // Builds the mass hypothesis rings for all mass hypotheses for given RichRecSegment
      void massHypoRings( LHCb::RichRecSegment * segment ) const;

      // Returns a pointer to all mass hypothesis rings
      LHCb::RichRecRings * massHypoRings() const;

      // Returns a new default RichRecRing object
      // It is the reponsibility of the user to save or delete the ring
      LHCb::RichRecRing * newMassHypoRing() const;

      // Save the RichRecRing in the container
      void saveMassHypoRing( LHCb::RichRecRing * ring ) const;

    private: // methods

      /// Initialise for a new event
      void InitNewEvent();

      /// Build a new ring using photon ray tracing
      LHCb::RichRecRing * buildRing( LHCb::RichRecSegment * segment,
                                     const Rich::ParticleIDType id ) const;

      /// Access the # point scale factor for the given radiator
      inline double nPointScale( const Rich::RadiatorType rad ) const
      {
        if ( m_nPointScale[rad] < 0 )
        {
          const auto satCKang = m_ckAngle->nominalSaturatedCherenkovTheta(rad);
          m_nPointScale[rad] = fabs( m_maxPoint[rad] - m_minPoint[rad] ) / satCKang;
        }
        return m_nPointScale[rad];
      }

    private: // data

      /// Cherenkov angle tool
      const ICherenkovAngle * m_ckAngle = nullptr;

      /// Pointer to Rings
      mutable LHCb::RichRecRings * m_rings = nullptr;

      /// Cherenkov cone ray tracing tool
      const IRayTraceCherenkovCone * m_coneTrace = nullptr;

      /// Pointer to RichParticleProperties interface
      const IParticleProperties * m_richPartProp = nullptr;

      /// Location of Rings in TES
      std::string m_ringLocation;

      /// JO flag to switch between simple or detail HPD description in ray tracing
      bool m_useDetailedHPDsForRayT;

      /// Cached trace modes for each radiator
      std::vector<LHCb::RichTraceMode> m_traceModeRad;

      /// Flag to turn on or off checking of intersections with beampipe
      bool m_checkBeamPipe;

      /// Scale number for number of points on ring
      mutable std::vector<double> m_nPointScale;

      /// Max number of points on a ring
      std::vector<unsigned int> m_maxPoint;

      /// Min number of points on a ring
      std::vector<unsigned int> m_minPoint;

      /// Particle ID types to consider in the photon creation checks
      Rich::Particles m_pidTypes;

    };

    inline void MassHypothesisRingCreator::InitNewEvent()
    {
      // Initialise navigation data
      m_rings = NULL;
    }

  }
}

#endif // RICHRECTOOLS_RICHMASSHYPOTHESISRINGCREATOR_H
