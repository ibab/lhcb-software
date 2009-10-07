/** @class FileIdInfo FileIdInfo.h
 **  
 **
 **  @author Albert Puig Navarro (albert.puig@cern.ch)
 **  @date   2008-03-03
 **/

#include "GaudiKernel/DataObject.h"
#include "MDF/MDFHeader.h"

namespace LHCb {

  class FileIdInfo    {
  public:
    int id, ip0,ip1,checksum;
    int l0, l1, l2;
    char pdata[1];
    size_t sizeOf() const {  return sizeof(FileIdInfo)+l0+l1+l2;  }
    void reset();
    void setID(int data) { id = data; }
    void setpar(const std::string* data);
    void setipar(const unsigned long* data);
    void setguid(const std::string& data);
    const char* par0() const {  return pdata; }
    const char* par1() const {  return pdata+l0; }
    const char* guid() const {  return pdata+l0+l1; }
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
