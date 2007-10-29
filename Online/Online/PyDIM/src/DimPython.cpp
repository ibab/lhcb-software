#include "DimPython.h"
#include <iostream>

namespace DIM  {
  static PyThreadState* s_mainThreadState = 0;
};

static void _init_section()  {
  static bool first = true;
  if ( first )  {
    first = false;
    PyEval_InitThreads();
    DIM::s_mainThreadState = PyThreadState_Get();
  }
}

/// Constructor
DIM::Caller::Caller(PyObject* obj) : m_call(obj), m_type(&typeid(Unknown)) {
  _init_section();
}

/// Cleanup and handler of pyuthon errors
static void handlePythonError(PyThreadState* st)  {
  if ( PyErr_Occurred() )   {
    PyErr_Print(); 
    PyErr_Clear();
  }
  if ( st )  {
    ::PyEval_ReleaseThread(st);
    ::PyThreadState_Clear(st);
    ::PyThreadState_Delete(st);
  }
}

/// Operator implementing real python call. Note that the python interpreter must be locked!
void DIM::Caller::operator()(const char* attr)  {
  PyThreadState* st = 0;
  try   {
    PyObject* r = 0;
    st = PyThreadState_New(s_mainThreadState->interp);
    PyEval_AcquireThread(st);
    r = PyObject_CallMethod(m_call, (char*)attr, "");
    PyEval_ReleaseThread(st);
    PyThreadState_Clear(st);
    PyThreadState_Delete(st);
    if( r && (r == Py_None || PyInt_AsLong(r)) )  {
      return;
    }
  }
  catch(std::exception& e)  {
    std::cout << "Exception occurred:" << e.what() << std::endl;
    handlePythonError(st);
  }
  catch(...)  {
    std::cout << "Unknwon exception occurred...." << std::endl;
    handlePythonError(st);
  }
  st = 0;
  PyErr_Print(); 
}

/// Check for python attribute
int DIM::Caller::hasAttr(const char* attr)  {
  return PyObject_HasAttrString(m_call, (char*)attr);
}

/// Execute any python attribute (if its a callback)
void DIM::Caller::execVoid(const char* attr)  {
  if ( attr )   {
    (*this)(attr);
    return;
  }
  std::cout << "Invalid Call: No member name given." << std::endl;
}

// Debug printing....
void DIM::Caller::print(const char* attr)  {
  std::cout << "Test function - should not be called...." << type() << "[" << m_call << "] ";
  if ( m_call && attr )   {
    std::cout << attr << ": " << hasAttr(attr);
  }
  std::cout << std::endl;
}
