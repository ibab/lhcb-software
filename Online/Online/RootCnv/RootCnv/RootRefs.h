#include <string>

namespace Gaudi {
  class RootRef {
  public:
    int      dbase,container,link,clid,entry;
    unsigned char svc;
    RootRef() {}
    virtual ~RootRef() {}
  };
  class ObjectRefs {
  public:
    std::vector<int>       links;
    std::vector<RootRef>   refs;
    ObjectRefs() {}
    ObjectRefs(const ObjectRefs& r) : links(r.links), refs(r.refs) {}
    ObjectRefs& operator=(const ObjectRefs& r) {
      links = r.links;
      refs = r.refs; 
      return *this;
    }
    virtual ~ObjectRefs() {}
  };
}
