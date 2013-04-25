// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineKernel/dict/PyInteractor.cpp,v 1.3 2008-06-09 08:55:46 frankb Exp $
//  ====================================================================
//  PythonCallback.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_ONLINEKERNEL_PYINTERACTOR_H
#define ONLINE_ONLINEKERNEL_PYINTERACTOR_H

// Framework include files
#if defined(__GCCXML) && !defined(__REAL_COMP)
struct _object;
typedef _object PyObject;
#else
#ifdef _POSIX_C_SOURCE  // Gets redefined in python.h
#undef _POSIX_C_SOURCE
#endif
#ifdef _XOPEN_SOURCE    // Gets redefined in python.h
#undef _XOPEN_SOURCE
#endif
#include "Python.h"
#define __COMPILE_PYTHON
#endif

/*
 * CPP namespace
 */
namespace CPP {

  // Forward declarations
  class PythonCall;
  class ControlsManager;

  class PyInteractor : public Interactor {
    /// Dressed python callback
    PythonCall* m_call;
    /// Sensor callback data
    const Event* m_currentEvent;
  public:
    /// Default destructor
    PyInteractor(PyObject* self);
    /// Standard destructor
    virtual ~PyInteractor();
    /// Access Sensor callback  data
    const Event* event() {  return m_currentEvent; }
    /// Sensor callback
    virtual void handle(const Event& ev);
    /// Overloaded Sensor callback: call to event() is valid!
    virtual void handleEvent();
  };
}      // End namespace CPP


#ifdef __COMPILE_PYTHON
namespace CPP {
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
}

static PyThreadState* s_mainThreadState = 0;

static void s_init() {
  static bool first = true;
  if ( first )  {
    first = false;
    ::PyEval_InitThreads();
    s_mainThreadState = PyThreadState_Get();
  }
}

/// Constructor
CPP::PythonCall::PythonCall(PyObject* obj) : m_call(obj), m_type(&typeid(Unknown)) {
  s_init();
}

/// Cleanup and handler of pyuthon errors
static void handlePythonError(PyThreadState* st)  {
  if ( PyErr_Occurred() )   {
    ::PyErr_Print(); 
    ::PyErr_Clear();
  }
  if ( st )  {
    ::PyEval_ReleaseThread(st);
    ::PyThreadState_Clear(st);
    ::PyThreadState_Delete(st);
  }
}

/// Operator implementing real python call. Note that the python interpreter must be locked!
void CPP::PythonCall::operator()(const char* attr)  {
  PyThreadState* st = 0;
  try   {
    PyObject* r = 0;
    st = ::PyThreadState_New(s_mainThreadState->interp);
    ::PyEval_AcquireThread(st);
    r = ::PyObject_CallMethod(m_call, (char*)attr, (char*)"");
    if ( r == 0 )  {
      ::PyErr_Print(); 
      ::PyErr_Clear();
      ::PyEval_ReleaseThread(st);
      // PyThreadState_Clear(st);
      ::PyThreadState_Delete(st);
      return;
    }
    if( r == Py_None )  {
      ::PyEval_ReleaseThread(st);
      // PyThreadState_Clear(st);
      ::PyThreadState_Delete(st);
      return;
    }
    else if(r)  {
      Py_XDECREF( r ) ;
      ::PyEval_ReleaseThread(st);
      // PyThreadState_Clear(st);
      ::PyThreadState_Delete(st);
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
int CPP::PythonCall::hasAttr(const char* attr)  {
  return ::PyObject_HasAttrString(m_call, (char*)attr);
}

/// Execute any python attribute (if its a callback)
void CPP::PythonCall::execVoid(const char* attr)  {
  if ( attr )   {
    (*this)(attr);
    return;
  }
  std::cout << "Invalid Call: No member name given." << std::endl;
}

// Debug printing....
void CPP::PythonCall::print(const char* attr)  {
  std::cout << "Test function - should not be called...." << type() << "[" << m_call << "] ";
  if ( m_call && attr )   {
    std::cout << attr << ": " << hasAttr((char*)attr);
  }
  std::cout << std::endl;
}

/// Default destructor
CPP::PyInteractor::PyInteractor(PyObject* self) : m_currentEvent(0) {
  m_call = new PythonCall(self);
  m_call->m_type = &typeid(*this); 
}

/// Standard destructor
CPP::PyInteractor::~PyInteractor() {
  delete m_call;
}

/// Sensor callback
void CPP::PyInteractor::handle(const Event& ev) {
  m_currentEvent = &ev;
  handleEvent();
  m_currentEvent = 0;
}

/// Overloaded Sensor callback: call to event() is valid!
void CPP::PyInteractor::handleEvent() {
  (*m_call)("handleEvent");  
}

#endif

#endif // ONLINE_ONLINEKERNEL_PYINTERACTOR_H
