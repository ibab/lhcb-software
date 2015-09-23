/** **************************************************************************
 * \brief This file creates Python wrappers for the DimRpc and DimRpcInfo
 * DIM classes.
 *
 * \authors M. Frank, N. Neufeld, R. Stoica
 * \date Nov. 2007 - September 2008
 *
 * *************************************************************************/

#define DIMCPP_MODULE
extern "C" {
#include "Python.h"
#include "structmember.h"
}
#include "dis.hxx"
#include "dic.hxx"
#include <cctype>
#include <cstdlib>
#include <cstdio>

#include "pydim_utils.cpp"

/** @addtogroup dim
 * @{
 */

/****************************************************************************
 * DimRpc Wrapper
 ****************************************************************************/
class DimRpcWrapper: public DimRpc
/**
 * This class is only a proxy to pass the call from the C++ rpcHandler
 * function to the Python implementation. For this a reference to the
 * real Python object is needed.
 * Need to reimplement the constructor, destructor and rpcHandler
 */
{
  PyObject *self; // note that self can be a derived class

public:
  DimRpcWrapper (char *name,
                 char *format_in,
                 char *format_out,
                 PyObject *pyobj
                 ): DimRpc(name, format_in, format_out)
  {
    if (pyobj) {
      Py_INCREF(pyobj);
    }
    this->self = pyobj;
  }

  void rpcHandler()
  {
    /** This function is called by the DIM library and will not have
     * the Python interpretor lock. All Python API calls must be protected
     * using Ensure() and Release().
     */
    PyObject *res;
    PyGILState_STATE gstate;

    if (!this->self) {
      print("No 'self' Python object found. Can't call to python layer");
      return;
    } else {
      gstate = PyGILState_Ensure();
      res = PyObject_CallMethod(this->self, (char*)"rpcHandler", NULL);
      if (!res) {
        // print ("Invalid call to virtual rpcHandler method %p", res);
        PyErr_Print();
      }
      Py_XDECREF(res);
      PyGILState_Release(gstate);
    }
  }

  ~DimRpcWrapper()
  {
    Py_XDECREF(this->self);
  }

}; //end DimRpcWrapper


typedef struct {
  PyObject_HEAD
  DimRpcWrapper *cpp_dimRpc;
  char *format_in;
  char *format_out;
} DimRpc_Object;


static void
DimRpc_dealloc(DimRpc_Object *self) {
  /** Dealocates a DimRpc object */
  delete self->cpp_dimRpc;
  self->ob_type->tp_free((PyObject*)self);
}


static int
DimRpc_init(DimRpc_Object *self, PyObject *args, PyObject *kwds) {
  /** Allocates a new DimRpc_Object and a DimRpcWrapper inside it
   */
  char *name=NULL, *format_in=NULL, *format_out=NULL;
  static const char *kwlist[] = {"name", "format_in", "format_out", NULL};

  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sss", (char**)kwlist,
                                    &name, &format_in, &format_out)
       ) {
    print("Invalid arguments. Received: %s %s %s",
	  name, format_in, format_out);
    return -1;
  }
  if (!verify_dim_format(format_in) || !verify_dim_format(format_out)) {
    PyErr_SetString(PyExc_AttributeError, "Invalid formats specified");
    return -1;
  }
  self->cpp_dimRpc = new DimRpcWrapper(name, format_in, format_out,
				       (PyObject *)self);
  if (!self->cpp_dimRpc) {
	return -1;
  }
  self->format_in  = new char[strlen(format_in)+1];
  self->format_out = new char[strlen(format_out)+1];
  strcpy(self->format_in, format_in);
  strcpy(self->format_out, format_out);
  print("Created new dimRpc proxy");

  return 0;
}


static PyObject *
DimRpc_new(PyTypeObject *type, PyObject* /* args */, PyObject* /* kwds */)
{
  /** Allocates a new DimRpc_Object and initialises the cpp_dimRpc to NULL */
  DimRpc_Object *self;

  self = (DimRpc_Object*)type->tp_alloc(type, 0);
  if (self != NULL) {
    self->cpp_dimRpc=NULL;
    self->format_in=NULL;
    self->format_out=NULL;
  }

  /* in case of errors self is NULL and the error string is already set */
  return (PyObject *)self;
}


