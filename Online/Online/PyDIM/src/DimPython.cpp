#include <iostream>
#include "DimPython.h"
namespace win {
#ifdef _WIN32
  #include <windows.h>
#else
  typedef int CRITICAL_SECTION;
  void InitializeCriticalSection(CRITICAL_SECTION* ) {}
  void EnterCriticalSection(CRITICAL_SECTION*) {}
  void LeaveCriticalSection(CRITICAL_SECTION*) {}
#endif
}

namespace DIM  {

  /** @class InterpreterLock
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class InterpreterLock  {
  private:
    /// Lock environemt
    PyGILState_STATE m_env;
  public:
    /// Standard Constructor
    InterpreterLock();
    /// Standard Destructor
    ~InterpreterLock();
  };

  /// Global variable
  static win::CRITICAL_SECTION s_lock; 

  /** @class LockedSection
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class LockedSection  {
  public:
    /// Standard Constructor
    LockedSection();
    /// Standard Destructor
    ~LockedSection();
  };
  static PyThreadState* s_mainThreadState = 0;
};

static void _init_section()  {
  static bool first = true;
  if ( first )  {
    first = false;
    // Initialize the critical section one time only.
    win::InitializeCriticalSection(&DIM::s_lock);
    PyEval_InitThreads();
    DIM::s_mainThreadState = PyThreadState_Get();
  }
}

/// Standard Constructor
DIM::InterpreterLock::InterpreterLock() {  
  m_env = PyGILState_Ensure(); 
}
DIM::InterpreterLock::~InterpreterLock() {  
  PyGILState_Release(m_env);   
}

/// Standard Constructor
DIM::LockedSection::LockedSection() {  
  win::EnterCriticalSection(&s_lock);
}

DIM::LockedSection::~LockedSection() {  
  win::LeaveCriticalSection(&s_lock);
}

/// Constructor
DIM::Caller::Caller(_Obj obj) : m_call(obj), m_type(&typeid(Unknown)) {
  _init_section();
}

/// Operator implementing real python call. Note that the python interpreter must be locked!
void DIM::Caller::operator()(const char* attr)  {
  PyObject* r = 0;
  // LockedSection sect;
  // InterpreterLock lock;
  //PyEval_AcquireLock();
  PyThreadState* st = PyThreadState_New(s_mainThreadState->interp);
  PyEval_AcquireThread(st);
  //PyThreadState_Swap(st);
  //Py_BEGIN_ALLOW_THREADS;
  r = PyObject_CallMethod(m_call, (char*)attr, "");
  //PyThreadState_Swap(s_mainThreadState);
  PyEval_ReleaseThread(st);
  PyThreadState_Clear(st);
  PyThreadState_Delete(st);
  //PyEval_ReleaseLock();
  //Py_END_ALLOW_THREADS;
  // std::cout << "Result=" << r << std::endl;
  if( r && (r == Py_None || PyInt_AsLong(r)) )  {
    return;
  } 
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
