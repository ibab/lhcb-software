// $Id: RichTrSegMakerFromTrStoredTracks.h,v 1.1.1.1 2004-06-17 12:04:09 cattanem Exp $
#ifndef RICHDETTOOLS_RICHTRSEGMAKERFROMTRSTOREDTRACKS_H
#define RICHDETTOOLS_RICHTRSEGMAKERFROMTRSTOREDTRACKS_H 1

// base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichTrSegMaker.h"

// Event model
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"

// RichEvent
#include "RichEvent/RichTrackSegment.h"

// LHCbKernel
#include "Kernel/RichSmartID.h"

// RichKernel
#include "RichKernel/BoostArray.h"
#include "RichKernel/Rich1DTabProperty.h"
#include "RichKernel/IRichRayTracing.h"

// RichDet
#include "RichDet/DeRich.h"
#include "RichDet/DeRichRadiator.h"

// Tr Extrapolator
#include "TrKernel/ITrExtrapolator.h"

/** @class RichTrSegMakerFromTrStoredTracks RichTrSegMakerFromTrStoredTracks.h RichDetTools/RichTrSegMakerFromTrStoredTracks.h
 *
 *  Tool to create RichTrackSegments from TrStoredTracks
 *
 *  @author Antonis Papanestis
 *  @author Chris Jones
 *  @date   14/01/2002
 */

class RichTrSegMakerFromTrStoredTracks : public RichToolBase,
                                         virtual public IRichTrSegMaker {

public:

  RichTrSegMakerFromTrStoredTracks( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent );

  virtual ~RichTrSegMakerFromTrStoredTracks( );

  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  int constructSegments( const ContainedObject* obj,
                         std::vector<RichTrackSegment>& segments ) const;

private: // methods

  void fixC4F10EntryPoint( TrStateP* state ) const;

  void correctRadExitMirror( DeRichRadiator* radiator,
                             TrStateP * state ) const;

  StatusCode moveState( TrStateP* state, const double z ) const;

private: // data

  /// Rich1 and Rich2
  DeRich* m_rich[Rich::NRiches];

  /// Spherical mirror parameters
  HepPoint3D m_nominalCoC[Rich::NRiches][2];
  double m_nomSphMirrorRadius[Rich::NRiches];

  /// Radiators
  typedef boost::array<DeRichRadiator*, Rich::NRadiatorTypes> Radiators;
  Radiators m_radiators;

  /// Allowable tolerance on state z positions
  std::vector<double> m_zTolerance;
  /// Nominal z positions of states at RICHes
  double m_nomZstates[4];

  // shifts for mirror correction
  double m_mirrShift[Rich::NRiches];

  // sanity checks on state information
  double m_minStateDiff[Rich::NRadiatorTypes];

  /// Tools
  IRichRayTracing* m_rayTracing;
  // Track extrapolators
  ITrExtrapolator * m_trExt1;
  ITrExtrapolator * m_trExt2;
  std::string m_Ext1;
  std::string m_Ext2;

};


#endif // RICHDETTOOLS_RICHTRSEGMAKERFROMTRSTOREDTRACKS_H
