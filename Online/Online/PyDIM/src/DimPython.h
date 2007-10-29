#ifndef PYDIM_DIMPYTHON_H
#define PYDIM_DIMPYTHON_H

#include "Python.h"
#include <string>
#include <typeinfo>

/*
 *  DIM namespace declaration
 */
namespace DIM {

  /** @class Caller
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class Caller  {
  public:
    class Unknown {};
    PyObject* m_call;
    const std::type_info* m_type;

  public:
    /// Constructor
    Caller(PyObject* obj);
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

}       // End namespace DIM
#endif  // PYDIM_DIMPYTHON_H
