
//---------------------------------------------------------------------------------
/** @file RichPhotonCreatorBase.h
 *
 *  Header file for tool base class : RichPhotonCreatorBase
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorBase.h,v 1.1 2005-05-26 16:45:51 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/05/2005
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHPHOTONCREATORBASE_H
#define RICHRECBASE_RICHPHOTONCREATORBASE_H 1

// Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichPhotonCreator.h"
#include "RichRecBase/IRichPhotonPredictor.h"
#include "RichRecBase/IRichPhotonSignal.h"

// RichKernel
#include "RichKernel/RichStatDivFunctor.h"
#include "RichKernel/RichHashMap.h"

//---------------------------------------------------------------------------------------
/** @class RichPhotonCreatorBase RichPhotonCreatorBase.h RichRecBase/RichPhotonCreatorBase.h
 *
 *  Base class for all RichRecPhoton creator tools.
 *
 *  Implements common functionality needed by all concrete implementations.
 *  Derived classes need to implement the method buildPhoton which creates a photon
 *  candidate using whatever means appropriate for that implementation.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/05/2005
 */
//---------------------------------------------------------------------------------------

class RichPhotonCreatorBase : public RichRecToolBase,
                              virtual public IRichPhotonCreator,
                              virtual public IIncidentListener 
{

public: // methods for Gaudi

  /// Standard constructor
  RichPhotonCreatorBase( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// Destructor
  virtual ~RichPhotonCreatorBase( ) {};

  // Initialize method
  virtual StatusCode initialize();

  // Finalize method
  virtual StatusCode finalize();

  // Implement the handle method for the Incident service.
  virtual void handle( const Incident& incident );

public: // methods from interface

  // Method to perform the photon reconstruction of all tracks and pixels
  void reconstructPhotons() const;

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

protected: // methods

  /// Initialise for a new event
  void InitNewEvent();

  /// Finalise current event
  void FinishEvent();

  /** Should book-keeping be performed ?
   *
   *  @return boolean indicating if photon book-keeping should be performed
   *  @retval true  Book-keeping is to be done
   *  @retval false Book-keeping is to be skipped for increased speed
   */
  bool bookKeep() const;

  /** Max Cherenkov theta value to reconstuct for given radiator
   *
   *  @param rad The radiator type
   *
   *  @return The maximum Cherenkov angle to reconstruct for the given radiator
   */
  double maxCKTheta( const Rich::RadiatorType rad ) const;

  /** Min Cherenkov theta value to reconstuct for given radiator
   *
   *  @param rad The radiator type
   *
   *  @return The minimum Cherenkov angle to reconstruct for the given radiator
   */
  double minCKTheta( const Rich::RadiatorType rad ) const;

  /** Form a Photon candidate from a Segment and a pixel.
   *
   *  The one method that derived concrete photon creator classes
   *  must implement
   *
   *  @param segment Pointer to the RichRecSegment to use in the
   *                 photon reconstruction
   *  @param pixel   Pointer to the RichRecPixcel to use in the
   *                 photon reconstruction
   *  @param key     The key to use for the reconstruction photon
   *
   *  @return Pointer to the reconstructed photon candidate for
   *          the given segment and pixel
   *  @retval NULL Reconstruction was not possible
   *  @retval non-NULL Reconstruction was successful
   */
  virtual RichRecPhoton * buildPhoton( RichRecSegment * segment,
                                       RichRecPixel * pixel,
                                       const RichRecPhotonKey key ) const = 0;

  /** Save a given photon to the TES container
   *
   *  @param photon The photon candidate to save in the TES container
   *  @param key    The key to save the photon with
   */
  void savePhoton( RichRecPhoton * photon,
                   const RichRecPhotonKey key ) const;

  /** Performs simple tests to see if the given photon is worth keeping
   *
   *  @param photon The photon candidate to test
   *
   *  @return boolean indicating if the photon is to be kept
   *  @retval true  The photon candidate is a good one and is to be saved
   *  @retval false The photon candidate is to be rejected
   */
  bool keepPhoton( RichRecPhoton * photon ) const;

  /** Build cross references between the various reconstruction data
   *  objects for this photon
   *
   *  @param photon The photon candidate to build the references for
   */
  void buildCrossReferences( RichRecPhoton * photon ) const;

protected: // data

  /// Flag to indicate if the tool has been used in a given event
  mutable bool m_hasBeenCalled;

  /// photon done map
  mutable RichHashMap<long int, bool> m_photonDone;

private: // data

  /// Pointer to the RichPhotonPredictor tool
  IRichPhotonPredictor * m_photonPredictor;

  /// Pointer to the RichPhotonSignal tool
  IRichPhotonSignal * m_photonSignal;

  /// Number of events processed tally
  unsigned int m_Nevts;

  /// Max Cherenkov theta angle
  std::vector<double> m_maxCKtheta;

  /// Min Cherenkov theta angle
  std::vector<double> m_minCKtheta;

  /// minimum cut value for photon probability
  mutable std::vector<double> m_minPhotonProb;

  /// Flag to turn on or off the book keeping features to save cpu time.
  bool m_bookKeep;

  /// Pointer to RichRecPhotons
  mutable RichRecPhotons * m_photons;

  /// Location of RichRecPhotons in TES
  std::string m_richRecPhotonLocation;

  // debug photon counting
  mutable std::vector<unsigned long int> m_photCount;
  mutable std::vector<unsigned long int> m_photCountLast;

private: // methods

  /// Printout the photon creation statistics
  void printStats() const;

};

inline void RichPhotonCreatorBase::InitNewEvent()
{
  m_hasBeenCalled = false;
  if ( bookKeep() ) m_photonDone.clear();
  m_photons = 0;
  m_photCountLast = m_photCount;
}

inline void RichPhotonCreatorBase::FinishEvent()
{
  if ( m_hasBeenCalled ) ++m_Nevts;
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created " << richPhotons()->size() << " RichRecPhotons : Aerogel="
            << m_photCount[Rich::Aerogel]-m_photCountLast[Rich::Aerogel]
            << " C4F10=" << m_photCount[Rich::C4F10]-m_photCountLast[Rich::C4F10]
            << " CF4=" << m_photCount[Rich::CF4]-m_photCountLast[Rich::CF4] << endreq;
  }
}

