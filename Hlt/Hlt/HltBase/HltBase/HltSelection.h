// $Id: HltSelection.h,v 1.2 2008-07-04 08:07:12 graven Exp $
#ifndef HLTBASE_HLTDATA_H 
#define HLTBASE_HLTDATA_H 1

#include <vector>
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ContainedObject.h"
#include "HltBase/stringKey.h"

namespace LHCb 
{
  class Track;
  class RecVertex;
}

namespace Hlt 
{
  class Selection : public ContainedObject, public DataObject {
  public:
    Selection(const stringKey& id) : m_id(id) {}
    virtual ~Selection() {}

    const stringKey& id() const {return m_id;}
    const std::vector<stringKey>& inputSelectionsIDs() const {return m_inputSelectionsIDs;}    
    
    template <typename I> // I is assumed to be iterator over a range of Selection*
    void addInputSelectionIDs(I i, I end) {
        while (i!=end) m_inputSelectionsIDs.push_back( (*i++)->id() );
    }

    void setDecision(bool value) {m_decision = value;}
    bool decision() const {return m_decision;}

    virtual CLID classID() const {return DataObject::clID();}
    virtual size_t ncandidates() const {return 0;}
    virtual void clean() {m_decision = false;}    

  private:
    std::vector<stringKey> m_inputSelectionsIDs;
    stringKey m_id;
    bool m_decision;
  };
  
  template <typename T>
  class TSelection : public Selection {
  public:
    typedef std::vector<T*>                       container_type;

    TSelection(const stringKey& id) : Selection(id) {}
    virtual ~TSelection() {}

    CLID classID() const { return T::classID(); }
    size_t ncandidates() const  {return m_candidates.size();}
    void clean() { Selection::clean(); m_candidates.clear();}

    // forward container functionality..
    typedef typename container_type::iterator       iterator;
    typedef typename container_type::value_type      value_type;
    typedef typename container_type::size_type       size_type;
    typedef typename container_type::const_iterator const_iterator;
    typedef typename container_type::const_reference const_reference;
    const_iterator begin() const { return m_candidates.begin(); }
    const_iterator end() const { return m_candidates.end(); }
    iterator begin() { return m_candidates.begin(); }
    iterator end()   { return m_candidates.end(); }
    void push_back(T* t) { m_candidates.push_back(t); }
    size_type size() const { return m_candidates.size(); }
    bool empty() const { return m_candidates.empty(); }
    template <typename ITER> void insert(iterator i, ITER begin, ITER end) { m_candidates.insert(i,begin,end); }
  private:
    container_type m_candidates; //@TODO do we own them? -- no, our owner owns them!
  };
  
  typedef TSelection<LHCb::Track>      TrackSelection;
  typedef TSelection<LHCb::RecVertex> VertexSelection;
}
#endif 
