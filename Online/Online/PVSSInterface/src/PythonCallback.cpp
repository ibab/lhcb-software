// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/PythonCallback.cpp,v 1.1 2007-03-12 10:07:24 frankb Exp $
//  ====================================================================
//  PythonCallback.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: PythonCallback.cpp,v 1.1 2007-03-12 10:07:24 frankb Exp $

#include "PVSS/PythonCallback.h"
namespace PVSS  {

  /** @class PythonCall
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class PythonCall  {
  public:
    class Unknown {};
    /// Pointer to python object
    PyObject*             m_call;
    /// Type information
    const std::type_info* m_type;

  public:
    /// Constructor
    PythonCall(PyObject* obj);
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

  static PyThreadState* s_mainThreadState = 0;
}

using namespace PVSS;

/// Constructor
PythonCall::PythonCall(PyObject* obj) : m_call(obj), m_type(&typeid(Unknown)) {
  static bool first = true;
  if ( first )  {
    first = false;
    PyEval_InitThreads();
    s_mainThreadState = PyThreadState_Get();
  }
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
void PythonCall::operator()(const char* attr)  {
  PyThreadState* st = 0;
  try   {
    PyObject* r = 0;
    st = PyThreadState_New(s_mainThreadState->interp);
    PyEval_AcquireThread(st);
    r = PyObject_CallMethod(m_call, (char*)attr, "");
    PyEval_ReleaseThread(st);
    PyThreadState_Clear(st);
    PyThreadState_Delete(st);
    if ( r == 0 )  {
      PyErr_Print(); 
      PyErr_Clear();
      return;
    }
    if( r == Py_None )  {
      return;
    }
    else if(r)  {
      Py_XDECREF( r ) ;
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
int PythonCall::hasAttr(const char* attr)  {
  return PyObject_HasAttrString(m_call, (char*)attr);
}

/// Execute any python attribute (if its a callback)
void PythonCall::execVoid(const char* attr)  {
  if ( attr )   {
    (*this)(attr);
    return;
  }
  std::cout << "Invalid Call: No member name given." << std::endl;
}

// Debug printing....
void PythonCall::print(const char* attr)  {
  std::cout << "Test function - should not be called...." << type() << "[" << m_call << "] ";
  if ( m_call && attr )   {
    std::cout << attr << ": " << hasAttr(attr);
  }
  std::cout << std::endl;
}

/// Standard constructor: Argument self: instance to python object implementation
PyDeviceListener::PyDeviceListener(PyObject* self, ControlsManager* m) 
: PVSS::DeviceListener(m), m_call(new PythonCall(self)) 
{ 
  m_call->m_type = &typeid(*this); 
}

/// Default destructor
PyDeviceListener::~PyDeviceListener()       {
  delete m_call;
}

/// DIM overloaded callback calling python itself.
void PyDeviceListener::handleDevices()   { 
  (*m_call)("handleDevices");  
}

/// DIM overloaded callback calling python itself.
void PyDeviceListener::handleDevice()   { 
  (*m_call)("handleDevice");
}
