// $Id: HltStage.h,v 1.10 2010-08-30 13:00:44 amazurov Exp $
// =============================================================================
#ifndef HltEvent_Stage_H_
#define HltEvent_Stage_H_ 1
// =============================================================================
// Include files
// =============================================================================
// STD & STL
// =============================================================================
#include <vector>
#include <ostream>
// =============================================================================
#include "boost/variant.hpp"
// =============================================================================
// GaudiKernel
// =============================================================================
#include "GaudiKernel/NamedRange.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Kernel.h"
// =============================================================================
// TrackEvent
// =============================================================================
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/Vertex.h"
// =============================================================================
// L0Event
// =============================================================================
#include "Event/L0CaloCandidate.h"
#include "Event/L0MuonCandidate.h"
// =============================================================================
// HltBase
// =============================================================================
#include "Event/NamedEntry.h"
#include "Event/HltMultiTrack.h"
#include "Event/HltL0DiMuonCandidate.h"
#include "HltBase/Cache.h"
// =============================================================================
#ifdef _WIN32
// =============================================================================
// Avoid conflict of Windows macro with std::max
// =============================================================================
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif
// =============================================================================
// allocators form Boost
// =============================================================================
#include "GaudiKernel/boost_allocator.h"
// =============================================================================

namespace Hlt_Stage_details {
template <typename T> struct is_ : boost::static_visitor<bool> {
    bool operator()( const SmartRef<T>& ) const { return true; }
    template <typename Any>
    bool operator()( const Any&         ) const { return false; }
};
template <typename T> struct get_ : boost::static_visitor<T*> {
    T* operator()( SmartRef<T>& ref ) const { return ref.target(); }
    template <typename Any>
    T* operator()( Any&             ) const { return NULL; }
};
template <typename T> struct getc_ : boost::static_visitor<const T*> {
    const T* operator()( const SmartRef<T>& ref ) const { return ref.target(); }
    template <typename Any>
    const T* operator()( const Any&             ) const { return NULL; }
};
}

namespace Hlt
{
// ===========================================================================
// Forward declarations
class Stage;
class Candidate;
// ===========================================================================
// Namespace for locations in TDS
namespace StageLocation
{
// =========================================================================
/** @var Default
 *  The default TES-location of HLT-Stages
 */
const std::string Default = "Hlt/Stages";
// =========================================================================
}
// ===========================================================================
/** @class Stage Event/HltStage.h
 *
 * Wrapper class for tracks, l0 candidates,...
 *
 * @author alexander.mazurov@gmail.com
 * created Thu May 20 14:15:40 2010
 *
 */
class GAUDI_API Stage : public ContainedObject
{
    // ========================================================================
    // friend class
    friend class Hlt::Candidate;
    // ========================================================================
  public:
    Stage()
#ifdef __GCCXML__
        ;
#else
        : m_locker{nullptr}  {}
#endif

