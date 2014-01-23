// $Id:$
// ============================================================================
#ifndef HltEvent_L0DiMuonCandidate_H
#define HltEvent_L0DiMuonCandidate_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <vector>
#include <ostream>
// ============================================================================
#include "GaudiKernel/NamedRange.h"
#include "Event/L0MuonCandidate.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
// ============================================================================
#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif
// ============================================================================
#include "GaudiKernel/boost_allocator.h"
#include "GaudiKernel/SmartRef.h"
// ============================================================================
// Forward declarations
namespace Hlt {
// ==========================================================================
// Namespace for locations in TDS
namespace L0DiMuonCandidateLocation {
const std::string Default = "Hlt/DiTracks";
}
// ==========================================================================
/** @class L0DiMuonCandidate L0DiMuonCandidate.h
 *
 * Representation of di-muon for Hlt
 *
 * @author alexander.mazurov@gmail.com
 * created Mon Jun 14 15:57:48 2010
 *
 */
class L0DiMuonCandidate : public ContainedObject {
  public:
    // ========================================================================
    /// typedef for std::vector of L0DiMuonCandidate
    typedef std::vector<L0DiMuonCandidate *> Vector;
    typedef std::vector<const L0DiMuonCandidate *> ConstVector;
    /// typedef for ObjectVector of L0DiMuonCandidate
    typedef ObjectVector<L0DiMuonCandidate> Container;
    /// The container type for shared di-tracks (without ownership)
    typedef SharedObjectsContainer<Hlt::L0DiMuonCandidate> Selection;
    /// For uniform access to containers in TES (KeyedContainer,SharedContainer)
    typedef Gaudi::NamedRange_<ConstVector> Range;
    // ========================================================================
  public:
    // ========================================================================
    /// Default Constructor
    L0DiMuonCandidate() {}
    /// Default Destructor
    virtual ~L0DiMuonCandidate() {}
    // Retrieve pointer to class definition structure
    virtual const CLID &clID() const;
    static const CLID &classID();

    /// Fill the ASCII output stream
    virtual std::ostream &fillStream( std::ostream &s ) const;

    /// Retrieve (const)  Reference to the first track
    const LHCb::L0MuonCandidate *first() const;

    /// Retrieve  Reference to the first track
    LHCb::L0MuonCandidate *first();

    /// Update  Reference to the first track
    void setFirst( const SmartRef<LHCb::L0MuonCandidate> &value );

    /// Update (pointer)  Reference to the first track
    void setFirst( const LHCb::L0MuonCandidate *value );

    /// Retrieve (const)  Reference to the second track
    const LHCb::L0MuonCandidate *second() const;

    /// Retrieve  Reference to the second track
    LHCb::L0MuonCandidate *second();

    /// Update  Reference to the second track
    void setSecond( const SmartRef<LHCb::L0MuonCandidate> &value );

    /// Update (pointer)  Reference to the second track
    void setSecond( const LHCb::L0MuonCandidate *value );

#ifndef GOD_NOALLOC
    /// operator new
    static void *operator new( size_t size ) {
        return ( sizeof( L0DiMuonCandidate ) == size
                     ? boost::singleton_pool<L0DiMuonCandidate,
                                             sizeof( L0DiMuonCandidate )>::malloc()
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
        boost::singleton_pool<L0DiMuonCandidate,
                              sizeof( L0DiMuonCandidate )>::is_from( p )
            ? boost::singleton_pool<L0DiMuonCandidate,
                                    sizeof( L0DiMuonCandidate )>::free( p )
            : ::operator delete( p );
    }

    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete( void *p, void *pObj ) {
        ::operator delete( p, pObj );
    }
#endif
  protected:
  private:
    SmartRef<LHCb::L0MuonCandidate> m_first;  ///< Reference to the first track
    SmartRef<LHCb::L0MuonCandidate> m_second; ///< Reference to the second track
};                                            // class L0DiMuonCandidate

/// Definition of vector container type for L0DiMuonCandidate
typedef ObjectVector<L0DiMuonCandidate> L0DiMuonCandidates;

inline std::ostream &operator<<( std::ostream &str, const L0DiMuonCandidate &obj ) {
    return obj.fillStream( str );
}

} // namespace LHCb;

// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------

inline const LHCb::L0MuonCandidate *Hlt::L0DiMuonCandidate::first() const {
    return m_first;
}

inline LHCb::L0MuonCandidate *Hlt::L0DiMuonCandidate::first() { return m_first; }

inline void
Hlt::L0DiMuonCandidate::setFirst( const SmartRef<LHCb::L0MuonCandidate> &value ) {
    m_first = value;
}

inline void Hlt::L0DiMuonCandidate::setFirst( const LHCb::L0MuonCandidate *value ) {
    m_first = value;
}

inline const LHCb::L0MuonCandidate *Hlt::L0DiMuonCandidate::second() const {
    return m_second;
}

inline LHCb::L0MuonCandidate *Hlt::L0DiMuonCandidate::second() { return m_second; }

inline void
Hlt::L0DiMuonCandidate::setSecond( const SmartRef<LHCb::L0MuonCandidate> &value ) {
    m_second = value;
}

inline void Hlt::L0DiMuonCandidate::setSecond( const LHCb::L0MuonCandidate *value ) {
    m_second = value;
}

#endif /// HltEvent_L0DiMuonCandidate_H
