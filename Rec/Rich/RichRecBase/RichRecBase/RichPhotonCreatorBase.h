
//---------------------------------------------------------------------------------
/** @file RichPhotonCreatorBase.h
 *
 *  Header file for tool base class : Rich::Rec::PhotonCreatorBase
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/05/2005
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHPHOTONCREATORBASE_H
#define RICHRECBASE_RICHPHOTONCREATORBASE_H 1

// STD
#include <set>

// Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichPhotonCreator.h"
#include "RichRecBase/IRichPhotonPredictor.h"
#include "RichRecBase/IRichPhotonSignal.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichKernel/IRichParticleProperties.h"

// RichKernel
#include "RichKernel/RichStatDivFunctor.h"
#include "RichKernel/RichHashMap.h"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------------------
    /** @class PhotonCreatorBase RichPhotonCreatorBase.h RichRecBase/RichPhotonCreatorBase.h
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

    class PhotonCreatorBase : public Rich::Rec::ToolBase,
                              virtual public IPhotonCreator,
                              virtual public IIncidentListener
    {

    public: // methods for Gaudi

      /// Standard constructor
      PhotonCreatorBase( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

      /// Destructor
      virtual ~PhotonCreatorBase( ) {}

      // Initialize method
      virtual StatusCode initialize();

      // Finalize method
      virtual StatusCode finalize();

      // Implement the handle method for the Incident service.
      virtual void handle( const Incident& incident );

    public: // methods from interface

      // Method to perform the photon reconstruction of all tracks and pixels
      StatusCode reconstructPhotons() const;

      // Return Pointer to RichRecPhotons
      LHCb::RichRecPhotons * richPhotons() const;

      // Checks whether a photon candidate exists for the given segment and pixel pair
      LHCb::RichRecPhoton * checkForExistingPhoton( LHCb::RichRecSegment * segment,
                                                    LHCb::RichRecPixel * pixel ) const;

      // Form a Photon candidate from a Segment and a pixel.
      LHCb::RichRecPhoton * reconstructPhoton( LHCb::RichRecSegment * segment,
                                               LHCb::RichRecPixel * pixel ) const;

      // Form all photon candidates for a given track and pixel
      LHCb::RichRecTrack::Photons reconstructPhotons( LHCb::RichRecTrack * track,
                                                      LHCb::RichRecPixel * pixel ) const;

      // Form all photon candidates for a given track, with all possible pixels.
      const LHCb::RichRecTrack::Photons & reconstructPhotons( LHCb::RichRecTrack * track ) const;

      // Form all photon candidates for a given pixel, with all possible tracks.
      const LHCb::RichRecPixel::Photons & reconstructPhotons( LHCb::RichRecPixel * pixel ) const;

      // Form all photon candidates for a given segment, with all possible pixels.
      const LHCb::RichRecSegment::Photons & reconstructPhotons( LHCb::RichRecSegment * segment ) const;

    protected: // methods

      /// Initialise for a new event
      virtual void InitNewEvent();

      /// Finalise current event
      virtual void FinishEvent();

      /** Should book-keeping be performed ?
       *
       *  @return boolean indicating if photon book-keeping should be performed
       *  @retval true  Book-keeping is to be done
       *  @retval false Book-keeping is to be skipped for increased speed
       */
      bool bookKeep() const;

      /*  Computes the cherenkov range to look in for a given track segment
       *
       *  @param segment The track segment
       *  @param id      The mass hypothesis
       *
       *  @return The Cherenkov angle range to search for photons in
       */
      double ckSearchRange( LHCb::RichRecSegment * segment,
                            const Rich::ParticleIDType id ) const;

      /*  Computes the cherenkov range to look in for a given track segment
       *
       *  @param segment The track segment
       *  @param tkSeg   The RICH geometrical track segment (passed as well for speed).
       *  @param id      The mass hypothesis
       *
       *  @return The Cherenkov angle range to search for photons in
       */
      double ckSearchRange( LHCb::RichRecSegment * segment,
                            const LHCb::RichTrackSegment & tkSeg,
                            const Rich::ParticleIDType id ) const;

      /** Absolute maximum Cherenkov theta value to reconstuct for given track segment
       *
       *  @param segment The track segment
       *
       *  @return The maximum Cherenkov angle to reconstruct
       */
      double absMaxCKTheta( LHCb::RichRecSegment * segment ) const;

      /** Absolute minimum Cherenkov theta value to reconstuct for given track segment
       *
       *  @param segment The track segment
       *
       *  @return The minimum Cherenkov angle to reconstruct
       */
      double absMinCKTheta( LHCb::RichRecSegment * segment ) const;

      /** Absolute maximum Cherenkov theta value to reconstuct for given track segment
       *
       *  @param segment The track segment
       *
       *  @return The maximum Cherenkov angle to reconstruct
       */
      double absMaxCKTheta( const LHCb::RichTrackSegment & segment ) const;

      /** Absolute minimum Cherenkov theta value to reconstuct for given track segment
       *
       *  @param segment The track segment
       *
       *  @return The minimum Cherenkov angle to reconstruct
       */
      double absMinCKTheta( const LHCb::RichTrackSegment & segment ) const;

      /** Maximum Cherenkov theta value to reconstuct for
       *  given track segment and mass hypothesis
       *
       *  @param segment The track segment
       *  @param id      The mass hypothesis
       *
       *  @return The maximum Cherenkov angle to reconstruct
       */
      double maxCKTheta( LHCb::RichRecSegment * segment,
                         const Rich::ParticleIDType id = Rich::Electron ) const;

      /** Minimum Cherenkov theta value to reconstuct for
       *  given track segment and mass hypothesis
       *
       *  @param segment The track segment
       *  @param id      The mass hypothesis
       *
       *  @return The minimum Cherenkov angle to reconstruct
       */
      double minCKTheta( LHCb::RichRecSegment * segment,
                         const Rich::ParticleIDType id = Rich::Proton ) const;

      /** Check if the given Cherenkov theta is within tolerences for any mass
       *  hypothesis for given track segment
       *
       *  @param segment The track segment
       *  @param ckTheta The cherenkov angle to test
       *
       *  @return boolean indicating if angle is within tolerence
       *  @retval true  Angle is within tolerence for at least one mass hypothesis
       *  @retval false Angle is outside tolerence for all mass hypotheses
       */
      bool checkAngleInRange( LHCb::RichRecSegment * segment,
                              const double ckTheta ) const;

      /** Form a Photon candidate from a Segment and a pixel.
       *
       *  The one method that derived concrete photon creator classes
       *  must implement
       *
       *  @param segment Pointer to the RichRecSegment to use in the
       *                 photon reconstruction
       *  @param pixel   Pointer to the RichRecPixel to use in the
       *                 photon reconstruction
       *  @param key     The key to use for the reconstruction photon
       *
       *  @return Pointer to the reconstructed photon candidate for
       *          the given segment and pixel
       *  @retval NULL Reconstruction was not possible
       *  @retval non-NULL Reconstruction was successful
       */
      virtual LHCb::RichRecPhoton * buildPhoton( LHCb::RichRecSegment * segment,
                                                 LHCb::RichRecPixel * pixel,
                                                 const Rich::Rec::PhotonKey key ) const = 0;

      /** Save a given photon to the TES container
       *
       *  @param photon The photon candidate to save in the TES container
       *  @param key    The key to save the photon with
       */
      void savePhoton( LHCb::RichRecPhoton * photon,
                       const Rich::Rec::PhotonKey key ) const;

      /** Checks the photon signal probability is above a threshold value for
       *  any mass hypothesis
       *
       *  @param photon The photon candidate to test
       *
       *  @return boolean indicating if the photon is to be kept
       *  @retval true  The photon candidate is a good one and is to be saved
       *  @retval false The photon candidate is to be rejected
       */
      bool checkPhotonProb( LHCb::RichRecPhoton * photon ) const;

      /** Build cross references between the various reconstruction data
       *  objects for this photon
       *
       *  @param photon The photon candidate to build the references for
       */
      void buildCrossReferences( LHCb::RichRecPhoton * photon ) const;

      /// Returns the number of processed events
      inline unsigned int nEvents() const { return m_Nevts; }

      /// Delete all photon cross references
      void deleteAllCrossReferences() const;

      /// Access the list of job options specific to the photon creator settings
      const std::vector<std::string> & photonCreatorJobOptions() const;

    private: // methods

      /// Printout the photon creation statistics
      void printStats() const;

    protected: // data

      /// Flag to indicate if the tool has been used in a given event
      mutable bool m_hasBeenCalled = false;

      /// photon done map
      mutable Rich::HashMap<Rich::Rec::PhotonKey::KeyType,bool> m_photonDone;

    private: // data

      /// Pointer to the RichPhotonPredictor tool
      const IPhotonPredictor * m_photonPredictor = nullptr;

      /// Pointer to the RichPhotonSignal tool
      const IPhotonSignal * m_photonSignal = nullptr;

      /// Pointer to Cherenkov angle tool
      const ICherenkovAngle * m_ckAngle = nullptr;

      /// Pointer to Cherenkov angle resolution tool
      const ICherenkovResolution * m_ckRes = nullptr;

      /// Pointer to RichParticleProperties interface
      const IParticleProperties * m_richPartProp = nullptr;

      /// Number of events processed tally
      unsigned int m_Nevts = 0;

      /// N sigma for acceptance bands
      std::vector<double> m_nSigma;   

      /// Absolute maximum allowed Cherenkov Angle
      std::vector<double> m_maxCKtheta;

      /// Absolute minimum allowed Cherenkov Angle
      std::vector<double> m_minCKtheta;

      /// minimum cut value for photon probability
      std::vector<double> m_minPhotonProb;

      /// Max Photon Detector occupancy for photon reconstruction (per radiator)
      std::vector<double> m_maxHPDOccForReco;

      /// Flag to turn on or off the book keeping features to save cpu time.
      bool m_bookKeep = false;

      /// Flag to turn on or off the rejection of aerogel photons based on gas photons
      bool m_rejAeroPhotsIfGas = false;

      /// Pointer to RichRecPhotons
      mutable LHCb::RichRecPhotons * m_photons = nullptr;

      /// Location of RichRecPhotons in TES
      std::string m_richRecPhotonLocation;

      /// Runtime name for photon predictor tool
      std::string m_photPredName;

      // debug photon counting
      mutable std::vector<unsigned int> m_photCount;
      mutable std::vector<unsigned int> m_photCountLast;

      /// Particle ID types to consider in the photon creation checks
      Rich::Particles m_pidTypes;

    protected: // data

      /// Maximum number of photon candidates
      unsigned int m_maxPhotons = 0;

    };

    inline double
    PhotonCreatorBase::ckSearchRange( LHCb::RichRecSegment * segment,
                                      const LHCb::RichTrackSegment & tkSeg,
                                      const Rich::ParticleIDType id ) const
    {
      // # sigma * resolution
      return m_nSigma[tkSeg.radiator()] * m_ckRes->ckThetaResolution(segment,id);
    }

    inline double
    PhotonCreatorBase::ckSearchRange( LHCb::RichRecSegment * segment,
                                      const Rich::ParticleIDType id ) const
    {
      return ckSearchRange( segment, segment->trackSegment(), id );
    }
    
    inline double
    PhotonCreatorBase::absMaxCKTheta( LHCb::RichRecSegment * segment ) const
    {
      return absMaxCKTheta(segment->trackSegment());
    }

    inline double
    PhotonCreatorBase::absMaxCKTheta( const LHCb::RichTrackSegment & segment ) const
    {
      return m_maxCKtheta[segment.radiator()];
    }

    inline double
    PhotonCreatorBase::maxCKTheta( LHCb::RichRecSegment * segment,
                                   const Rich::ParticleIDType id ) const
    {
      return ( m_ckAngle->avgCherenkovTheta(segment,id) + ckSearchRange(segment,id) );
    }

    inline double
    PhotonCreatorBase::absMinCKTheta( LHCb::RichRecSegment * segment ) const
    {
      return absMinCKTheta(segment->trackSegment());
    }

    inline double
    PhotonCreatorBase::absMinCKTheta( const LHCb::RichTrackSegment & segment ) const
    {
      return m_minCKtheta[segment.radiator()];
    }

    inline double
    PhotonCreatorBase::minCKTheta( LHCb::RichRecSegment * segment,
                                   const Rich::ParticleIDType id ) const
    {
      return ( m_ckAngle->avgCherenkovTheta(segment,id) - ckSearchRange(segment,id) );
    }

    inline bool PhotonCreatorBase::bookKeep() const
    {
      return m_bookKeep;
    }

    inline void PhotonCreatorBase::savePhoton( LHCb::RichRecPhoton    * photon,
                                               const Rich::Rec::PhotonKey key ) const
    {
      // save photon
      richPhotons()->insert( photon, key );
      // count
      ++m_photCount[ photon->richRecSegment()->trackSegment().radiator() ];
    }

  }
} // RICH

#endif // RICHRECBASE_RICHPHOTONCREATORBASE_H
