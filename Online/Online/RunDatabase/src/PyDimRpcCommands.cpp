#include "PyDimRpcCommands.h"
#include <iostream>
#include <stdexcept>

typedef boost::python::object OBJ;
typedef const std::string&    CSTR;
static PyThreadState* s_mainThreadState = 0;

namespace  {
  class PyDimRpcCommandHandler : public LHCb::DimRpcCommandHandler  {
    std::string m_call;
    OBJ         m_object;

  public:
    /// Initializing constructor
    PyDimRpcCommandHandler(OBJ o, const std::string& call) 
    : m_call(call), m_object(o) {    }
    /// Default destructor
    virtual ~PyDimRpcCommandHandler() {}
    /// Virtual destructor
    virtual void release()  {
      delete this;
    }
    /// Cleanup and handler of pyuthon errors
    void handlePythonError(PyThreadState* st)  {
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
    /// Handler callback
    virtual std::string handle(CSTR msg)   {
      std::string result;
      PyThreadState* st = 0;
      try   {
        st = ::PyThreadState_New(s_mainThreadState->interp);
        ::PyEval_AcquireThread(st);
        PyObject* r = ::PyObject_CallMethod(m_object.ptr(),(char*)m_call.c_str(),"s",msg.c_str());
        int len = ::PyString_Size(r);
        std::string result = len > 0 ? ::PyString_AsString(r) : "";
        ::PyEval_ReleaseThread(st);
        ::PyThreadState_Clear(st);
        ::PyThreadState_Delete(st);
        st = 0;
        return result;
      }
      catch(std::exception& e)  {
        handlePythonError(st);
        throw(e);
      }
      catch(...)  {
        handlePythonError(st);
        throw std::runtime_error("Unknwon exception occurred....");
      }
      return "";
    }
  };
}

/// Initializing constructor
LHCb::PyDimRpcCommandServer::PyDimRpcCommandServer(OBJ obj, CSTR call, CSTR mount)
: DimRpcCommandServer(new PyDimRpcCommandHandler(obj, call), mount)
{
}

/// Default destructor
LHCb::PyDimRpcCommandServer::~PyDimRpcCommandServer()   {
}

/// Overloaded execption handler
std::string LHCb::PyDimRpcCommandServer::handleException(CSTR msg)    {
  std::string result = "(0, '" + msg + "')";
  return result;
}

// Initializing constructor
LHCb::PyDimRpcCommandClient::PyDimRpcCommandClient(OBJ cb, CSTR nam)
: DimRpcCommandClient(nam), m_callback(cb)    { 
}

// Default destructor
LHCb::PyDimRpcCommandClient::~PyDimRpcCommandClient()  {
}

/// Callback execution function
std::string LHCb::PyDimRpcCommandClient::executeCmd(CSTR cmd)   {
  setData((char*)cmd.c_str()); 
  char* val = getString(); 
  ::PyObject_CallFunction(m_callback.ptr(),"s",val ? val : "");
  return "";
}

// Python module definition
BOOST_PYTHON_MODULE(DimRunDb)    {
  using namespace boost::python;

  ::PyEval_InitThreads();
  s_mainThreadState = ::PyThreadState_Get();

  class_<LHCb::PyDimRpcCommandServer>("PyDimRpcCommandServer",init<OBJ, CSTR, CSTR>());

  class_<LHCb::DimRpcCommandClient>("PyDimRpcCommandClient",init<CSTR>())
    .def("executeCmd", &LHCb::DimRpcCommandClient::executeCmd);

}
