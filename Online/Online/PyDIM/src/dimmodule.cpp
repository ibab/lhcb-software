#define DIMC_MODULE
extern "C" {
#include <Python.h>
#include "structmember.h"
#include "dic.h"
#include "dis.h"
#include "dim_common.h"
}
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <string>

#ifndef _WIN32
#include <unistd.h>
#else
#define HOST_NAME_MAX 256
#include <winsock.h>
#endif


#include "pydim_utils.cpp"

using namespace std;

static int server_active = 0;
static char server_name[HOST_NAME_MAX + 1];

PyObject* dim_buf_to_tuple(const char *schema, const char *buf, int len);

static PyObject* dim_dis_start_serving(PyObject* /* self */, PyObject *args)  {
  char *name = NULL;
  if (server_active) 
    Py_RETURN_NONE;
  if (!PyArg_ParseTuple(args, "|s", &name)) 
    return NULL;
  if (name) 
    strncpy(server_name, name, HOST_NAME_MAX + 1);
  else {
    gethostname(server_name, HOST_NAME_MAX);
    printf("No server name specified. Using machine hostname.\n");
  }
  if (!dis_start_serving(server_name)) {
    PyErr_SetString(PyExc_RuntimeError, "Failed to activate DIM server");
    return NULL;
  }
  server_active=1;
  Py_RETURN_NONE; // no return values :-)
}

static PyObject* dim_dis_stop_serving(PyObject* /* self */, PyObject* /* args */) {
  /* Call: void dis_stop_serving(void)
   */
  dis_stop_serving(); // no return values :-)
  server_active=0;
  Py_RETURN_NONE;
}

static PyObject* dim_dis_set_dns_node(PyObject* /* self */, PyObject* args) {
  /* calls dis_set_dns_node(char* node)
     return success or failure
  */
  char* name = NULL;
  if ( !PyArg_ParseTuple(args, "s", &name) ) {
    printf("Incorect arguments passed\n");
    return NULL;
  }
  if ( !dis_set_dns_node(name) )
    return NULL;
  Py_RETURN_NONE;
}

static PyObject* dim_dis_get_dns_node(PyObject* /* self */, PyObject* /* args */) {
  /* calls dis_get_dns_node(char* node)
     the function should return the DNS node
  */
  char* names=(char*)malloc(256*sizeof(char));
  //if ( !PyArg_ParseTuple(args, "s", &names) )
  //     return NULL;
  if ( !dis_get_dns_node(names) ) {
    return NULL;
  }
  return Py_BuildValue("s", names);
}

static PyObject* dim_dis_set_dns_port(PyObject* /* self */, PyObject* args) {
  /* calls dis_set_dns_port(int port)
     return success or failure
  */
  unsigned int port;
  if ( !PyArg_ParseTuple(args, "I", &port) ) {
    PyErr_SetString(PyExc_TypeError, 
                    "Argument 'port' must be a pozitive integer"
                    );
    return NULL;
  }
  if ( !dis_set_dns_port(port) ) {
    return NULL;
  }
  printf("New port is %d\n", port);
  Py_RETURN_NONE;
}

static PyObject* dim_dis_get_dns_port(PyObject* /* self */, PyObject* /* args */) {
  /* calls dis_get_dns_port()
     the function should return the DNS port
  */
  int port;
  port = dis_get_dns_port();
  return Py_BuildValue("i", port);
}


typedef struct {
  PyObject* self;
  PyObject* func;
} PyCallback; //for Python callbacks

static PyCallback dis_callbackExitHandler_func,
  dis_callbackErrorHandler_func,
  dis_callbackClientExitHandler_func;

static void dim_dis_callbackExitHandler(int* code) {
  PyObject *arg, *res;

  if ( dis_callbackExitHandler_func.self ) {
    //res = PyObject_CallMethod(dis_callbackExitHandler_func.self,
    //                          dis_callbackExitHandler_func.func,
    //                           "i" ,code);
    //Py_XDECREF(res);
  } else if (dis_callbackExitHandler_func.func) {
    arg = Py_BuildValue("i", *code);
    res = PyEval_CallObject(dis_callbackExitHandler_func.func, arg);
    Py_DECREF(arg);
    Py_XDECREF(res);
    return;
  }
  else
    return;
}

static void
dim_dis_callbackErrorHandler(int severity, int error_code, char* message) {
  PyObject *arg, *res;

  if ( dis_callbackErrorHandler_func.self ) {
    //res = PyObject_CallMethod(dis_callbackErrorHandler_func.self,
    //                          dis_callbackErrorHandler_func.func,
    //                          "i" ,code);
    //Py_XDECREF(res);
  }
  else if (dis_callbackErrorHandler_func.func) {
    arg = Py_BuildValue("iis", severity, error_code, message);
    res = PyEval_CallObject(dis_callbackErrorHandler_func.func, arg);
    Py_DECREF(arg);
    Py_XDECREF(res);
    return;
  }
  else
    return;
}

static void
dim_dis_callbackClientExitHandler(int* tag) {
  /*Interface function with signature: void client_exit_user_routine (int* tag)
    Calls the associated Python function.
  */
  PyObject *arg, *res;

  if ( dis_callbackClientExitHandler_func.self ) {
    //res = PyObject_CallMethod(dis_callbackErrorHandler_func.self,
    //                          dis_callbackErrorHandler_func.func,
    //                           "i" ,code);
    //Py_XDECREF(res);
  }
  else if (dis_callbackClientExitHandler_func.func) {
    arg = Py_BuildValue("i", *tag);
    res = PyEval_CallObject(dis_callbackClientExitHandler_func.func, arg);
    Py_DECREF(arg);
    Py_XDECREF(res);
    return;
  }
  else
    return;
}

static PyObject*
dim_dis_add_exit_handler(PyObject* self, PyObject* args) {
  PyObject *temp;
  
  if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
    if (!PyCallable_Check(temp)) {
      PyErr_SetString(PyExc_TypeError, "parameter must be callable");
      return NULL;
    }
    
    Py_XINCREF(temp); /* Adds a reference to the new callback */
    Py_XINCREF(self);
    Py_XDECREF(dis_callbackExitHandler_func.self);/* Dispose of previous callback */
    Py_XDECREF(dis_callbackExitHandler_func.func);
    dis_callbackExitHandler_func.self = self;
    dis_callbackExitHandler_func.func = temp;
    print("Got function pointer %p and obj pointer %p\n", temp, self);
  } else {
    PyErr_SetString(PyExc_TypeError, 
                    "Argument has to be a function or a bound method");
    return NULL;
  }
  dis_add_exit_handler(dim_dis_callbackExitHandler);

  Py_RETURN_NONE;
}


