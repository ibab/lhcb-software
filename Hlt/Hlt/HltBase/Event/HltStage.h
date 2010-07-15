// $Id: HltStage.h,v 1.3 2010-07-15 16:06:42 amazurov Exp $ 
// =============================================================================
#ifndef HltEvent_Stage_H
#define HltEvent_Stage_H 1
// =============================================================================
// Include files
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/L0CaloCandidate.h"
#include "Event/L0MuonCandidate.h"
#include "Event/HltMultiTrack.h"
#include "Event/HltL0DiMuonCandidate.h"

#include "HltBase/Cache.h"

#include "GaudiKernel/NamedRange.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
// =============================================================================
#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif
// =============================================================================
#include "GaudiKernel/boost_allocator.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartIF.h"
#include <vector>
#include <ostream>
// =============================================================================
// Forward declarations
// =============================================================================
namespace Hlt {
// =============================================================================
// Forward declarations
// =============================================================================
// Class ID definition
static const CLID CLID_Stage = 7562;

// =============================================================================
// Namespace for locations in TDS
namespace StageLocation {
static const std::string& Default = "Hlt/Stages";
}
// =============================================================================
/** @class Stage Stage.h
 *
 * Wrapper class for tracks, l0 candidates,...
 *
 * @author alexander.mazurov@gmail.com
 * created Thu May 20 14:15:40 2010
 *
 */
class Stage: public ContainedObject {
public:
  /// typedef for std::vector of Stage
  typedef std::vector<Stage*>       Vector;
  typedef std::vector<const Stage*> ConstVector;

  /// typedef for ObjectVector of Stage
  typedef ObjectVector<Stage> Container;

  /// The container type for shared di-tracks (without ownership)
  typedef SharedObjectsContainer<Stage>   Selection;
  /// For uniform access to containers in TES (KeyedContainer,SharedContainer)
  typedef Gaudi::NamedRange_<ConstVector> Range;

  /** @class Lock Stage.h
   * Locker class for stage
   * @author alexander.mazurov@gmail.com
   * @author ivan.belyaev@cern.ch
   * @code
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
  class Lock  {
  public:
    // constructor
    Lock (  Stage* stage , const INamedInterface* locker );
    // destructor
    ~Lock ();
  private:
    // no default constructor
    Lock() ;
  private:
      SmartIF<INamedInterface> m_locker ;
      Stage* m_stage ;
  }; // class Stage::Lock


  /// Default Constructor
  Stage() {}

  /// Default Destructor
  virtual ~Stage() {
  }

  void SetAllToNull();

  // Retrieve pointer to class definition structure
  virtual const CLID& clID() const;
  static const CLID& classID();

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

  /** Get a wrapped object
   *   @code
   *  LHCb::Track* track = stage->get<LHCb::Track>();
   *  @endcode
   */
  template<typename T> const T* get() const;

  /**
   *  Get a wrapped object (suitable for python)
   */
  const ContainedObject* _get() const;

  /** Set a wrapped object
   *   @code
   *  Hlt::Stage* stage = ...;
   *  LHCb::L0MuonCandidate* l0mc = ...;
   *  ...
   *  stage->set<LHCb::L0MuonCandidate>(l0mc);
   *  @endcode
   */
  template<typename T> void set(const T* value);


  /** Check a type of contained object
   *   @code
   *  Hlt::Stage* stage = ...;
   *  ...
   *  if (stage<LHCb::Track>()) {
   *    LHCb::Track* track = stage->get<LHCb::Track>();
   *      ...
   *  }
   *  @endcode
   */
  template<typename T> bool is() const;

#ifndef GOD_NOALLOC
  /// operator new
  static void* operator new(size_t size) {
    return (sizeof(Stage) == size ? boost::singleton_pool<Stage,
        sizeof(Stage)>::malloc() : ::operator new(size));
  }

  /// placement operator new
  /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
  /// it is not needed in libstdc++ >= 3.4
  static void* operator new(size_t size, void* pObj) {
    return ::operator new(size, pObj);
  }

