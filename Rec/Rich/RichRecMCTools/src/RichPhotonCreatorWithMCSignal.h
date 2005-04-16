
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorWithMCSignal.h
 *
 *  Header file for RICH reconstruction tool : RichPhotonCreatorWithMCSignal
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorWithMCSignal.h,v 1.1 2005-04-16 10:30:00 jonrob Exp $
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
#include "RichRecBase/IRichRecMCTruthTool.h"

// RichKernel
#include "RichKernel/RichMap.h"

// Event model
#include "Event/MCRichOpticalPhoton.h"

//-----------------------------------------------------------------------------------------------
/** @class RichPhotonCreatorWithMCSignal RichPhotonCreatorWithMCSignal.h
 *
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecPhotons. MC cheating is performed
 *  by for true Cherenkov photon candidates referring to the associated
 *  MCRichOpticalPhoton to get the true Cherenkov angles etc. For fake photons
 *  the reconstructed quantities are used.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------------------------

class RichPhotonCreatorWithMCSignal : public RichRecToolBase,
                                      virtual public IRichPhotonCreator,
                                      virtual public IIncidentListener {

public: // methods for Gaudi framework

  /// Standard constructor
  RichPhotonCreatorWithMCSignal( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

  /// Destructor
  virtual ~RichPhotonCreatorWithMCSignal(){}

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
  IRichRecMCTruthTool * m_mcRecTool;  ///< MC Truth tool

  /// Pointer to RichRecPhotons
  mutable RichRecPhotons * m_photons;

  /// Location of RichRecPhotons in TES
  std::string m_richRecPhotonLocation;

  /// photon done map
  mutable RichMap<long int, bool> m_photonDone;

  IRichPhotonCreator * m_mcPhotCr;
  IRichPhotonCreator * m_recoPhotCr;

};

inline void RichPhotonCreatorWithMCSignal::InitNewEvent()
{
  // Initialise navigation data
  m_photonDone.clear();
  m_photons = 0;
}

#endif // RICHRECMCTOOLS_RICHPHOTONCREATORFROMMCRICHOPTICALPHOTONS_H