typedef void* func(int*);

static PyObject*
dim_dis_add_error_handler(PyObject* self, PyObject* args) {
  PyObject *temp;
  
  if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
    if (!PyCallable_Check(temp)) {
      PyErr_SetString(PyExc_TypeError, "Parameter must be callable");
      return NULL;
    }
    /* Add a reference to new callback */
    Py_XINCREF(temp);
    Py_XINCREF(self);
    /* Dispose of previous callback */
    Py_XDECREF(dis_callbackErrorHandler_func.self);  
    Py_XDECREF(dis_callbackErrorHandler_func.func);
    dis_callbackErrorHandler_func.self = self;
    dis_callbackErrorHandler_func.func = temp;
    //print("Got function pointer %p and obj pointer %p", temp, self);
  } else {
    PyErr_SetString(PyExc_TypeError, "Argument is not a function or a bound method");
    return NULL;
  }
  dis_add_error_handler(dim_dis_callbackErrorHandler);

  Py_RETURN_NONE;
}

static PyObject*
dim_dis_add_client_exit_handler(PyObject* self, PyObject* args) {
  PyObject *temp;
  if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
    if (!PyCallable_Check(temp)) {
      PyErr_SetString(PyExc_TypeError, "parameter must be callable");
      return NULL;
    }
    /* Add a reference to new callback */
    Py_XINCREF(temp);
    Py_XINCREF(self);
    /* Dispose of previous callback */
    Py_XDECREF(dis_callbackClientExitHandler_func.self);  
    Py_XDECREF(dis_callbackClientExitHandler_func.func);
    dis_callbackClientExitHandler_func.self = self;
    dis_callbackClientExitHandler_func.func = temp;
    printf("Client exit handler: Got function pointer %p and obj pointer %p", 
           temp, self);
  } else {
    PyErr_SetString(PyExc_TypeError, 
                    "Argument has to be a function or a bound method");
    return NULL;
  }
  dis_add_client_exit_handler(dim_dis_callbackClientExitHandler);
  
  Py_RETURN_NONE;
}

static PyObject*
dim_dis_selective_update_service(PyObject* /* self */, PyObject* args) {
  /* Calls: int dis_selective_update_service (int service_id, int** client_ids)
   */
  int* client_ids=NULL, res;
  PyObject* listOrTuple;
  int service_id;

  if (!PyArg_ParseTuple(args, "iO;list or tuple", &service_id, &listOrTuple)) {
    PyErr_SetString(PyExc_TypeError, 
       "Invalid arguments: expected and integer and a list/tuple of integers");
    return NULL;
  }
  if (!listOrTuple2Int(listOrTuple, &client_ids)) {
    PyErr_SetString(PyExc_TypeError, 
                    "Second argument must a list/tuple of integers"
                    );
    return NULL;
  }
  res = dis_selective_update_service(service_id, client_ids);
 
  return Py_BuildValue("i", res);
}

static PyObject*
dim_dis_set_quality(PyObject* /* self */, PyObject* args) {
  /*Calls: void dis_set_quality (service_id, quality)
   */
  unsigned int service_id;
  int quality;

  if (!PyArg_ParseTuple(args, "Ii", &service_id, &quality)) {
    PyErr_SetString(PyExc_TypeError, 
            "Invalid arguments: expected an unsigned integer and an integer");
    return NULL;
  }
  dis_set_quality(service_id, quality);
  Py_RETURN_NONE;
}

static PyObject*
dim_dis_set_timestamp(PyObject* /* self */, PyObject* args) {
  /*Calls: void dis_set_timestamp(unsigned int service_id, int secs, int millisecs)
   */
  unsigned int service_id;
  int secs, milisecs;

  if (!PyArg_ParseTuple(args, "Iii", &service_id, &secs, &milisecs)) {
    PyErr_SetString(PyExc_TypeError, 
           "Invalid arguments: expected an unsigned integer and two integers");
    return NULL;
  }
  dis_set_timestamp(service_id, secs, milisecs);
  
  Py_RETURN_NONE;
}

static PyObject*
dim_dis_remove_service(PyObject* /* self */, PyObject* args) {
  /*Calls: int dis_remove_service (unsigned int service_id)
   */
  unsigned int service_id;
  int res;

  if ( !PyArg_ParseTuple(args, "I", &service_id) ) {
    PyErr_SetString(PyExc_TypeError, 
                    "Invalid argument: expected an unsigned integer");
    return NULL;
  }
  res = dis_remove_service(service_id);
  
  return Py_BuildValue("i", res);
}

static PyObject*
dim_dis_get_next_cmnd(PyObject* /* self */, PyObject* args) {
  /* Calls: int dis_get_next_cmnd (long* tag, int* buffer, int* size)
     Problem: the way the results are received is not done in a "pythonic" way.
     Better to return a tuple with all the results.
     INFO: the DIM function implementation does not define a value for buffer,
     size or tag in case a command is not found.
  */
  int res=0, *buffer, size;
  long tag=0;
  PyObject* tmp;

  if ( !PyArg_ParseTuple(args, "I", &size) ) {
    PyErr_SetString(PyExc_TypeError, 
                    "Invalid argument: expected an unsigned integer");
    return NULL;
  }
  buffer = (int*)malloc(size*sizeof(int));
  res = dis_get_next_cmnd(&tag, buffer, &size);
  tmp = Py_BuildValue("(iis#)", res, tag, buffer, size);
  free(buffer);

  return tmp;
}

static PyObject*
dim_dis_get_client(PyObject* /* self */, PyObject* args) {
  /*Calls: int dis_get_client (char* name)
   */
  char* name; int res;

  if ( !PyArg_ParseTuple(args, "s", &name) ) {
    PyErr_SetString(PyExc_TypeError, "Invalid argument: expected an string");
    return NULL;
  }
  res = dis_get_client(name);

  return Py_BuildValue("i", res);
}

static PyObject*
dim_dis_get_conn_id(PyObject* /* self */, PyObject* /* args */) {
  /*Calls: int dis_get_conn_id ()
   */
  int res;
  res = dis_get_conn_id();

  return Py_BuildValue("i", res);
}

