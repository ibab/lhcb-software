// $Id: HltData.h,v 1.5 2008-06-23 11:22:20 graven Exp $
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
  class Object: public DataObject {
    Object(){}
    virtual ~Object() {}
    
    virtual CLID classID() {return DataObject::clID();}
    
    virtual ContainedObject& object() 
    {return *m_obj;}
    
    virtual const ContainedObject& object() const
    {return *m_obj;}
    
    virtual std::vector<Object*>& ancestors() 
    {return m_ancestors;}
    
    virtual const std::vector<Object*>& ancestors() const 
    {return m_ancestors;}
    
  protected:
    ContainedObject* m_obj;
    std::vector<Hlt::Object*> m_ancestors;
  };
  
  
  class Selection : public ContainedObject, public DataObject {
  public:
    Selection(const stringKey& id):m_id(id) {}
    virtual ~Selection() {}

  public:

    const stringKey& id() const {return m_id;}
    void setDecision(bool value) {m_decision = value;}
    bool decision() const {return m_decision;}
    virtual CLID classID() const {return DataObject::clID();}
    virtual size_t ncandidates() const {return 0;}
    virtual void clean() {m_decision = false;}    
    const std::vector<Hlt::Object*>& objects() const 
    {return m_objects;}
    
    virtual std::vector<Hlt::Object*>& objects() 
    {return m_objects;}
    
    const std::vector<stringKey>& inputSelectionsIDs() const
    {return m_inputSelectionsIDs;}    
    
    template <typename I> // I is assumed to be iterator over a range of Selection*
    void addInputSelectionIDs(I i, I end) {
        while (i!=end) m_inputSelectionsIDs.push_back( (*i++)->id() );
    }

  private:
    std::vector<Hlt::Object*> m_objects;
    std::vector<stringKey> m_inputSelectionsIDs;
    stringKey m_id;
    bool m_decision;
  };
  
  template <class T>
  class TSelection : public std::vector<T*>, public Selection {
  public:
    TSelection(const stringKey& id) : Selection(id) {}
    virtual ~TSelection() {}
    CLID classID() const { return T::classID(); }
    size_t ncandidates() const {return this->size();}
    void clean() { Selection::clean(); this->clear();}
  };
  
  typedef TSelection<LHCb::Track>      TrackSelection;
  typedef TSelection<LHCb::RecVertex> VertexSelection;
}
#endif 
