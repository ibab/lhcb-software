
//-----------------------------------------------------------------------------
/** @file RichTrackID.h
 *
 * Header file for utility class : RichTrackID
 *
 * CVS Log :-
 * $Id: RichTrackID.h,v 1.11 2005-01-13 14:20:24 jonrob Exp $
 * $Log: not supported by cvs2svn $
 * Revision 1.10  2004/10/27 14:34:16  jonrob
 * Various updates
 *
 * Revision 1.9  2004/10/13 09:29:43  jonrob
 * Improvements to RichTrackID and RichTrackSelector
 *
 * Revision 1.8  2004/07/26 18:00:58  jonrob
 * Various improvements to the doxygen comments
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   08/07/2004
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHTRACKID_H
#define RICHRECBASE_RICHTRACKID_H 1

// Event
#include "Event/TrStoredTrack.h"
#include "Event/TrgTrack.h"
#include "Event/MCParticle.h"

/** @namespace Rich
 *
 *  General namespace for RICH specific definitions
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
namespace Rich {

  /** @namespace Rich::Track
   *
   *  Namespace for track based qualities.
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace Track {

    /// Number of Track types
    static const unsigned int NTrTypes = 8;

    /** @enum Rich::Track::Type
     *
     *  Enumeration for the various different track algorithm types
     *  that are avaialble.
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   08/07/2004
     */
    enum Type
      {
        Unknown  = -2, ///< Track algorithm type is not known
        Unusable = -1, ///< Track type is known, but is not usable by the RICH
        Forward  =  0, ///< Track algorithm type is Forward
        Match    =  1, ///< Track algorithm type is Match
        Follow   =  2, ///< Track algorithm type is Follow
        Seed     =  3, ///< Track algorithm type is Seed
        VeloTT   =  4, ///< Track algorithm type is VeloTT
        KsTrack  =  5, ///< Track algorithm type is KsTrack
        Velo     =  6, ///< Track algorithm type is Velo
        Trigger  =  7  ///< Track algorithm type Trigger track
      };

    /** Access the enumerated type for given TrgTrack
     *
     * @param track Pointer to a TrgTrack object
     *
     * @return enumerated type information
     */
    Rich::Track::Type type( const TrgTrack * track );

    /** Access the enumerated type for given TrStoredTrack
     *
     * @param track Pointer to a TrStoredTrack object
     *
     * @return enumerated type information
     */
    Rich::Track::Type type( const TrStoredTrack * track );

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

    /** Evaluate if track is potentially usable for the RICH
     *
     *  @param track Pointer to a TrStoredTrack object
     *
     *  @return Boolean indicating the track usability
     *  @retval true  Track is of a type that is usable by the RICH reconstruction
     *  @retval false track type contains no RICH information
     */
    inline bool isUsable( const TrStoredTrack * track )
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
        TrStoredTrack,  ///< Track derives from a reconstructed TrStoredTrack object
        TrgTrack,       ///< Track derives from a reconstructed TrgTrack object
        MCParticle      ///< Track derives from Monte Carlo MCParticle information
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

  /** Text conversion for TrStoredTracks
   *
   *  @param track Pointer to a TrStoredTrack object
   *
   *  @return Track type as a string
   */
  inline std::string text( const TrStoredTrack * track )
  {
    return Rich::text( Rich::Track::type(track) );
  }

}

/** @class RichTrackID RichTrackID.h RichRecBase/RichTrackID.h
 *
 *  Utility class used as an indentification object for tracking objects used in
 *  the RICH reconstruction software; such as TrStoredTrack, TrgTrack and MCParticle.
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

  /** Constructor from a TrStoredTrack
   *
   * @param track Pointer to a TrStoredTrack
   */
  explicit RichTrackID( const TrStoredTrack * track )
    : m_tkType     ( Rich::Track::type(track)         ),
      m_parentType ( Rich::TrackParent::TrStoredTrack ),
      m_unique     ( 0 != track->unique()             ) { }

  /** Constructor from a TrStoredTrack
   *
   * @param track Pointer to a TrgTrack
   */
  explicit RichTrackID( const TrgTrack * track )
    : m_tkType     ( Rich::Track::type(track)         ),
      m_parentType ( Rich::TrackParent::TrgTrack      ),
      m_unique     ( true                             ) ///< Need to decide what to do here
  { }

  /** Constructor from an MCParticle
   *
   * @param mcPart Pointer to an MCParticle
   */
  explicit RichTrackID( const MCParticle * mcPart )
    : m_tkType     ( Rich::Track::Unknown             ),
      m_parentType ( Rich::TrackParent::MCParticle    ),
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

  /** Initialise from a TrStoredTrack
   *
   *  @param track Pointer to a TrStoredTrack from which to initialise
   */
  void initialiseFor( const TrStoredTrack * track );

  /** Initialise from a TrgTrack
   *
   *  @param track Pointer to a TrgTrack from which to initialise
   */
  void initialiseFor( const TrgTrack * track );

  /** Initialise from a MCParticle
   *
   *  @param track Pointer to an MCParticle from which to initialise
   */
  void initialiseFor( const MCParticle * track );

private: // data

  /// The track type
  Rich::Track::Type m_tkType;

  /// The parent type
  Rich::TrackParent::Type m_parentType;

  /// Is this track unique
  bool m_unique;

};

inline void RichTrackID::initialiseFor( const TrStoredTrack * track )
{
  setParentType ( Rich::TrackParent::TrStoredTrack );
  setTrackType  ( Rich::Track::type(track)         );
  setUnique     ( 0 != track->unique()             );
}

inline void RichTrackID::initialiseFor( const TrgTrack * track )
{
  setParentType ( Rich::TrackParent::TrgTrack      );
  setTrackType  ( Rich::Track::type(track)         );
  setUnique     ( true                             ); ///< Need to decide what to do here
}

inline void RichTrackID::initialiseFor( const MCParticle * )
{
  setParentType ( Rich::TrackParent::MCParticle );
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

/// Implement StreamBuffer >> method for RichTrackID
inline StreamBuffer& operator >> ( StreamBuffer& s,
                                   RichTrackID& id )
{
  int iTemp;
  s >> iTemp; id.setTrackType( static_cast<Rich::Track::Type>(iTemp) );
  s >> iTemp; id.setParentType( static_cast<Rich::TrackParent::Type>(iTemp) );
  s >> iTemp; id.setUnique( 0 != iTemp );
  return s;
}

/// Implement StreamBuffer << method for RichTrackID
inline StreamBuffer& operator << ( StreamBuffer& s,
                                   const RichTrackID& id )
{
  s << static_cast<int>(id.trackType())
    << static_cast<int>(id.parentType())
    << static_cast<int>(id.unique());
  return s;
}

#endif // RICHRECBASE_RICHTRACKID_H
