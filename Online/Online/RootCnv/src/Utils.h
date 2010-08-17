namespace GaudiRoot {
  void popCurrentDataObject();
  void pushCurrentDataObject(DataObject** pobjAddr);
}

namespace {
  /// Release an interface pointer
  template<class T> inline void releasePtr(T*& p)  {
    if ( 0 != p )    {
      p->release();
      p = 0;
    }
  }
  struct DataObjectPush {
    DataObject* m_ptr;
    DataObjectPush(DataObject* p) : m_ptr(p) {
      GaudiRoot::pushCurrentDataObject(&m_ptr);
    }
    ~DataObjectPush() {
      GaudiRoot::popCurrentDataObject();
    }
  };
}
