
/** @file RichMassHypothesisRingCreator.h
 *
 *  Header file for tool : RichMassHypothesisRingCreator
 *
 *  CVS Log :-
 *  $Id: RichMassHypothesisRingCreator.h,v 1.3 2004-07-27 20:15:30 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

#ifndef RICHRECTOOLS_RICHMASSHYPOTHESISRINGCREATOR_H
#define RICHRECTOOLS_RICHMASSHYPOTHESISRINGCREATOR_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
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

/** @class RichMassHypothesisRingCreator RichMassHypothesisRingCreator.h
 *
 *  Tool to create RichRecRings that represent the rings a given
 *  segment would produce under a given mass hypothesis
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichMassHypothesisRingCreator : public RichRecToolBase,
                                      virtual public IRichMassHypothesisRingCreator,
                                      virtual public IIncidentListener {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichMassHypothesisRingCreator( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

  /// Destructor
  virtual ~RichMassHypothesisRingCreator(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Implement the handle method for the Incident service.
  // This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

  // Returns the mas hypothesis ring for a given segment and mass hypothesis
  RichRecRing * newMassHypoRing( RichRecSegment * segment,
                                 const Rich::ParticleIDType id ) const;

  // Returns a new default RichRecRing object
  // It is the reponsibility of the user to save or delete the ring
  RichRecRing * newMassHypoRing() const;

  // Save the RichRecRing in the container
  void saveMassHypoRing( RichRecRing * ring ) const;

  // Builds the mass hypothesis rings for all mass hypotheses for given RichRecSegment
  void newMassHypoRings( RichRecSegment * segment ) const;

  // Returns a pointer to all mass hypothesis rings
  RichRecRings * massHypoRings() const;

private: // methods

  /// Initialise for a new event
  void InitNewEvent();

  /// Build a new ring using photon ray tracing
  RichRecRing * buildRing( RichRecSegment * segment,
                           const Rich::ParticleIDType id ) const;

private: // data

  // Pointers to tool instances
  IRichRayTraceCherenkovCone * m_rayTrace;
  IRichCherenkovAngle * m_ckAngle;

  /// Pointer to Rings
  mutable RichRecRings * m_rings;

  /// Location of Rings in TES
  std::string m_ringLocation;

};

inline void RichMassHypothesisRingCreator::InitNewEvent()
{
  // Initialise navigation data
  m_rings = 0;
}


#endif // RICHRECTOOLS_RICHMASSHYPOTHESISRINGCREATOR_H
