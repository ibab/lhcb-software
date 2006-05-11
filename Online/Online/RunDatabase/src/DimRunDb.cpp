#include "dis.hxx" 
#include "dic.hxx"
#include "Python.h"
#include <boost/python.hpp>
#include <string>
#include <iostream>
#include <stdexcept>

static PyThreadState* s_mainThreadState = 0;

struct RunDbPyDimWrapper : public DimRpc     {
  boost::python::object m_rundb;
public:
  RunDbPyDimWrapper(boost::python::object rundb, std::string mount, std::string nam)
  : DimRpc(std::string(mount+"/"+nam).c_str(),"C","C"), m_rundb(rundb)
  {
    DimServer::start(mount.c_str()); 
  }
  virtual void rpcHandler()   {
    std::string result;
    PyThreadState* st = 0;
    try   {
      char* data = getString();
      st = ::PyThreadState_New(s_mainThreadState->interp);
      ::PyEval_AcquireThread(st);
      PyObject* r = ::PyObject_CallFunction(m_rundb.ptr(),"s",data);
      int len = ::PyString_Size(r);
      result = len > 0 ? ::PyString_AsString(r) : "";
      ::PyEval_ReleaseThread(st);
      ::PyThreadState_Clear(st);
      ::PyThreadState_Delete(st);
      st = 0;
      setData((char*)result.c_str());
      return;
    }
    catch(std::exception& e)  {
      std::cout << "exception occurred...." << e.what() << std::endl;
    }
    catch(...)  {
      std::cout << "Unknwon exception occurred...." << std::endl;
    }
    if ( PyErr_Occurred() )   {
      PyErr_Print(); 
      PyErr_Clear();
    }
    if ( st )  {
      ::PyEval_ReleaseThread(st);
      ::PyThreadState_Clear(st);
      ::PyThreadState_Delete(st);
    }
    result = "(0, 'Exception occurred while processing request.')";
    setData((char*)result.c_str());
  }
};

class RunDbPyDimClient : public DimRpcInfo   {
  boost::python::object m_callback;
public:
  RunDbPyDimClient(boost::python::object cb, std::string nam)
  : DimRpcInfo(nam.c_str(), -1), m_callback(cb)    { 
  }
  int executeCmd(std::string cmd)   {
    setData((char*)cmd.c_str()); 
    char* val = getString(); 
    if ( val )  {
      std::cout << "Callback RPC Received : " << val << std::endl; 
      ::PyObject_CallFunction(m_callback.ptr(),"s",val);
    }
    else  {
      std::cout << "Callback RPC Received : (Null-Pointer)" << std::endl; 
    }
    return 1;
  }
}; 

BOOST_PYTHON_MODULE(DimRunDb)    {
  using namespace boost::python;
  ::PyEval_InitThreads();
  s_mainThreadState = ::PyThreadState_Get();
  class_<RunDbPyDimWrapper>("RunDbWrapper",init<object, std::string, std::string>())
    ;
  class_<RunDbPyDimClient>("RunDbClient",init<object, std::string>())
    .def("executeCmd", &RunDbPyDimClient::executeCmd)
    ;
}