static PyObject *
DimRpc_name (DimRpc_Object *self)  {

  PyObject *res=NULL;

  if (!self->cpp_dimRpc) {
    // should never reach this point
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }
  res = PyString_FromString(self->cpp_dimRpc->getName());

  return res; // res is a new reference and will be owned by the caller
}


static PyObject *DimRpc_getInt (DimRpc_Object *self)  {
  PyObject *res=NULL;
  int cpp_res=0;

  if (!self->cpp_dimRpc) {
    // should never reach this point
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }
  cpp_res = self->cpp_dimRpc->getInt();
  res = PyInt_FromLong( (long)cpp_res );

  return res; // res is a new reference and will be owned by the caller
}


static PyObject *
DimRpc_getFloat (DimRpc_Object *self)  {
  PyObject *res=NULL;
  float cpp_res=0;

  if (!self->cpp_dimRpc) {
    // should never reach this point
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }
  cpp_res = self->cpp_dimRpc->getFloat();
  res = PyFloat_FromDouble((double)cpp_res);

  return res; // res is a new reference and will be owned by the caller
}


static PyObject *
DimRpc_getDouble (DimRpc_Object *self)  {
  PyObject *res=NULL;
  double cpp_res=0;

  if (!self->cpp_dimRpc) {
    // should never reach this point
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }
  cpp_res = self->cpp_dimRpc->getDouble();
  res = PyFloat_FromDouble(cpp_res);

  return res; // res is a new reference and will be owned by the caller
}


static PyObject *
DimRpc_getString (DimRpc_Object *self)  {
  PyObject *res=NULL;
  char *cpp_res=NULL;

  if (!self->cpp_dimRpc) {
    // should never reach this point
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }
  cpp_res = self->cpp_dimRpc->getString();
  res = PyString_FromString(cpp_res);

  return res; // res is a new reference and will be owned by the caller
}


static PyObject *
DimRpc_getSize (DimRpc_Object *self)  {
  PyObject *res=NULL;
  int cpp_res=0;

  if (!self->cpp_dimRpc) {
    // should never reach this point
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }
  cpp_res = self->cpp_dimRpc->getSize();
  res = PyInt_FromLong(cpp_res);

  return res; // res is a new reference and will be owned by the caller
}


static PyObject *
DimRpc_getData(DimRpc_Object * self)   {
  PyObject *res=NULL, *tmp=NULL;
  char *buff=NULL;
  int buff_size=0;

  if (!self->cpp_dimRpc) {
    // should never reach this point
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }
  buff = (char*)self->cpp_dimRpc->getData();
  buff_size = self->cpp_dimRpc->getSize();
  res = dim_buf_to_tuple(self->format_in, buff, buff_size);
  if (res && PyTuple_Size(res)==1){
    // in case there is only an object
    tmp = PyTuple_GetItem(res, 0);
    Py_INCREF(tmp);
    Py_DECREF(res);
    return tmp;
  }

  return res; // res is a new reference and will be owned by the caller
}


static PyObject *
DimRpc_setData (DimRpc_Object* self, PyObject* args)  {
  /** Gets some Python objects and converts them to the appropiate C++ values.
   * The conversion is done based on the arguments supplied when the RPC
   * command was created.
   */
  char *buff=NULL;
  unsigned int buff_size=0;

  if (!self->cpp_dimRpc) {
    // should never reach this point
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }
  if (iterator_to_allocated_buffer(args, self->format_out,
				   (char **)&buff, &buff_size) ) {
    self->cpp_dimRpc->setData(buff, buff_size);
    delete buff;
  } else {
    PyErr_SetString(PyExc_AttributeError,
		    "Could not convert arguments to C buffer");
    return NULL;
  }

  Py_RETURN_NONE;
}


static PyObject *DimRpc_rpcHandler (DimRpc_Object* /* self */) {
  print("RPC call received in C++\n");
  Py_RETURN_NONE;
}