static PyObject*
dim_dis_get_timeout(PyObject* /* self */, PyObject* args) {
  /*Calls: int dis_get_timeout (unsigned int service_id, int client_id)
   */
  unsigned int service_id;
  int client_id, res;
  if ( !PyArg_ParseTuple(args, "Ii", &service_id, &client_id) ) {
    PyErr_SetString(PyExc_TypeError, 
                    "Invalid argument: expected an unsigned int and an int");
    return NULL;
  }
  res = dis_get_timeout(service_id, client_id);

  return Py_BuildValue("i", res);
}

static PyObject*
dim_dis_get_client_services(PyObject* /* self */, PyObject* args) {
  /*Calls: char* dis_get_client_services (int conn_id)
    Info: what happens when the list is empty? None or empty string
  */
  char* res=NULL;
  int conn_id;
  if ( !PyArg_ParseTuple(args, "i", &conn_id) ) {
    PyErr_SetString(PyExc_TypeError, "Invalid argument: expected an int");
    return NULL;
  }
  res = dis_get_client_services(conn_id);

  return Py_BuildValue("s", res);
}

static PyObject*
dim_dis_set_client_exit_handler(PyObject* /* self */, PyObject* args) {
  /*Calls: void dis_set_client_exit_handler (int conn_id, int tag)
   */
  //char* res=NULL;
  int conn_id, tag;
  if ( !PyArg_ParseTuple(args, "ii", &conn_id, &tag) ) {
    PyErr_SetString(PyExc_TypeError, "Invalid argument: expected two ints");
    return NULL;
  }
  dis_set_client_exit_handler(conn_id, tag);

  Py_RETURN_NONE;
}

static PyObject*
dim_dis_get_error_services(PyObject* /* self */, PyObject* /* args */) {
  /*Calls: char* dis_get_error_services (int conn_id)
   */
  char* res=NULL;
  res = dis_get_error_services();

  return Py_BuildValue("s", res);
}

/******************************************************************************/
/* DIS containers for callbacks*/
/******************************************************************************/

typedef struct {
  char *name;
  char *format;
  long tag;
  long uniqueTag;
  PyObject *func;
} CmndCallback;
typedef CmndCallback* CmndCallbackPtr;

map <string,CmndCallbackPtr> cmndName2PythonFunc;
map <long, CmndCallbackPtr> cmndUniqueTag2PythonFunc;

static long commandCallbackNumber = 0;

typedef struct {
  char *name;
  char *format;
  long tag;
  char *buffer;
  unsigned int bufferSize;
  PyObject *func;
} ServiceCallback;
typedef ServiceCallback *ServiceCallbackPtr;
map<unsigned int, ServiceCallbackPtr> serviceID2Callback;

static void dim_callbackCommandFunc(void*, void*, int*);
/******************************************************************************/
/* DIC containers for callbacks and function prototypes                       */
/******************************************************************************/
struct _dic_info_service_callback{
  PyObject* func;
  char* name;
  char* format;
  PyObject* defaultArgument;
  char* buffer;
  int size;
  int tag;
};

struct _dic_cmnd_callback {
  PyObject* func;
  long tag;
};

unsigned long _dic_cmnd_callback_ID;
PyObject* _dic_callback_errorHandler=NULL;

static map<char*,_dic_info_service_callback*>_dic_info_service_buffer2Callback;
static map<unsigned int,_dic_info_service_callback*>_dic_info_service_id2Callback;
static map<unsigned int,_dic_cmnd_callback*>_dic_cmnd_callback_tag2Callback; 

void _dic_error_user_routine_dummy(int, int, char*);
void _dic_info_service_dummy (void*, void*, int*);
void _dic_cmnd_callback_dummy(void*, int*);
void _dic_error_user_routine_dummy(int, int, char*);

/******************************************************************************/

static PyObject*
dim_dis_add_cmnd(PyObject* /* self */, PyObject* args) {
  /* Arguments: name, description, py_function, tag
   * 
   * Implementation details: in order to preserve the C interface, unique 
   * tags are generated for each call. The original tags are preserved with 
   * the python function pointer and passed back when doing a callback.
   * 
   * Calls: unsigned int dis_add_cmnd(name,description,cmnd_user_routine,tag)
   * Where: 
   *  char* name - the name of the command. Will be used for identifing the python callback
   *  char* description
   *  void cmnd_user_routine (long* tag, int* address, int* size) - The long 
   * pointer will be transformed in A long Python object for the callback. The
   *  address will become a string long tag - will be passed back to the python 
   * callback
   */
  unsigned int res=0;
  char *name=NULL, *format=NULL;
  long tag, uniqueTag;
  int sizeFormat, sizeName;
  PyObject *pyFunc;
  CmndCallback *callback;
  string s;

  if (!PyArg_ParseTuple(args, "s#s#Ol", 
			&name, &sizeName, &format, &sizeFormat, &pyFunc, &tag) 
      || !PyCallable_Check(pyFunc) ) {
    PyErr_SetString(PyExc_TypeError, 
		    "Invalid arguments: expected two strings, a function pointer and an integer");
    return NULL;
  }
  Py_INCREF(pyFunc);
  //print("Adding command name %s, format %s and tag %d", name, format, tag);
  uniqueTag = ++commandCallbackNumber;
  callback = (CmndCallback*)malloc(sizeof(CmndCallback));
  callback->func = pyFunc;
  callback->format = (char*)malloc(sizeof(char)*sizeFormat);
  strcpy(callback->format, format);
  callback->name = (char*)malloc(sizeof(char)*sizeName);
  strcpy(callback->name, name);
  callback->tag = tag;
  callback->uniqueTag = uniqueTag;
  s = name;
  if (cmndName2PythonFunc[s])
    free(cmndName2PythonFunc[s]);
  cmndName2PythonFunc[s] = callback;
  cmndUniqueTag2PythonFunc[uniqueTag] = callback;
  res = dis_add_cmnd(name, format, dim_callbackCommandFunc, uniqueTag);
  
  return Py_BuildValue("i", res);
}

