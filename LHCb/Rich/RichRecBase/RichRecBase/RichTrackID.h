// $Id: RichTrackID.h,v 1.2 2003-11-06 11:36:22 jonrob Exp $
#ifndef RICHRECBASE_RICHTRACKID_H
#define RICHRECBASE_RICHTRACKID_H 1

// Include files
#include <string>
#include <iostream>

// from Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

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

/// namespace for RICH detector specifics
namespace Rich {

  namespace Track {

    /// Number of Track types
    static const int NTrTypes = 6;

    enum Type {
      Unknown = -2,
      Unusable,
      Forward,
      Match,
      UpStream,
      Seed,
      VeloTT,
      Velo
    };

    Rich::Track::Type type( const TrStoredTrack * track );

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

  inline std::string text( const TrStoredTrack * track ) {
    return Rich::text( Rich::Track::type(track) );
  }

}

class RichTrackID {

public:

  /// Standard constructor
  RichTrackID()
    : m_tkType     ( Rich::Track::Unknown ),
      m_parentType ( Rich::TrackParent::Unknown ),
      m_history    ( 0 ),
      m_unique     ( true ) { }
  
  /// Constructor from a ContainedObject
  RichTrackID( const ContainedObject * obj );
  
  ~RichTrackID( ) {} ///< Destructor
  
  /// Retrieve the track type
  Rich::Track::Type trackType() const { return m_tkType; }

  /// Set the track type
  void setTrackType( Rich::Track::Type type ) { m_tkType = type; }

  /// Retrieve the parent type
  Rich::TrackParent::Type parentType() const { return m_parentType; }

  /// Set the parent type
  void setParentType( Rich::TrackParent::Type type ) { m_parentType = type; }

  /// Retrieve origin of the track
  unsigned long history() const { return m_history; }

  /// Set the origin of the track
  void setHistory( unsigned long hist ) { m_history = hist; }

  /// Is this track unique ?
  bool unique() const { return m_unique; }

  /// Set if this track unique ?
  void setUnique( bool unique ) { m_unique = unique; }

  /// Initialise from a TrStoredTrack
  void initTrStoredTrack( const TrStoredTrack * track );

  /// Initialise from a MCParticle
  void initMCParticle( const MCParticle * track );

private: // data

  /// The track type
  Rich::Track::Type m_tkType;

  /// The parent type
  Rich::TrackParent::Type m_parentType;

  /// Copy of history code from TrStoredTrack
  unsigned long m_history;

  /// Unique ?
  bool m_unique;

};

inline void RichTrackID::initTrStoredTrack( const TrStoredTrack * track )
{
  setParentType ( Rich::TrackParent::TrStoredTrack );
  setTrackType  ( Rich::Track::type(track)         );
  setHistory    ( track->history()                 );
  setUnique     ( 0 != track->unique()             );
}

inline void RichTrackID::initMCParticle( const MCParticle * )
{
  setParentType ( Rich::TrackParent::MCParticle );
}

/// Implement textual ostream << method for Rich::Track::Type enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::Track::Type type ) {
  s << Rich::text( type );
  return s;
}

/// Implement textual ostream << method for Rich::TrackParent::Type enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::TrackParent::Type track ) {
  s << Rich::text( track );
  return s;
}

/// Implement textual ostream << method for RichTrackID
inline std::ostream& operator << ( std::ostream& s,
                                   const RichTrackID& id ) {
  s << "{ " << std::endl
    << " track type:\t" << id.trackType() << std::endl
    << " parent type:\t" << id.parentType() << std::endl
    << " history code:\t" << id.history() << std::endl
    << " unique:\t" << id.unique() <<  std::endl
    << " } " << std::endl;
  return s;
}

/// Implement textual MsgStream << method for Rich::Track::Type enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::Track::Type type ) {
  s << Rich::text( type );
  return s;
}

/// Implement textual MsgStream << method for Rich::TrackParent::Typ enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::TrackParent::Type track ) {
  s << Rich::text( track );
  return s;
}

/// Implement StreamBuffer >> method for RichTrackID
inline StreamBuffer& operator >> ( StreamBuffer& s,
                                   RichTrackID& id ) {
  int iTemp;
  s >> iTemp; id.setTrackType( (Rich::Track::Type)iTemp );
  s >> iTemp; id.setParentType( ( Rich::TrackParent::Type)iTemp );
  unsigned long history; s >> history; id.setHistory( history );
  s >> iTemp; id.setUnique( 0 != iTemp );
  return s;
}

/// Implement StreamBuffer << method for RichTrackID
inline StreamBuffer& operator << ( StreamBuffer& s,
                                   const RichTrackID& id ) {
  s << (int)id.trackType()
    << (int)id.parentType()
    << id.history()
    << (int)id.unique();
  return s;
}

#endif // RICHRECBASE_RICHTRACKID_H