static PyMethodDef DimRpc_methods[] = {
  {"name"      , (PyCFunction)DimRpc_name       , METH_NOARGS,
   "Returns the name of the service."                        },
  {"getData"   , (PyCFunction)DimRpc_getData    , METH_NOARGS,
   "Returns received integer data as a Python int"           },
  {"getInt"    , (PyCFunction)DimRpc_getInt     , METH_NOARGS,
   "Returns received integer data as a Python int"           },
  {"getDouble" , (PyCFunction)DimRpc_getDouble  , METH_NOARGS,
   "Returns received double data as Python float"            },
  {"getFloat"  , (PyCFunction)DimRpc_getFloat   , METH_NOARGS,
   "Returns received float data as a Python float"           },
  {"getString" , (PyCFunction)DimRpc_getString  , METH_NOARGS,
   "Returns received string data as a Python string"         },
  {"getSize"   , (PyCFunction)DimRpc_getSize    , METH_NOARGS,
   "Returns the total received data size as a Python int"    },
  {"setData"   , (PyCFunction)DimRpc_setData    , METH_VARARGS,
   "Sets results data according to the initial format string"},
  {"rpcHandler", (PyCFunction)DimRpc_rpcHandler , METH_NOARGS,
   "Dummy function for the rpcHandler part"                  },
  {NULL, NULL, 0, NULL}  /* Sentinel */
};


static PyMethodDef dimcpp_methods[] = {
  {NULL, NULL, 0, NULL}  /* Sentinel */
};


static PyTypeObject DimRpc_Type = {
  PyObject_HEAD_INIT(NULL)
  0,                          /* ob_size*/
  "dim.DimRpc",               /* tp_name*/
  sizeof(DimRpc_Object),      /* tp_basicsize*/
  0,                          /* tp_itemsize*/
  (destructor)DimRpc_dealloc, /* tp_dealloc*/
  0,                          /* tp_print*/
  0,                          /* tp_getattr*/
  0,                          /* tp_setattr*/
  0,                          /* tp_compare*/
  0,                          /* tp_repr*/
  0,                          /* tp_as_number*/
  0,                          /* tp_as_sequence*/
  0,                          /* tp_as_mapping*/
  0,                          /* tp_hash */
  0,                          /* tp_call*/
  0,                          /* tp_str*/
  0,                          /* tp_getattro*/
  0,                          /* tp_setattro*/
  0,                          /* tp_as_buffer*/
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
  "DimRpc object",            /* tp_doc */
  0,                          /* tp_traverse */
  0,                          /* tp_clear */
  0,                          /* tp_richcompare */
  0,                          /* tp_weaklistoffset */
  0,                          /* tp_iter */
  0,                          /* tp_iternext */
  DimRpc_methods,             /* tp_methods */
  0,                          /* tp_members */
  0,                          /* tp_getset */
  0,                          /* tp_base */
  0,                          /* tp_dict */
  0,                          /* tp_descr_get */
  0,                          /* tp_descr_set */
  0,                          /* tp_dictoffset */
  (initproc)DimRpc_init,      /* tp_init */ /* init is the same as new */
  0,                          /* tp_alloc */
  DimRpc_new,                 /* tp_new */
  _PyObject_Del,              /* tp_free */
  0,                          /* tp_is_gc */
  0,                          /* tp_bases */
  0,                          /* tp_mro */
  0,                          /* tp_cache */
  0,                          /* tp_subclasses */
  0,                          /* tp_weaklist */
  (destructor)DimRpc_dealloc, /* tp_del */
  0                           /* tp_version_tag */
};


/****************************************************************************
 * DimRpcInfo Wrapper
 * **************************************************************************/