static PyObject*
dim_dis_update_service(PyObject* /* self */, PyObject* args) {
  /* Calls: int dis_update_service (int service_id)
   */
  int service_id, res;
  PyObject *svc_args=NULL, *arg;
  ServiceCallbackPtr svc;
  PyGILState_STATE gstate;

  if ( !PyArg_ParseTuple(args, "i|O", &service_id, &svc_args) ){
    PyErr_SetString(PyExc_TypeError, "Argument has to be an integer");
    return NULL;
  }
  svc = serviceID2Callback[service_id];
  if (!svc){
    PyErr_SetString(PyExc_RuntimeError, "ID doesn't match any service");
    return NULL;
  }
  if (!svc_args && svc->func) {    
    arg = Py_BuildValue("(i)", svc->tag);
    gstate = PyGILState_Ensure();
    svc_args = PyEval_CallObject(svc->func, arg);
    PyGILState_Release(gstate);
    if (!svc_args) {
      print("Error in calling python function %p", svc->func);
      PyErr_Print();      
      return NULL;
    }
    Py_DECREF(arg);
  }  
  if (svc_args && !iterator_to_buffer(svc_args, svc->buffer, 
				      svc->bufferSize, svc->format)) 
    {
      PyErr_SetString(PyExc_TypeError, 
		      "Arguments do not match service format"
		      );
      return NULL;
    }
  
  Py_BEGIN_ALLOW_THREADS
  res = dis_update_service(service_id);
  Py_END_ALLOW_THREADS
  
  //print("Returning result %d", res);  
  return Py_BuildValue("i", res);
}


static PyObject*
dim_dis_add_service(PyObject* /* self */, PyObject* args) {
  /*Arguments: name, description, python function, tag
    Calls: unsigned int dis_add_service (char* name,
    char* description,
    int*  address,
    int   size,
    func  user_routine,
    long tag)
    func = void user_routine(long* tag, int** address, int* size)

    Observation: the size of the command is calculated using the
    description parameter so if size is omitted it is considered 1.
  */

  int size=0, name_size, format_size, service_id=0;
  char *name, *format, *buffer;
  long tag;
  PyObject *pyFunc;
  ServiceCallbackPtr svc;

  if (!PyArg_ParseTuple(args, "s#s#Ol", &name, 
                        &name_size, &format, &format_size, &pyFunc, &tag)
      || !PyCallable_Check(pyFunc))
    {
      PyErr_SetString(PyExc_TypeError, 
                      "Invalid arguments: expected two strings and a callable object."
                      );
      return NULL;
    }
  size = getSizeFromFormat(format); 
  if (!size) {
    PyErr_SetString(PyExc_TypeError, "Invalid format of service parameters");
    return NULL;
  }
  buffer = (char*)malloc(size);
  if (!buffer) {
    PyErr_SetString(PyExc_MemoryError, "Could not allocate memory");
    return NULL;
  }
  memset(buffer, '\0', size);
  service_id = dis_add_service(name, format, buffer, size, NULL, tag);
  if (!service_id) {
    PyErr_SetString(PyExc_RuntimeError, "Could not create service");
    free(buffer);
    return NULL;
  }
  Py_INCREF(pyFunc);
  svc = (ServiceCallbackPtr)malloc(sizeof(ServiceCallback));
  svc->name = (char*)malloc(sizeof(char)*name_size);
  if (!svc->name){
    Py_DECREF(pyFunc);
    PyErr_SetString(PyExc_MemoryError, "Could not allocate memory");
    free(buffer);
    return NULL;
  }
  strcpy(svc->name, name);
  svc->format = (char*)malloc(sizeof(char)*format_size);
  if (!svc->name){
    free(buffer);
    free(svc->name);
    Py_DECREF(pyFunc);
    PyErr_SetString(PyExc_MemoryError, "Could not allocate memory");
    return NULL;
  }
  strcpy(svc->format, format);
  svc->tag = tag;
  svc->func = pyFunc;
  svc->buffer = buffer;
  svc->bufferSize = size;
  if (serviceID2Callback[service_id]){
    print("Replacing service %s",svc->name);
    print("Old service struct pointer %p", serviceID2Callback[service_id]);
    //a service with the same name added again?
    Py_XDECREF(serviceID2Callback[service_id]->func);
    free(serviceID2Callback[service_id]->name);
    free(serviceID2Callback[service_id]->format);
    free(serviceID2Callback[service_id]->buffer);
    free(serviceID2Callback[service_id]);
  }
  serviceID2Callback[service_id] = svc;
  print ("Service %s added successfully", svc->name);
  
  return Py_BuildValue("i", service_id);
}

static void
dim_callbackCommandFunc(void* uTag, void* address, int* size) {
  long uniqueTag = *(long*)uTag;
  CmndCallbackPtr pycall;
  PyObject *args, *res;
  char *cptr;
  cptr  = (char*)address;
  PyGILState_STATE gstate;    

  // printf("dim_callbackCommandFunc:: Got unique tag %l \n", uniqueTag);
  // printf("%d %d %d\n", sizeof(int), sizeof(long double), sizeof(char));
  pycall = cmndUniqueTag2PythonFunc[uniqueTag];
  args = dim_buf_to_tuple(pycall->format, (char*)address, *size);
  //printPyObject(args);
  //  print("dim_buf_to_list returned %s %d\n", 
  //        cmndUniqueTag2PythonFunc[uniqueTag]->format, len
  //        );
  //  printf("dim_callbackCommandFunc:: calling python func: %p\n", pyFunc);
  if (!args) {
    /* convertion of the arguments failed */
    print ("Could not convert received DIM buffer to Python objects");
    return;
  }
  /* performing the python callback */
  gstate = PyGILState_Ensure();
  res = PyEval_CallObject(pycall->func, args);
  PyGILState_Release(gstate);
  
  Py_DECREF(args);
  if (!res) {
    PyErr_Print();
  } else {
    Py_DECREF(res);
  }
}

/******************************************************************************/
/* DIC interface functions */
/******************************************************************************/
static PyObject*
dim_dic_set_dns_node(PyObject* /* self */, PyObject* args) {
  /* calls dic_set_dns_node(char* node)
     return success or failure
  */
  char* name = NULL;
  int i;

  if ( !PyArg_ParseTuple(args, "s", &name) ) {
    printf("Incorect service name\n");
    return NULL;
  }
  i = dic_set_dns_node(name);
  
  return Py_BuildValue("i",i);
}

static PyObject*
dim_dic_get_dns_node(PyObject* /* self */, PyObject* /* args */) {
  /* calls dic_get_dns_node(char* node)
     the function should return the DNS node
  */  
  char names[256];

  if ( !dic_get_dns_node(names) )
    names[0] = 0;
  
  return Py_BuildValue("s", names);
}

