// $Id: RichRecMCTracks.h,v 1.1.1.1 2002-07-28 10:46:22 jonesc Exp $
#ifndef RICHRECTOOLS_RICHRECMCTRACKS_H
#define RICHRECTOOLS_RICHRECMCTRACKS_H 1

// Fram Gaudi framework
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"

// From Event model
#include "Event/MCParticle.h"
#include "Event/MCRichRadiatorHit.h"

// From RichRecEvent
#include "Event/RichRecTrack.h"

#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichRecMCTracks RichRecMCTracks.h
 *
 *  Algorithm for forming RichRecTracks from MCParticles
 *
 *  @author Christopher.Rob.Jones@cern.ch
 *  created Fri Mar 15 11:10:17 2002
 *
 */

class RichRecMCTracks : public Algorithm {

public:

  /// Standard algorithm constructor
  RichRecMCTracks (const std::string &name,
                   ISvcLocator *svcLoc);

  /// Default destructor
  ~RichRecMCTracks();

  /// Gaudi framework initialize method
  virtual StatusCode initialize ();

  /// Gaudi framework execute method
  virtual StatusCode execute ();

  /// Gaudi framework finalize method
  virtual StatusCode finalize ();


private:

  /// Default constructor
  RichRecMCTracks();

  /// Default copy constructor
  RichRecMCTracks(const RichRecMCTracks &right);

  /// Default copy constructor
  RichRecMCTracks & operator=(const RichRecMCTracks &right);

  /// String containing input MCParticle location in TDS
  std::string m_mcParticleLocation;

  /// String containing MCRichRadiatorHits location in TDS
  std::string m_mcRichRadHitsLocation;

  /// String containing output RichRecTrack location in TDS
  std::string m_richRecTracksLocation;

  /// RichRecSegment location in TDS
  std::string m_richRecSegmentLocation;

  /// Minimum momentum Cut for track selection
  double m_minMomentum;

};

#endif // RICHRECTOOLS_RICHRECMCTRACKS_H
