extern "C" {
#include <Python.h>
#include "structmember.h"
#include "dic.h"
#include "dis.h"
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
}

#include <map>
#include <string>

#ifndef Py_RETURN_NONE
#define Py_RETURN_NONE do { Py_INCREF(Py_None); return Py_None; } while(0);
#endif
#define errmsg(x) do { fprintf(stderr, "%s: %s\n", __FUNCTION__, x); } while(0);

#define print(...) printf("%s:%u ::%s: ", __FILE__, __LINE__, __FUNCTION__); printf(__VA_ARGS__); printf("\n");

using namespace std;

namespace DIM  {
  static PyThreadState* s_mainThreadState = 0;
};

static void init_threading()  {
  static bool firstInit = true;
  if ( firstInit )  {
    firstInit = false;
    PyEval_InitThreads();
    DIM::s_mainThreadState = PyThreadState_Get();
		print("Threads initialized")
  }
}


int list_to_schema(PyObject *, char *);
int tuple_to_schema(PyObject *, char *);

int listOrTuple2Int(PyObject* pyObj, int** buffer) {
  int size, i, res=1;
  PyObject* tmp;

  if ( PyTuple_Check(pyObj) ) { //tuple
    size = PyTuple_Size(pyObj);
    if (!size)
      res = 0;
    (*buffer) = (int*)malloc(size*sizeof(int)+1);
    if ( !(*buffer) )
      res = 0;
    for (i=0; i<size; i++) {
      tmp = PyTuple_GetItem(pyObj, i);
      if (!tmp)
        res = 0;
      (*buffer)[i] = (int)PyInt_AsLong(tmp);
    }
  } else if ( PyList_Check(pyObj) ) { //list
    size = PyList_Size(pyObj);
    if (!size)
      res = 0;
    (*buffer) = (int*)malloc(size*sizeof(int)+1);
    if ( !(*buffer) )
      res = 0;
    for (i=0; i<size; i++) {
      tmp = PyList_GetItem(pyObj, i);
      if (!tmp)
        res = 0;
      (*buffer)[i] = (int)PyInt_AsLong(tmp);
    }
  } else
    res = 0;

  if (res) {
    (*buffer)[size] = 0;
    return 1;
  }
  else {
    free(*buffer);
    buffer = NULL;
    return 0;
  }
}
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#endif
static int server_active = 0;
static char server_name[HOST_NAME_MAX + 1];

//static int
//start_serving(void)
//{
//  int rc;
//
//  rc = dis_start_serving(server_name);
//  if (rc) {
//  server_active = 1;
//  return 0;
//  }
//  server_active = 0;
//  return -1;
//}

//static void
//stop_serving(void)
//{
//  dis_stop_serving();
//  server_active = 0;
//}
//
//static void
//update_publish(void)
//{
//  if (!server_active) return;
//  //   stop_serving();
//  start_serving();
//}
//
static PyObject *
dim_dis_start_serving(PyObject *self, PyObject *args)
{
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
      PyErr_SetString(PyExc_RuntimeError, "Failed to activate DIM serving");
      return NULL;
  }
  server_active=1;
  Py_RETURN_NONE;
}

static PyObject *
dim_dis_stop_serving(PyObject *self, PyObject *args) {
  /* Call: void dis_stop_serving(void)
   */
  dis_stop_serving(); // no return values :-)
  server_active=0;
  Py_RETURN_NONE;
}