    // ========================================================================
    // Thus MUST match the order of the boost::variant, such that
    // Type( boost::variant::which ) makes sense...
    enum Type {
        Unknown = 0,
        L0Muon = 1,
        L0Calo = 2,
        L0DiMuon = 3,
        HltTrack = 4,
        HltRecVertex = 5,
        HltMultiTrack = 6,
        HltParticle = 7,
        HltProtoParticle = 8,
        HltVertex = 9,
        HltStage = 10
    };
    // ========================================================================
  public:
    // ========================================================================
    /// typedef for std::vector of Stage
    typedef std::vector<Stage*> Vector;
    typedef std::vector<const Stage*> ConstVector;
    /// typedef for ObjectVector of Stage
    typedef ObjectVector<Stage> Container;
    /// The container type for shared di-tracks (without ownership)
    typedef SharedObjectsContainer<Stage> Selection;
    /// For uniform access to containers in TES (KeyedContainer,SharedContainer)
    typedef Gaudi::NamedRange_<ConstVector> Range;
    /// the actual type of history
    typedef std::vector<Hlt::NamedEntry> History;
    /// the actual KeyType for the info cache
    typedef Cache::KeyType KeyType;
    // ========================================================================
    /** @class Lock Event/HltStage.h
     *  Locker class for stage
     *
     *  @author alexander.mazurov@gmail.com
     *  @author ivan.belyaev@cern.ch
     *
     *  @code
     *  // Inside some algoritms
     *  Hlt::Stage* stage = ...;
     *  LHCb::L0MuonCandidate* l0mc = ...;
     *  Hlt::Stage::Lock locker(stage,this); // Unlock  when  we leave current
     *                                       // scope
     *  stage->set<LHCb::L0MuonCandidate>(l0mc);
     *  // Lock is only for state modification methods. Without lock it will be
     *  // exception when we call this method.
     * @endcode
     */
    class Lock
    {
      public:
        // ======================================================================
        /// constructor
        Lock( Stage* stage, const INamedInterface* locker )
            : m_locker(locker), m_stage(stage)
        {
            if ( UNLIKELY( !m_stage || !m_locker ) ) {
                throw GaudiException( "Stage or locker is null", "Stage::Lock::Lock",
                              StatusCode::FAILURE );
            }
            stage->_lock( m_locker );
        }
        /// destructor
        ~Lock() { m_stage->_unlock(m_locker); }
        // ======================================================================
      public:
        // ======================================================================
        /// add the entries to the history
        const History& addToHistory( std::string entry );
        /// add the entries to the history
        const History& addToHistory( const INamedInterface& iface );
        /// add the entries to the history
        const History& addToHistory( const History& entries );
        // ======================================================================
      private:
        // ======================================================================
        /// no default constructor
        Lock(); // no default constructor
        // ======================================================================
      private:
        // ======================================================================
        const INamedInterface* m_locker;
        Stage* m_stage;
        // ======================================================================
    };
    // ========================================================================
  public:
    // ========================================================================
    /// Default Destructor
    virtual ~Stage() = default;
    // ========================================================================
  public:
    // ========================================================================
    void SetToNull()
#ifdef __GCCXML__
    ;
#else
    {
        _checkLock(); m_object = boost::blank{};
    }
#endif
    // ========================================================================
  public:
    // ========================================================================
    /// Retrieve pointer to class definition structure (virtual)
    virtual const CLID& clID() const
    {
        return Hlt::Stage::classID();
    }
    /// Retrieve pointer to class definition structure (static)
    static const CLID& classID();
    // ========================================================================
  public:
    // ========================================================================
    /// Fill the ASCII output stream
    virtual std::ostream& fillStream( std::ostream& s ) const;
    /** Get a wrapped object
     *  @code
     *  const LHCb::Track* track = stage->get<LHCb::Track>();
     *  @endcode
     */
    template <typename T> const T* get() const
    { return boost::apply_visitor( Hlt_Stage_details::getc_<T>(), m_object); }