  /// operator delete
  static void operator delete(void* p) {
    boost::singleton_pool<Stage, sizeof(Stage)>::is_from(p) ? boost::singleton_pool<
        Stage, sizeof(Stage)>::free(p)
        : ::operator delete(p);
  }

  /// placement operator delete
  /// not sure if really needed, but it does not harm
  static void operator delete(void* p, void* pObj) {
    ::operator delete(p, pObj);
  }
#endif
// =============================================================================
// Cache
// =============================================================================
  /**
   * @brief Check if cache contains value with the specified key
   * @param[in] key
   */
  bool hasInfo(const std::string& key){
    return m_cache.has(key);
  }
// =============================================================================
  /**
   * @brief Check if cache contains value with the specified key and type
   */
  template<typename T>
      bool hasInfo_(const std::string& key) const{
        return m_cache.has_<T>(key);
      }
// =============================================================================
  /**
   * @brief Add value with the specified type and key
   * @return true if value was added and false if we try insert an existing key
   */
  template<typename T> bool insertInfo(const std::string&key,const T& value);
 
// =============================================================================
   /**
   * @brief Update  value with the specified type and key
   */
  template<typename T> void updateInfo(const std::string& key, 
                                                                const T& value);
 
 
// =============================================================================
  /**
   * @brief Get a cached value associated with the specified type and key.
   * @returns The value associated with the specified type and key. If the such
   * value does not exists def is returned.
   */
    template<typename T>
       T info(const std::string& key, const T& default_value) const;

   
// =============================================================================
  /**
   * @brief Erase all cached values with the specified key
   * @param[in] key
   */
  void eraseInfo(const std::string& key){
    _checkLock();
    m_cache.erase(key);
  }
// =============================================================================
  /**
   * @brief Get a cache object
   */
  const Cache& cache() const { return m_cache;}
// =============================================================================
// Locker
// =============================================================================
/**
 * @ brief Get a current locker
 */
  const INamedInterface* locked() const { return m_locker; }
// =============================================================================
protected:
// =============================================================================
private:
// =============================================================================
  SmartRef<LHCb::Track> m_track;
  SmartRef<LHCb::RecVertex> m_rec_vertex;
  SmartRef<LHCb::L0CaloCandidate> m_l0_calo_candidate;
  SmartRef<LHCb::L0MuonCandidate> m_l0_muon_candidate;
  SmartRef<Hlt::MultiTrack> m_multitrack;
  SmartRef<Hlt::L0DiMuonCandidate> m_l0_dimuon_candidate;

  Cache m_cache;

  typedef std::vector<std::string> History;
  History  m_history;
  SmartIF<INamedInterface>  m_locker;

  void _lock (const INamedInterface* locker);
  void _unlock (const INamedInterface* locker);
  void _checkLock() const;
  
 template<typename T>
    bool _insertInfo(const std::string& key, const T& value);
  
 template<typename T>
    void _updateInfo(const std::string& key, const T& value);
 