static PyObject*
dim_dis_set_dns_node(PyObject* self, PyObject* args) {
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

static PyObject*
dim_dis_get_dns_node(PyObject* self, PyObject* args) {
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

static PyObject*
dim_dis_set_dns_port(PyObject* self, PyObject* args) {
  /* calls dis_set_dns_port(int port)
     return success or failure
  */
  unsigned int port;
  if ( !PyArg_ParseTuple(args, "I", &port) ) {
    PyErr_SetString(PyExc_TypeError, "Argument 'port' must be a pozitive integer");
    return NULL;
  }
  if ( !dis_set_dns_port(port) ) {
    return NULL;
  }
  printf("New port is %d\n", port);
  Py_RETURN_NONE;
}

static PyObject*
dim_dis_get_dns_port(PyObject* self, PyObject* args) {
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
} PyCallback; //for callbacks

static PyCallback dis_callbackExitHandler_func,
  dis_callbackErrorHandler_func,
  dis_callbackClientExitHandler_func;

static void
dim_dis_callbackExitHandler(int* code) {
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
    //                           "i" ,code);
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
  /*Interface function with signature:
    void client_exit_user_routine (int* tag)
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
  //PyObject *result = NULL;
  PyObject *temp;
  if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
    if (!PyCallable_Check(temp)) {
      PyErr_SetString(PyExc_TypeError, "parameter must be callable");
      return NULL;
    }
    Py_XINCREF(temp);         /* Add a reference to new callback */
    Py_XINCREF(self);
    Py_XDECREF(dis_callbackExitHandler_func.self);  /* Dispose of previous callback */
    Py_XDECREF(dis_callbackExitHandler_func.func);
    dis_callbackExitHandler_func.self = self;
    dis_callbackExitHandler_func.func = temp;
    printf("Exit handler: Got function pointer %p and obj pointer %p\n", temp, self);
  } else {
    PyErr_SetString(PyExc_TypeError, "Argument has to be a function or a bound method");
    return NULL;
  }
  dis_add_exit_handler(dim_dis_callbackExitHandler);
  Py_RETURN_NONE;
}


typedef void* func(int*);

static PyObject*
dim_dis_add_error_handler(PyObject* self, PyObject* args) {
  //PyObject *result = NULL;
  PyObject *temp;
  if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
    if (!PyCallable_Check(temp)) {
      PyErr_SetString(PyExc_TypeError, "parameter must be callable");
      return NULL;
    }
    Py_XINCREF(temp);         /* Add a reference to new callback */
    Py_XINCREF(self);
    Py_XDECREF(dis_callbackErrorHandler_func.self);  /* Dispose of previous callback */
    Py_XDECREF(dis_callbackErrorHandler_func.func);
    dis_callbackErrorHandler_func.self = self;
    dis_callbackErrorHandler_func.func = temp;
    printf("Error handler: Got function pointer %p and obj pointer %p\n", temp, self);
  } else {
    PyErr_SetString(PyExc_TypeError, "Argument has to be a function or a bound method");
    return NULL;
  }
  dis_add_error_handler( dim_dis_callbackErrorHandler);
  Py_RETURN_NONE;
}

static PyObject*
dim_dis_add_client_exit_handler(PyObject* self, PyObject* args) {
  //PyObject *result = NULL;
  PyObject *temp;
  if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
    if (!PyCallable_Check(temp)) {
      PyErr_SetString(PyExc_TypeError, "parameter must be callable");
      return NULL;
    }
    Py_XINCREF(temp);         /* Add a reference to new callback */
    Py_XINCREF(self);
    Py_XDECREF(dis_callbackClientExitHandler_func.self);  /* Dispose of previous callback */
    Py_XDECREF(dis_callbackClientExitHandler_func.func);
    dis_callbackClientExitHandler_func.self = self;
    dis_callbackClientExitHandler_func.func = temp;
    printf("Client exit handler: Got function pointer %p and obj pointer %p\n", temp, self);
  } else {
    PyErr_SetString(PyExc_TypeError, "Argument has to be a function or a bound method");
    return NULL;
  }
  dis_add_client_exit_handler(dim_dis_callbackClientExitHandler);
  Py_RETURN_NONE;
}

static PyObject*
dim_dis_selective_update_service(PyObject* self, PyObject* args) {
  /* Calls: int dis_selective_update_service (int service_id, int** client_ids)
   */
  int* client_ids=NULL, res;
  PyObject* listOrTuple;
  int service_id;

  if ( !PyArg_ParseTuple(args, "iO;list or tuple", &service_id, &listOrTuple) ){
    PyErr_SetString(PyExc_TypeError, 
        "Arguments have to be an integer and a list/tuple of integers");
    return NULL;
  }
  if ( !listOrTuple2Int(listOrTuple, &client_ids) ) {
    PyErr_SetString(PyExc_TypeError, "Second argument must a list/tuple of integers");
    return NULL;
  }
  res = dis_selective_update_service(service_id, client_ids);
  return Py_BuildValue("i", res);
}

static PyObject*
dim_dis_set_quality(PyObject* self, PyObject* args) {
  /*Calls: void dis_set_quality (service_id, quality)
   */
  unsigned int service_id;
  int quality;

  if ( !PyArg_ParseTuple(args, "Ii", &service_id, &quality) ) {
    PyErr_SetString(PyExc_TypeError, "Invalid arguments: expected an unsigned integer and an integer");
    return NULL;
  }
  dis_set_quality(service_id, quality);
  Py_RETURN_NONE;
}

static PyObject*
dim_dis_set_timestamp(PyObject* self, PyObject* args) {
  /*Calls: void dis_set_timestamp(unsigned int service_id, int secs, int millisecs)
   */
  unsigned int service_id;
  int secs, milisecs;

  if ( !PyArg_ParseTuple(args, "Iii", &service_id, &secs, &milisecs) ) {
    PyErr_SetString(PyExc_TypeError, "Invalid arguments: expected an unsigned integer and two integers");
    return NULL;
  }
  dis_set_timestamp(service_id, secs, milisecs);
  Py_RETURN_NONE;
}

static PyObject*
dim_dis_remove_service(PyObject* self, PyObject* args) {
  /*Calls: int dis_remove_service (unsigned int service_id)
   */
  unsigned int service_id;
  int res;

  if ( !PyArg_ParseTuple(args, "I", &service_id) ) {
    PyErr_SetString(PyExc_TypeError, "Invalid argument: expected an unsigned integer");
    return NULL;
  }
  res = dis_remove_service(service_id);
  return Py_BuildValue("i", res);
}

