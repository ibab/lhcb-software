
//-----------------------------------------------------------------------------
/** @file RichPhotonCreator.h
 *
 *  Header file for tool : RichPhotonCreator
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreator.h,v 1.19 2005-04-06 20:23:17 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPHOTONCREATOR_H
#define RICHRECTOOLS_RICHPHOTONCREATOR_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// RichKernel
#include "RichKernel/RichHashMap.h"
#include "RichKernel/RichStatDivFunctor.h"

// Interfaces
#include "RichRecBase/IRichPhotonCreator.h"
#include "RichRecBase/IRichPhotonPredictor.h"
#include "RichRecBase/IRichPhotonSignal.h"
#include "RichKernel/IRichPhotonReconstruction.h"

//-----------------------------------------------------------------------------
/** @class RichPhotonCreator RichPhotonCreator.h
 *
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecPhotons
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichPhotonCreator : public RichRecToolBase,
                          virtual public IRichPhotonCreator,
                          virtual public IIncidentListener {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichPhotonCreator( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

  /// Destructor
  virtual ~RichPhotonCreator(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

  // Implement the handle method for the Incident service.
  void handle( const Incident& incident );

public: // methods (and doxygen comments) inherited from public interface

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
  void InitEvent();

  /// Finalise for each event
  void FinishEvent();

private: // private data

  // Pointers to tool instances
  IRichPhotonPredictor * m_photonPredictor; ///< Pointer to RichPhotonPredictor
  IRichPhotonSignal * m_photonSignal; ///< Pointer to RichPhotonSignal
  IRichPhotonReconstruction * m_photonReco; ///< Pointer to photon reconstruction tool

  /// Pointer to RichRecPhotons
  mutable RichRecPhotons * m_photons;

  /// Location of RichRecPhotons in TES
  std::string m_richRecPhotonLocation;

  /// photon done map
  mutable RichHashMap<long int, bool> m_photonDone;

  /// Max Cherenkov theta angle
  std::vector<double> m_maxCKtheta;

  /// Min Cherenkov theta angle
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

  /// Flag to indicate if the tool has been used in a given event
  mutable bool m_hasBeenCalled;

};

inline void RichPhotonCreator::InitEvent()
{
  if ( m_bookKeep ) m_photonDone.clear();
  m_photons = 0;
  m_photCountLast = m_photCount;
  ++m_Nevts;
  m_hasBeenCalled = false;
}

inline void RichPhotonCreator::FinishEvent()
{
  if ( m_hasBeenCalled ) ++m_Nevts;
}

#endif // RICHRECTOOLS_RICHPHOTONCREATOR_H
