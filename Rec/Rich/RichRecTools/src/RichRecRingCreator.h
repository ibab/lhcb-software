// $Id: RichRecRingCreator.h,v 1.1 2004-05-30 16:48:13 jonrob Exp $
#ifndef RICHRECTOOLS_RICHRECRINGCREATOR_H
#define RICHRECTOOLS_RICHRECRINGCREATOR_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichRecRingCreator.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichDetTools/IRichRayTracing.h"

// Event
#include "Event/RichRecRing.h"
class RichRecSegment;

/** @class RichRecRingCreator RichRecRingCreator.h
 *
 *  Tool for the creation and book-keeping of RichRecRing objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichRecRingCreator : public RichRecToolBase,
                           virtual public IRichRecRingCreator,
                           virtual public IIncidentListener {

public:

  /// Standard constructor
  RichRecRingCreator( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  /// Destructor
  virtual ~RichRecRingCreator(){}

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  /// This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

  /// Returns a RichRecRing object pointer for given RichRecSegment and mass hypothesis
  RichRecRing * newRing( RichRecSegment * segment,
                         const Rich::ParticleIDType id,
                         const RichRecRing::RingType type = RichRecRing::RayTracedPhotons ) const;


  /// Builds Richrecrings for all mass hypotheses for given RichRecSegment
  void newRings( RichRecSegment * segment,
                 const RichRecRing::RingType type = RichRecRing::RayTracedPhotons ) const;

  /// Returns a pointer to the RichRecRings
  RichRecRings * richRings() const;

private: // methods

  /// Initialise for a new event
  void InitNewEvent();

  /// Build a new ring using photon ray tracing
  RichRecRing * buildRayTracedRing( RichRecSegment * segment,
                                    const Rich::ParticleIDType id ) const;

private: // data

  // Pointers to tool instances
  IRichRayTracing * m_rayTrace;
  IRichCherenkovAngle * m_ckAngle;

  /// Pointer to RichRecRings
  mutable RichRecRings * m_rings;

  /// Location of RichRecRings in TES
  std::string m_richRecRingLocation;

  /// Number of photons to use in ray tracing
  int m_nRayTrace;

  /// Cherenkov phi incrementation
  double m_incPhi;

  typedef std::vector<double> AngleVector;
  /// Cache Sin(angle) for geometrical efficiency calculation
  AngleVector m_sinCkPhi;
  /// Cache Cos(angle) for geometrical efficiency calculation
  AngleVector m_cosCkPhi;

};

inline void RichRecRingCreator::InitNewEvent()
{
  // Initialise navigation data
  m_rings = 0;
}


#endif // RICHRECTOOLS_RICHRECRINGCREATOR_H
