
//---------------------------------------------------------------------------------
/** @file RichDetailedTrSegMakerFromRecoTracks.h
 *
 *  Header file for tool : Rich::Rec::DetailedTrSegMakerFromRecoTracks
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   14/01/2002
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECTRACKTOOLS_RichDetailedTrSegMakerFromRecoTracks_H
#define RICHRECTRACKTOOLS_RichDetailedTrSegMakerFromRecoTracks_H 1

// STL
#include <sstream>
#include <iostream>
#include <memory>

// Gaudi
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

// base class
#include "RichBaseTrSegMakerFromRecoTracks.h"

// RichKernel
#include "RichKernel/BoostArray.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichParticleProperties.h"
#include "RichKernel/IRichRadiatorTool.h"
#include "RichKernel/RichTrackSegment.h"

// Track Interfaces
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackStateProvider.h"

// Event
#include "TrackKernel/TrackTraj.h"

// histogramming numbers
//#include "RichRecBase/RichDetParams.h"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------------
    /** @class DetailedTrSegMakerFromRecoTracks RichDetailedTrSegMakerFromRecoTracks.h
     *
     *  Tool to create RichTrackSegments from Tracks.
     *
     *  Uses the tracking extrapolation tools to access the state information at the
     *  entrance and exit points to the radiators, which is then used to create the
     *  RichTrackSegments using a detailed approach.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   14/01/2002
     *
     *  @todo Look into optimising a little how the radiator intersections are handled.
     *        Can probably be a little faster.
     *  @todo Consider moving 'radiator fixups' into radiator intersection tool
     */
    //---------------------------------------------------------------------------------

    class DetailedTrSegMakerFromRecoTracks : public BaseTrSegMakerFromRecoTracks
    {

    public: // Methods for Gaudi Framework

      /// Standard Constructor
      DetailedTrSegMakerFromRecoTracks( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent );

      /// Standard Destructor
      virtual ~DetailedTrSegMakerFromRecoTracks( );

      // Initialization of the tool after creation
      virtual StatusCode initialize();

    public: // methods (and doxygen comments) inherited from interface

      // Create RichTrackSegments for a given tracking object
      int constructSegments( const ContainedObject* track,
                             std::vector<LHCb::RichTrackSegment*>& segments ) const;

    private: // methods

      /// Checks for 'strange' States
      void checkState( const LHCb::State * state,
                       const Rich::RadiatorType rad ) const;

      /** Find all intersections with the given radiator volume(s)
       *  @return The number of radiator intersections
       */
      unsigned int
      getRadIntersections( const Gaudi::XYZPoint&   point,     ///< The start point
                           const Gaudi::XYZVector&  direction, ///< The direction from the start point
                           const DeRichRadiator * rad,         ///< The radiator
                           Rich::RadIntersection::Vector & intersections ///< The intersections with the given radiator
                           ) const;

      /** Get just the first intersection with the radiator volume
       *  @return boolean indicating if an intersection was found or not
       */
      bool getNextInterPoint( const Gaudi::XYZPoint&   point,     ///< The start point
                              const Gaudi::XYZVector&  direction, ///< The direction from the start point
                              const DeRichRadiator * rad,         ///< The radiator
                              Gaudi::XYZPoint& interP             ///< The first intersection point
                              ) const;

      /** Correct the entrance point for the Rich1Gas radiators due to the fact the aerogel
       *  is contained inside this medium. This means the start of the visable Rich1Gas
       *  segment is the aerogel exit point, and not the Rich1Gas entrance point.
       *
       *  @param track        The Track object
       *  @param state        State information to correct
       *  @param refState     Reference starting state.
       */
      void fixRich1GasEntryPoint( const LHCb::Track & track,
                                  LHCb::State & state,
                                  const LHCb::State * refState = 0 ) const;

      /** Correct the exit state to the point where the track traverses the spherical mirror
       *
       *  @param radiator     Pointer to the apropriate radiator detector element
       *  @param track        The Track object
       *  @param state        State information to correct
       *  @param refState     Reference starting state.
       */
      void correctRadExitMirror( const DeRichRadiator* radiator,
                                 const LHCb::Track & track,
                                 LHCb::State & state,
                                 const LHCb::State * refState = 0  ) const;

      /** Extrapolate a state to a new z position
       *
       * @param stateToMove  The state to extrapolate
       * @param track        The Track object
       * @param z            The z position to extrapolate the state to
       * @param refState     Reference starting state.
       *
       * @return The status of the extrapolation
       * @retval true  State was successfully extrapolated to the new z position
       * @retval false State could not be extrapolated to the z position.
       *         State remains unaltered.
       */
      bool moveState( LHCb::State & stateToMove,
                      const LHCb::Track& track,
                      const double z,
                      const LHCb::State * refState = 0 ) const;

      /** Check if a track type should be skipped in a given radiator, 
       *  when using the State Provider tool */
      bool skipByType( const LHCb::Track * track, const Rich::RadiatorType rad ) const
      {
        TrackTypesRads::const_iterator iT = m_radsToSkip.find(track->type());
        return ( iT == m_radsToSkip.end() ? false :
                 std::find( iT->second.begin(), iT->second.end(), rad ) != iT->second.end() );
      }

      /// Creates the middle point information
      bool createMiddleInfo( const LHCb::Track & track,
                             const Rich::RadiatorType rad,
                             LHCb::State & fState,
                             const LHCb::State * fStateRef,
                             LHCb::State & lState,
                             const LHCb::State * lStateRef,
                             Gaudi::XYZPoint & midPoint,
                             Gaudi::XYZVector & midMomentum,
                             LHCb::RichTrackSegment::StateErrors & errors ) const;    

      /// Access primary track extrapolator tool
      inline ITrackExtrapolator * primaryExtrapolator() const
      {
        if ( !m_trExt1 ) { m_trExt1 = tool<ITrackExtrapolator>( m_trExt1Name ); }
        return m_trExt1;
      }

      /// Access on-demand backup track extrapolator tool
      inline ITrackExtrapolator * backupExtrapolator() const
      {
        if ( !m_trExt2 ) { m_trExt2 = tool<ITrackExtrapolator>( m_trExt2Name ); }
        return m_trExt2;
      }

      /// Access track state provider tool on-demand
      inline ITrackStateProvider * stateProvider() const
      {
        if ( !m_trStateP ) 
        { 
          // propagate the extrapolator type to our privately owned TrackStateProvider
          const StringProperty  exProp ("Extrapolator",m_trExt1Name);
          //const BooleanProperty matProp("ApplyMaterialCorrections",false);
          if ( ! joSvc()->addPropertyToCatalogue(name()+".StateProvider",exProp) )
          { Exception( "Failed to propagate options to StateProvider" ); }
          // load the tool
          m_trStateP = tool<ITrackStateProvider>( "TrackStateProvider", "StateProvider", this ); 
        }
        return m_trStateP;
      }

      /// Access on demand the magnetic field service
      const IMagneticFieldSvc * magFieldSvc() const
      {
        if ( !m_magFieldSvc ) { m_magFieldSvc = svc<IMagneticFieldSvc>("MagneticFieldSvc"); }
        return m_magFieldSvc;
      }
      
    private: // data

      /// Ray tracing tool
      const IRayTracing* m_rayTracing = nullptr;

      /// Pointer to RichParticleProperties interface
      const IParticleProperties * m_richPartProp = nullptr;

      /// Pointer to the radiator intersections tool
      const IRadiatorTool * m_radTool = nullptr;

      /// Track state provider
      mutable ITrackStateProvider * m_trStateP = nullptr;

      /// Rich1 and Rich2 detector elements
      DeRich* m_rich[Rich::NRiches];

      /// Type for pointers to RICH radiator detector elements
      typedef std::vector<const DeRichRadiator*> Radiators;
      /// Pointers to RICH radiator detector elements
      Radiators m_radiators;

      /// Allowable tolerance on state z positions
      std::vector<double> m_zTolerance;

      /// Nominal z positions of states at RICHes
      std::vector<double> m_nomZstates;

      /// shifts for mirror correction
      std::vector<double> m_mirrShift;

      /// sanity checks on state information
      std::vector<double> m_minStateDiff;

      // Track extrapolators
      mutable ITrackExtrapolator * m_trExt1 = nullptr; ///< Primary track extrapolation tool
      mutable ITrackExtrapolator * m_trExt2 = nullptr; ///< Secondary (backup) track extrapolation tool
      std::string m_trExt1Name; ///< Primary track extrapolation tool name
      std::string m_trExt2Name; ///< Secondary track extrapolation tool name

      /// Flag to indicate if extrapolation should always be done from the reference states
      bool m_extrapFromRef;

      /// Minimum state movement in z to bother with
      double m_minZmove;

      /// Job option to define type of track segments to create
      std::string m_trSegTypeJO;

      /// Type of track segments to create
      LHCb::RichTrackSegment::SegmentType m_trSegType;

      /// Min radius at exit for each radiator (temp hack)
      std::vector<double> m_minRadLength;

      /// Locally cached states for the TrackStateProvider
      mutable LHCb::State m_states[2];

      /// Flag to turn on/off the use of the TrackStateProvider to create missing states
      bool m_createMissingStates;

      /// Flag to use TrackTraj to move states, instead of the extrapolator
      bool m_useTrackTraj;

      /// Use the State provider instead of the extrapolator to move states
      bool m_useStateProvider;

      /// Flag to turn on the final State sanity checks
      bool m_checkStates;

      /// Radiators to skip, by track type
      typedef std::map< LHCb::Track::Types, Rich::Radiators > TrackTypesRads;
      TrackTypesRads m_radsToSkip;

      /// Pointer to the Magnetic Field Service
      mutable IMagneticFieldSvc * m_magFieldSvc = nullptr;

      /// Cached pointer to a TrackTraj object
      mutable LHCb::TrackTraj * m_trackTraj = nullptr;

    };

  }
}

#endif // RICHRECTRACKTOOLS_RichDetailedTrSegMakerFromRecoTracks_H
