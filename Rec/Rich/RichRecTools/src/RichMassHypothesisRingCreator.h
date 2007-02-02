
//-----------------------------------------------------------------------------
/** @file RichMassHypothesisRingCreator.h
 *
 *  Header file for tool : Rich::Rec::MassHypothesisRingCreator
 *
 *  CVS Log :-
 *  $Id: RichMassHypothesisRingCreator.h,v 1.9 2007-02-02 10:10:41 jonrob Exp $
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
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichMassHypothesisRingCreator.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichRayTraceCherenkovCone.h"

// Event
#include "Event/RichRecRing.h"
#include "Event/RichRecSegment.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
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

      // Finalize method
      StatusCode finalize();

    public: // methods (and doxygen comments) inherited from public interface

      // Implement the handle method for the Incident service.
      // This is used to inform the tool of software incidents.
      void handle( const Incident& incident );

      // Returns the mas hypothesis ring for a given segment and mass hypothesis
      LHCb::RichRecRing * newMassHypoRing( LHCb::RichRecSegment * segment,
                                           const Rich::ParticleIDType id ) const;

      // Returns a new default RichRecRing object
      // It is the reponsibility of the user to save or delete the ring
      LHCb::RichRecRing * newMassHypoRing() const;

      // Save the RichRecRing in the container
      void saveMassHypoRing( LHCb::RichRecRing * ring ) const;

      // Builds the mass hypothesis rings for all mass hypotheses for given RichRecSegment
      void newMassHypoRings( LHCb::RichRecSegment * segment ) const;

      // Returns a pointer to all mass hypothesis rings
      LHCb::RichRecRings * massHypoRings() const;

    private: // methods

      /// Initialise for a new event
      void InitNewEvent();

      /// Build a new ring using photon ray tracing
      LHCb::RichRecRing * buildRing( LHCb::RichRecSegment * segment,
                                     const Rich::ParticleIDType id ) const;

    private: // data

      // Pointers to tool instances
      const ICherenkovAngle * m_ckAngle;

      /// Pointer to Rings
      mutable LHCb::RichRecRings * m_rings;

      /// Cherenkov cone ray tracing tool
      const IRayTraceCherenkovCone * m_coneTrace;

      /// Location of Rings in TES
      std::string m_ringLocation;

      /// Ray-tracing configuration object
      LHCb::RichTraceMode m_traceMode;

    };

    inline void MassHypothesisRingCreator::InitNewEvent()
    {
      // Initialise navigation data
      m_rings = 0;
    }

  }
}

#endif // RICHRECTOOLS_RICHMASSHYPOTHESISRINGCREATOR_H