static PyObject*
dim_dic_set_dns_port(PyObject* /* self */, PyObject* args) {
  /* Calls dic_set_dns_port(int port).
     Return success or failure (1 or 0).
  */
  unsigned int port;
  int i;

  if ( !PyArg_ParseTuple(args, "I", &port) ) {
    PyErr_SetString(PyExc_TypeError, 
                    "Invalid argument: expected a pozitive integer"
                    );
    return NULL;
  }
  i = dic_set_dns_port(port);
  
  return Py_BuildValue("i", i);
}

static PyObject*
dim_dic_get_dns_port(PyObject* /* self */, PyObject* /* args */) {
  /* Calls dis_get_dns_port().
     Returns the DNS port.
  */
  int port;
  port = dim_get_dns_port();
  if ( !port )
    return Py_BuildValue("i", port);
  
  return NULL;
}

static PyObject*
dim_dic_get_id(PyObject* /* self */, PyObject* /* args */) {
  /* Calls: int dic_get_id (char* name)
   * Returns the client name or an empty string if the command was not
   * successful
   */
  char name[256];
  int res;

  res = dic_get_id(name);
  if (!res)
    name[0] = 0;
  
  return Py_BuildValue("s", name);
}

static PyObject*
dim_dic_disable_padding(PyObject* /* self */, PyObject* /* args */) {
  /* Calls: int dic_disable_padding(void)
   * Returns None.
   */
  dic_disable_padding();
  
  Py_RETURN_NONE;
}

static PyObject*
dim_dic_get_quality(PyObject* /* self */, PyObject* args) {
  /* Calls: int dic_get_quality (unsigned int service_id);
     Returns the service quality
  */
  unsigned int service_id;
  int res;

  if ( !PyArg_ParseTuple(args, "I", &service_id) ) {
    PyErr_SetString(PyExc_TypeError, 
                    "Invalid argument: expected an unsigned integer");
    return NULL;
  }
  res = dic_get_quality(service_id);
  
  return Py_BuildValue("i", res);
}

static PyObject*
dim_dic_get_timestamp(PyObject* /* self */, PyObject* args) {
  /* Calls int dic_get_timestamp (service_id, secs, milisecs).
     With: unsigned int service_id
     int *secs
     int *milisecs
     Returns: whether the command was successful
  */
  unsigned int service_id;
  int secs, milisecs=0, res=0;

  if (! PyArg_ParseTuple(args, "Iii", &service_id) ) {
    PyErr_SetString(PyExc_TypeError, 
                    "service id should be an unsigned integer"
                    );
    return NULL;
  }
  res = dic_get_timestamp(service_id, &secs, &milisecs);
  
  return Py_BuildValue("ii", secs, milisecs);
}

static PyObject*
dim_dic_get_format(PyObject* /* self */, PyObject* args) {
  /*Calls: char *dic_get_format (unsigned int service_id)
    Returns a string containing the format
  */
  unsigned int service_id;
  char* format=NULL;

  if (! PyArg_ParseTuple(args, "I", &service_id) ) {
    PyErr_SetString(PyExc_TypeError, 
                    "Service id should be an unsigned integer"
                    );
    return NULL;
  }
  format = dic_get_format(service_id);
  
  return Py_BuildValue("s", format);
}

static PyObject*
dim_dic_release_service(PyObject* /* self */, PyObject* args) {
  /* Calls: void dic_release_service (unsigned int service_id)
     Returns: void
  */
  unsigned int service_id;
  _dic_info_service_callback *tmp;

  if (!PyArg_ParseTuple(args, "I", &service_id)) {
    print("Invalid service id specified");
    PyErr_SetString(PyExc_TypeError, 
                    "Service id should be an unsigned integer"
                    );
    return NULL;
  }
  dic_release_service(service_id);
  tmp = _dic_info_service_id2Callback[service_id];
  if ( !tmp ) {
    print("Service with id %d is not known", service_id);
    Py_RETURN_NONE;
  }
  _dic_info_service_buffer2Callback.erase(tmp->buffer);
  _dic_info_service_id2Callback.erase(service_id);
  free(tmp->buffer);
  free(tmp->format);
  free(tmp->name);
  Py_XDECREF(tmp->func);
  if (tmp->defaultArgument)
    Py_XDECREF(tmp->defaultArgument);
  free(tmp);
  
  Py_RETURN_NONE;
}

static PyObject*
dim_dic_info_service(PyObject* /* self */, PyObject* args) {
  /*Arguments: string name                 ,
    string format               ,
    PyObject* callbackFunction  ,
    int service_type            , (optional, default=MONITORED:)
    int timeout                 , (optional, default=0)
    int tag                     , (optional, default=0)
    PyObject* default_value       (optional, default=NULL)

    Calls : unsigned int dic_info_service ( char* name         ,
    int   type         ,
    int   timeout      ,
    int*  address      ,
    int*  size         ,
    void* user_routine ,
    long  tag          ,
    int*  fill_address ,
    int   fill_size)
    Returns: the id of the new created service
  */
  char* name, *format;
  int service_type=MONITORED;
  int timeout=0;
  int  tag=0;
  int format_size;
  int name_size;
  int buffer_size;
  unsigned int service_id;
  PyObject* pyFunc=NULL, *default_value=NULL ;
  _dic_info_service_callback* tmp;

  if (!PyArg_ParseTuple(args, "s#s#O|iiiO",&name,
			&name_size,
			&format,
			&format_size,
			&pyFunc ,
			&service_type,
			&timeout,
			&tag,
			&default_value) 
      || !PyCallable_Check(pyFunc) )
    {
      goto invalid_args;
    }
  /* increasing the ref count of the python objects */
  Py_INCREF(pyFunc);
  if (default_value)
    Py_INCREF(default_value);
  /* creating a new service structure */
  tmp = (_dic_info_service_callback*)malloc(sizeof(_dic_info_service_callback));
  if (!tmp)
    goto no_memory;
  buffer_size = getSizeFromFormat(format);
  tmp->tag = tag;
  tmp->size = buffer_size;
  tmp->buffer = (char*)malloc(sizeof(char*)*buffer_size);
  tmp->format = (char*)malloc(sizeof(char)*format_size+1);
  tmp->name = (char*)malloc(sizeof(char)*name_size+1);
  tmp->defaultArgument = default_value;
  tmp->func = pyFunc;
  if ( !tmp->format || !tmp->name || !tmp->buffer)
    goto no_memory;
  strcpy(tmp->name, name);
  strcpy(tmp->format, format);
  _dic_info_service_buffer2Callback[tmp->buffer] = tmp;
  _dic_info_service_id2Callback[service_id] = tmp;

  //registering the stub function
  service_id = dic_info_service(name,
                                service_type,
                                timeout,
                                tmp->buffer,
                                buffer_size,
                                _dic_info_service_dummy,
                                tag,
                                tmp->buffer,
                                0);
  if ( !service_id ){
    print("Service %s creation failed. Received result %d", name, service_id);
    goto dealocate;
  }
  
  return Py_BuildValue("i", service_id);

  //invalid arguments
 invalid_args:
  PyErr_SetString(PyExc_TypeError,
		  "Invalid parameters. Expected:string name        ,"\
                  "                      string format             ,"\
                  "                      int service_type          ,"\
                  "                      int timeout               ,"\
                  "                      PyObject* callbackFunction,"\
                  "                      int tag                   ,"\
                  "                      PyObject* default_value"
		  );
  return NULL;

  //alocate memory problem
no_memory:
  PyErr_SetString(PyExc_MemoryError, "Could not allocate memory");
  Py_DECREF(pyFunc);
  return NULL;

  //invalid service registering
 dealocate:
  Py_XDECREF(tmp->func);
  _dic_info_service_buffer2Callback.erase(tmp->buffer);
  //_dic_info_service_id2Callback.erase(tmp->buffer);
  free(tmp->buffer);
  free(tmp->format);
  free(tmp->name);
  free(tmp);
  return 0;
}