static PyObject*
dim_dis_get_next_cmnd(PyObject* self, PyObject* args) {
  /* Calls: int dis_get_next_cmnd (long* tag, int* buffer, int* size)
     Problem: the way the results are received is not done in a "pythonic" way.
     Better to return a tuple with all the results.
     Info: the DIM function implementation does not define a value for buffer, size or tag
     in case a command is not found.
  */
  int res=0, *buffer, size;
  long tag=0;
  PyObject* tmp;

  if ( !PyArg_ParseTuple(args, "I", &size) ) {
    PyErr_SetString(PyExc_TypeError, "Invalid argument: expected an unsigned integer");
    return NULL;
  }
  buffer = (int*)malloc(size*sizeof(int));
  res = dis_get_next_cmnd(&tag, buffer, &size);
  tmp = Py_BuildValue("(iis#)", res, tag, buffer, size);
  free(buffer);
  return tmp;
}

static PyObject*
dim_dis_get_client(PyObject* self, PyObject* args) {
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
dim_dis_get_conn_id(PyObject* self, PyObject* args) {
  /*Calls: int dis_get_conn_id ()
   */
  int res;
  res = dis_get_conn_id();
  return Py_BuildValue("i", res);
}

static PyObject*
dim_dis_get_timeout(PyObject* self, PyObject* args) {
  /*Calls: int dis_get_timeout (unsigned int service_id, int client_id)
   */
  unsigned int service_id;
  int client_id, res;
  if ( !PyArg_ParseTuple(args, "Ii", &service_id, &client_id) ) {
    PyErr_SetString(PyExc_TypeError, "Invalid argument: expected an unsigned int and an int");
    return NULL;
  }
  res = dis_get_timeout(service_id, client_id);
  return Py_BuildValue("i", res);
}

static PyObject*
dim_dis_get_client_services(PyObject* self, PyObject* args) {
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
dim_dis_set_client_exit_handler(PyObject* self, PyObject* args) {
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
dim_dis_get_error_services(PyObject* self, PyObject* args) {
  /*Calls: char* dis_get_error_services (int conn_id)
   */
  char* res=NULL;
  res = dis_get_error_services();
  return Py_BuildValue("s", res);
}

/******************************************************************************/
/* DIS containers for callbacks*/
/******************************************************************************/

#define _DIM_INT 0
#define _DIM_INT_LEN 4

#define _DIM_LONG 6
#define _DIM_LONG_LEN sizeof(long)
//#define _DIM_INT_LEN sizeof(int)

#define _DIM_FLOAT 1
#define _DIM_FLOAT_LEN 4
//#define _DIM_FLOAT_LEN sizeof(float)

#define _DIM_DOUBLE 2
#define _DIM_DOUBLE_LEN 8
//#define _DIM_DOUBLE_LEN  sizeof(double)

#define _DIM_XTRA 3
#define _DIM_XTRA_LEN 8
//#define _DIM_XTRA_LEN  sizeof(long double)

#define _DIM_STRING 4
#define _DIM_CHAR_LEN 1

#define _DIM_SHORT 5
#define _DIM_SHORT_LEN 2

#define MUL_INFINITE -1
/* multiplicity == MUL_INFINITE  means "...;I" arbitrary amount of Is */
/* multiplicity ==  0 is an illegal value and will not be returned by this
   function */

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
  PyObject *func;
} ServiceCallback;
typedef ServiceCallback *ServiceCallbackPtr;
map<unsigned int, ServiceCallbackPtr> serviceID2Callback;

static void dim_callbackCommandFunc(void*, void*, int*);
static int next_element(const char*, int*, int*, int*);
static int tuple2Buffer (char*, const char*, PyObject*);
static int getSizeFromFormat(const char*);

/******************************************************************************/
/* DIC containers for callbacks and function prototypes*/
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

static map<char*, _dic_info_service_callback*> _dic_info_service_buffer2Callback;
static map<unsigned int, _dic_info_service_callback*> _dic_info_service_id2Callback;

void _dic_info_service_dummy (void*, void*, int*);

/******************************************************************************/

static PyObject*
dim_dis_add_cmnd(PyObject* self, PyObject* args) {
  /* Arguments: name, description, py_function, tag

  Implementation details: in order to preserve the C interface unique tags
  are generated for each call. The original tags are preserved with the python
  function pointer and passed back when doing a callback.

  Calls: unsigned int dis_add_cmnd (name, description, cmnd_user_routine, tag)
  Where:
  char* name - the name of the command. Will be used for identifing the python callback
  char* description
  void cmnd_user_routine (long* tag, int* address, int* size) - The long pointer will be transformed
  in long Python object for the callback. The address will become a string
  long tag - will be passed back to the python callback
  */
  unsigned int res=0;
  char *name=NULL, *format=NULL;
  long tag, uniqueTag;
  int sizeFormat, sizeName;
  PyObject *pyFunc;
  CmndCallback *callback;
  string s;

  if ( !PyArg_ParseTuple(args, "s#s#Ol", &name, &sizeName, &format, &sizeFormat, &pyFunc, &tag) &&  PyCallable_Check(pyFunc) ) {
    PyErr_SetString(PyExc_TypeError, "Invalid argument: expected two strings, a function pointer and an integer");
    return NULL;
  }
  Py_INCREF(pyFunc);
  //print("Adding command %p with name %s, description %s and tag %l", pyFunc, name, format, tag);
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
  if ( cmndName2PythonFunc[s] )
    free( cmndName2PythonFunc[s] );
  cmndName2PythonFunc[s] = callback;
  cmndUniqueTag2PythonFunc[uniqueTag] = callback;
  res = dis_add_cmnd(name, format, dim_callbackCommandFunc, uniqueTag);
  return Py_BuildValue("i", res);
}

static PyObject*
dim_dis_update_service(PyObject* self, PyObject* args) {
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
  if ( !svc_args and svc->func ){
    //print("Calling Python function %p for service id %d with tag %l", svc->func, service_id, svc->tag);
    arg = Py_BuildValue("(i)", svc->tag);
    gstate = PyGILState_Ensure();
    svc_args = PyEval_CallObject(svc->func, arg);
    PyGILState_Release(gstate);
    if ( !svc_args ) {
      print("Error in calling python function %p. Received pointer %p as result. ", svc->func, svc_args);
      PyErr_Print();
      print("Error ended");
      return NULL;
    }
    Py_DECREF(arg);
    print(" Received result from service %d callback", service_id);
  }
  print("Converting arguments to a memory buffer");
  if ( svc_args && !tuple2Buffer(svc->buffer, svc->format, svc_args) ) {
    PyErr_SetString(PyExc_TypeError, "Arguments passed don't match the format of the service");
    return NULL;
  }
  print("Calling dis_update_service");
  res = dis_update_service(service_id);
  print("Returning result %d", res);
  return Py_BuildValue("i", res);
}


static PyObject*
dim_dis_add_service(PyObject* self, PyObject* args) {
  /*Arguments: name, description, python function, tag
    Calls: unsigned int dis_add_service (char* name,
    char* description,
    int*  address,
    int   size,
    func  user_routine,
    long tag)
    func = void user_routine(long* tag, int** address, int* size)

    Observation: the size of the command is calculated using the
    description parameter so if a size is omitted it is considered 1.
  */

  int size=0, name_size, format_size, service_id=0;
  char *name, *format, *buffer;
  long tag;
  PyObject *pyFunc;
  ServiceCallbackPtr svc;

  if ( !PyArg_ParseTuple(args, "s#s#Ol", &name, &name_size, &format, &format_size, &pyFunc, &tag) ){
    PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected two strings and a callable object.");
    return NULL;
  }
  if ( !PyCallable_Check(pyFunc) ){
    PyErr_SetString(PyExc_TypeError, "Invalid arguments. Expected a callable object as the third argument");
    return NULL;
  }
  size = getSizeFromFormat(format);
  if ( !size ){
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
    Py_DECREF(pyFunc);
    PyErr_SetString(PyExc_MemoryError, "Could not allocate memory");
    free(buffer);
   free(svc->name);
    return NULL;
  }
  strcpy(svc->format, format);
  svc->tag = tag;
  svc->func = pyFunc;
  svc->buffer = buffer;
  if ( serviceID2Callback[service_id] ){
    print("Another service registered under the name %s. Replacing it.",svc->name);
    print("Old service struct pointer %p", serviceID2Callback[service_id]);
    //a service with the same name added again?
    Py_XDECREF(serviceID2Callback[service_id]->func);
    free(serviceID2Callback[service_id]->name);
    free(serviceID2Callback[service_id]->format);
    free(serviceID2Callback[service_id]->buffer);
    free(serviceID2Callback[service_id]);
  }
  serviceID2Callback[service_id] = svc;
  print ("Service %s added successfully. Callback address is %p and allocated buffer %p with size %d",
         svc->name, svc->func, svc->buffer, size);
  return Py_BuildValue("i", service_id);
}

static void printPyObject(PyObject* object) {
   if ( !object ) return;
   PyObject_Print(object, stdout, Py_PRINT_RAW);
   print("\n");
   return;
}


static PyObject*
list2Tuple(PyObject* list) {
  int size, i;
  PyObject* tuple;
  if ( !PyList_Check(list) ) return NULL;
  size = PyList_Size(list);
  tuple = PyTuple_New(size);
//  print("Converting list to tuple. Found tuple elements:");
  for(i=0; i<size; i++) {
    PyTuple_SetItem(tuple, i, PyList_GetItem(list, i) );
//    PyObject_Print( PyList_GetItem(list, i), stdout, Py_PRINT_RAW );
//    printf(" ");
  }
  printf("\n");
  printPyObject(list);
  printPyObject(tuple);
  return tuple;
}


static int
next_element(const char *schema, int *p, int *type, int *mult)
{
  char *endptr;
  if (schema[*p] == '\0') return 0;
  switch (toupper(schema[*p])) {
  case 'I':
  case 'L': *type = _DIM_INT; break;
  case 'C': *type = _DIM_STRING; break;
  case 'F': *type = _DIM_FLOAT; break;
  case 'D': *type = _DIM_DOUBLE; break;
  case 'X': *type = _DIM_XTRA; break;
  case 'S': *type = _DIM_SHORT; break;
  default: print("bad character %c", schema[*p]);
    *type = -1;
    return 0;
  }
//  print("Type is %d", *type, *mult);
  switch (schema[++(*p)]) {
  case '\0': *mult = MUL_INFINITE; print("End of string"); break;
  case ';': *mult = 1; ++(*p); print("Multiplicity 1");break;
  case ':':
    //printf("2 %c\n", schema[*p]);
    (*p)++;
    //printf("3 %c\n", schema[*p]);
    *mult = strtoul(&schema[*p], &endptr, 10);
    if (endptr == &schema[*p]) {
      print("illegal number\n");
      return 0;
    }
    //printf("4 %d %c\n", *endptr, *endptr);
    *p += (endptr - &schema[*p]) + ((*endptr == ';') ? 1 : 0);
    //printf("5 %c\n", schema[*p]);
    break;
  default:
    printf("Bad character %c\n", schema[*p]);
    return 0;
  }
  //print("Multiplicity is %d and internal pointer %d", *mult, *p);
  return 1;
}

static PyObject *
dim_buf_to_list(const char *schema, const char *buf, int len, int *n)
{
  /* TO DO: check for memory leaks. Not sure who owns the new reference after Py<type>_From<type>
   */
  int j = 0, m = 0;
  int type=0, mult=0;
  PyObject *list, *tmp;
  float f;

//  print("Converting buffer to a list. Using format %s", schema);
  if (!(list = PyList_New(0))) return NULL;
  while (next_element(schema, &m, &type, &mult)) {
//    print("schema=%s m=%d type=%d mult=%d, len=%d n=%d", schema, m, type, mult, len , *n);
    switch (type) {
    case _DIM_INT:
      if (mult == MUL_INFINITE)
        mult = (len + _DIM_INT_LEN - 1 - *n) / _DIM_INT_LEN;
      for (j = 0; j < mult; ++j)
        if (*n + _DIM_INT_LEN <= len) {
          PyList_Append(list, PyInt_FromLong( *(int *) &buf[*n]));
          print(" --> Creating PyInteger %d", *(int*) &buf[*n]);
          *n += _DIM_INT_LEN;
        } else goto short_buffer;
      break;
    case _DIM_FLOAT:
      if (mult == MUL_INFINITE) mult = (len + _DIM_FLOAT_LEN - 1 - *n) /
        _DIM_FLOAT_LEN;
      for (j = 0; j < mult; ++j)
        if (*n + _DIM_FLOAT_LEN <= len) {
          f = *(float *)(&buf[*n]);
          PyList_Append(list, PyFloat_FromDouble(f));
          print(" --> Creating Pyfloat %f", f);
          *n += _DIM_FLOAT_LEN;
        } else goto short_buffer;
      break;
    case _DIM_DOUBLE:
      if (mult == MUL_INFINITE)
        mult = (len + _DIM_DOUBLE_LEN - 1 - *n) / _DIM_DOUBLE_LEN;
      for (j = 0; j < mult; ++j)
        if (*n + _DIM_DOUBLE_LEN <= len) {
          PyList_Append(list, PyFloat_FromDouble(*(double *)
                                                 &buf[*n]));
          *n += _DIM_DOUBLE_LEN;
          print(" --> Creating Pydouble %f", *(double *)&buf[*n] );
        } else goto short_buffer;
      break;
    case _DIM_XTRA:
      if (mult == MUL_INFINITE) mult = (len + _DIM_XTRA_LEN - 1 - *n) /
        _DIM_XTRA_LEN;
      for (j = 0; j < mult; ++j)
        if (*n + _DIM_XTRA_LEN <= len) {
          PyList_Append(list, PyLong_FromLongLong(*(long long *)
                                                  &buf[*n]));
          *n += _DIM_XTRA_LEN;
        } else goto short_buffer;
      break;
    case _DIM_STRING:
      if (mult == MUL_INFINITE)  mult = len - *n - 1;
      if (*n + mult < len) {
        tmp = PyString_FromStringAndSize( &buf[*n], mult);
        PyList_Append(list, tmp);
        print(" --> Creating long PyString: '%s'", PyString_AsString(tmp) )
        *n += mult;
      } else {
        tmp =  PyString_FromStringAndSize(&buf[*n], len - *n);
        PyList_Append(list, tmp);
        print(" --> Creating short PyString: '%s'", PyString_AsString(tmp) );
        goto short_buffer;
      }
      break;
    case _DIM_SHORT:
      if (mult == MUL_INFINITE)
        mult = (len + _DIM_SHORT_LEN - 1 - *n) / _DIM_SHORT_LEN;
      for (j = 0; j < mult; ++j)
        if (*n + _DIM_SHORT_LEN <= len) {
          PyList_Append(list, PyInt_FromLong( *(short int *) &buf[*n]));
          // print("adding short int %h", *(short int*) &buf[*n]);
          *n += _DIM_SHORT_LEN;
        } else goto short_buffer;
      break;
    default:
      goto short_buffer;
    }
  }
 short_buffer:
  return list;
}


static PyObject*
dim_buf_to_tuple(const char *schema, const char *buf, int len, int *n) {
  PyObject *list, *tuple;
  list = dim_buf_to_list(schema, buf, len, n);
  if ( !list ) {
    print("Could not convert buffer to list");
    return NULL;
  }
//  print("Converting list to tuple");
  tuple = list2Tuple(list);
  Py_DECREF(list);
  return tuple;
}

static int
getSizeFromFormat(const char* format) {
  int ptr=0, type=0, mult=0, size=0;
  while ( next_element(format, &ptr, &type, &mult) ) {
    switch (type) {
    case _DIM_LONG:size += mult * _DIM_LONG_LEN; break;
    case _DIM_INT: size += mult * _DIM_INT_LEN; break;
    case _DIM_STRING: size += mult * _DIM_CHAR_LEN; break;
    case _DIM_FLOAT: size += mult * _DIM_FLOAT_LEN;  break;
    case _DIM_DOUBLE: size += (mult * _DIM_DOUBLE_LEN); break;
    case _DIM_XTRA: size += mult * _DIM_XTRA_LEN;  break;
    case _DIM_SHORT: size += mult * _DIM_SHORT_LEN; break;
    default: print("Bad type character extracted from character(%c). Type is %d\n", format[ptr], type);
      return 0;
    }
//    print("Found element %d with multiplicity %d", type, mult);
  }
//  print("Size of buffer is %d", size);
  return size;
}

static int
tuple2Buffer (char *buffer, const char *format, PyObject *pyTuple){
  int ptr=0, buf_ptr=0, type=0, mult=0, elem=0, j, str_size;
  PyObject *tmp;

  int i;
  float f;
  double d;
  short s;
  long l;
  long long x;
  char *str;


  print("Copying Python values to a buffer. Format is %s", format );
  while ( next_element(format, &ptr, &type, &mult) ){
    print( "Got type %d, mult %d, ptr %d", type, mult, ptr);
    for (j=0; j<mult; j++){
      if ( PyTuple_Check(pyTuple) ) {
        tmp = PyTuple_GetItem(pyTuple, elem++);
      } else {
        tmp = pyTuple;
      }
      if ( !tmp ) {
        print("Could not get item for format char %c\n", format[ptr]);
        return 0;
      }
      switch (type) {
      case _DIM_INT : i = PyInt_AsLong(tmp);
        memcpy(&buffer[buf_ptr], &i, _DIM_INT_LEN);
        print("Adding int %d to buffer", i);
        buf_ptr += _DIM_INT_LEN;
        break;
      case _DIM_LONG : l = PyInt_AsLong(tmp);
        memcpy(&buffer[buf_ptr], &l, _DIM_LONG_LEN);
        print("Adding long %ld to buffer", PyInt_AsLong(tmp));
        buf_ptr += _DIM_LONG_LEN;
        break;
      case _DIM_STRING:
        str = PyString_AsString(tmp);
        if ( strlen(str) < (unsigned int)mult )
          str_size = strlen(str)+1;
        else
          str_size = mult;
        memcpy(&buffer[buf_ptr], str, str_size);
        print("Adding string %s to buffer", PyString_AsString(tmp));
        buf_ptr += _DIM_CHAR_LEN * mult;
        mult = 0;
        break;
      case _DIM_FLOAT: f = PyFloat_AsDouble(tmp);
        memcpy(&buffer[buf_ptr], &f, _DIM_FLOAT_LEN);
        buf_ptr += _DIM_FLOAT_LEN;
        print("Adding float %f to buffer", PyFloat_AsDouble(tmp));
        break;
      case _DIM_DOUBLE: d = PyFloat_AsDouble(tmp);
        memcpy(&buffer[buf_ptr], &d, _DIM_DOUBLE_LEN);
        buf_ptr += _DIM_DOUBLE_LEN;
        print("Adding double %f to buffer", PyFloat_AsDouble(tmp));
        break;
      case _DIM_XTRA: x = PyLong_AsLongLong(tmp);
        memcpy(&buffer[buf_ptr], &x, _DIM_XTRA_LEN);
        buf_ptr += _DIM_XTRA_LEN;
        print( "Adding extra long %Ld to buffer", PyLong_AsLongLong(tmp) );
        break;
      case _DIM_SHORT: s = PyInt_AsLong(tmp);
        memcpy(&buffer[buf_ptr], &s, _DIM_SHORT_LEN);
        buf_ptr += _DIM_SHORT_LEN;
        print( "Adding short %hd to buffer", s );
        break;
      default: print("bad character %c. Type is %d\n", format[ptr], type);
        return 0;
      }
    }
  }
  print("Succesfully converted arguments");
  return 1;
}

static void
dim_callbackCommandFunc(void* uTag, void* address, int* size) {
  long tag, uniqueTag = *(long*)uTag;
  PyObject *pyFunc, *arg, *res;
  char *cptr;
  // char *buf = (char*)malloc( (*size) * sizeof(char));
  cptr  = (char*)address;
  int len = *size;
  PyGILState_STATE gstate;
  // ServiceCallback *callback;
  int n=0;

  // printf("dim_callbackCommandFunc:: Got unique tag %l \n", uniqueTag);
  // printf("%d %d %d\n", sizeof(int), sizeof(long double), sizeof(char));
  pyFunc = cmndUniqueTag2PythonFunc[uniqueTag]->func;
  tag = cmndUniqueTag2PythonFunc[uniqueTag]->tag;
  arg = dim_buf_to_tuple(cmndUniqueTag2PythonFunc[uniqueTag]->format,(char*)address, *size, &n);
  printf("dim_callbackCommandFunc:: dim_buf_to_list returned %s %d\n", cmndUniqueTag2PythonFunc[uniqueTag]->format, len);
  gstate = PyGILState_Ensure();
  printf("dim_callbackCommandFunc:: calling python func: %p\n", pyFunc);
  res = PyEval_CallObject(pyFunc, arg);
  Py_DECREF(arg);
  if (!res) {
    PyErr_Print();
  } else {
    print("Got result %p", res);
    Py_DECREF(res);
  }
  PyGILState_Release(gstate);
}

/******************************************************************************/
/* DIC interface functions */
/******************************************************************************/

static PyObject*
dim_dic_set_dns_node(PyObject* self, PyObject* args) {
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
dim_dic_get_dns_node(PyObject* self, PyObject* args) {
  /* calls dic_get_dns_node(char* node)
     the function should return the DNS node
  */  
  char names[256];

  if ( !dic_get_dns_node(names) )
    names[0] = 0;
  return Py_BuildValue("s", names);
}

static PyObject*
dim_dic_set_dns_port(PyObject* self, PyObject* args) {
  /* Calls dic_set_dns_port(int port).
     Return success or failure (1 or 0).
  */
  unsigned int port;
  int i;

  if ( !PyArg_ParseTuple(args, "I", &port) ) {
    PyErr_SetString(PyExc_TypeError, "Argument 'port' must be a pozitive integer");
    return NULL;
  }
  i = dic_set_dns_port(port);
  return Py_BuildValue("i", i);
}

static PyObject*
dim_dic_get_dns_port(PyObject* self, PyObject* args) {
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
dim_dic_get_id(PyObject* self, PyObject* args) {
  /* Calls: int dic_get_id (char* name)
     Return the client name or an empty string if the command was not successful
  */
  char name[256];
  int res;

  res = dic_get_id(name);
  if (!res)
    name[0] = 0;
  return Py_BuildValue("s", name);
}

static PyObject*
dim_dic_disable_padding(PyObject* self, PyObject* args) {
  /* Calls: int dic_disable_padding(void)
     Returns void.
  */
  dic_disable_padding();
  Py_RETURN_NONE;
}

static PyObject*
dim_dic_get_quality(PyObject* self, PyObject* args) {
  /* Calls: int dic_get_quality (unsigned int service_id);
     Returns the service quality
  */
  unsigned int service_id;
  int res;

  if ( !PyArg_ParseTuple(args, "I", &service_id) ) {
    PyErr_SetString(PyExc_TypeError, "service id should be an unsigned integer");
    return NULL;
  }
  res = dic_get_quality(service_id);
  return Py_BuildValue("i", res);
}

static PyObject*
dim_dic_get_timestamp(PyObject* self, PyObject* args) {
  /* Calls int dic_get_timestamp (service_id, secs, milisecs).
     With: unsigned int service_id
            int *secs
            int *milisecs
     Returns: whether the command was successfully
  */
  unsigned int service_id;
  int secs, milisecs=0, res=0;

  if (! PyArg_ParseTuple(args, "Iii", &service_id) ) {
    PyErr_SetString(PyExc_TypeError, "service id should be an unsigned integer");
    return NULL;
  }
  res = dic_get_timestamp(service_id, &secs, &milisecs);
  return Py_BuildValue("ii", secs, milisecs);
}

static PyObject*
dim_dic_get_format(PyObject* self, PyObject* args) {
  /*Calls: char *dic_get_format (unsigned int service_id)
    Returns a string containing the format
  */
  unsigned int service_id;
  char* format=NULL;

  if (! PyArg_ParseTuple(args, "I", &service_id) ) {
    PyErr_SetString(PyExc_TypeError, "Service id should be an unsigned integer");
    return NULL;
  }
  format = dic_get_format(service_id);
  return Py_BuildValue("s", format);
}

static PyObject*
dim_dic_release_service(PyObject* self, PyObject* args){
  /* Calls: void dic_release_service (unsigned int service_id)
     Returns: void
  */
  unsigned int service_id;
  _dic_info_service_callback *tmp;

  if ( !PyArg_ParseTuple(args, "I", &service_id) ) {
    print("Invalid service id specified");
    PyErr_SetString(PyExc_TypeError, "Service id should be an unsigned integer");
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
dim_dic_info_service(PyObject* self, PyObject* args) {
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
	// Init threading if needed
	init_threading();

  print("Registering service: ");
  print("timeout = %d", timeout);
  if ( !PyArg_ParseTuple(args, "s#s#O|iiiO",&name,
                                           &name_size,
                                           &format,
                                           &format_size,
                                           &pyFunc ,
                                           &service_type,
                                           &timeout,
                                           &tag,
                                           &default_value) )
  {
    goto invalid_args;
  }
  print("Callback is %p", pyFunc);
  if ( !PyCallable_Check(pyFunc) ){
    goto invalid_args;
  }
  //increasing the ref count of the python objects
  Py_INCREF(pyFunc);
  if (default_value) {
      Py_INCREF(default_value);
  }
  //creating a new service structure
  tmp = (_dic_info_service_callback*)malloc(sizeof(_dic_info_service_callback));
  if (!tmp) {
      goto no_memory;
  }
  buffer_size = getSizeFromFormat(format);
  tmp->tag = tag;
  tmp->size = buffer_size;
  print("Buffer size is %d", buffer_size);
  tmp->buffer = (char*)malloc(sizeof(char*)*buffer_size);
  tmp->format = (char*)malloc(sizeof(char)*format_size+1);
  tmp->name = (char*)malloc(sizeof(char)*name_size+1);
  tmp->defaultArgument = default_value;
  tmp->func = pyFunc;
  if ( !tmp->format || !tmp->name || !tmp->buffer) {
    goto no_memory;
  }
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
    print("Creating service %s failed. Receive service ID %d", name, service_id);
    goto dealocate;
  }
  print ("Service %s registered with id %d", name, service_id);
  return Py_BuildValue("i", service_id);

  //invalid arguments
  invalid_args:
    PyErr_SetString(PyExc_TypeError,
          "Invalid parameters. Expected arguments:string name               ,\
                                                  string format             ,\
                                                  int service_type          ,\
                                                  int timeout               ,\
                                                  PyObject* callbackFunction,\
                                                  int tag                   ,\
                                                  PyObject* default_value ");
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


/******************************************************************************/
/* DIC interface internal functions */
/******************************************************************************/

void _dic_info_service_dummy (void* tag, void* buffer, int* size) {
  /* Stub function for passing the received data from a service to a python callback.
     Receives tag, buffer, size
  */
  PyObject* args, *res;
  _dic_info_service_callback* svc;
  int n=0;
  PyThreadState* st = 0;
  try   {
// Let us try to handle the threadStates correctly (should we run _init_section() here?)
	st = PyThreadState_New(DIM::s_mainThreadState->interp);
  PyEval_AcquireThread(st);
	}
  catch(std::exception& e)  {
    print("Exception occurred: %s",e.what());
    handlePythonError(st);
  }
  catch(...)  {
    print("Unknwon exception occurred....");
    handlePythonError(st);
  }
//  print(">------------------------------------------------------------------>");
  svc = _dic_info_service_buffer2Callback[(char*)buffer];
  if ( !svc ){
    print("Invalid update service request");
    return;
  }
  if ( !(*size) ) {
    /* The service update request failed. Passing default argument */
    args = svc->defaultArgument;
  } else {
    print("Converting C buffer to python arguments. Size of the received buffer is %d and format '%s'", svc->size, svc->format);
    args = dim_buf_to_tuple(svc->format, (char*)buffer, *size, &n);
  }
  if (args) {
      print("Executing callback %p for service %s with n %d", svc->func, svc->name,n);
      res = PyEval_CallObject(svc->func, args);
  } else {
    //service failed and a default value was not specified
    print("Service error or could not get values from it");
    return;
  }

  if ( !res ){
     print("Callback to python function %p failed", svc->func);
     return;
  }
  Py_DECREF(res);
  Py_XDECREF(args);
  PyEval_ReleaseThread(st);
  PyThreadState_Clear(st);
  PyThreadState_Delete(st);
//  print("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
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
  {    "dic_info_service"              ,
       dim_dic_info_service                ,
       METH_VARARGS                    ,
       "Called by a client when a service is not needed anymore." //This should be corrected, I think
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

