// $Id: RichRecPhotonTool.h,v 1.5 2002-12-20 09:33:08 cattanem Exp $
#ifndef RICHRECTOOLS_RICHRECPHOTONTOOL_H
#define RICHRECTOOLS_RICHRECPHOTONTOOL_H 1

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IChronoStatSvc.h"

// Rich Detector
#include "RichDetTools/IRichDetInterface.h"

// Interface
#include "RichRecInterfaces/IRichRecPhotonTool.h"

// Forward declarations
class IDataProviderSvc;

/** @class RichRecPhotonTool RichRecPhotonTool.h
 *
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecPhotons
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichRecPhotonTool : public AlgTool,
                          virtual public IRichRecPhotonTool,
                          virtual public IIncidentListener {

public:

  /// Standard constructor
  RichRecPhotonTool( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

  /// Destructor
  virtual ~RichRecPhotonTool(){}

  /// Initialize and finalize methods
  StatusCode initialize();
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  /// This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

  /// Return Pointer to RichRecPhotons
  RichRecPhotons * richPhotons();

  /// Form a Photon candidate from a Segment and a pixel.
  RichRecPhoton * reconstructPhoton( RichRecSegment * segment,
                                     RichRecPixel * pixel );

  /// Form all photon candidates for a given track and pixel
  SmartRefVector<RichRecPhoton> reconstructPhotons( RichRecTrack * track,
                                                    RichRecPixel * pixel );

  /// Form all photon candidates for a given track, with all possible pixels.
  SmartRefVector<RichRecPhoton>& reconstructPhotons( RichRecTrack * track );

  /// Form all photon candidates for a given pixel, with all possible tracks.
  SmartRefVector<RichRecPhoton>& reconstructPhotons( RichRecPixel * pixel );

  /// Form all photon candidates for a given segment, with all possible pixels.
  SmartRefVector<RichRecPhoton>& reconstructPhotons( RichRecSegment * segment );

  /// Method to perform the asssociation of all tracks and pixels
  void reconstructPhotons();

  /// Probability of observing a signal in associated pixel for given id
  double pixelSignalProb( RichRecPhoton * photon,
                          const Rich::ParticleIDType id );

  /// Photon resolution
  double photonResolution( RichRecPhoton * photon,
                           const Rich::ParticleIDType id );

  /// Returns square of distance seperating pixel and track hits on HPDs
  double trackPixelHitSep2( const RichRecSegment * segment,
                            const RichRecPixel * pixel );

private:

  /// Form a Photon candidate from a Segment and a pixel.
  RichRecPhoton * buildPhoton( RichRecSegment * segment,
                               RichRecPixel * pixel,
                               RichRecPhotonKey & key );

private:

  /// Pointer to RichRecTrackTool interface
  IRichRecTrackTool * m_richRecTrackTool;

  /// Pointer to RichRecPixelTool interface
  IRichRecPixelTool * m_richRecPixelTool;

  /// Pointer to RichRecSegmentTool interface
  IRichRecSegmentTool * m_richRecSegmentTool;

  /// Pointer to RichRecPhotons
  RichRecPhotons * m_photons;

  /// Is it possible to make a photon candidate using this segment and pixel.
  bool photonPossible( RichRecSegment * segment,
                       RichRecPixel * pixel );

  /// Signal Probability
  double signalProb( RichRecPhoton * photon,
                     const Rich::ParticleIDType id );

  /// Scatter Probability
  double scatterProb( RichRecPhoton * photon,
                      const Rich::ParticleIDType id );

private:

  /// Pointer to event data service
  IDataProviderSvc * m_evtDataSvc;

  /// Pointer to RichDetInterface tool
  IRichDetInterface * m_richDetInterface;

  /// Location of RichRecPhotons in TES
  std::string m_richRecPhotonLocation;

  /// Flag for code profiling using ChronoStatSvc
  bool m_timing;

  /// Pointer to ChronoStat Service
  IChronoStatSvc * m_chrono;

  /// photon done map
  std::map< int, bool > m_photonDone;

  // Physics parameters
  std::vector<double> m_minROI;  ///< Min hit radius of interest around track centres
  std::vector<double> m_maxROI;  ///< Max hit radius of interest around track centres
  std::vector<double> m_maxROI2; ///< Square of m_maxROI
  std::vector<double> m_minROI2; ///< Square of m_minROI

  /// Max Cherenkov theta angle
  std::vector<double> m_maxCKtheta;

  /// Max Cherenkov theta angle
  std::vector<double> m_minCKtheta;

  /// minimum cut value for pixel probability
  double m_minPixelProb;

  /// Temporary local value for Radii of curvature
  std::map< Rich::Detector, double > m_radiusCurv;

  /// Temporary local value pixel area
  double m_pixelArea;

  /// Variables for photon resolution
  std::map< Rich::RadiatorType, double > m_sigmaMin;
  std::map< Rich::RadiatorType, double > m_sigmaMax;
  double m_theerr[3][3];
  double m_thebin[2][3];

  /// Flag to say if all photons have been built or not
  bool m_allBuilt;

};

#endif // RICHRECTOOLS_RICHRECPHOTONTOOL_H
