
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorFromMCRichOpticalPhotons.h
 *
 *  Header file for RICH reconstruction tool : RichPhotonCreatorFromMCRichOpticalPhotons
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorFromMCRichOpticalPhotons.h,v 1.4 2005-04-15 16:32:30 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RICHPHOTONCREATORFROMMCRICHOPTICALPHOTONS_H
#define RICHRECMCTOOLS_RICHPHOTONCREATORFROMMCRICHOPTICALPHOTONS_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Interfaces
#include "RichRecBase/IRichPhotonCreator.h"
#include "RichRecBase/IRichPhotonSignal.h"
#include "RichRecBase/IRichRecMCTruthTool.h"

// RichKernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichStatDivFunctor.h"

// Event model
#include "Event/MCRichOpticalPhoton.h"

//-----------------------------------------------------------------------------------------------
/** @class RichPhotonCreatorFromMCRichOpticalPhotons RichPhotonCreatorFromMCRichOpticalPhotons.h
 *
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecPhotons. MC cheating is performed
 *  by only creating true cherenkov photons using MCRichOpticalPhoton
 *  information and also by using the MC geometrical information instead of
 *  the reconstructed values (angles etc.).
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------------------------

class RichPhotonCreatorFromMCRichOpticalPhotons : public RichRecToolBase,
                                                  virtual public IRichPhotonCreator,
                                                  virtual public IIncidentListener {

public: // methods for Gaudi framework

  /// Standard constructor
  RichPhotonCreatorFromMCRichOpticalPhotons( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent );

  /// Destructor
  virtual ~RichPhotonCreatorFromMCRichOpticalPhotons(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

  // Implement the handle method for the Incident service.
  void handle( const Incident& incident );

public: // Public interface methods

  // Return Pointer to RichRecPhotons
  RichRecPhotons * richPhotons() const;

  // Form a Photon candidate from a Segment and a pixel.
  RichRecPhoton * reconstructPhoton( RichRecSegment * segment,
                                     RichRecPixel * pixel ) const;

  // Form all photon candidates for a given track and pixel
  RichRecTrack::Photons reconstructPhotons( RichRecTrack * track,
                                            RichRecPixel * pixel ) const;

  // Form all photon candidates for a given track, with all possible pixels.
  const RichRecTrack::Photons & reconstructPhotons( RichRecTrack * track ) const;

  // Form all photon candidates for a given pixel, with all possible tracks.
  const RichRecPixel::Photons & reconstructPhotons( RichRecPixel * pixel ) const;

  // Form all photon candidates for a given segment, with all possible pixels.
  const RichRecSegment::Photons & reconstructPhotons( RichRecSegment * segment ) const;

  // Method to perform the reconstruction of all tracks and pixels
  void reconstructPhotons() const;

private: // private methods

  /// Form a Photon candidate from a Segment and a pixel.
  RichRecPhoton * buildPhoton( RichRecSegment * segment,
                               RichRecPixel * pixel,
                               const RichRecPhotonKey key ) const;

  /// Initialise for a new event
  void InitNewEvent();

private: // private data

  // Pointers to tool instances
  IRichPhotonSignal * m_photonSignal; ///< Pointer to RichPhotonSignal
  IRichRecMCTruthTool * m_mcRecTool;  ///< MC Truth tool

  /// Pointer to RichRecPhotons
  mutable RichRecPhotons * m_photons;

  /// Location of RichRecPhotons in TES
  std::string m_richRecPhotonLocation;

  /// photon done map
  mutable RichMap<long int, bool> m_photonDone;

  /// Max Cherenkov theta angle
  std::vector<double> m_maxCKtheta;

  /// Max Cherenkov theta angle
  std::vector<double> m_minCKtheta;

  /// minimum cut value for photon probability
  std::vector<double> m_minPhotonProb;

  /// Flag to turn on or off the book keeping features to save cpu time.
  bool m_bookKeep;

  // debug photon counting
  mutable std::vector<unsigned long int> m_photCount;
  mutable std::vector<unsigned long int> m_photCountLast;

  /// Number of events processed tally
  unsigned int m_Nevts;

};

inline void RichPhotonCreatorFromMCRichOpticalPhotons::InitNewEvent()
{
  if ( m_bookKeep ) m_photonDone.clear();
  m_photons = 0;
  m_photCountLast = m_photCount;
  ++m_Nevts;
}


#endif // RICHRECMCTOOLS_RICHPHOTONCREATORFROMMCRICHOPTICALPHOTONS_H
