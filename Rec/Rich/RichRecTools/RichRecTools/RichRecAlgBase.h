// $Id: RichRecAlgBase.h,v 1.1 2002-11-14 13:54:21 jonrob Exp $
#ifndef RICHRECALGS_RICHRECALGBASE_H
#define RICHRECALGS_RICHRECALGBASE_H 1

// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IChronoStatSvc.h"

// From Rich tools
#include "RichRecTools/IRichRecPhotonTool.h"
#include "RichRecTools/IRichRecTrackTool.h"
#include "RichRecTools/IRichRecPixelTool.h"
#include "RichRecTools/IRichRecSegmentTool.h"

// Rich Detector
#include "RichDetTools/IRichDetInterface.h"

#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichRecAlgBase RichRecAlgBase.h RichRecAlgs/RichRecAlgBase.h
 *
 *  Base class for Rich Reconstruction Algorithms providing some 
 *  basic functionality.
 *
 *  @author Chris Jones
 *  @date   05/04/2002
 */

class RichRecAlgBase : public Algorithm {

public:

  /// Standard constructor
  RichRecAlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator );

  virtual ~RichRecAlgBase( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  /// Get Rich Reconstruction data
  StatusCode richTracks();   ///< Update RichRecTracks pointer
  StatusCode trTracks();     ///< Update TrStoredTracks pointer
  StatusCode richPixels();   ///< Update RichRecPixels pointer
  StatusCode richSegments(); ///< Update RichRecSegments pointer
  StatusCode richPhotons();  ///< Update RichRecPhotons pointer

protected:

  /// Pointer to RichRecDataTool interface
  IRichRecPixelTool* m_richRecPixelTool;

  /// Pointer to RichRecPhotonTool interface
  IRichRecPhotonTool* m_richRecPhotonTool;

  /// Pointer to RichRecTrackTool interface
  IRichRecTrackTool* m_richRecTrackTool;

  /// Pointer to RichRecSegmentTool interface
  IRichRecSegmentTool* m_richRecSegmentTool;

  /// Pointer to RichDetInterface tool
  IRichDetInterface * m_richDetInterface;

  /// Pointer to RichRecTracks
  RichRecTracks * m_richTracks;

  /// Pointer to RichRecPixels
  RichRecPixels * m_richPixels;

  /// Pointer to RichRecSegments
  RichRecSegments * m_richSegments;

  /// Pointer to RichRecPhotons
  RichRecPhotons * m_richPhotons;

  /// Pointer to TrStoredTracks
  TrStoredTracks * m_trTracks;

  /// Location of TrStoredTracks in TES
  std::string m_trTracksLocation;

  /// Flag for code profiling using ChronoStatSvc
  bool m_timing;

  /// Pointer to ChronoStat Service
  IChronoStatSvc * m_chrono;

  /// Message service printout level
  int m_msgLevel;

};

#endif // RICHRECALGS_RICHRECALGBASE_H