class DimRpcInfoWrapper: public DimRpcInfo
/** This class is only a proxy to pass the call from the C++ rpcInfoHandler
 * function to the Python implementation. For this a reference to the
 * real Python object is needed.
 * Need to reimplement the constructor, destructor and rpcInfoHandler
 */
{
  PyObject *self; // note that self can be a derived class

public:
  /** it is much easier to provide a simple setter for the python object
   * reference rather than reimplement all 12 costructors.
   * This needs to be called imediatelly after the init method of the
   * object.
   */
  DimRpcInfoWrapper(const char *name, int time, void *nolink,
		    int nolinksize): DimRpcInfo(name, time, nolink, nolinksize) {}

  DimRpcInfoWrapper(const char *name, void *nolink, int nolinksize):
    DimRpcInfo(name, nolink, nolinksize) {}

  int setPyRef (PyObject *self)
  {
    if (!self)
      return 0;
    Py_INCREF(self);
    this->self=self;
    return 1;
  }

  void rpcInfoHandler()
  {
    PyObject *res;
    PyGILState_STATE gstate;

    if (!this->self) {
      print("No 'self' Python object found. Can't pass the function call");
      return;
    } else {
      gstate = PyGILState_Ensure();
      res = PyObject_CallMethod(this->self, (char*)"rpcInfoHandler", NULL);
      if (res) {
        print ("Invalid call to virtual rpcInfoHandler method %p", (void*)res);
        PyErr_Print();
      } else {
        Py_XDECREF(res);
      }
      PyGILState_Release(gstate);
    }
  }

  ~DimRpcInfoWrapper()
  {
    Py_XDECREF(this->self);
  }

}; //end DimRpcInfoWrapper

typedef struct {
  PyObject_HEAD
  DimRpcInfoWrapper *cpp_dimRpcInfo;
  char *format_in;
  char *format_out;
  PyObject *nolink;
} DimRpcInfo_Object;

static void
DimRpcInfo_dealloc(DimRpcInfo_Object *self) {
  /** Dealocates a DimRpcInfo objet */
  delete self->cpp_dimRpcInfo;
  Py_XDECREF(self->nolink);
  self->ob_type->tp_free((PyObject*)self);
}


static int
DimRpcInfo_init(DimRpcInfo_Object* self, PyObject* args, PyObject* kwds)  {
  /** Allocates a new DimRpcInfo_Object and a DimRpcInfoWrapper inside it.
   */
  char *name=NULL, *format_in=NULL, *format_out=NULL;
  PyObject *arg1=NULL, *arg2=NULL;
  static const char *kwlist[] = {"name", "format_in", "format_out",
                           "time", "nolink", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "sssO|O", (char**)kwlist,
                                   &name,
                                   &format_in,
                                   &format_out,
				                           &arg1, &arg2)
     )
  {
    print("Invalid arguments for RPC Info %s", name);
    return -1;
  }
  if ((arg2 && !PyInt_Check(arg1)) || (!arg1))  {
    /* we have an unknown object as argument arg1 */
    print ("Invalid arguments");
    return -1;
  }
  /* It is useless to perform a back and forth conversion for the default
   * failure arguments
   */
  if (arg2 && PyInt_Check(arg2)) {
    /* this means we have a time argument */
    Py_INCREF(arg2);
    self->cpp_dimRpcInfo = new DimRpcInfoWrapper((const char*)name,
                               PyInt_AsLong(arg1), (void*)NULL, 0);
    self->nolink=arg2;
  } else {
    /* we don't have a time argument and arg1 is the failure param */
    Py_INCREF(arg1);
    self->cpp_dimRpcInfo = new DimRpcInfoWrapper((const char*)name,
						                         (void*)NULL, 0);
    self->nolink=arg1;

  }
  self->cpp_dimRpcInfo->setPyRef( (PyObject *)self );
  if (self->cpp_dimRpcInfo) {
    self->format_in = new char[strlen(format_in)+1];
    self->format_out = new char[strlen(format_out)+1];
    strcpy(self->format_in, format_in);
    strcpy(self->format_out, format_out);
    print("Created new dimRpcInfo proxy");
    return 0;
  }

  return -1;
}

static PyObject *DimRpcInfo_new(PyTypeObject* type, PyObject* /* args */, PyObject* /* kwds */) {
  /** Allocates a new DimRpcInfo_Object and initialises the cpp_dimRpcInfo to NULL
   */
  DimRpcInfo_Object *self = (DimRpcInfo_Object*)type->tp_alloc(type, 0);
  if (self != NULL) {
    self->cpp_dimRpcInfo=NULL;
    self->format_in=NULL;
    self->format_out=NULL;
    self->nolink=NULL;
  }

  /* in case of errors self is NULL and the error string is already set */
  return (PyObject *)self;
}

