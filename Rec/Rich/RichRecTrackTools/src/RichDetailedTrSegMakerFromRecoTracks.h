
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

// base class
#include "RichBaseTrSegMakerFromRecoTracks.h"

// RichKernel
#include "RichKernel/BoostArray.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichParticleProperties.h"
#include "RichKernel/IRichRadiatorTool.h"
#include "RichKernel/RichTrackSegment.h"

// Track Extrapolator
#include "TrackInterfaces/ITrackExtrapolator.h"

// histogramming numbers
//#include "RichRecBase/RichDetParams.h"

// boost
#include "boost/assign/list_of.hpp"

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
                       const Rich::RadiatorType rad,
                       const std::string& desc ) const;

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
       *  @param state        State information to correct
       *  @param refState     Reference starting state.
       */
      void fixRich1GasEntryPoint( LHCb::State *& state,
                                  const LHCb::State * refState = 0 ) const;

      /** Correct the exit state to the point where the track traverses the spherical mirror
       *
       *  @param radiator     Pointer to the apropriate radiator detector element
       *  @param state        State information to correct
       *  @param refState     Reference starting state.
       */
      void correctRadExitMirror( const DeRichRadiator* radiator,
                                 LHCb::State *& state,
                                 const LHCb::State * refState = 0  ) const;

      /** Extrapolate a state to a new z position
       *
       * @param stateToMove  The state to extrapolate
       * @param z            The z position to extrapolate the state to
       * @param refState     Reference starting state.
       *
       * @return The status of the extrapolation
       * @retval true  State was successfully extrapolated to the new z position
       * @retval false State could not be extrapolated to the z position.
       *         State remains unaltered.
       */
      bool moveState( LHCb::State *& stateToMove,
                      const double z,
                      const LHCb::State * refState = 0 ) const;

      /// Access primary track extrapolator tool
      inline ITrackExtrapolator * primaryExtrapolator() const
      {
        return m_trExt1;
      }

      /// Access on-demand backup track extrapolator tool
      inline ITrackExtrapolator * backupExtrapolator() const
      {
        if ( !m_trExt2 ) { m_trExt2 = tool<ITrackExtrapolator>( m_trExt2Name ); }
        return m_trExt2;
      }

      /// Creates the middle point information
      bool createMiddleInfo( const Rich::RadiatorType rad,
                             LHCb::State *& fState,
                             const LHCb::State * fStateRef,
                             LHCb::State *& lState,
                             const LHCb::State * lStateRef,
                             Gaudi::XYZPoint & midPoint,
                             Gaudi::XYZVector & midMomentum,
                             LHCb::RichTrackSegment::StateErrors & errors ) const;      
    private: // data

      /// Ray tracing tool
      const IRayTracing* m_rayTracing;

      /// Pointer to RichParticleProperties interface
      const IParticleProperties * m_richPartProp;

      /// Pointer to the radiator intersections tool
      const IRadiatorTool * m_radTool;

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
      ITrackExtrapolator * m_trExt1; ///< Primary track extrapolation tool
      mutable ITrackExtrapolator * m_trExt2; ///< Secondary (backup if primary fails) track extrapolation tool
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

    };

  }
}

#endif // RICHRECTRACKTOOLS_RichDetailedTrSegMakerFromRecoTracks_H
