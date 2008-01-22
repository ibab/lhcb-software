// $Id: HltData.h,v 1.1 2008-01-22 09:34:59 hernando Exp $
#ifndef HLTBASE_HLTDATA_H 
#define HLTBASE_HLTDATA_H 1

#include <vector>
#include <map>
#include <boost/lexical_cast.hpp>

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ContainedObject.h"
#include "HltBase/EAssertions.h"

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
    Selection(int id):m_id(id) {}
    virtual ~Selection() {}

  public:

    int id() const
    {return m_id;}
    
    void setDecision(bool value) 
    {m_decision = value;}

    bool decision() const
    {return m_decision;}

    virtual CLID classID() 
    {return DataObject::clID();}

    virtual size_t ncandidates() const 
    {return 0;}
    
    virtual void clean() 
    {m_decision = false;}    
    
    const std::vector<Hlt::Object*>& objects() const 
    {return m_objects;}
    
    virtual std::vector<Hlt::Object*>& objects() 
    {return m_objects;}
    
    const std::vector<int>& inputSelectionsIDs() const
    {return m_inputSelectionsIDs;}
    
    std::vector<int>& inputSelectionsIDs()
    {return m_inputSelectionsIDs;}    
    
  protected:

    int m_id;

    bool m_decision;

    std::vector<Hlt::Object*> m_objects;

    std::vector<int> m_inputSelectionsIDs;
      
  };
  
  template <class T>
  class TSelection : public std::vector<T*>, public Selection {
  public:
    TSelection(int id):Selection(id) {}
    virtual ~TSelection() {}
    CLID classID() {return T::classID();}
    size_t ncandidates() const {return this->size();}
    void clean() 
    {m_decision = false,this->clear();}
  };
  
  
  typedef TSelection<LHCb::Track> TrackSelection;
  typedef TSelection<LHCb::RecVertex> VertexSelection;

  class Data : public DataObject {
  public:
    Data() {}
    virtual ~Data();

    void addSelection(int id, Hlt::Selection* sel) {
      if (hasSelection(id)) 
        throw zen::invalid_key(" map has already id "+sid(id));
      m_mapselections[id] = sel;
      m_selectionsIDs.push_back(id);
      m_selections.push_back(sel);
    }

    bool hasSelection(int id) const
    {return (m_mapselections.find(id) != m_mapselections.end());}
    

    const Hlt::Selection& selection(int id) const {
      if (!hasSelection(id)) 
        throw zen::invalid_key(" no key "+sid(id));
      return *(m_mapselections.find(id)->second);
    }
    
    Hlt::Selection& selection(int id) {
      if (!hasSelection(id)) 
        throw zen::invalid_key(" no key "+sid(id));
      return *m_mapselections[id];
    }    

    const std::vector<int>& selectionIDs() const 
    {return m_selectionsIDs;}
    
    const std::vector<Hlt::Selection*>& selections() const
    {return m_selections;}
    
    std::vector<Hlt::Selection*>& selections()
    {return m_selections;}
    
  private:
    
    std::string sid(int id) const
    {return boost::lexical_cast<std::string>(id);}
    
  protected:
    
    std::map<int,Hlt::Selection*> m_mapselections;
    std::vector<int> m_selectionsIDs;
    std::vector<Hlt::Selection*> m_selections;
    
  };  
 
}

#endif 
