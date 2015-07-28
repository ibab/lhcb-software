#ifndef ONLINEKERNEL_CPP_VALUE_H
#define ONLINEKERNEL_CPP_VALUE_H

namespace CPP {
  template <class T> class _Item {
  public:
    union {
      const void* cpointer;
      T           value;
    } _data;
    _Item(const void* p) { _data.cpointer = p; }
    T& data()  const { return _data.value; }
  };
  template <class T> class _PtrItem {
  public:
    union {
      const void* cpointer;
      T*          value;
    } _data;
    _PtrItem(const void* p) { _data.cpointer = p; }
    T& data()  const { return *(_data.value); }
    T* ptr() const { return _data.value; }
  };
}
#endif // ONLINEKERNEL_CPP_VALUE_H
