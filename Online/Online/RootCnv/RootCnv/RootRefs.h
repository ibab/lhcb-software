#include <string>
#include <vector>

namespace Gaudi {
  struct RootRef {
    int      dbase,container,link,clid,entry;
    unsigned char svc;

    RootRef() 
      : dbase(0),container(0),link(0),clid(0),entry(-1), svc(0) 
    {
    }

    RootRef(const RootRef& c) 
      : dbase(c.dbase),container(c.container),link(c.link),clid(c.clid),entry(c.entry), svc(c.svc) 
    {
    }

    RootRef& operator=(const RootRef& c)  {
      dbase     = c.dbase;
      container = c.container;
      link      = c.link;
      clid      = c.clid;
      entry     = c.entry;
      svc       = c.svc;
      return *this;
    }
  };


  struct ObjectRefs {
    std::vector<int>       links;
    std::vector<RootRef>   refs;
    ObjectRefs() {}
    ObjectRefs(const ObjectRefs& r) : links(r.links), refs(r.refs) {}
    ObjectRefs& operator=(const ObjectRefs& r) {
      links = r.links;
      refs = r.refs; 
      return *this;
    }
    ~ObjectRefs() {}
  };


}
