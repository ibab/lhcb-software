// $Id: RichTrackID.h,v 1.7 2004-06-29 19:35:42 jonesc Exp $
#ifndef RICHRECBASE_RICHTRACKID_H
#define RICHRECBASE_RICHTRACKID_H 1

// Include files
#include <string>
#include <iostream>
#include <sstream>

// from Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// Event
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"

/** @class RichTrackID RichTrackID.h RichRecBase/RichTrackID.h
 *
 *  Indentifies the track type
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-09-23
 */

// namespace for RICH detector specifics
namespace Rich {

  namespace Track {

    /// Number of Track types
    static const unsigned int NTrTypes = 7;

    enum Type {
      Unknown  = -2,
      Unusable = -1,
      Forward  =  0,
      Match    =  1,
      Follow   =  2,
      Seed     =  3,
      VeloTT   =  4,
      KsTrack  =  5,
      Velo     =  6
    };

    /// Returns the enumerated type for given TrStoredTrack
    Rich::Track::Type type( const TrStoredTrack * track );

    /// Returns true if track is potentially usable for the RICH
    inline bool isUsable( const Rich::Track::Type type )
    {
      return ( type != Rich::Track::Unusable );
    }

    /// Returns true if track is potentially usable for the RICH
    inline bool isUsable( const TrStoredTrack * track )
    {
      return ( track ? Rich::Track::isUsable(Rich::Track::type(track)) : false );
    }

  }

  namespace TrackParent {
    enum Type {
      Unknown = -1,
      TrStoredTrack,
      MCParticle
    };
  }

  /// Text conversion for Rich::Track::ParentType enumeration
  std::string text( const Rich::TrackParent::Type parent ) ;

  /// Text conversion for Rich::Track::Type enumeration
  std::string text( const Rich::Track::Type track );

  /// Returns the track type
  inline std::string text( const TrStoredTrack * track )
  {
    return Rich::text( Rich::Track::type(track) );
  }

}

class RichTrackID {

public:

  /// Standard constructor
  RichTrackID()
    : m_tkType     ( Rich::Track::Unknown             ),
      m_parentType ( Rich::TrackParent::Unknown       ),
      m_unique     ( true                             ) { }

  /// Constructor from a TrStoredTrack
  explicit RichTrackID( const TrStoredTrack * track )
    : m_tkType     ( Rich::Track::type(track)         ),
      m_parentType ( Rich::TrackParent::TrStoredTrack ),
      m_unique     ( 0 != track->unique()             ) { }

  /// Constructor from an MCParticle
  explicit RichTrackID( const MCParticle * mcPart )
    : m_tkType     ( Rich::Track::Unknown             ),
      m_parentType ( Rich::TrackParent::MCParticle    ),
      m_unique     ( true                             ) { }

  ~RichTrackID( ) {} ///< Destructor

  /// Retrieve the track type
  Rich::Track::Type trackType() const { return m_tkType; }

  /// Set the track type
  void setTrackType( const Rich::Track::Type type ) { m_tkType = type; }

  /// Retrieve the parent type
  Rich::TrackParent::Type parentType() const { return m_parentType; }

  /// Set the parent type
  void setParentType( const Rich::TrackParent::Type type ) { m_parentType = type; }

  /// Is this track unique ?
  bool unique() const { return m_unique; }

  /// Set if this track unique ?
  void setUnique( bool unique ) { m_unique = unique; }

  /// Initialise from a TrStoredTrack
  void initialiseFor( const TrStoredTrack * track );

  /// Initialise from a MCParticle
  void initialiseFor( const MCParticle * track );

private: // data

  /// The track type
  Rich::Track::Type m_tkType;

  /// The parent type
  Rich::TrackParent::Type m_parentType;

  /// Unique ?
  bool m_unique;

};

inline void RichTrackID::initialiseFor( const TrStoredTrack * track )
{
  setParentType ( Rich::TrackParent::TrStoredTrack );
  setTrackType  ( Rich::Track::type(track)         );
  setUnique     ( 0 != track->unique()             );
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
