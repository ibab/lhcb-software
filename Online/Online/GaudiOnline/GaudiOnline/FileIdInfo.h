/** @class FileIdInfo FileIdInfo.h
 **  
 **
 **  @author Albert Puig Navarro (albert.puig@cern.ch)
 **  @date   2008-03-03
 **/

#include "GaudiKernel/DataObject.h"
#define DAQ_FILEID_BANK 255

namespace LHCb {
  class FileIdInfo{
  public:
    int ip0,ip1;
    int l0, l1, l2;
    char pdata[1];
    size_t sizeOf() {  return sizeof(FileIdInfo)+l0+l1+l2;  }
    void reset();
    void setpar(const std::string* data);
    void setipar(const unsigned long* data);
    void setguid(const std::string& data);
    const char* par0() const {  return pdata; }
    const char* par1() const {  return pdata+l0+1; }
    const char* guid() const {  return pdata+l0+l1+1; }
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