static PyObject*
dim_dic_info_service_stamped(PyObject* self, PyObject* args){
  /* Delegates to dim_dic_info_service (PyObject* self, 
   *                                    PyObject* args, 
   *                                    PyObject* keywds
   *                                   )
   * Accepts same arguments and returns the new ID of the created service.
   */
  return dim_dic_info_service(self, args);
}

static PyObject*
dim_dic_get_server(PyObject* /* self */, PyObject* args) {
  /* Arguments : server name (string)
   * Calls     : dic_get_server(char* name)
   * Returns   : server id
   */
  int service_id;
  char* server_name;
     
  if ( !PyArg_ParseTuple(args, "s", &server_name) ) {
    goto invalid_arguments;
  }
  service_id = dic_get_server(server_name);
  
 return Py_BuildValue("i", service_id);
    
invalid_arguments:
  PyErr_SetString(PyExc_TypeError,
		  "Invalid parameters. Expected argument:string service_name");                                         
  return NULL;      
} 

static PyObject*
dim_dic_get_conn_id(PyObject* /* self */, PyObject* /* args */) {
  /* Arguments : None
   * Calls     : dic_get_conn_id(char* name)
   * Returns   : connection id
   */
  int service_id;     
          
  service_id = dic_get_conn_id();
  return Py_BuildValue("i", service_id);
} 

static PyObject*
dim_dic_get_server_services(PyObject* /* self */, PyObject* args) {
  /* Arguments : connection id
   * Calls     : dic_get_server_services(int conn_id)
   * Returns   : list of services
   */
  int conn_id;
  char* server_names=NULL;
  PyObject* ret;
    
  if ( !PyArg_ParseTuple(args, "i", &conn_id) ){
    goto invalid_arguments;
  }
  server_names = dic_get_server_services(conn_id);
  ret = stringList_to_tuple(server_names);    

  return ret;
    
 invalid_arguments:
  PyErr_SetString(PyExc_TypeError, 
		  "Invalid parameters. Expected argument:int conn_id");                                                  
  return NULL;    
} 
        
static PyObject*
dim_dic_get_error_services(PyObject* /* self */, PyObject* args) {
  /* Arguments : None
   * Calls     : dic_get_error_services(int conn_id)
   * Returns   : list of the error services
   * To be called inside the error handler to see what originated the error.
   */    
  char* server_names=NULL;
  PyObject* ret;
    
  server_names = dic_get_error_services();
  ret = stringList_to_tuple(server_names);
    
  return args;        
} 

static PyObject*
dim_dic_add_error_handler(PyObject* /* self */, PyObject* args) {
  /* Arguments : python callback (callable object)
   * Calls     : dic_add_error_handler ( void* error_routine(int, int, char*) )
   * Returns   : None
   */    
  PyObject* pyFunc;
    
  if (!PyArg_ParseTuple(args, "O", &pyFunc)
      || !PyCallable_Check(pyFunc)) 
    {
      goto invalid_arguments;
    }
      
  dic_add_error_handler(_dic_error_user_routine_dummy);    
  Py_RETURN_NONE;
    
 invalid_arguments:
  PyErr_SetString(PyExc_TypeError, 
		  "Invalid parameters. Expected argument: callable object ");                                                
  return NULL;    
} 

static PyObject*
dim_dic_cmnd_service(PyObject* /* self */, PyObject* args) {
  /* Arguments : service name (string),
   *             command data (tuple),
   *             format (string)
   * Calls     : dic_cmnd_service(char* name, int* address, int size)
   * Returns   : return code (1 for a successful request)
   */
  char *service_name, *format, *buffer;
  int buffer_size, res;
  PyObject* tuple;
    
  if (!PyArg_ParseTuple(args, "sOs", &service_name, &tuple, &format) )
    goto invalid_arguments;
  /* creating dummy command request */
  buffer_size = getSizeFromFormat(format);
  buffer = (char*)malloc(sizeof(char)*buffer_size);
  if (!buffer)
    goto memory_error; 
  if (!iterator_to_buffer(tuple, buffer, buffer_size, format))
    goto error;    
  Py_BEGIN_ALLOW_THREADS
  res = dic_cmnd_service(service_name, buffer, buffer_size);
  Py_END_ALLOW_THREADS
  /* freeing allocated buffer */
  free(buffer);
    
  return Py_BuildValue("i", res);    
    
 invalid_arguments:
  PyErr_SetString(PyExc_TypeError, 
		  "Invalid parameters. Expected: string service_name (string), \n"\
                  "                              tuple data, \n"\
                  "                              string format" 
		  );
  return NULL;
            
 error:
  PyErr_SetString(PyExc_RuntimeError, "Could not convert arguments to buffer");
  free(buffer);                                                
  return NULL;
                
 memory_error:
  PyErr_SetString(PyExc_MemoryError, "Could not allocate memory");
  return NULL;    
} 

