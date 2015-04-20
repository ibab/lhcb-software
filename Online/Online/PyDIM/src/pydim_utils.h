/** **************************************************************************
 * \brief Utility functions to used for creating the PyDim module.
 * These are used primarily for converting between DIM C buffers and python
 * objects.
 *
 * \authors M. Frank, N. Neufeld, R. Stoica
 * \date Nov. 2007 - September 2008
 *
 * Various defines necessary for converting between the dim buffers and Python
 * objects.
 *
 * TODO: All the tests until were done communicating between the same
 * architectures (32 bits and 64 bits). Not sure DIM supports communicating
 * between platforms that have different sizes for the basic types.
 * Anyway I can't fix this if DIM doesn't support this.
 * *************************************************************************/

#ifndef PYDIM_UTILS
#define PYDIM_UTILS 1

// includes
extern "C" {
#include <Python.h>
#include "structmember.h"
}
#include <cstdlib>
#include <cstdio>

#ifdef _WIN32
 #include <cstdarg> 
  static inline void ___print(const char* fmt,...)
  {
    va_list args; va_start(args,fmt); 
    vprintf(fmt,args); printf("\n"); va_end(args); 
  }
 #define print printf("DIM Wrapper: %s:%u ::%s: ", __FILE__, __LINE__, __FUNCTION__); ___print 
#else
 #define print(...) printf("DIM Wrapper: %s:%u ::%s: ", __FILE__, __LINE__, __FUNCTION__); printf(__VA_ARGS__); printf("\n");
#endif

#ifdef __DEBUG
 #define debug print
 #define debugPyObject printPyObject
#else
 #define debug(...) /* __VA_ARGS__ */
 #define debugPyObject(...) /* __VA_ARGS__ */
#endif

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#endif

#ifndef Py_RETURN_NONE
#define Py_RETURN_NONE do { Py_INCREF(Py_None); return Py_None; } while(0);
#endif
#define errmsg(x) do { fprintf(stderr, "%s: %s\n", __FUNCTION__, x); } while(0);

#define _DIM_INT 0
#define _DIM_INT_LEN sizeof(int)

#define _DIM_FLOAT 1
#define _DIM_FLOAT_LEN sizeof(float)

#define _DIM_DOUBLE 2
#define _DIM_DOUBLE_LEN sizeof(double)

#define _DIM_XTRA 3
#define _DIM_XTRA_LEN sizeof(long long)

#define _DIM_STRING 4
#define _DIM_CHAR_LEN 1

#define _DIM_SHORT 5
#define _DIM_SHORT_LEN sizeof(short)

#define _DIM_LONG 6
#define _DIM_LONG_LEN sizeof(long)

#define MUL_INFINITE -1
/* multiplicity == MUL_INFINITE  means an arbitrary amount of data types
 *                  (e.g. ..;I)
 * multiplicity == 0 is an illegal value and will not be returned by this
 *                 function
 */

/* **************************************************************************
 * Utility functions
 * *************************************************************************/
#ifndef DIMCPP_MODULE
static int listOrTuple2Int(PyObject* pyObj, int** buffer);
#endif

#ifndef DIMCPP_MODULE
static PyObject* stringList_to_tuple (char* services);
#endif

#ifndef DIMCPP_MODULE
static PyObject* pyCallFunction (PyObject* pyFunc, PyObject* args);
#endif

#ifdef __DEBUG
static void printPyObject(PyObject *object);
   
static void printDimBuf(const char *buf, int size);
#endif


#ifndef DIMC_MODULE
static int verify_dim_format(const char *format);
#endif


static int next_element(const char *schema, unsigned int *p, int *type, int *mult);


static PyObject *dim_buf_to_list(const char *schema, const char *buf, unsigned int len);

#ifdef __DEBUG 
static PyObject* list2Tuple(PyObject* list);
#endif

static PyObject* dim_buf_to_tuple(const char *schema, const char *buf, int len);

static int getSizeFromFormat(const char* format);

static unsigned int getElemNrFromFormat(const char *format);

static unsigned int getSizeFromFormatAndObjects(PyObject *iter, const char* format);

static int iterator_to_buffer(PyObject     *iter,    /* list or tuple PyObject */
                              char         *buffer,  /* buffer to perform conversion */
                              unsigned int size,     /* maximum size of buffer */
                              const char   *format); /* the format to use */

static int iterator_to_allocated_buffer(PyObject  *iter,       /* list or tuple */
                                        const char   *format,  /* the format to use */
                                        char  **buffer,        /* conversion buffer */
                                        unsigned int *size );  /* maximum size of buffer */
#endif