static PyObject* DimRpcInfo_name (DimRpcInfo_Object * self)  {
  PyObject *res=NULL;

  if (!self->cpp_dimRpcInfo) {
    // should never reach this point
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }
  res = PyString_FromString(self->cpp_dimRpcInfo->getName());

  return res; // res is a new reference and will be owned by the caller
}


static PyObject *DimRpcInfo_getInt (DimRpcInfo_Object * self)  {
  PyObject *res=NULL;
  int *cpp_res=NULL, size;

  if (!self->cpp_dimRpcInfo) {
    // should never reach this point
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }
  /* Making sure we don't block the other Python threads
   * All calls to the DIM API are potentialy blocking
   */
  Py_BEGIN_ALLOW_THREADS
  size = self->cpp_dimRpcInfo->getSize();
  Py_END_ALLOW_THREADS
  if (!size) {
      /* this means the RPC failed and we must provide the
       * default failure parameter
       */
      return  self->nolink;
  }
  Py_BEGIN_ALLOW_THREADS
  cpp_res = (int*)self->cpp_dimRpcInfo->getData();
  Py_END_ALLOW_THREADS
  res = PyInt_FromLong( (long)*cpp_res );

  return res; /* res is a new reference and will be owned by the caller */
}


static PyObject *
DimRpcInfo_getFloat (DimRpcInfo_Object * self)  {
  PyObject *res=NULL;
  float *cpp_res=NULL;
  int size;

  if (!self->cpp_dimRpcInfo) {
    // should never reach this point
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }
  /* Making sure we don't block the other Python threads
   * All calls to the DIM API are potentialy blocking
   */
  Py_BEGIN_ALLOW_THREADS
  size = self->cpp_dimRpcInfo->getSize();
  Py_END_ALLOW_THREADS
  if (!size) {
    /* this means the RPC failed and we must provide the
     * default failure parameter
     */
    return self->nolink;
  }
  Py_BEGIN_ALLOW_THREADS
  cpp_res = (float*)self->cpp_dimRpcInfo->getData();
  Py_END_ALLOW_THREADS
  res = PyFloat_FromDouble( (double)*cpp_res );

  return res; /* res is a new reference and will be owned by the caller */
}


static PyObject *
DimRpcInfo_getDouble (DimRpcInfo_Object * self) {
  PyObject *res=NULL;
  double *cpp_res=NULL;
  int size;

  if (!self->cpp_dimRpcInfo) {
    // should never reach this point
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }
  /* Making sure we don't block the other Python threads
   * All calls to the DIM API are potentialy blocking
   */
  Py_BEGIN_ALLOW_THREADS
  size = self->cpp_dimRpcInfo->getSize();
  Py_END_ALLOW_THREADS
  if (!size) {
    /* this means the RPC failed and we must provide the
     * default failure parameter
     */
    return self->nolink;
  }
  Py_BEGIN_ALLOW_THREADS
  cpp_res = (double*)self->cpp_dimRpcInfo->getData();
  Py_END_ALLOW_THREADS
  res = PyFloat_FromDouble(*cpp_res);

  return res; /* res is a new reference and will be owned by the caller */
}


static PyObject *
DimRpcInfo_getString (DimRpcInfo_Object * self) {
  PyObject *res=NULL;
  char * cpp_res=NULL;
  int size;

  if (!self->cpp_dimRpcInfo) {
    // should never reach this point
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }
  /* Making sure we don't block the other Python threads.
   * All calls to the DIM API are potentialy blocking!
   */
  Py_BEGIN_ALLOW_THREADS
  size = self->cpp_dimRpcInfo->getSize();
  Py_END_ALLOW_THREADS
  if (!size) {
    /* this means the RPC failed and we must provide the
     * default failure parameter
     */
    return  self->nolink;
  }
  Py_BEGIN_ALLOW_THREADS
  cpp_res = self->cpp_dimRpcInfo->getString();
  Py_END_ALLOW_THREADS
  res = PyString_FromString(cpp_res);

  return res; /* res is a new reference and will be owned by the caller */
}


