// $Id: RichPhotonCreator.h,v 1.7 2003-11-25 14:06:40 jonrob Exp $
#ifndef RICHRECTOOLS_RICHPHOTONCREATOR_H
#define RICHRECTOOLS_RICHPHOTONCREATOR_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Interfaces
#include "RichRecBase/IRichPhotonCreator.h"
#include "RichRecBase/IRichTrackCreator.h"
#include "RichRecBase/IRichPixelCreator.h"
#include "RichRecBase/IRichPhotonPredictor.h"
#include "RichRecBase/IRichPhotonSignal.h"
#include "RichDetTools/IRichDetInterface.h"

// Rich Kernel
#include "RichKernel/MessageSvcStl.h"
//#include "RichKernel/BoostArray.h"

/** @class RichPhotonCreator RichPhotonCreator.h
 *
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecPhotons
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichPhotonCreator : public RichRecToolBase,
                          virtual public IRichPhotonCreator,
                          virtual public IIncidentListener {

public:

  /// Standard constructor
  RichPhotonCreator( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

  /// Destructor
  virtual ~RichPhotonCreator(){}

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  void handle( const Incident& incident );

  /// Return Pointer to RichRecPhotons
  RichRecPhotons *& richPhotons();

  /// Form a Photon candidate from a Segment and a pixel.
  RichRecPhoton * reconstructPhoton( RichRecSegment * segment,
                                     RichRecPixel * pixel );

  /// Form all photon candidates for a given track and pixel
  RichRecTrack::Photons reconstructPhotons( RichRecTrack * track,
                                             RichRecPixel * pixel );

  /// Form all photon candidates for a given track, with all possible pixels.
  RichRecTrack::Photons& reconstructPhotons( RichRecTrack * track );

  /// Form all photon candidates for a given pixel, with all possible tracks.
  RichRecPixel::Photons& reconstructPhotons( RichRecPixel * pixel );

  /// Form all photon candidates for a given segment, with all possible pixels.
  RichRecSegment::Photons & reconstructPhotons( RichRecSegment * segment );

  /// Method to perform the reconstruction of all tracks and pixels
  void reconstructPhotons();

private: // private methods

  /// Form a Photon candidate from a Segment and a pixel.
  RichRecPhoton * buildPhoton( RichRecSegment * segment,
                               RichRecPixel * pixel,
                               RichRecPhotonKey & key );

private: // private data

  // Pointers to tool instances
  IRichTrackCreator * m_trackCreator; ///< Pointer to RichTrackCreator
  IRichPixelCreator * m_pixelCreator; ///< Pointer to RichPixelCreator
  IRichPhotonPredictor * m_photonPredictor; ///< Pointer to RichPhotonPredictor
  IRichPhotonSignal * m_photonSignal; ///< Pointer to RichPhotonSignal
  IRichDetInterface * m_richDetInt;   ///< Pointer to RichDetInterface

  /// Pointer to RichRecPhotons
  RichRecPhotons * m_photons;

  /// Location of RichRecPhotons in TES
  std::string m_richRecPhotonLocation;

  /// photon done map
  std::map<int, bool> m_photonDone;

  /// Max Cherenkov theta angle
  std::vector<double> m_maxCKtheta;

  /// Max Cherenkov theta angle
  std::vector<double> m_minCKtheta;

  /// minimum cut value for photon probability
  std::vector<double> m_minPhotonProb;

};

#endif // RICHRECTOOLS_RICHPHOTONCREATOR_H
