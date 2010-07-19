// $Id: HltCandidate.h,v 1.3 2010-07-19 16:23:52 ibelyaev Exp $ 
// ============================================================================
#ifndef HltEvent_Candidate_H
#define HltEvent_Candidate_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
#include <ostream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/NamedRange.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/SmartRefVector.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "Event/HltStage.h"
// ============================================================================
#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif
// ============================================================================
// Allocators from  Gaudi
// ============================================================================
#include "GaudiKernel/boost_allocator.h"
// ============================================================================
// Forward declarations
// ============================================================================
namespace Hlt
{
  // ==========================================================================
  // Namespace for locations in TDS
  namespace CandidateLocation 
  {
    // =========================================================================
    /** @var Default 
     *  The defaull TES-location of HLT-candidates 
     */
    static const std::string& Default = "Hlt/Candidates";
    // =========================================================================
  } //                                   end of namespace Hlt::CandidateLocation 
  // ===========================================================================
  /** @class Candidate Event/Candidate.h
   *
   * Shows mutation history of tracks, particles,... (which are wrapped by Phases objects)
   * Store processing history: list of algorithms (Workers)
   *
   * @author alexander.mazurov@gmail.com
   *
   * @example Test.py
   * @example Ex1Writer.cpp
   * @example Ex1Updater.cpp
   * @example Ex1Observer.cpp
   *
   * created Wed May 26 15:35:22 2010
   * 
   */
  class Candidate: public ContainedObject
  {
  public:
    // ========================================================================
    /// typedef for std::vector of Candidate
    typedef std::vector<Candidate*> Vector;
    typedef std::vector<const Candidate*> ConstVector;
    /// typedef for ObjectVector of Candidate
    typedef ObjectVector<Candidate> Container;
    /// The container type for objects
    typedef SharedObjectsContainer<Hlt::Candidate> Selection;
    /// For uniform access to containers in TES (KeyedContainer,SharedContainer) 
    typedef Gaudi::NamedRange_<ConstVector> Range;
    /// For uniform access to containers in TES (KeyedContainer,SharedContainer) 
    typedef Gaudi::NamedRange_<Vector> RangeModifiable;
    /// Phases
    typedef SmartRefVector<Hlt::Stage> Stages;
    /// Phases iterator (const)
    typedef SmartRefVector<Hlt::Stage>::const_iterator StagesConstIterator;
    /// Worker (Algorithm name) 
    typedef std::string Worker;
    /// Workers (Algorithm names) 
    typedef std::vector<Worker> Workers;
    /// Workers iterator (const)
    typedef Workers::const_iterator WorkersConstIterator;
    // ========================================================================
  public:
    // ========================================================================
    /// Destructor
    virtual ~Candidate() ;
    // ========================================================================
  public:
    // ========================================================================    
    /// Retrieve pointer to class definition structure     (virtual)
    virtual const CLID& clID() const { return Hlt::Candidate::classID() ; }
    /// Retrieve pointer to class definition structure     (static) 
    static const CLID& classID();
    // ========================================================================
  public: // standard accessors 
    // ========================================================================
    /// Add worker
    void addToWorkers(const std::string& worker ) { m_workers.push_back(worker) ; }
    /// Get the initial stage
    const Hlt::Stage* initiatorStage() const
    { if ( m_stages.empty()) return NULL ; return m_stages.front() ; }
    /// Get the current stage
    const Hlt::Stage* currentStage() const
    { if ( m_stages.empty()) return NULL ; return m_stages.back () ; }
    /// Last worker (algotithm name)
    Worker lastWorker() const
    { if ( m_workers.empty()) return "" ;  return m_workers.back() ; }
    /// print
    virtual std::ostream& fillStream(std::ostream& s) const;
    /// Retrieve const  list of workers
    const Workers& workers() const { return m_workers ; }
    /// Retrieve (const)  Reference to the stages
    inline const SmartRefVector<Hlt::Stage>& stages() const { return m_stages ; }
    /// Retrieve (non-const)  Reference to the stages
    inline       SmartRefVector<Hlt::Stage>& stages()       { return m_stages ; }
    /// Update  Reference to the stages
    void setStages(const SmartRefVector<Hlt::Stage>& value) { m_stages = value ; }
    /// Add to (pointer) reference to the stages
    void addToStages(const Hlt::Stage* value) { m_stages.push_back(value); }
    /// Remove from  reference to the stages
    void removeFromStages(const Hlt::Stage* value) ;
    /// Clear  Reference to the stages
    void clearStages() { m_stages.clear() ; }
    // ========================================================================
  public: // standard 
    // ========================================================================  
#ifndef GOD_NOALLOC
    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(Candidate) == size ?
               boost::singleton_pool<Candidate, sizeof(Candidate)>::malloc() :
               ::operator new(size) );
    }
    /// placement operator new
    /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
    /// it is not needed in libstdc++ >= 3.4
    static void* operator new ( size_t size, void* pObj )
    {
      return ::operator new (size,pObj);
    }
    /// operator delete
    static void operator delete ( void* p )
    {
      boost::singleton_pool<Candidate, sizeof(Candidate)>::is_from(p) ?
        boost::singleton_pool<Candidate, sizeof(Candidate)>::free(p) :
        ::operator delete(p);
    }
    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete (p, pObj);
    }
    // ========================================================================
  #endif
    // ========================================================================
  private: // the data
    // ========================================================================
    /// list of workers 
    Workers                     m_workers ;  // list of workers
    /// list of stages 
    SmartRefVector<Hlt::Stage>  m_stages  ;  // Reference to the stages
    // ========================================================================
  }; // class Candidate
  // ==========================================================================
  /// Definition of vector container type for Candidate
  typedef Hlt::Candidate::Container Candidates;
  // ==========================================================================
  inline std::ostream& operator<< (std::ostream& str, const Candidate& obj)
  {
    return obj.fillStream(str);
  }
  // ===========================================================================
} //                                                       end of namespace LHCb
// ============================================================================
// the END 
// ============================================================================
#endif  /// HltEvent_Candidate_H
// ============================================================================