static PyObject*
dim_dic_cmnd_callback(PyObject* /* self */, PyObject* args) {
  /* Arguments: service name (string),
   *             command data (tuple),
   *             format (string)
   *             cmnd_callback (callable object)
   *             tag (int)
   * Calls     : dic_cmnd_service(char* name, 
   *                              int* address, 
   *                              int size, 
   *                              void* cmd_callback, 
   *                              long tag)
   * Returns   : return code (int), 1 for successful request
   */
  char *service_name, *format, *buffer;
  int buffer_size, res;
  long tag; 
  unsigned long uniqueTag;
  PyObject *tuple, *pyFunc;
  _dic_cmnd_callback *callback;
    
  if (!PyArg_ParseTuple(args, "sOsOi",
			&service_name, &tuple, &format, &pyFunc, &tag)
      || !PyCallable_Check(pyFunc)) 
    {
      goto invalid_arguments;
    }
  /* saving callback */
  Py_INCREF(pyFunc);
  uniqueTag =  ++_dic_cmnd_callback_ID;
  callback = (_dic_cmnd_callback*)malloc(sizeof(_dic_cmnd_callback));
  if (!callback) 
    goto memory_error;
  callback->func = pyFunc;
  callback->tag = tag;
  _dic_cmnd_callback_tag2Callback[uniqueTag] = callback;
    
  /* creating dummy cmnd_callback request */
  buffer_size = getSizeFromFormat(format);
  buffer = (char*)malloc( sizeof(char)*buffer_size );
  if ( !buffer )
    goto memory_error; 
  if ( !iterator_to_buffer(tuple, buffer, sizeof(buffer), format) )
    goto error;
  //print("Creating command callback %s: buffer size=%d, tag=%l, uniqueTag=%l",service_name, buffer_size, callback->tag, uniqueTag);            
  res = dic_cmnd_callback(service_name, 
			  buffer, 
			  buffer_size, 
			  _dic_cmnd_callback_dummy, 
			  uniqueTag
			  );
  /* freeing temporary buffer and returning */
  free(buffer);
    
  return Py_BuildValue("i", res);
    
 invalid_arguments:
  PyErr_SetString(PyExc_TypeError, 
	  "Invalid parameters. Expected: string service_name, \n"\
          "                              tuple command_data, \n"\
          "                              string format, \n"\
          "                              Callable Object cmnd_callback \n"\
          "                              int tag"
                 );
  return NULL;
    
 error:
  PyErr_SetString(PyExc_RuntimeError, 
		  "Could not convert arguments to buffer");
  free(buffer);
  free(callback);             
  return NULL;
        
 memory_error:
  PyErr_SetString(PyExc_MemoryError, "Could not allocate memory");
  return NULL;    
} 
    
    
/******************************************************************************/
/* DIC interface internal functions */
/******************************************************************************/
void _dic_cmnd_callback_dummy(void* uniqueTagPtr, int* ret_code) {
  /* Stub function for passing the callback to the error handler to the Python
   * layer. 
   */
  unsigned long uniqueTag = *(long*)uniqueTagPtr;
  _dic_cmnd_callback* callback=NULL;
  PyObject* res;
    
  callback = _dic_cmnd_callback_tag2Callback[uniqueTag];
  //print("Return code is %d tag %l and uniqueTag %l", 
  //      *ret_code, callback->tag, *(long*)uniqueTagPtr );
  if (!callback) {        
    print("Could not find callback");
    return;
  }
  res = pyCallFunction(callback->func, 
		       Py_BuildValue("li", callback->tag, *ret_code));    
  //deleting callback information
  Py_DECREF(res);
  Py_DECREF(callback->func);
  _dic_cmnd_callback_tag2Callback.erase(uniqueTag);
  free(callback);
  print("Callback successfully completed");

  return;   
}


void _dic_error_user_routine_dummy(int severity, int error_code, char* message) {
  /* Stub function for passing the callback to the error handler to the Python
   * layer. 
   */
  if ( !_dic_callback_errorHandler ) {
    print("Python callback does not exists");
    PyErr_SetString(PyExc_RuntimeError, "Could not find python callback");
    return;
  }
  pyCallFunction(_dic_callback_errorHandler, 
		 Py_BuildValue("iis", severity, error_code, message));
}

void _dic_info_service_dummy (void* /* tag */, void* buffer, int* size) {
  /* Stub function for passing the received data from a service to a python
   *  callback. Receives tag, buffer, size
   */
  PyObject* args, *res;
  _dic_info_service_callback* svc;
  PyGILState_STATE gstate; 
      
  svc = _dic_info_service_buffer2Callback[(char*)buffer];
  if (!svc) {
    print("Invalid update service request");
    return;
  }
  if (!(*size)) {
    /* The service update request failed. Passing default argument */
    args = svc->defaultArgument;
  } else {    
    args = dim_buf_to_tuple(svc->format, (char*)buffer, *size);
  }
  if (args) {      
    gstate = PyGILState_Ensure();  
    res = PyEval_CallObject(svc->func, args);
    PyGILState_Release(gstate);
  } else {
    /* service failed and a default value was not specified */
    print("Service error or could not get values from it");
    return;
  }

  if (!res){
    print("Callback to python function %p failed", svc->func);
    return;
  }
  Py_DECREF(res);
  Py_XDECREF(args);  
}

