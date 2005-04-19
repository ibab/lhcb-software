#ifndef PYDIM_DIMPYTHON_H
#define PYDIM_DIMPYTHON_H

#include <string>
#include <typeinfo>
#include "Python.h"

namespace DIM {

  typedef PyObject* _Obj;

  /** @class Caller
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class Caller  {
  public:
    class Unknown {};
    _Obj m_call;
    const std::type_info* m_type;

  public:
    /// Constructor
    Caller(_Obj obj);
    /// Operator implementing real python call. Note that the python interpreter must be locked!
    void operator()(const char* attr);
    /// Print type of callee
    const char* type()  {  return m_type->name(); }
    /// Check for python attribute
    int hasAttr(const char* attr);
    /// Execute any python attribute (if its a callback)
    void execVoid(const char* attr);
    /// Debug printing....
    void print(const char* attr=0);
  };

}

#endif // PYDIM_DIMPYTHON_H