inline double
RichPhotonCreatorBase::maxCKTheta( const Rich::RadiatorType rad ) const
{
  return m_maxCKtheta[rad];
}

inline double
RichPhotonCreatorBase::minCKTheta( const Rich::RadiatorType rad ) const
{
  return m_minCKtheta[rad];
}

inline bool RichPhotonCreatorBase::bookKeep() const
{
  return m_bookKeep;
}

inline void RichPhotonCreatorBase::savePhoton( RichRecPhoton * photon,
                                               const RichRecPhotonKey key ) const
{
  // save photon
  richPhotons()->insert( photon, key );
  // count
  ++m_photCount[ photon->richRecSegment()->trackSegment().radiator() ];
}

inline bool RichPhotonCreatorBase::keepPhoton( RichRecPhoton * photon ) const
{
  // check photon has significant probability to be signal for any
  // hypothesis. If not then reject
  bool keepPhoton = false;
  for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
    if ( m_photonSignal->predictedPixelSignal( photon,
                                               static_cast<Rich::ParticleIDType>(iHypo) )
         > m_minPhotonProb[ photon->richRecSegment()->trackSegment().radiator() ] )
    {
      keepPhoton = true;
      break;
    }
  }
  return keepPhoton;
}

inline void RichPhotonCreatorBase::buildCrossReferences( RichRecPhoton * photon ) const
{
  RichRecSegment * segment = photon->richRecSegment();
  RichRecPixel * pixel     = photon->richRecPixel();
  segment->addToRichRecPixels( pixel );
  segment->addToRichRecPhotons( photon );
  segment->richRecTrack()->addToRichRecPhotons( photon );
  pixel->addToRichRecPhotons( photon );
  pixel->addToRichRecTracks( segment->richRecTrack() );
  RichRecTrack::Pixels & tkPixs = segment->richRecTrack()->richRecPixels();
  bool notThere = true;
  for ( RichRecTrack::Pixels::iterator pix = tkPixs.begin();
        pix != tkPixs.end(); ++pix )
  {
    if ( (RichRecPixel*)(*pix) == pixel ) { notThere = false; break; }
  }
  if ( notThere )
  {
    segment->richRecTrack()->addToRichRecPixels( pixel );
  }
}

#endif // RICHRECBASE_RICHPHOTONCREATORBASE_H
