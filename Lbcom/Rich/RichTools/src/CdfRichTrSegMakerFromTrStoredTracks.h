// $Id: CdfRichTrSegMakerFromTrStoredTracks.h,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
#ifndef RICHDETTOOLS_CDFRICHTRSEGMAKERFROMTRSTOREDTRACKS_H
#define RICHDETTOOLS_CDFRICHTRSEGMAKERFROMTRSTOREDTRACKS_H 1

// base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichTrSegMaker.h"

// Event model
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"

// Kernel
#include "Kernel/RichSmartID.h"

// RichKernel
#include "RichKernel/Rich1DTabProperty.h"

// local
#include "Rich1.h"
#include "Rich2.h"

/** @class CdfRichTrSegMakerFromTrStoredTracks CdfRichTrSegMakerFromTrStoredTracks.h RichDetTools/CdfRichTrSegMakerFromTrStoredTracks.h
 *
 *  Tool to create RichTrackSegments from TrStoredTracks.
 *  Old version optimised for SICB data
 *
 *  @author Artur Barczyk, Chris Jones
 *  @date   14/01/2002
 */

class CdfRichTrSegMakerFromTrStoredTracks : public RichToolBase,
                                            virtual public IRichTrSegMaker {

public:

  CdfRichTrSegMakerFromTrStoredTracks( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent );

  virtual ~CdfRichTrSegMakerFromTrStoredTracks( );

  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  int constructSegments( const ContainedObject * obj,
                         std::vector<RichTrackSegment>& segments ) const;

private: // methods

  void fixC4F10EntryPoint( HepPoint3D & entryPoint,
                           const HepPoint3D & entryStatePoint,
                           const HepVector3D & entryStateMomentum ) const;

  void correctRadExitMirror( RichRadiator * radiator,
                             HepPoint3D & exitPoint,
                             const HepPoint3D & exitStatePoint,
                             const HepVector3D & exitStateMomentum ) const;

private: // data

  RichX::PointerCollection m_rich;

  // for fast reference. All radiators (both riches) in one container
  RichRadiator::PointerCollection m_radiator;

  // Reflectors/detectors in separate container for each rich1/2
  RichReflector::PointerCollection m_reflector[2];
  RichDetector::PointerCollection  m_detector[2];

  /// Allowable tolerance on state z positions
  std::vector<double> m_zTolerance;
  /// Nominal z positions of states at RICHes
  std::vector<double> m_nomZstates;

};


#endif // RICHDETTOOLS_CDFRICHTRSEGMAKERFROMTRSTOREDTRACKS_H
