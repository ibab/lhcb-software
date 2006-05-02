
//-----------------------------------------------------------------------------
/** @file RichTrackID.h
 *
 * Header file for utility class : RichTrackID
 *
 * $Id: RichTrackID.h,v 1.22 2006-05-02 12:34:02 erodrigu Exp $
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

  /** @namespace Rich::Track
   *
   *  Namespace for track based qualities.
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace Track 
  {

    /// Number of Track types
    static const unsigned int NTrTypes = 9;

    /** @enum Rich::Track::Type
     *
     *  Enumeration for the various different track algorithm types
     *  that are available.
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   08/07/2004
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
        MCRichTrack =  8  ///< MCRichTrack track
      };

    /** Access the enumerated type for given Track
     *
     * @param track Pointer to a Track object
     *
     * @return enumerated type information
     */
    Rich::Track::Type type( const LHCb::Track * track );

    /** Access the enumerated type for given MCRichTrack
     *
     * @param track Pointer to an MCRichTrack object
     *
     * @return enumerated type information
     */
    Rich::Track::Type type( const LHCb::MCRichTrack * track );

    /** Converts a string name into the associated type enumeration
     *
     *  @param name Track type as a string
     *
     *  @return Track enumeration type
     */
    Rich::Track::Type type( const std::string & name );

    /** Evaluate if a given track is potentially usable for the RICH reconstruction
     *
     *  @param type Track type enumeration
     *
     *  @return Boolean indicating the track usability
     *  @retval true  Track is of a type that is usable by the RICH reconstruction
     *  @retval false track type contains no RICH information
     */
    inline bool isUsable( const Rich::Track::Type type )
    {
      return ( type != Rich::Track::Unusable );
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
      return ( track ? Rich::Track::isUsable(Rich::Track::type(track)) : false );
    }

  }

  /** @namespace Rich::TrackParent
   *
   *  Namespace for information on the track parent type
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace TrackParent {

    /** @enum Rich::TrackParent::Type
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

  /** Text conversion for Rich::Track::ParentType enumeration
   *
   *  @param parent Track parent type enumeration
   *
   *  @return Parent type as a string
   */
  std::string text( const Rich::TrackParent::Type parent ) ;

  /** Text conversion for Rich::Track::Type enumeration
   *
   *  @param track Track type enumeration
   *
   *  @return Track type as a string
   */
  std::string text( const Rich::Track::Type track );

}

/** @class RichTrackID RichTrackID.h RichRecBase/RichTrackID.h
 *
 *  Utility class used as an indentification object for tracking objects used in
 *  the RICH reconstruction software; such as Track and MCParticle.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-09-23
 */

class RichTrackID {

public:

  /// Standard constructor
  RichTrackID()
    : m_tkType     ( Rich::Track::Unknown             ),
      m_parentType ( Rich::TrackParent::Unknown       ),
      m_unique     ( true                             ) { }

  /** Constructor from a Track
   *
   *  @param track Pointer to a Track
   */
  explicit RichTrackID( const LHCb::Track * track )
    : m_tkType     ( Rich::Track::type(track)            ),
      m_parentType ( Rich::TrackParent::Track            ),
      m_unique     ( !track->checkFlag(LHCb::Track::Clone) ) { }

  /** Constructor from an MCParticle
   *
   *  @param mcPart Pointer to an MCParticle
   */
  explicit RichTrackID( const LHCb::MCParticle * )
    : m_tkType     ( Rich::Track::Unknown             ),
      m_parentType ( Rich::TrackParent::MCParticle    ),
      m_unique     ( true                             ) { }

  /** Constructor from an MCRichTrack
   *
   *  @param mcPart Pointer to an MCRichTrack
   */
  explicit RichTrackID( const LHCb::MCRichTrack * )
    : m_tkType     ( Rich::Track::MCRichTrack         ),
      m_parentType ( Rich::TrackParent::MCRichTrack   ),
      m_unique     ( true                             ) { }