  template<typename T>
    T _info(const std::string& key, const T& default_value) const;
 
// =============================================================================
}; // class Stage
// =============================================================================

/// Definition of vector container type for Stage
typedef ObjectVector<Stage> Stages;

inline std::ostream& operator<<(std::ostream& str, const Stage& obj) {
  return obj.fillStream(str);
}
// =============================================================================

template<>
inline const LHCb::Track* Hlt::Stage::get<LHCb::Track>() const {
  return m_track;
}

template<> inline void Hlt::Stage::set(const LHCb::Track* value) {
  SetAllToNull();
  m_track = value;
}

template<> inline bool Hlt::Stage::is<LHCb::Track>() const {
  return m_track != 0;
}
// =============================================================================
template<> inline
const LHCb::RecVertex* Hlt::Stage::get<LHCb::RecVertex>() const {
  return m_rec_vertex;
}

template<> inline void Hlt::Stage::set(const LHCb::RecVertex* value) {
  SetAllToNull();
  m_rec_vertex = value;
}

template<> inline bool Hlt::Stage::is<LHCb::RecVertex>() const {
  return m_rec_vertex != 0;
}

// =============================================================================
template<> inline const LHCb::L0CaloCandidate* Hlt::Stage::get<LHCb::L0CaloCandidate>() const {
  return m_l0_calo_candidate;
}


template<> inline
void Hlt::Stage::set(const LHCb::L0CaloCandidate* value) {
  SetAllToNull();
  m_l0_calo_candidate = value;
}

template<> inline bool Hlt::Stage::is<LHCb::L0CaloCandidate>() const {
  return m_l0_calo_candidate != 0;
}

// =============================================================================
template<>  inline
  const LHCb::L0MuonCandidate*
            Hlt::Stage::get<LHCb::L0MuonCandidate>() const {
  return m_l0_muon_candidate;
}

template<> inline
        void Hlt::Stage::set(const LHCb::L0MuonCandidate* value) {
  SetAllToNull();
  m_l0_muon_candidate = value;
}

template<> inline bool Hlt::Stage::is<LHCb::L0MuonCandidate>() const {
  return m_l0_muon_candidate != 0;
}

// =============================================================================
template<> inline
      const Hlt::MultiTrack* Hlt::Stage::get<Hlt::MultiTrack>() const {
  return m_multitrack;
}

template<> inline void Hlt::Stage::set(const Hlt::MultiTrack* value) {
  SetAllToNull();
  m_multitrack = value;
}

template<> inline bool Hlt::Stage::is<Hlt::MultiTrack>() const {
  return m_multitrack != 0;
}

// =============================================================================
template<> inline
  const Hlt::L0DiMuonCandidate*
          Hlt::Stage::get<Hlt::L0DiMuonCandidate>() const {
  return m_l0_dimuon_candidate;
}

template<> inline
      void Hlt::Stage::set(const Hlt::L0DiMuonCandidate* value) {
  SetAllToNull();
  m_l0_dimuon_candidate = value;
}

template<> inline
      bool Hlt::Stage::is<Hlt::L0DiMuonCandidate>() const {
  return m_l0_dimuon_candidate != 0;
}
// =============================================================================
template<typename T> inline
    bool Stage::_insertInfo(const std::string& key, const T& value) {
      _checkLock();
      return m_cache.insert<T>(key, value);
    }

template<> inline
    bool Stage::insertInfo(const std::string &key, const bool& value){
      return _insertInfo(key, value);
}

template<> inline
    bool Stage::insertInfo(const std::string &key, const int& value){
      return _insertInfo(key, value);
}

template<> inline
    bool Stage::insertInfo(const std::string &key, const double& value) {
      return _insertInfo(key, value);
}
template<> inline
    bool Stage::insertInfo(const std::string &key, const std::string& value){
      return _insertInfo(key, value);
}
// =============================================================================
template<typename T> inline
    void Stage::_updateInfo(const std::string& key, const T& value) {
      _checkLock();
       m_cache.update<T>(key, value);
}

template<> inline
  void Stage::updateInfo(const std::string& key, const bool& value) {
       _updateInfo(key,value);
}

template<> inline
  void Stage::updateInfo(const std::string& key, const int& value) {
       _updateInfo(key,value);
}

template<> inline
  void Stage::updateInfo(const std::string& key, const double& value) {
       _updateInfo(key,value);
}

template<> inline
  void Stage::updateInfo(const std::string& key, const std::string& value) {
       _updateInfo(key,value);
}
// =============================================================================
template<typename T> inline
    T Stage::_info(const std::string& key, const T& default_value) const {
        return m_cache.info<T>(key, default_value);
}

template<> inline
 bool Stage::info(const std::string& key, const bool& default_value) const {
        return _info(key, default_value);
}

template<> inline
 int Stage::info(const std::string& key, const int& default_value) const {
        return _info(key, default_value);
}

template<> inline
 double Stage::info(const std::string& key, const double& default_value) const {
        return _info(key, default_value);
}

template<> inline
 std::string Stage::info(const std::string& key, 
                                       const std::string& default_value) const {
        return _info(key, default_value);
}
// =============================================================================
} // namespace Hlt

// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline const CLID& Hlt::Stage::clID() const {
  return Hlt::Stage::classID();
}

inline const CLID& Hlt::Stage::classID() {
  return CLID_Stage;
}

#endif ///HltEvent_Stage_H