static PyMethodDef DimMethods[] = {
  {    "dis_start_serving"          ,
       dim_dis_start_serving        ,
       METH_VARARGS                 ,
       "Start providing DIM service"
  },
  {    "dis_stop_serving"           ,
       dim_dis_stop_serving         ,
       METH_VARARGS                 ,
       "Stop DIM service"
  },
  {    "dis_set_dns_node"           ,
       dim_dis_set_dns_node         ,
       METH_VARARGS                 ,
       "Function for setting the DNS node"
  },
  {    "dis_get_dns_node"           ,
       dim_dis_get_dns_node         ,
       METH_VARARGS                 ,
       "Function for getting the DNS node"
  },
  {    "dis_set_dns_port"           ,
       dim_dis_set_dns_port         ,
       METH_VARARGS                 ,
       "Function for setting the DNS port"
  },
  {    "dis_get_dns_port"           ,
       dim_dis_get_dns_port         ,
       METH_VARARGS                 ,
       "Function for getting the DNS port"
  },
  {    "dis_add_exit_handler"       ,
       dim_dis_add_exit_handler     ,
       METH_VARARGS                 ,
       "Function for setting the DIM exit handler"
  },
  {    "dis_add_error_handler"      ,
       dim_dis_add_error_handler    ,
       METH_VARARGS                 ,
       "Function for setting the DIM error handler"
  },
  {    "dis_add_client_exit_handler"  ,
       dim_dis_add_client_exit_handler,
       METH_VARARGS                   ,
       "Function for setting the DIM Client exit handler"
  },
  {    "dis_update_service"           ,
       dim_dis_update_service         ,
       METH_VARARGS                   ,
       "Function for updating the specified service"
  },
  {    "dis_selective_update_service"  ,
       dim_dis_selective_update_service,
       METH_VARARGS                    ,
       "Function for updating the specified service and clients"
  },
  {    "dis_set_quality"               ,
       dim_dis_set_quality             ,
       METH_VARARGS                    ,
       "Function for setting the quality of a service"
  },
  {    "dis_set_timestamp"             ,
       dim_dis_set_timestamp           ,
       METH_VARARGS                    ,
       "Function for setting the the timestamp of a service"
  },
  {    "dis_remove_service"            ,
       dim_dis_remove_service          ,
       METH_VARARGS                    ,
       "Function for removing a service"
  },
  {    "dis_get_next_cmnd"             ,
       dim_dis_get_next_cmnd           ,
       METH_VARARGS                    ,
       "Get a command from the list of waiting command requests"
  },
  {    "dis_remove_service"            ,
       dim_dis_remove_service          ,
       METH_VARARGS                    ,
       "Remove a Service from the list of provided services"
  },
  {    "dis_get_client"                ,
       dim_dis_get_client              ,
       METH_VARARGS                    ,
       "Gets the process identification of the current DIM client"
  },
  {    "dis_get_conn_id"               ,
       dim_dis_get_conn_id             ,
       METH_VARARGS                    ,
       "Gets the connection ID of the current DIM client"
  },
  {    "dis_get_timeout"               ,
       dim_dis_get_timeout             ,
       METH_VARARGS                    ,
       "Gets the update rate that was specified by a client for a specific service"
  },
  {    "dis_get_client_services"       ,
       dim_dis_get_client_services     ,
       METH_VARARGS                    ,
       "Gets the services of a DIM client, which has subscribed to this DIM server"
  },
  {    "dis_set_client_exit_handler"   ,
       dim_dis_set_client_exit_handler ,
       METH_VARARGS                    ,
       "Activates the client exit handler for a specific client and a specific service"
  },
  {    "dis_get_error_services"        ,
       dim_dis_get_error_services      ,
       METH_VARARGS                    ,
       "Gets the names of DIM services that have an error"
  },
  {    "dis_add_cmnd"                  ,
       dim_dis_add_cmnd                ,
       METH_VARARGS                    ,
       "Adds a new command"
  },
  {    "dis_add_service"               ,
       dim_dis_add_service             ,
       METH_VARARGS                    ,
       "Adds a new service"
  },
  {    "dic_set_dns_node"              ,
       dim_dic_set_dns_node            ,
       METH_VARARGS                    ,
       "Function for setting the DNS node"
  },
  {    "dic_get_dns_node"              ,
       dim_dic_get_dns_node            ,
       METH_VARARGS                    ,
       "Function for getting the DNS node"
  },
  {    "dic_set_dns_port"              ,
       dim_dic_set_dns_port            ,
       METH_VARARGS                    ,
       "Function for setting the DNS port"
  },
  {    "dic_get_dns_port"              ,
       dim_dic_get_dns_port            ,
       METH_VARARGS                    ,
       "Function for getting the DNS port"
  },
  {    "dic_get_id"                    ,
       dim_dic_get_id                  ,
       METH_VARARGS                    ,
       "Gets the process identification of this DIM client."
  },
  {    "dic_disable_padding"           ,
       dim_dic_disable_padding         ,
       METH_VARARGS                    ,
       "Disable padding of received services."
  },
  {    "dic_get_quality"               ,
       dim_dic_get_quality             ,
       METH_VARARGS                    ,
       "Gets the quality of  a received service."
  },
  {    "dic_get_timestamp"             ,
       dim_dic_get_timestamp           ,
       METH_VARARGS                    ,
       "Gets the time stamp of a received service."
  },
  {    "dic_get_format"                ,
       dim_dic_get_format              ,
       METH_VARARGS                    ,
       "Gets the format description of a received service."
  },
  {    "dic_release_service"           ,
       dim_dic_release_service         ,
       METH_VARARGS                    ,
       "Called by a client when a service is not needed anymore."
  },
  {    "dic_info_service"               ,
       dim_dic_info_service,
       METH_VARARGS | METH_KEYWORDS     ,
       "Called by a client for subscribing to a service." 
  },
  {    "dic_cmnd_service"        ,
       dim_dic_cmnd_service      ,
       METH_VARARGS              ,
       "Request the execution of a command by a server." 
  },
  {    "dic_cmnd_callback"       ,
       dim_dic_cmnd_callback     ,
       METH_VARARGS              ,
       "Request the execution of a command by a server provind an asyncronious callback when the command is executed." 
  },
  {    "dic_info_service_stamped"       ,
       dim_dic_info_service_stamped  ,
       METH_VARARGS              ,
       "TODO: add documentation" 
  },
  {    "dic_get_server"          ,
       dim_dic_get_server        ,
       METH_VARARGS              ,
       "TODO: add documentation" 
  }, 
  {    "dic_get_conn_id"          ,
       dim_dic_get_conn_id        ,
       METH_VARARGS               ,
       "TODO: add documentation" 
  }, 
  {    "dic_get_server_services"          ,
       dim_dic_get_server_services     ,
       METH_VARARGS              ,
       "TODO: add documentation" 
  }, 
  {    "dic_get_error_services"          ,
       dim_dic_get_error_services       ,
       METH_VARARGS              ,
       "TODO: add documentation" 
  }, 
  {    "dic_dic_add_error_handler"          ,
       dim_dic_add_error_handler   ,
       METH_VARARGS              ,
       "TODO: add documentation" 
  }, 
  {NULL, NULL, 0, NULL}        /* Sentinel */
};


PyMODINIT_FUNC
initdimc(void)
{
  PyObject *m;
  printf("Initializing C DIM interface... \n");
  m = Py_InitModule3("dimc", DimMethods, "DIM methods");
  if (m == NULL)
    return;
  dic_disable_padding();
  dis_disable_padding();
  PyEval_InitThreads();
}

