// $Id: RichTrSegMakerFromMCRichTracks.h,v 1.1 2004-06-17 12:01:43 cattanem Exp $
#ifndef RICHDETTOOLS_RICHTRSEGMAKERFROMMCRICHTRACKS_H
#define RICHDETTOOLS_RICHTRSEGMAKERFROMMCRICHTRACKS_H 1

// LHCbKernel
#include "Kernel/RichSmartID.h"

// RichKernel
#include "RichKernel/BoostArray.h"
#include "RichKernel/Rich1DTabProperty.h"
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichTrSegMaker.h"
#include "RichKernel/IRichMCTruthTool.h"

// RichEvent
#include "RichEvent/RichTrackSegment.h"
#include "RichEvent/RichGeomPhoton.h"

// Event
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"
#include "Event/MCRichTrack.h"

// RichDet
#include "RichDet/DeRich.h"
#include "RichDet/DeRichRadiator.h"

/** @class RichTrSegMakerFromMCRichTracks RichTrSegMakerFromMCRichTracks.h RichDetTools/RichTrSegMakerFromMCRichTracks.h
 *
 *  Tool to create RichTrackSegments from MCRichTracks and MCRichSegments
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   14/01/2002
 */

class RichTrSegMakerFromMCRichTracks : public RichToolBase,
                                       virtual public IRichTrSegMaker {

public:

  RichTrSegMakerFromMCRichTracks( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent );

  virtual ~RichTrSegMakerFromMCRichTracks( );

  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  // Build the segments from the input data object
  int constructSegments( const ContainedObject * obj,
                         std::vector<RichTrackSegment>& segments ) const;

private: // methods

  /// Performs the correction to the segment exit point at the spherical mirror
  void correctRadExitMirror( DeRichRadiator* radiator,
                             HepPoint3D & exitPoint,
                             const HepPoint3D & exitStatePoint,
                             const HepVector3D & exitStateMomentum ) const;

  /// Returns a pointer to the MCRichTrack associated to the input data object
  const MCRichTrack * mcRichTrack( const ContainedObject * obj ) const;

private: // data

  /// Rich1 and Rich2
  DeRich* m_rich[Rich::NRiches];

  /// Radiators
  typedef boost::array<DeRichRadiator*, Rich::NRadiatorTypes> Radiators;
  Radiators m_radiators;

  // Pointer to the MC truth tool
  IRichMCTruthTool * m_truth;

};


#endif // RICHDETTOOLS_RICHTRSEGMAKERFROMMCRICHTRACKS_H