    /** Get a wrapped object (non-const)
     *  @code
     *  LHCb::Track* track = stage->get<LHCb::Track>();
     *  if (!track) {
     *     ... stage was not a track...
     *  } else {
     *     ... do something with track
     *  }
     *  @endcode
     */
    template <typename T> T* get()
    { _checkLock(); return boost::apply_visitor( Hlt_Stage_details::get_<T>(), m_object); }
    /** Set a wrapped object
     *   @code
     *  Hlt::Stage* stage = ...;
     *  LHCb::L0MuonCandidate* l0mc = ...;
     *  ...
     *  stage->set(l0mc);
     *  @endcode
     */
    template <typename T>
    void set( const T* value )
    { m_object = SmartRef<T>(value); }
    /** Check a type of contained object
     *   @code
     *  Hlt::Stage* stage = ...;
     *  ...
     *  if (stage->is<LHCb::Track>()) {
     *    LHCb::Track* track = stage->get<LHCb::Track>();
     *      ...
     *  }
     *  @endcode
     */
    template <typename T> bool is() const
    { return boost::apply_visitor( Hlt_Stage_details::is_<T>(), m_object ); }
    // ========================================================================
  public: // python-friendly access
    // ========================================================================
    /// Get a wrapped object (suitable for python)
    const ContainedObject* _get() const;
    /// Get a wrapped object (suitable for python)
    const ContainedObject* get_() const
    {
        return _get();
    }
    // ========================================================================
  public:
// ========================================================================
#ifndef GOD_NOALLOC
    /// operator new
    static void* operator new( size_t size )
    {
        return ( sizeof( Stage ) == size
                     ? boost::singleton_pool<Stage, sizeof( Stage )>::malloc()
                     : ::operator new( size ) );
    }
    /// placement operator new
    /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
    /// it is not needed in libstdc++ >= 3.4
    static void* operator new( size_t size, void* pObj )
    {
        return ::operator new( size, pObj );
    }
    /// operator delete
    static void operator delete( void* p )
    {
        boost::singleton_pool<Stage, sizeof( Stage )>::is_from( p )
            ? boost::singleton_pool<Stage, sizeof( Stage )>::free( p )
            : ::operator delete( p );
    }

    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete( void* p, void* pObj )
    {
        ::operator delete( p, pObj );
    }
#endif
    // ========================================================================
  public: // cache
          // ========================================================================
          /** Check if cache contains value with the specified key
* @param[in] key
*/
    bool hasInfo( const KeyType& key )
    {
        return m_cache.has( key );
    }
    /// Check if cache contains value with the specified key and type
    template <typename T>
    bool hasInfo_( const KeyType& key ) const
    {
        return m_cache.has_<T>( key );
    }
    /** Add value with the specified type and key
     *  @return true if value was added and false if we try insert an existing key
     */
    template <typename T>
    bool insertInfo( const KeyType& key, const T& value );
    /// Update  value with the specified type and key
    template <typename T>
    void updateInfo( const KeyType& key, const T& value );
    /** Get a cached value associated with the specified type and key.
     *  @returns The value associated with the specified type and key. If the such
     *  value does not exists def is returned.
     */
    template <typename T>
    T info( const KeyType& key, const T& default_value ) const;
    /** Erase all cached values with the specified key
     * @param[in] key
     */
    void eraseInfo( const KeyType& key )
    {
        _checkLock();
        m_cache.erase( key );
    }
    /// Get a cache object
    const Cache& cache() const
    {
        return m_cache;
    }
    // ========================================================================
  public: // Locker
    // ========================================================================
    // get the current locker
    const INamedInterface* locked() const
    {
        return m_locker;
    }
    // get own candidate
    const Hlt::Candidate* owner() const
    {
        return m_owner;
    }
    // ========================================================================
  protected:
    // ========================================================================
    /// set own candidate
    void setOwner( const Hlt::Candidate* c ) // set own candidate
    {
        if ( UNLIKELY( !c ) ) {
            throw GaudiException( "Invalid Hlt::Candidate", "Stage::setOwner",
                                  StatusCode::FAILURE );
        }
        if ( UNLIKELY( m_owner && c != m_owner ) ) {
            throw GaudiException( "Owner is already set", "Stage::setOwner",
                                  StatusCode::FAILURE );
        }
        m_owner = c;
    }
    // ========================================================================
  public:
    // ========================================================================
    /// history
    const History& history() const
    {
        return m_history;
    } // history
    // ========================================================================
    /// the actual type of the stage
    Type stageType() const
    {
        return Hlt::Stage::Type( m_object.which() );
    }
    // ========================================================================
  private:
    // ========================================================================
    //
    void _lock( const INamedInterface* locker );
    void _unlock( const INamedInterface* locker );
    void _checkLock() const
    {
        if ( UNLIKELY( !locked() ) ) {
            throw GaudiException( "Not locked", "Stage::_checkLock",
                              StatusCode::FAILURE );
        }
    }
    //
    template <typename T>
    bool _insertInfo( const KeyType& key, const T& value );
    //
    template <typename T>
    void _updateInfo( const KeyType& key, const T& value );
    //
    template <typename T>
    T _info( const KeyType& key, const T& default_value ) const;
    // ========================================================================
  private:
    // ========================================================================
    boost::variant< boost::blank                      // 0
                  , SmartRef<LHCb::L0MuonCandidate>   // 1
                  , SmartRef<LHCb::L0CaloCandidate>   // 2
                  , SmartRef<Hlt::L0DiMuonCandidate>  // 3
                  , SmartRef<LHCb::Track>             // 4
                  , SmartRef<LHCb::RecVertex>         // 5
                  , SmartRef<Hlt::MultiTrack>         // 6
                  , SmartRef<LHCb::Particle>          // 7
                  , SmartRef<LHCb::ProtoParticle>     // 8
                  , SmartRef<LHCb::Vertex>            // 9
                  , SmartRef<Hlt::Stage> >  m_object; // 10
    /// cache
    Cache m_cache; // cache
    /// history
    History m_history; // history
    /// locker
    const INamedInterface* m_locker; // locker
    /// my candidate
    SmartRef<Hlt::Candidate> m_owner; // my candidate
    // ========================================================================
}; //
// ==========================================================================
/// Definition of vector container type for Stage
typedef Hlt::Stage::Container Stages;
// ==========================================================================
} //                                                       end of namespace Hlt
// ============================================================================
namespace Gaudi
{
// ==========================================================================
namespace Utils
{
// ========================================================================
/// print-out
GAUDI_API
std::ostream& toStream( const Hlt::Stage* c, std::ostream& s );
// ========================================================================
} //                                            end of namespace Gaudi::Utils
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
namespace Hlt
{
// ==========================================================================
// Helper trick with VertexBase -- forward to RecVertex...
// ==========================================================================
template <>
inline const LHCb::VertexBase* Hlt::Stage::get<LHCb::VertexBase>() const
{
    return this->get<LHCb::RecVertex>();
}
// ==========================================================================
template <>
inline LHCb::VertexBase* Hlt::Stage::get<LHCb::VertexBase>()
{
    return this->get<LHCb::RecVertex>();
}
// ==========================================================================
template <>
inline bool Hlt::Stage::is<LHCb::VertexBase>() const
{
    return this->is<LHCb::RecVertex>();
}
// ==========================================================================
// * INFO * INFO * INFO * INFO * INFO * INFO * INFO * INFO * INFO * INFO *
// ==========================================================================
template <typename T>
inline bool Stage::_insertInfo( const KeyType& key, const T& value )
{
    _checkLock();
    return m_cache.insert<T>( key, value );
}
// ==========================================================================
template <>
inline bool Stage::insertInfo<bool>( const KeyType& key, const bool& value )
{
    return _insertInfo( key, value );
}
// ==========================================================================
template <>
inline bool Stage::insertInfo<int>( const KeyType& key, const int& value )
{
    return _insertInfo( key, value );
}
// ==========================================================================
template <>
inline bool Stage::insertInfo<double>( const KeyType& key, const double& value )
{
    return _insertInfo( key, value );
}
// ==========================================================================
template <>
inline bool Stage::insertInfo<std::string>( const KeyType& key,
                                            const std::string& value )
{
    return _insertInfo( key, value );
}
// ==========================================================================
template <typename T>
inline void Stage::_updateInfo( const KeyType& key, const T& value )
{
    _checkLock();
    m_cache.update<T>( key, value );
}
// ==========================================================================
template <>
inline void Stage::updateInfo<bool>( const KeyType& key, const bool& value )
{
    _updateInfo( key, value );
}
// ==========================================================================
template <>
inline void Stage::updateInfo<int>( const KeyType& key, const int& value )
{
    _updateInfo( key, value );
}
// ==========================================================================
template <>
inline void Stage::updateInfo<double>( const KeyType& key, const double& value )
{
    _updateInfo( key, value );
}
// ==========================================================================
template <>
inline void Stage::updateInfo<std::string>( const KeyType& key,
                                            const std::string& value )
{
    _updateInfo( key, value );
}
// ==========================================================================
template <typename T>
inline T Stage::_info( const KeyType& key, const T& default_value ) const
{
    return m_cache.info<T>( key, default_value );
}
// ==========================================================================
template <>
inline bool Stage::info<bool>( const KeyType& key,
                               const bool& default_value ) const
{
    return _info( key, default_value );
}
// ==========================================================================
template <>
inline int Stage::info<int>( const KeyType& key, const int& default_value ) const
{
    return _info( key, default_value );
}
// ==========================================================================
template <>
inline double Stage::info<double>( const KeyType& key,
                                   const double& default_value ) const
{
    return _info( key, default_value );
}
// ==========================================================================
template <>
inline std::string Stage::info<std::string>( const KeyType& key,
                                             const std::string& default_value ) const
{
    return _info( key, default_value );
}
// ==========================================================================
//  explicitly instantiate  so that these appear in the python dictionary...
//  (or should we remove the generic template implementation, and rely on specialization???
//  ( that way, code that wants a signature that isn't specialized won't link..., which
//    is good, as I'd rather have a linktime error than a runtime error)
// ==========================================================================
template void Hlt::Stage::set(const LHCb::L0MuonCandidate*) ;
template void Hlt::Stage::set(const LHCb::L0CaloCandidate*) ;
template void Hlt::Stage::set(const Hlt::L0DiMuonCandidate*) ;
template void Hlt::Stage::set(const LHCb::Track*) ;
template void Hlt::Stage::set(const LHCb::RecVertex*) ;
template void Hlt::Stage::set(const Hlt::MultiTrack*) ;
template void Hlt::Stage::set(const LHCb::Particle*) ;
template void Hlt::Stage::set(const LHCb::ProtoParticle*) ;
template void Hlt::Stage::set(const Hlt::Stage*) ;

// ==========================================================================
/// output operatoor
inline std::ostream& operator<<( std::ostream& str, const Stage& obj )
{
    return obj.fillStream( str );
}
/// output operatoor
inline std::ostream& operator<<( std::ostream& str, const Stage* obj )
{
    return obj ? obj -> fillStream(str) : ( str << "<NULL>" ) ;
}
// ==========================================================================
} //                                                       end of namespace Hlt
// ============================================================================
//                                                                      The END
// ============================================================================
#endif /// HltEvent_Stage_H
// ============================================================================
