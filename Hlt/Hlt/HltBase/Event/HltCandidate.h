// $Id: HltCandidate.h,v 1.2 2010-07-14 15:14:53 amazurov Exp $ 
// ============================================================================
#ifndef HltEvent_Candidate_H
#define HltEvent_Candidate_H 1
// ============================================================================
// Include files
// ============================================================================
#include "Event/HltStage.h"
#include "GaudiKernel/NamedRange.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
// ============================================================================
#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
#endif
#include "GaudiKernel/boost_allocator.h"
#include "GaudiKernel/SmartRefVector.h"
#include <vector>
#include <ostream>

// Forward declarations

namespace Hlt
{

  // Forward declarations
  
  // Class ID definition
  static const CLID CLID_Candidate = 7561;
  
  // Namespace for locations in TDS
  namespace CandidateLocation {
    static const std::string& Default = "Hlt/Candidates";
  }
  

  /** @class Candidate Candidate.h
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
    /// Default Constructor
    Candidate() : m_workers() {}
  
    /// Default Destructor
    virtual ~Candidate() {}
  
    // Retrieve pointer to class definition structure
    virtual const CLID& clID() const;
    static const CLID& classID();
  
    /// Add worker
    void addToWorkers(const std::string& worker);
  
    /** Get initial stage
     */
    const Hlt::Stage* initiatorStage() const;
  
    /** Get current stage
     */
    const Hlt::Stage* currentStage() const;
  
    /** Last worker (algotithm name)
    */
    Worker lastWorker() const;
  
    /// print
    virtual std::ostream& fillStream(std::ostream& s) const;
  
    /** Retrieve const  list of workers
     */
    const Workers& workers() const;
  
    /** Retrieve (const)  Reference to the stages
     */
    const SmartRefVector<Hlt::Stage>& stages() const;
  
	/** Retrieve (const)  Reference to the stages
     */
    SmartRefVector<Hlt::Stage>& stages();
  

    /** Update  Reference to the stages
     */
    void setStages(const SmartRefVector<Hlt::Stage>& value);
    
    /** Add to (pointer) reference to the stages
     */
    void addToStages(const Hlt::Stage* value);
  
  
  
    /** Remove from  reference to the stages
     */
    void removeFromStages(const Hlt::Stage* value);
  
    /** Clear  Reference to the stages
     */
    void clearStages();
  
  
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
  #endif
  protected:

  private:

    Workers                     m_workers; ///< list of workers
    SmartRefVector<Hlt::Stage> m_stages;  ///< Reference to the stages
  
  }; // class Candidate

  /// Definition of vector container type for Candidate
  typedef ObjectVector<Candidate> Candidates;
  
  inline std::ostream& operator<< (std::ostream& str, const Candidate& obj)
  {
    return obj.fillStream(str);
  }
  
} // namespace LHCb;

// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline const CLID& Hlt::Candidate::clID() const
{
  return Hlt::Candidate::classID();
}

inline const CLID& Hlt::Candidate::classID()
{
  return CLID_Candidate;
}

inline const Hlt::Candidate::Workers& Hlt::Candidate::workers() const
{
  return m_workers;
}

inline const SmartRefVector<Hlt::Stage>& Hlt::Candidate::stages() const
{
  return m_stages;
}

inline SmartRefVector<Hlt::Stage>& Hlt::Candidate::stages()
{
  return m_stages;
}

inline void Hlt::Candidate::setStages(const SmartRefVector<Hlt::Stage>& value)
{
  m_stages = value;
}


inline void Hlt::Candidate::addToStages(const Hlt::Stage* value)
{
  m_stages.push_back(value);
}

inline void Hlt::Candidate::removeFromStages(const Hlt::Stage* value)
{
  SmartRef<Hlt::Stage> ref(value);
  SmartRefVector<Hlt::Stage>::iterator iter =
    std::remove(m_stages.begin(), m_stages.end(), ref);
  m_stages.erase(iter, m_stages.end());
}

inline void Hlt::Candidate::clearStages()
{
  m_stages.clear();
}

inline void Hlt::Candidate::addToWorkers(const std::string& worker)
{
m_workers.push_back(worker);
}

inline const Hlt::Stage* Hlt::Candidate::initiatorStage() const
{
 if (stages().empty()) return NULL; return *stages().begin();
}

inline const Hlt::Stage* Hlt::Candidate::currentStage() const
{
 if (stages().empty()) return NULL; return *(stages().end()-1);
}

inline Hlt::Candidate::Worker Hlt::Candidate::lastWorker() const
{
 if (workers().empty()) return NULL;  return *(workers().end()-1);
}

inline std::ostream& Hlt::Candidate::fillStream(std::ostream& s) const
{

            s << "('Candidate',[ ";
            std::string delim = "";
            for (StagesConstIterator i = stages().begin(); i != stages().end(); ++i) {
              s << delim << "'" << **i << "'";
              delim = ",";
            }
            s << "],[";
            delim = "";
            for (WorkersConstIterator i = workers().begin(); i != workers().end(); ++i) {
               s << delim << "'" << *i << "'";
               delim = ", ";
            }
            s << "])";
            return s;
        
}


#endif  /// HltEvent_Candidate_H
