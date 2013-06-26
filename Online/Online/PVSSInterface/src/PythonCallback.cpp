// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/PythonCallback.cpp,v 1.7 2007-11-07 15:15:07 frankm Exp $
//  ====================================================================
//  PythonCallback.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: PythonCallback.cpp,v 1.7 2007-11-07 15:15:07 frankm Exp $

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

static void s_init() {
  static bool first = true;
  if ( first )  {
    first = false;
    PyEval_InitThreads();
    s_mainThreadState = PyThreadState_Get();
  }
}

/// Constructor
PythonCall::PythonCall(PyObject* obj) : m_call(obj), m_type(&typeid(Unknown)) {
  s_init();
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
    r = PyObject_CallMethod(m_call, (char*)attr, (char*)"");
    if ( r == 0 )  {
      PyErr_Print(); 
      PyErr_Clear();
      PyEval_ReleaseThread(st);
      // PyThreadState_Clear(st);
      PyThreadState_Delete(st);
      return;
    }
    if( r == Py_None )  {
      PyEval_ReleaseThread(st);
      // PyThreadState_Clear(st);
      PyThreadState_Delete(st);
      return;
    }
    else if(r)  {
      Py_XDECREF( r ) ;
      PyEval_ReleaseThread(st);
      // PyThreadState_Clear(st);
      PyThreadState_Delete(st);
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
PyCallbackTest::PyCallbackTest(PyObject* self) 
: m_call(new PythonCall(self)) 
{ 
  if ( 0 == self )  {
    std::cout << "PVSS::PyCallbackTest> Warning: SELF==NULL" << std::endl;
  }
  m_call->m_type = &typeid(*this); 
}

/// Default destructor
PyCallbackTest::~PyCallbackTest()       {
  delete m_call;
}

/// DIM overloaded callback calling python itself.
void PyCallbackTest::handle()   { 
  (*m_call)("handle");  
}
/// Default destructor
PyInteractor::PyInteractor(PyObject* self) : m_currentEvent(0) {
  m_call = new PythonCall(self);
  m_call->m_type = &typeid(*this); 
}

/// Standard destructor
PyInteractor::~PyInteractor() {
  delete m_call;
}

/// Sensor callback
void PyInteractor::handle(const Event& ev) {
  m_currentEvent = &ev;
  handleEvent();
  m_currentEvent = 0;
}

/// Overloaded Sensor callback: call to event() is valid!
void PyInteractor::handleEvent() {
  (*m_call)("handleEvent");  
}

/// Standard constructor: Argument self: instance to python object implementation
PyDeviceListener::PyDeviceListener(PyObject* self, ControlsManager* m) 
: PVSS::DeviceListener(m), m_call(0) 
{ 
  setSelf(self);
}

/// Default destructor
PyDeviceListener::~PyDeviceListener()       {
  delete m_call;
}

/// Hack!
void PyDeviceListener::setSelf(PyObject* self)  {
  if ( m_call ) delete m_call;
  m_call = new PythonCall(self);
  if ( 0 == self )  {
    std::cout << "PVSS::PyDeviceListener> Warning: SELF==NULL" << std::endl;
  }
  else  {
    // std::cout << "PVSS::PyDeviceListener> SUCCESS: SELF:" << (void*)self << std::endl;
  }
  m_call->m_type = &typeid(*this); 
}

/// DIM overloaded callback calling python itself.
void PyDeviceListener::handleDevices()   { 
  (*m_call)("handleDevices");  
}

/// DIM overloaded callback calling python itself.
void PyDeviceListener::handleDevice()   { 
  (*m_call)("handleDevice");
}

/// DIM overloaded callback calling python itself.
void PyDeviceListener::handleInvalidDevice()   { 
  (*m_call)("handleInvalidDevice");
}

/// Execute python main program
extern "C" int pvss_pymain(void (*exit_call)(int), int argc, char** argv)  {
  Py_Initialize();
  s_init();
  for(int i = 0; i < argc; ++i) {
    if ( strcmp(argv[i],"-SCRIPT")==0 ) {
      ++i;
      std::cout << "Executing python file:" << argv[i] << std::endl;
      PyRun_AnyFile(fopen(argv[i],"r"),argv[i]);
    }
    else if ( strcmp(argv[i],"-CMD")==0 ) {
      ++i;
      std::cout << "Executing python string:" << argv[i] << std::endl;
      PyRun_SimpleString(argv[i]);
    }
    else if ( strcmp(argv[i],"-PROMPT")==0 ) {
      ++i;
      std::cout << "Executing python command prompt..." << std::endl;
      Py_Main(argc, argv);
    }
  }
  Py_Finalize();
  if ( exit_call ) (*exit_call)(1);
  return 1;
}
