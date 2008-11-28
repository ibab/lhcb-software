/** @class FileIdInfo FileIdInfo.h
 **  
 **
 **  @author Albert Puig Navarro (albert.puig@cern.ch)
 **  @date   2008-03-03
 **/

#include "GaudiKernel/DataObject.h"

namespace LHCb {
  class FileIdInfo{
  public:
    int ip0,ip1;
    int l0, l1;
    char pdata[1];
  };

  class FileIdObject : public DataObject {
  public:
    union  {
      void*       ptr;
      FileIdInfo* info;
    } data;
    FileIdObject() : DataObject() { data.ptr = 0; }
    FileIdObject(size_t len) : DataObject() { data.ptr = ::operator new(len); }
    virtual ~FileIdObject() { if ( data.ptr ) ::operator delete(data.ptr); }
  };
}