static PyObject *
DimRpcInfo_getSize (DimRpcInfo_Object * self)  {
  /** From the Python point of view, calling this procedure is useless.
   * Proving it for the sake of completion
   */
  PyObject *res=NULL;
  int cpp_res=0;

  if (!self->cpp_dimRpcInfo) {
    // should never reach this point
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }
  /* Making sure we don't block the other Python threads */
  Py_BEGIN_ALLOW_THREADS
  cpp_res = self->cpp_dimRpcInfo->getSize();
  Py_END_ALLOW_THREADS
  res = PyInt_FromLong(cpp_res);

  return res; // res is a new reference and will be owned by the caller
}


static PyObject *DimRpcInfo_getData(DimRpcInfo_Object * self)   {
  PyObject *res=NULL, *tmp=NULL;
  char *buff=NULL;
  int buff_size=0;

  if (!self->cpp_dimRpcInfo) {
    // should never reach this point
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }
  /* Making sure we don't block the other Python threads */
  Py_BEGIN_ALLOW_THREADS
  buff = (char*)self->cpp_dimRpcInfo->getData();
  buff_size = self->cpp_dimRpcInfo->getSize();
  Py_END_ALLOW_THREADS
  if (!buff || !buff_size) {
    /* The callback has failed and we provide the error object */
    return self->nolink;
  }
  res = dim_buf_to_tuple(self->format_out, buff, buff_size);
  /* In case of a single object tuple, return the object not the tuple */
  if (res && PyTuple_Size(res)==1) {
    // in case there is only an object
    tmp = PyTuple_GetItem(res, 0);
    Py_INCREF(tmp);
    Py_DECREF(res);
    return tmp;
  }

  return res; // res is a new reference and will be owned by the caller
}


static PyObject *
DimRpcInfo_setData (DimRpcInfo_Object* self, PyObject* args)  {
  /** Gets some Python objects and converts them to the appropiate C++ values.
   * The conversion is done based on the arguments supplied when the RPC
   * command was created.
   */
  char *buff=NULL;
  unsigned int buff_size=0;

  if (!self->cpp_dimRpcInfo) {
    /* should never reach this point */
    PyErr_SetString(PyExc_AttributeError, "C++ Dim RPC object is NULL");
    return NULL;
  }

  //printPyObject(args);
  if (iterator_to_allocated_buffer(args, self->format_in,
				   (char **)&buff, &buff_size) ) {
    /* The setData() method of DimInfo is blocking. This creates a
     * deadlock between the calling thread that holds the python
     * global interpretor lock and the DIM global lock
     */
    Py_BEGIN_ALLOW_THREADS
    self->cpp_dimRpcInfo->setData(buff, buff_size);
    Py_END_ALLOW_THREADS
    delete buff;
  } else {
    PyErr_SetString(PyExc_AttributeError,
		    "Could not convert arguments to C buffer");
    return NULL;
  }

  Py_RETURN_NONE;
}


static PyObject *
DimRpcInfo_rpcInfoHandler (DimRpcInfo_Object* /* self */) {
  /** Dummy method for the python DimRpcInfo class.
   * Not really needed, provided just to make all the DimRpcInfo class
   * methods visible in Python
   */
  //print("RPC call received in C++\n");
  Py_RETURN_NONE;
}

