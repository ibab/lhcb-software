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
namespace Hlt 
{
  // ===========================================================================
  // Forward declarations
  class Stage     ;
  class Candidate ;
  // ===========================================================================
  // Namespace for locations in TDS
  namespace StageLocation 
  {
    // =========================================================================
    /** @var Default 
     *  The default TES-location of HLT-Stages 
     */
    const std::string Default = "Hlt/Stages" ;
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
  class GAUDI_API Stage: public ContainedObject 
  {
    // ========================================================================
    // friend class 
    friend class Hlt::Candidate ;
    // ========================================================================
  public:
    // ========================================================================
    enum Type {
      Unknown       =   0 , 
      L0Muon        =   1 , 
      L0Calo        =   2 , 
      L0DiMuon      =   3 , 
      HltTrack      =   4 , 
      HltVertex     =   5 , 
      HltMultiTrack =   6 , 
      HltStage      =   7       
    } ;  
    // ========================================================================    
  public:
    // ========================================================================
    /// typedef for std::vector of Stage
    typedef std::vector<Stage*>       Vector;
    typedef std::vector<const Stage*> ConstVector;
    /// typedef for ObjectVector of Stage
    typedef ObjectVector<Stage> Container;
    /// The container type for shared di-tracks (without ownership)
    typedef SharedObjectsContainer<Stage>   Selection;
    /// For uniform access to containers in TES (KeyedContainer,SharedContainer)
    typedef Gaudi::NamedRange_<ConstVector> Range;
    /// the actual type of history 
    typedef std::vector<Hlt::NamedEntry> History;
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
      Lock (  Stage* stage , const INamedInterface* locker );
      /// destructor
      ~Lock ();
      // ======================================================================
    public:
      // ======================================================================
      /// add the entries to the history 
      const History& addToHistory ( std::string entry   ) ;
      /// add the entries to the history 
      const History& addToHistory ( const INamedInterface& iface ) ;
      /// add the entries to the history 
      const History& addToHistory ( const History&     entries ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// no default constructor
      Lock() ;                                        // no default constructor
      // ======================================================================
    private:
      // ======================================================================
      SmartIF<INamedInterface> m_locker ;
      Stage* m_stage ;
      // ======================================================================
    };
    // ========================================================================
  public:
    // ========================================================================
    /// Default Destructor
    virtual ~Stage() ;
    // ========================================================================
  public:
    // ========================================================================
    void SetAllToNull();
    // ========================================================================
  public:
    // ========================================================================
    /// Retrieve pointer to class definition structure (virtual) 
    virtual const CLID& clID() const { return Hlt::Stage::classID() ; }
    /// Retrieve pointer to class definition structure (static)
    static const CLID& classID();
    // ========================================================================
  public:
    // ========================================================================
    /// Fill the ASCII output stream
    virtual std::ostream& fillStream(std::ostream& s) const;
    /** Get a wrapped object
     *  @code
     *  const LHCb::Track* track = stage->get<LHCb::Track>();
     *  @endcode
     */
    template<typename T> const T* get() const;
    /** Get a wrapped object (non-const)
     *  @code
     *  LHCb::Track* track = stage->get<LHCb::Track>();
     *  @endcode
     */
    template<typename T> T* get() ;
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
    // ========================================================================
  public: // python-friendly access 
    // ========================================================================
    /// Get a wrapped object (suitable for python)
    const ContainedObject* _get  () const ;
    /// Get a wrapped object (suitable for python)
    const ContainedObject*  get_ () const { return _get() ; }
    // ========================================================================
  public:
    // ========================================================================
#ifndef GOD_NOALLOC
    /// operator new
    static void* operator new(size_t size) 
    {
      return (sizeof(Stage) == size ? boost::singleton_pool<Stage,
              sizeof(Stage)>::malloc() : ::operator new(size));
    }
    /// placement operator new
    /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
    /// it is not needed in libstdc++ >= 3.4
    static void* operator new(size_t size, void* pObj) 
    {
      return ::operator new(size, pObj);
    }
    /// operator delete
    static void operator delete(void* p) 
    {
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
    // ========================================================================
  public: // cache
    // ========================================================================
    /** Check if cache contains value with the specified key
     * @param[in] key
     */
    bool hasInfo(const std::string& key ) { return m_cache.has(key) ; }
    /// Check if cache contains value with the specified key and type
    template<typename T>
    bool hasInfo_(const std::string& key) const 
    { return m_cache.has_<T>(key); }
    /** Add value with the specified type and key
     *  @return true if value was added and false if we try insert an existing key
     */
    template<typename T> bool insertInfo ( const std::string&key , const T& value);
    /// Update  value with the specified type and key
    template<typename T> void updateInfo ( const std::string& key   , 
                                           const T&           value ) ;
    /** Get a cached value associated with the specified type and key.
     *  @returns The value associated with the specified type and key. If the such
     *  value does not exists def is returned.
     */
    template<typename T>
    T info(const std::string& key, const T& default_value) const;
    /** Erase all cached values with the specified key
     * @param[in] key
     */
    void eraseInfo ( const std::string& key )
    {
      _checkLock();
      m_cache.erase(key);
    }
    /// Get a cache object
    const Cache& cache() const { return m_cache;}
    // ========================================================================
  public: // Locker 
    // ========================================================================
    // get the current locker 
    const INamedInterface* locked () const { return m_locker ; }
    // get own candidate 
    const Hlt::Candidate*  owner  () const { return m_owner  ; }  
    // ========================================================================
  protected:
    // ========================================================================
    /// set own candidate 
    void  setOwner ( const Hlt::Candidate* c ) ;          // set own candidate 
    // ========================================================================
  public:
    // ========================================================================
    /// history 
    const History& history() const { return m_history ; }            // history 
    // ========================================================================
    /// the actual type of the stage 
    Type stageType() const ;                   // the actual type of the stage 
    // ========================================================================
  private:
    // ========================================================================
    //
    void _lock   ( const INamedInterface* locker);
    void _unlock ( const INamedInterface* locker);
    void _checkLock() const;
    //
    template<typename T>
    bool _insertInfo(const std::string& key, const T& value);
    //
    template<typename T>
    void _updateInfo(const std::string& key, const T& value);
    //
    template<typename T>
    T _info(const std::string& key, const T& default_value) const;
    // ========================================================================
  private:
    // ========================================================================
    SmartRef<LHCb::Track>             m_track               ;
    SmartRef<LHCb::RecVertex>         m_rec_vertex          ;
    SmartRef<LHCb::L0CaloCandidate>   m_l0_calo_candidate   ;
    SmartRef<LHCb::L0MuonCandidate>   m_l0_muon_candidate   ;
    SmartRef<Hlt::MultiTrack>         m_multitrack          ;
    SmartRef<Hlt::L0DiMuonCandidate>  m_l0_dimuon_candidate ;
    SmartRef<Hlt::Stage>              m_stage               ;
    /// cache 
    Cache                             m_cache               ; // cache 
    /// history 
    History                           m_history             ; // history
    /// locker 
    SmartIF<INamedInterface>          m_locker              ; // locker 
    /// my candidate 
    SmartRef<Hlt::Candidate>          m_owner               ; // my candidate 
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
    std::ostream& toStream ( const Hlt::Stage* c , std::ostream& s ) ;
    // ========================================================================
  } //                                            end of namespace Gaudi::Utils
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
namespace Hlt 
{
  // ==========================================================================
  // Stage
  // ==========================================================================
  template<>
  inline const Hlt::Stage* 
  Hlt::Stage::get<Hlt::Stage>() const { return m_stage ; }
  // ==========================================================================
  template<>
  inline       Hlt::Stage* 
  Hlt::Stage::get<Hlt::Stage>()       { _checkLock () ; return m_stage ; }
  // ==========================================================================
  template<> 
  inline void 
  Hlt::Stage::set<Hlt::Stage>(const Hlt::Stage* value) 
  {
    SetAllToNull();
    m_stage = value;
  }
  // ==========================================================================
  template<> 
  inline bool 
  Hlt::Stage::is<Hlt::Stage>() const { return m_stage != 0; }
  // ==========================================================================
  // Track
  // ==========================================================================
  template<>
  inline const LHCb::Track* 
  Hlt::Stage::get<LHCb::Track>() const { return m_track;}
  // ============================================================================
  template<>
  inline       LHCb::Track* 
  Hlt::Stage::get<LHCb::Track>()       { _checkLock() ; return m_track ; }
  // ============================================================================
  template<> 
  inline void 
  Hlt::Stage::set<LHCb::Track>(const LHCb::Track* value) 
  {
    SetAllToNull();
    m_track = value;
  }
  // ==========================================================================
  template<> 
  inline bool 
  Hlt::Stage::is<LHCb::Track>() const { return m_track != 0; }
  // ==========================================================================
  // RecVertex
  // ==========================================================================
  template<> 
  inline const LHCb::RecVertex* 
  Hlt::Stage::get<LHCb::RecVertex>() const { return m_rec_vertex; }
  // ==========================================================================
  template<> 
  inline       LHCb::RecVertex* 
  Hlt::Stage::get<LHCb::RecVertex>()       { _checkLock() ; return m_rec_vertex; }
  // ==========================================================================
  template<> 
  inline void 
  Hlt::Stage::set<LHCb::RecVertex> ( const LHCb::RecVertex* value ) 
  {
    SetAllToNull();
    m_rec_vertex = value;
  }
  // ==========================================================================
  template<> 
  inline bool 
  Hlt::Stage::is<LHCb::RecVertex>() const 
  { return m_rec_vertex != 0; }
  // ==========================================================================
  // Helper trick with VertexBase
  // ==========================================================================
  template<> 
  inline const LHCb::VertexBase* 
  Hlt::Stage::get<LHCb::VertexBase>() const 
  { return this->get<LHCb::RecVertex> () ; }
  // ==========================================================================
  template<> 
  inline       LHCb::VertexBase* 
  Hlt::Stage::get<LHCb::VertexBase>() 
  { return this->get<LHCb::RecVertex> () ; }
  // ==========================================================================
  template<> 
  inline bool 
  Hlt::Stage::is<LHCb::VertexBase> () const
  { return this->is<LHCb::RecVertex>  () ; }
  // ==========================================================================
  // L0CaloCandidate 
  // ==========================================================================
  template<> 
  inline const LHCb::L0CaloCandidate* 
  Hlt::Stage::get<LHCb::L0CaloCandidate>() const { return m_l0_calo_candidate; }
  // ==========================================================================
  template<> 
  inline       LHCb::L0CaloCandidate* 
  Hlt::Stage::get<LHCb::L0CaloCandidate>() 
  { _checkLock() ; return m_l0_calo_candidate; }
  // ==========================================================================
  template<> 
  inline void 
  Hlt::Stage::set<LHCb::L0CaloCandidate>(const LHCb::L0CaloCandidate* value) 
  {
    SetAllToNull();
    m_l0_calo_candidate = value;
  }
  // ==========================================================================
  template<> 
  inline bool 
  Hlt::Stage::is<LHCb::L0CaloCandidate>() const 
  { return m_l0_calo_candidate != 0; }
  // ==========================================================================
  // L0MuonCandidate
  // ==========================================================================
  template<>  
  inline const LHCb::L0MuonCandidate*
  Hlt::Stage::get<LHCb::L0MuonCandidate>() const { return m_l0_muon_candidate; }
  // ==========================================================================
  template<>  
  inline       LHCb::L0MuonCandidate*
  Hlt::Stage::get<LHCb::L0MuonCandidate>()      
  { _checkLock() ; return m_l0_muon_candidate; }
  // ==========================================================================
  template<> 
  inline void 
  Hlt::Stage::set<LHCb::L0MuonCandidate>(const LHCb::L0MuonCandidate* value) 
  {
    SetAllToNull();
    m_l0_muon_candidate = value;
  }
  // ==========================================================================
  template<> 
  inline bool Hlt::Stage::is<LHCb::L0MuonCandidate>() const 
  { return m_l0_muon_candidate != 0; }
  // ==========================================================================
  // MultiTrack
  // ==========================================================================
  template<> 
  inline const Hlt::MultiTrack* 
  Hlt::Stage::get<Hlt::MultiTrack>() const { return m_multitrack; }
  // ==========================================================================
  template<> 
  inline       Hlt::MultiTrack* 
  Hlt::Stage::get<Hlt::MultiTrack>()      
  { _checkLock() ; return m_multitrack; }
  // ==========================================================================
  template<> 
  inline void 
  Hlt::Stage::set<Hlt::MultiTrack>(const Hlt::MultiTrack* value) 
  {
    SetAllToNull();
    m_multitrack = value;
  }
  // ==========================================================================
  template<> 
  inline bool 
  Hlt::Stage::is<Hlt::MultiTrack>() const 
  { return m_multitrack != 0; }
  // ==========================================================================
  // L0DiMuonCandidate
  // ==========================================================================
  template<> 
  inline const Hlt::L0DiMuonCandidate*
  Hlt::Stage::get<Hlt::L0DiMuonCandidate>() const { return m_l0_dimuon_candidate; }
  // ==========================================================================
  template<> 
  inline       Hlt::L0DiMuonCandidate*
  Hlt::Stage::get<Hlt::L0DiMuonCandidate>() 
  { _checkLock() ; return m_l0_dimuon_candidate; }
  // ==========================================================================
  template<> 
  inline void 
  Hlt::Stage::set<Hlt::L0DiMuonCandidate>(const Hlt::L0DiMuonCandidate* value) 
  {
    SetAllToNull();
    m_l0_dimuon_candidate = value;
  }
  // ==========================================================================
  template<> 
  inline bool 
  Hlt::Stage::is<Hlt::L0DiMuonCandidate>() const 
  {return m_l0_dimuon_candidate != 0; }
  // ==========================================================================
  // * INFO * INFO * INFO * INFO * INFO * INFO * INFO * INFO * INFO * INFO * 
  // ==========================================================================
  template<typename T> 
  inline bool 
  Stage::_insertInfo(const std::string& key, const T& value) 
  {
    _checkLock();
    return m_cache.insert<T>(key, value);
  }
  // ==========================================================================
  template<>
  inline bool 
  Stage::insertInfo<bool>(const std::string &key, const bool& value)
  {
    return _insertInfo(key, value);
  }
  // ==========================================================================
  template<> 
  inline bool Stage::insertInfo<int>(const std::string &key, const int& value)
  {
    return _insertInfo(key, value);
  }
  // ==========================================================================
  template<> 
  inline bool 
  Stage::insertInfo<double>(const std::string &key, const double& value) 
  {
    return _insertInfo(key, value);
  }
  // ==========================================================================
  template<> 
  inline bool 
  Stage::insertInfo<std::string>(const std::string &key, 
                                 const std::string& value)
  {
    return _insertInfo(key, value);
  }
  // ==========================================================================
  template<typename T> 
  inline void 
  Stage::_updateInfo(const std::string& key, const T& value) 
  {
    _checkLock();
    m_cache.update<T>(key, value);
  }
  // ==========================================================================
  template<> 
  inline void 
  Stage::updateInfo<bool>(const std::string& key, const bool& value) 
  { _updateInfo(key,value); }
  // ==========================================================================
  template<> 
  inline void 
  Stage::updateInfo<int>(const std::string& key, const int& value) 
  { _updateInfo(key,value); }
  // ==========================================================================
  template<> 
  inline void 
  Stage::updateInfo<double>(const std::string& key, const double& value) 
  { _updateInfo(key,value); }
  // ==========================================================================
  template<> 
  inline void 
  Stage::updateInfo<std::string>(const std::string& key, 
                                 const std::string& value) 
  { _updateInfo(key,value); }
  // ==========================================================================
  template<typename T> 
  inline T 
  Stage::_info(const std::string& key, const T& default_value) const 
  { return m_cache.info<T>(key, default_value); }
  // ==========================================================================
  template<> 
  inline bool 
  Stage::info<bool>(const std::string& key, const bool& default_value) const 
  { return _info(key, default_value); }
  // ==========================================================================
  template<> 
  inline int 
  Stage::info<int>(const std::string& key, const int& default_value) const 
  { return _info(key, default_value); }
  // ==========================================================================
  template<> 
  inline double 
  Stage::info<double>(const std::string& key, 
                      const double& default_value) const 
  { return _info(key, default_value); }
  // ==========================================================================
  template<> 
  inline std::string 
  Stage::info<std::string>(const std::string& key, 
                           const std::string& default_value) const 
  {
    return _info(key, default_value);
  }
  // ==========================================================================
  /// output operatoor 
  inline std::ostream& operator<<(std::ostream& str, const Stage& obj) 
  {
    return obj .  fillStream ( str ) ;
  }
  /// output operatoor 
  inline std::ostream& operator<<(std::ostream& str, const Stage* obj) 
  {
    if ( 0 == obj ) { return str << "<NULL>" ;}
    return obj -> fillStream ( str ) ;
  }
  // ==========================================================================
} //                                                       end of namespace Hlt 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif ///HltEvent_Stage_H
// ============================================================================