  ~RichTrackID( ) {} ///< Destructor

  /** The track type
   *
   *  @return The track type enumeration
   */
  Rich::Track::Type trackType() const { return m_tkType; }

  /** Set the track type
   *
   * @param type The track type enumeration
   */
  void setTrackType( const Rich::Track::Type type ) { m_tkType = type; }

  /** The track parent type
   *
   *  @return The track parent type enumeration
   */
  Rich::TrackParent::Type parentType() const { return m_parentType; }

  /** Set the parent type
   *
   *  @param type The track parent type enumeration
   */
  void setParentType( const Rich::TrackParent::Type type ) { m_parentType = type; }

  /** Is this track unique
   *
   *  @return boolean indicatin if the track is flagged as unique or not
   *  @retval true  Track is unique
   *  @retval false Track is not unique, so possibly a clone
   */
  bool unique() const { return m_unique; }

  /** Set if this track is unique
   *
   *  @param unique Boolean indicating the track uniqueness
   */
  void setUnique( bool unique ) { m_unique = unique; }

  /** Initialise from a Track
   *
   *  @param track Pointer to a Track from which to initialise
   */
  void initialiseFor( const LHCb::Track * track );

  /** Initialise from a MCParticle
   *
   *  @param track Pointer to an MCParticle from which to initialise
   */
  void initialiseFor( const LHCb::MCParticle * track );

  /** Initialise from a MCRichTrack
   *
   *  @param track Pointer to an MCRichTrack from which to initialise
   */
  void initialiseFor( const LHCb::MCRichTrack * track );

  /** Operator < For sorting
   *
   *  @param id ID object for comparision
   */
  inline bool operator < ( const RichTrackID & id ) const
  {
    return ( ( trackType()  < id.trackType()  ) && 
             ( parentType() < id.parentType() ) &&
             ( unique()   && !id.unique()     )   );
  }

private: // data

  /// The track type
  Rich::Track::Type m_tkType;

  /// The parent type
  Rich::TrackParent::Type m_parentType;

  /// Is this track unique
  bool m_unique;

};

inline void RichTrackID::initialiseFor( const LHCb::Track * track )
{
  setParentType ( Rich::TrackParent::Track            );
  setTrackType  ( Rich::Track::type(track)            );
  setUnique     ( !track->checkFlag(LHCb::Track::Clone)   );
}

inline void RichTrackID::initialiseFor( const LHCb::MCParticle * )
{
  setParentType ( Rich::TrackParent::MCParticle );
  setTrackType  ( Rich::Track::Unknown          );
  setUnique     ( true                          );
}

inline void RichTrackID::initialiseFor( const LHCb::MCRichTrack * )
{
  setParentType ( Rich::TrackParent::MCRichTrack );
  setTrackType  ( Rich::Track::MCRichTrack       );
  setUnique     ( true                           );
}

/// Implement textual ostream << method for Rich::Track::Type enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::Track::Type type )
{
  s << Rich::text( type );
  return s;
}

/// Implement textual ostream << method for Rich::TrackParent::Type enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::TrackParent::Type track )
{
  s << Rich::text( track );
  return s;
}

/// Implement textual ostream << method for RichTrackID
inline std::ostream& operator << ( std::ostream& s,
                                   const RichTrackID& id )
{
  s << "{ " << std::endl
    << " track type:\t" << id.trackType() << std::endl
    << " parent type:\t" << id.parentType() << std::endl
    << " unique:\t" << id.unique() <<  std::endl
    << " } " << std::endl;
  return s;
}

/// Implement textual MsgStream << method for Rich::Track::Type enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::Track::Type type )
{
  s << Rich::text( type );
  return s;
}

/// Implement textual MsgStream << method for Rich::TrackParent::Typ enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::TrackParent::Type track )
{
  s << Rich::text( track );
  return s;
}

#endif // RICHRECBASE_RICHTRACKID_H