static PyMethodDef DimRpcInfo_methods[] = {
  {"name"      , (PyCFunction)DimRpcInfo_name       , METH_NOARGS,
   "Returns the name of the service."                        },
  {"getData"   , (PyCFunction)DimRpcInfo_getData    , METH_NOARGS,
   "Returns the received C complex data as a Python objects"           },
  {"getInt"    , (PyCFunction)DimRpcInfo_getInt     , METH_NOARGS,
   "Returns the received C integer data as a Python int"           },
  {"getDouble" , (PyCFunction)DimRpcInfo_getDouble  , METH_NOARGS,
   "Returns the received C double data as a Python float"            },
  {"getFloat"  , (PyCFunction)DimRpcInfo_getFloat   , METH_NOARGS,
   "Returns the received C float data as a Python float"           },
  {"getString" , (PyCFunction)DimRpcInfo_getString  , METH_NOARGS,
   "Returns the received C string data as a Python string"         },
  {"getSize"   , (PyCFunction)DimRpcInfo_getSize    , METH_NOARGS,
   "Returns the total received C data size as a Python int"    },
  {"setData"   , (PyCFunction)DimRpcInfo_setData    , METH_VARARGS,
   "Sets results data according to the initial format string"},
  {"rpcInfoHandler", (PyCFunction)DimRpcInfo_rpcInfoHandler , METH_NOARGS,
   "Dummy function for the rpcHandler part"                  },
  {NULL, NULL, 0, NULL}  /* Sentinel */
};


static PyTypeObject DimRpcInfo_Type = {
  PyObject_HEAD_INIT(NULL)
  0,                         /*ob_size*/
  "dim.DimRpcInfo",          /*tp_name*/
  sizeof(DimRpcInfo_Object), /*tp_basicsize*/
  0,                         /*tp_itemsize*/
  (destructor)DimRpcInfo_dealloc, /*tp_dealloc*/
  0,                         /*tp_print*/
  0,                         /*tp_getattr*/
  0,                         /*tp_setattr*/
  0,                         /*tp_compare*/
  0,                         /*tp_repr*/
  0,                         /*tp_as_number*/
  0,                         /*tp_as_sequence*/
  0,                         /*tp_as_mapping*/
  0,                         /*tp_hash */
  0,                         /*tp_call*/
  0,                         /*tp_str*/
  0,                         /*tp_getattro*/
  0,                         /*tp_setattro*/
  0,                         /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
  "DimRpcInfo object",       /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  DimRpcInfo_methods,        /* tp_methods */
  0,                         /* tp_members */
  0,                         /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  (initproc)DimRpcInfo_init, /* tp_init */ /* init is the same as new */
  0,                         /* tp_alloc */
  DimRpcInfo_new,            /* tp_new */
  _PyObject_Del,              /* tp_free */
  0,                          /* tp_is_gc */
  0,                          /* tp_bases */
  0,                          /* tp_mro */
  0,                          /* tp_cache */
  0,                          /* tp_subclasses */
  0,                          /* tp_weaklist */
  (destructor)DimRpcInfo_dealloc,  /* tp_del */
  0                           /* tp_version_tag */
};

/**@}
 */
#ifndef PyMODINIT_FUNC  /* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC initdimcpp(void)  {
  PyObject *m;

  /* This is needed if the program is threaded.
   * Creates the global interpretor lock
   */
  PyEval_InitThreads();
  /* Initializing Python classes.
   * Basically this functions will make the necessary initializations
   * to the _Type objects.
   */
  if (PyType_Ready(&DimRpc_Type) < 0) {
    print("Could not initialize type DimRpc\n");
    return;
  }
  if (PyType_Ready(&DimRpcInfo_Type) < 0) {
    printf("Could not initialize type DimRpcInfo\n");
    return;
  }
  /* Initializing Python module */
  m = Py_InitModule3("dimcpp", dimcpp_methods, "DIM C++ methods");
  PyEval_InitThreads();
  if (m == NULL) {
    print("Could not initialise dimcpp module\n");
    return;
  }
  /* Making sure that the new class objects will not by cleaned out by the
   * garbage collector
   */
  m = (PyObject *)&DimRpc_Type;
  Py_INCREF(m);
  m = (PyObject *)&DimRpcInfo_Type;
  Py_INCREF(m);
  /* Adding the objects to the created module */
  PyModule_AddObject(m, "DimRpc", (PyObject *)&DimRpc_Type);
  PyModule_AddObject(m, "DimRpcInfo", (PyObject *)&DimRpcInfo_Type);
  /* Magic call to DIM API. Makes sure all the data type sizes are respected.*/
  dic_disable_padding();
}

