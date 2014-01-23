// $Id: HltMultiTrack.h,v 1.4 2010-08-30 13:00:44 amazurov Exp $
// ============================================================================
#ifndef HltEvent_MultiTrack_H
#define HltEvent_MultiTrack_H 1
// ============================================================================
// $URL$
// ============================================================================
// Include files
// ============================================================================
#include "Event/Track.h"
#include "GaudiKernel/NamedRange.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif
#include "GaudiKernel/boost_allocator.h"
#include "GaudiKernel/SmartRef.h"
#include <vector>
#include <ostream>
// ============================================================================
// Forward declarations
// ============================================================================
namespace Hlt {
// ==========================================================================
// Namespace for locations in TDS
namespace MultiTrackLocation {
const std::string Default = "Hlt/MultiTracks";
}
// ==========================================================================
/** @class MultiTrack MultiTrack.h
 *
 * Representation of di-track for Hlt
 *
 * @author alexander.mazurov@gmail.com
 * created Mon Jun 14 15:57:48 2010
 *
 */
class MultiTrack : public ContainedObject {
    // ========================================================================
  public: // related types
    // ========================================================================
    /// typedef for std::vector of MultiTrack
    typedef std::vector<MultiTrack *> Vector;
    typedef std::vector<const MultiTrack *> ConstVector;
    /// typedef for ObjectVector of MultiTrack
    typedef ObjectVector<MultiTrack> Container;
    /// The container type for shared di-tracks (without ownership)
    typedef SharedObjectsContainer<Hlt::MultiTrack> Selection;
    /// For uniform access to containers in TES (KeyedContainer,SharedContainer)
    typedef Gaudi::NamedRange_<ConstVector> Range;
    /// references to tracks
    typedef SmartRefVector<LHCb::Track> Tracks;
    // ========================================================================
  public: // constrcuted & desctructor
    // ========================================================================
    /// Default Constructor
    MultiTrack() {}
    /// Default Destructor
    virtual ~MultiTrack();
    // ========================================================================
    // Retrieve pointer to class definition structure
    virtual const CLID &clID() const;
    static const CLID &classID();
    /// Fill the ASCII output stream
    virtual std::ostream &fillStream( std::ostream &s ) const;
    // ========================================================================
  public: // constrcuted & desctructor
    // ========================================================================
    /// get the tracks
    inline Tracks &tracks() { return m_tracks; }
    /// get the tracks
    inline const Tracks &tracks() const { return m_tracks; }
    /// Update  References to tarcks
    void setTracks( const Tracks &tracks );
    /// Add to (pointer) reference to the stages
    void addToTracks( const LHCb::Track *track );
    /// Remove from  reference to the tarcks
    void removeFromTracks( const LHCb::Track *track );
    /// Clear  Reference to the tarcks
    void clearTracks() { m_tracks.clear(); }
    /// Has track ?
    bool hasTrack( const LHCb::Track *track ) const;
    // ========================================================================
  public:
// ========================================================================
#ifndef GOD_NOALLOC
    /// operator new
    static void *operator new( size_t size ) {
        return (
            sizeof( MultiTrack ) == size
                ? boost::singleton_pool<MultiTrack, sizeof( MultiTrack )>::malloc()
                : ::operator new( size ) );
    }

    /// placement operator new
    /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
    /// it is not needed in libstdc++ >= 3.4
    static void *operator new( size_t size, void *pObj ) {
        return ::operator new( size, pObj );
    }

    /// operator delete
    static void operator delete( void *p ) {
        boost::singleton_pool<MultiTrack, sizeof( MultiTrack )>::is_from( p )
            ? boost::singleton_pool<MultiTrack, sizeof( MultiTrack )>::free( p )
            : ::operator delete( p );
    }

    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete( void *p, void *pObj ) {
        ::operator delete( p, pObj );
    }
#endif
    // ========================================================================
  protected:
    // ========================================================================
    // ========================================================================
  private:
    // ========================================================================
    /// the tracks:
    SmartRefVector<LHCb::Track> m_tracks; // tracks
    // ========================================================================
}; // class MultiTrack
// ==========================================================================
/// Definition of vector container type for MultiTrack
typedef ObjectVector<MultiTrack> MultiTracks;
/// outptu operator
inline std::ostream &operator<<( std::ostream &str, const MultiTrack &obj ) {
    return obj.fillStream( str );
}
// ==========================================================================
} //                                                      end of namespace LHCb
// ============================================================================
// The END
// ============================================================================
#endif /// HltEvent_MultiTrack_H
// ============================================================================
