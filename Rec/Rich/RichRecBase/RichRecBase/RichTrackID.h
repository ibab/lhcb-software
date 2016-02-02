
//-----------------------------------------------------------------------------
/** @file RichTrackID.h
 *
 * Header file for utility class : RichTrackID
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   08/07/2004
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHTRACKID_H
#define RICHRECBASE_RICHTRACKID_H 1

// Event
#include "Event/Track.h"
namespace LHCb
{
  class MCParticle;
  class MCRichTrack;
}

namespace Rich
{
  namespace Rec
  {

    /** @namespace Rich::Rec::Track
     *
     *  Namespace for track based qualities.
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   08/07/2004
     */
    namespace Track
    {

      /// Number of Track types
      static const unsigned int NTrTypes = 10;

      /** @enum Rich::Rec::Track::Type
       *
       *  Enumeration for the various different track algorithm types
       *  that are available.
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       clc     *  @date   08/07/2004
      */
      enum Type
        {
          Unknown     = -2, ///< Track algorithm type is not known
          Unusable    = -1, ///< Track type is known, but is not usable by the RICH
          Forward     =  0, ///< Track algorithm type is Forward
          Match       =  1, ///< Track algorithm type is Match
          Follow      =  2, ///< Track algorithm type is Follow
          Seed        =  3, ///< Track algorithm type is Seed
          VeloTT      =  4, ///< Track algorithm type is VeloTT
          KsTrack     =  5, ///< Track algorithm type is KsTrack
          Velo        =  6, ///< Track algorithm type is Velo
          Trigger     =  7, ///< Track algorithm type Trigger track
          MuonID      =  8, ///< Track produced by the Muon ID reconstruction
          MCRichTrack =  9  ///< MCRichTrack track
        };

      /** Access the enumerated type for given Track
       *
       * @param track Pointer to a Track object
       *
       * @return enumerated type information
       */
      Rich::Rec::Track::Type type( const LHCb::Track * track );

      /** Access the enumerated type for given MCRichTrack
       *
       * @param track Pointer to an MCRichTrack object
       *
       * @return enumerated type information
       */
      Rich::Rec::Track::Type type( const LHCb::MCRichTrack * track );

      /** Converts a string name into the associated type enumeration
       *
       *  @param name Track type as a string
       *
       *  @return Track enumeration type
       */
      Rich::Rec::Track::Type type( const std::string & name );

      /** Evaluate if a given track is potentially usable for the RICH reconstruction
       *
       *  @param type Track type enumeration
       *
       *  @return Boolean indicating the track usability
       *  @retval true  Track is of a type that is usable by the RICH reconstruction
       *  @retval false track type contains no RICH information
       */
      inline bool isUsable( const Rich::Rec::Track::Type type )
      {
        return ( type != Rich::Rec::Track::Unusable );
      }

      /** Evaluate if track is potentially usable for the RICH reconstruction
       *
       *  @param track Pointer to a Track object
       *
       *  @return Boolean indicating the track usability
       *  @retval true  Track is of a type that is usable by the RICH reconstruction
       *  @retval false track type contains no RICH information
       */
      inline bool isUsable( const LHCb::Track * track )
      {
        return ( track ? Rich::Rec::Track::isUsable(Rich::Rec::Track::type(track)) : false );
      }

    }

    /** @namespace Rich::Rec::TrackParent
     *
     *  Namespace for information on the track parent type
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   08/07/2004
     */
    namespace TrackParent
    {

      /** @enum Rich::Rec::TrackParent::Type
       *
       *  Enumeration for the various different pixel parent types
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   2002-07-12
       */
      enum Type
        {
          Unknown = -1,   ///< Parent type is unknown
          Track,          ///< Track derives from a reconstructed Track object
          MCParticle,     ///< Track derives from Monte Carlo MCParticle information
          MCRichTrack     ///< Track derives from Monte Carlo MCRichTrack information
        };

    }

    /** @class TrackID RichTrackID.h RichRecBase/RichTrackID.h
     *
     *  Utility class used as an indentification object for tracking objects used in
     *  the RICH reconstruction software; such as Track and MCParticle.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2003-09-23
     */
    class TrackID
    {

    public:

      /// Standard constructor
      TrackID() { }

      /// Default Destructor
      ~TrackID() = default;

      /// Default Copy Constructor
      TrackID( const TrackID& ) = default;

      /// Default Copy Operator
      TrackID& operator=( const TrackID& ) = default;

      /// Default Move Constructor
      TrackID( TrackID&& ) = default;

      /// Default Move Operator
      TrackID& operator=( TrackID&& ) = default;

    public:

      /** Constructor from a Track
       *
       *  @param track Pointer to a Track
       */
      explicit TrackID( const LHCb::Track * track )
        : m_tkType     ( Rich::Rec::Track::type(track)            ),
          m_parentType ( Rich::Rec::TrackParent::Track            ),
          m_unique     ( !track->checkFlag(LHCb::Track::Clone)    ) { }

      /** Constructor from an MCParticle
       */
      explicit TrackID( const LHCb::MCParticle * )
        : m_parentType ( Rich::Rec::TrackParent::MCParticle ) { }

      /** Constructor from an MCRichTrack
       */
      explicit TrackID( const LHCb::MCRichTrack * )
        : m_tkType     ( Rich::Rec::Track::MCRichTrack         ),
          m_parentType ( Rich::Rec::TrackParent::MCRichTrack   ) { }

      /** The track type
       *
       *  @return The track type enumeration
       */
      Rich::Rec::Track::Type trackType() const noexcept { return m_tkType; }

      /** Set the track type
       *
       * @param type The track type enumeration
       */
      void setTrackType( const Rich::Rec::Track::Type type ) noexcept { m_tkType = type; }

      /** The track parent type
       *
       *  @return The track parent type enumeration
       */
      Rich::Rec::TrackParent::Type parentType() const noexcept { return m_parentType; }

      /** Set the parent type
       *
       *  @param type The track parent type enumeration
       */
      void setParentType( const Rich::Rec::TrackParent::Type type ) noexcept { m_parentType = type; }

      /** Is this track unique
       *
       *  @return boolean indicatin if the track is flagged as unique or not
       *  @retval true  Track is unique
       *  @retval false Track is not unique, so possibly a clone
       */
      bool unique() const noexcept { return m_unique; }

      /** Set if this track is unique
       *
       *  @param unique Boolean indicating the track uniqueness
       */
      void setUnique( const bool unique ) noexcept { m_unique = unique; }

      /** Initialise from a Track
       *
       *  @param track Pointer to a Track from which to initialise
       */
      inline void initialiseFor( const LHCb::Track * track )
      {
        setParentType ( Rich::Rec::TrackParent::Track            );
        setTrackType  ( Rich::Rec::Track::type(track)            );
        setUnique     ( !track->checkFlag(LHCb::Track::Clone)    );
      }

      /** Initialise from a MCParticle
       */
      inline void initialiseFor( const LHCb::MCParticle * )
      {
        setParentType ( Rich::Rec::TrackParent::MCParticle );
        setTrackType  ( Rich::Rec::Track::Unknown          );
        setUnique     ( true                               );
      }

      /** Initialise from a MCRichTrack
       */
      inline void initialiseFor( const LHCb::MCRichTrack * )
      {
        setParentType ( Rich::Rec::TrackParent::MCRichTrack );
        setTrackType  ( Rich::Rec::Track::MCRichTrack       );
        setUnique     ( true                                );
      }

      /** Operator < For sorting
       *
       *  @param id ID object for comparision
       */
      inline bool operator < ( const TrackID & id ) const
      {
        return ( ( trackType()  < id.trackType()  ) &&
                 ( parentType() < id.parentType() ) &&
                 ( unique()   && !id.unique()     )   );
      }

    public:

      /// Implement textual ostream << method for Rich::Rec::TrackID
      friend inline std::ostream& operator << ( std::ostream& s,
                                                const TrackID& id )
      {
        s << "{ " << std::endl
          << " track type:\t" << id.trackType() << std::endl
          << " parent type:\t" << id.parentType() << std::endl
          << " unique:\t" << id.unique() <<  std::endl
          << " } " << std::endl;
        return s;
      }

    private: // data

      /// The track type
      Rich::Rec::Track::Type m_tkType { Rich::Rec::Track::Unknown };

      /// The parent type
      Rich::Rec::TrackParent::Type m_parentType { Rich::Rec::TrackParent::Unknown };

      /// Is this track unique ?
      bool m_unique { true };

    };

    /** Text conversion for Rich::Rec::Track::ParentType enumeration
     *
     *  @param parent Track parent type enumeration
     *
     *  @return Parent type as a string
     */
    std::string text( const Rich::Rec::TrackParent::Type parent ) ;

    /** Text conversion for Rich::Rec::Track::Type enumeration
     *
     *  @param track Track type enumeration
     *
     *  @return Track type as a string
     */
    std::string text( const Rich::Rec::Track::Type track );

    namespace Track
    {
      /// Implement textual ostream << method for Rich::Rec::Track::Type enumeration
      inline std::ostream& operator << ( std::ostream& s,
                                         const Rich::Rec::Track::Type& type )
      {
        return s << Rich::Rec::text( type );
      }
    }

    namespace TrackParent
    {
      /// Implement textual ostream << method for Rich::Rec::TrackParent::Type enumeration
      inline std::ostream& operator << ( std::ostream& s,
                                         const Rich::Rec::TrackParent::Type& track )
      {
        return s << Rich::Rec::text( track );
      }
    }

  }

  /// Backwards compatibility
  inline std::string text( const Rich::Rec::TrackParent::Type parent ) { return Rich::Rec::text(parent); }
  /// Backwards compatibility
  inline std::string text( const Rich::Rec::Track::Type track        ) { return Rich::Rec::text(track); }

}

#endif // RICHRECBASE_RICHTRACKID_H
