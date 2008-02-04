/* **************************************************************************
 * Utility functions to used for creating the PyDim module.
 * These are used primarily for converting between DIM C buffers and python 
 * objects.
 * 
 * Authors: N. Newfeld, R. Stoica
 * Date: Nov. 2007
 * *************************************************************************/

// includes 
extern "C" {
#include <Python.h>
#include "structmember.h"
#include <stdlib.h>
#include <stdio.h>
}

/* **************************************************************************
 * Various defines necesarry for converting between the dim buffers and Python
 * objects.
 * TODO: All the tests until were done communicating between the same 
 * arhitectures (32 bits and 64 bits). Not sure DIM supports communicating
 * between platforms that have different sizes for the basic types.
 * Anyway this I can't fix here if DIM doesn't support this.
 * **************************************************************************/
#define print(...) printf("DIM Wrapper: %s:%u ::%s: ", __FILE__, __LINE__, __FUNCTION__); printf(__VA_ARGS__); printf("\n");

#define _DIM_INT 0
#define _DIM_INT_LEN sizeof(int)

#define _DIM_FLOAT 1
// #define _DIM_FLOAT_LEN 4
#define _DIM_FLOAT_LEN sizeof(float)

#define _DIM_DOUBLE 2
// #define _DIM_DOUBLE_LEN 8
#define _DIM_DOUBLE_LEN sizeof(double)

#define _DIM_XTRA 3
// #define _DIM_XTRA_LEN 8
#define _DIM_XTRA_LEN sizeof(long long)

#define _DIM_STRING 4
#define _DIM_CHAR_LEN 1

#define _DIM_SHORT 5
#define _DIM_SHORT_LEN sizeof(short)

#define _DIM_LONG 6
#define _DIM_LONG_LEN sizeof(long)
// #define _DIM_INT_LEN sizeof(int)

#define MUL_INFINITE -1
/* multiplicity == MUL_INFINITE  means an arbitrary amount of data types (e.g. ..;I)
 * multiplicity == 0 is an illegal value and will not be returned by this
 * function 
 */

/* **************************************************************************
 * Utility functions
 * *************************************************************************/
static void printPyObject(PyObject *object) 
{
   if ( !object ) return;
   PyObject_Print(object, stdout, Py_PRINT_RAW);
   printf("\n");
   return;
}

void
printDimBuf(const char *buf, int size) {
    int i;

    for (i=0; i<size; i++) 
        printf("Byte %d: ASCII %c, CODE: %d\n", i, buf[i], buf[i]);

}

static int
verify_dim_format(const char *format) 
{
    /* Return True in case of success and False otherwise
     */
    int ptr=0, x, newptr;
    int size=strlen(format);
    char delimiter=';';    
    const char itemtypes[]="ICLSFDXiclsfdx";
    const char digits[]="0123456789";
    
    while (ptr<size) {        
        for (newptr=ptr; newptr<size && format[newptr]!=delimiter; newptr++);                 
        // We have found a new group of parameters
        if (!strchr(itemtypes, format[ptr]))
            // This means the type letter is unknown
            return 0;
        if (ptr+1 == newptr) {
            /* This means we only have the item type letter. 
            *  Checking to see if we are at the end
            */
            if (newptr == size)
                // We are at the end, newptr is on the \0 string terminating char
                return 1;
            else 
                // in the middle of the format string the number of items is missing
                return 0;            
        } else {
            // The multiplicity of the parameters is also specified
            if (format[ptr+1] != ':')
                /* We were expecting a colon separator between the type
                 * and the multiplicity 
                 */
                return 0;
            if (ptr+2 == newptr) 
                // This means that there is no digit after colon
                return 0; 
            for (x=ptr+2; x<=newptr-1; x++) {
                if (!strchr(digits, format[x]))
                    // We were expecting a digit
                    return 0;                
            }
        } // end if-else
        ptr=newptr+1;            
    } // end while
    return 1;
} // end verify_dim_format


static int
next_element(const char *schema, unsigned int *p, int *type, int *mult)
{
    char *endptr;
    
    // check to see if we haven't gone pass the schema string limit
    if (*p >= strlen(schema))
        return 0;
     
    if (schema[*p] == '\0') return 0;
    switch (toupper(schema[*p])) {
        case 'I':
        case 'L': *type = _DIM_INT; break;
        case 'C': *type = _DIM_STRING; break;
        case 'F': *type = _DIM_FLOAT; break;
        case 'D': *type = _DIM_DOUBLE; break;
        case 'X': *type = _DIM_XTRA; break;
        case 'S': *type = _DIM_SHORT; break;
        default: 
            print("Bad type character %c", schema[*p]);
            *type = -1;
            return 0;
    }
    
    switch (schema[++(*p)]) {
        case '\0': *mult = MUL_INFINITE; break;
        // this is not stricly correct from DIM point of view.
        case ';': *mult = 1; ++(*p); break; 
        case ':':              
              (*p)++;              
              *mult = strtoul(&schema[*p], &endptr, 10);
              if (endptr == &schema[*p]) {
                  print("illegal number\n");
                  return 0;
              }              
              *p += (endptr - &schema[*p]) + ((*endptr == ';') ? 1 : 0);              
              break;
        default:
              printf("Bad multiplicity character %c\n", schema[*p]);
              return 0;
    }    
    return 1;
}

static PyObject *
dim_buf_to_list(const char *schema, const char *buf, unsigned int len)
{
    /* TODO: check for memory leaks. 
     * Not sure who owns the new reference after Py<type>_From<type>
     * 
     * NOTE: Compilers will pad structures to the arhitecture word size.
     * It is NOT recommended to pass the lenght of the buffer using 
     * strlen() if the last last type field of the format has multiplicity
     * unlimited. You will see garbage in the buffer after conversion!
     * Workarounds: disable padding from the declared structure and/or
     * specify the exact size of the buffer.
     */
    int j=0, type=0, mult=0, n=0;
    unsigned int m=0;
    PyObject *list, *tmp;
    float f;
    
    if ( !(list = PyList_New(0)) ) {
        // could not create a new Python list
        print("Could not create list python object");      
        return NULL;
    }
    while (next_element(schema, &m, &type, &mult)) {        
        switch (type) {
            case _DIM_INT:
                if (mult == MUL_INFINITE)
                    mult = (len - n) / _DIM_INT_LEN;
                for (j = 0; j < mult; ++j)
                    if (n + _DIM_INT_LEN <= len) {
                        PyList_Append(list, PyInt_FromLong( *(int *) &buf[n]));                        
                        n += _DIM_INT_LEN;
                    } else goto short_buffer;
                break;
            case _DIM_FLOAT:
                if (mult == MUL_INFINITE) 
                    mult = (len - n) / _DIM_FLOAT_LEN;
                for (j = 0; j < mult; ++j)
                    if (n + _DIM_FLOAT_LEN <= len) {
                        f = *(float *)(&buf[n]);
                        PyList_Append(list, PyFloat_FromDouble(f));
                        n += _DIM_FLOAT_LEN;
                    } else goto short_buffer;
                break;
            case _DIM_DOUBLE:
                if (mult == MUL_INFINITE)
                    mult = (len - n) / _DIM_DOUBLE_LEN;
                for (j = 0; j < mult; ++j)
                    if (n + _DIM_DOUBLE_LEN <= len) {
                        PyList_Append(list, 
                                      PyFloat_FromDouble(*(double *)&buf[n]) );
                        n += _DIM_DOUBLE_LEN;                        
                    } else goto short_buffer;
                break;
            case _DIM_XTRA:
                if (mult == MUL_INFINITE) 
                    mult = (len - n) / _DIM_XTRA_LEN;
                for (j = 0; j < mult; ++j)
                    if (n + _DIM_XTRA_LEN <= len) {
                        PyList_Append(list, 
                                 PyLong_FromLongLong(*(long long *)&buf[n]));
                        n += _DIM_XTRA_LEN;
                    } else goto short_buffer;
                break;
            case _DIM_STRING:
                if (mult == MUL_INFINITE)  
                    mult = (len - n) / _DIM_CHAR_LEN;
                if ((unsigned int)(n + mult) <= len) { // ugly
                    tmp = PyString_FromStringAndSize(&buf[n], mult);
                    PyList_Append(list, tmp);                    
                        n += mult;
                } else {
                    tmp =  PyString_FromStringAndSize(&buf[n], len - n);
                    PyList_Append(list, tmp);                    
                    goto short_buffer;
                }
                break;
            case _DIM_SHORT:
                if (mult == MUL_INFINITE)
                    mult = (len - n) / _DIM_SHORT_LEN;
                for (j = 0; j < mult; ++j)
                    if (n + _DIM_SHORT_LEN <= len) {
                        // ugly
                        PyList_Append( list, PyInt_FromLong( *(short int *)&buf[n]) );                         
                        n += _DIM_SHORT_LEN;
                    } else goto short_buffer;
                break;
            default:
                goto short_buffer;
        } // end switch
    } // end while

    // print("Converting DIM buffer to list");
    // printDimBuf(buf, len);
    // printPyObject(list);
    return list;

short_buffer:
    print("Provided buffer was to short to convert all the arguments using format" );
    print("Buffer size %d, format %s", len, schema);
    Py_XDECREF(list);
    return NULL;
}


static PyObject*
list2Tuple(PyObject* list) 
{
  int size, i;
  PyObject* tuple;
  
  if ( !PyList_Check(list) ) return NULL;
  size = PyList_Size(list);
  tuple = PyTuple_New(size);  
  for(i=0; i<size; i++) {
    PyTuple_SetItem(tuple, i, PyList_GetItem(list, i) );
    //PyObject_Print( PyList_GetItem(list, i), stdout, Py_PRINT_RAW );    
  }  
//  printPyObject(list);
//  printPyObject(tuple);
  return tuple;
}


static PyObject*
dim_buf_to_tuple(const char *schema, const char *buf, int len) 
{
    PyObject *list, *tuple;    
    
    list = dim_buf_to_list(schema, buf, len);
    if ( !list ) {
        print("Could not convert buffer to list");
        return NULL;
    }    
    tuple = list2Tuple(list);
    Py_DECREF(list);
    return tuple;
}


static unsigned int
getSizeFromFormat(const char* format) {
    int type=0, mult=0, size=0;
    unsigned int ptr=0;
    
    while ( next_element(format, &ptr, &type, &mult) ) {
        if (mult == -1) {
            // we can't calculate the maximum size in this case
            // 0 means error
            return 0;
        } 
        switch (type) {
            case _DIM_LONG:size += mult * _DIM_LONG_LEN; break;
            case _DIM_INT: size += mult * _DIM_INT_LEN; break;
            case _DIM_STRING: size += mult * _DIM_CHAR_LEN; break;
            case _DIM_FLOAT: size += mult * _DIM_FLOAT_LEN;  break;
            case _DIM_DOUBLE: size += (mult * _DIM_DOUBLE_LEN); break;
            case _DIM_XTRA: size += mult * _DIM_XTRA_LEN;  break;
            case _DIM_SHORT: size += mult * _DIM_SHORT_LEN; break;
            default: 
                print("Bad type char extracted from (%c. Type is %d\n",
                      format[ptr], type);
                return 0;
        }        
    }    
    
    return size;
}


static unsigned int
getElemNrFromFormat(const char *format) 
{
    /* Receives a closed format string and returns the total number of 
     * elements expected from it.
     * In case of any errors returns 0.
     */
    int type=0, mult=0, total=0;
    unsigned int ptr=0;
    
    while (next_element(format, &ptr, &type, &mult) ) {
        if (mult == -1) {
            // we can't calculate the maximum size in this case
            // 0 means error
            return 0;
        } 
        switch (type) {
            case _DIM_LONG  : 
            case _DIM_INT   :             
            case _DIM_FLOAT : 
            case _DIM_DOUBLE: 
            case _DIM_XTRA  : 
            case _DIM_SHORT : total += mult; break;
            //in Python S:<number> is only one string
            case _DIM_STRING: ++total; break;  
            default: 
                print("Bad type char extracted from (%c. Type is %d\n",
                      format[ptr], type);
                return 0;
        }        
    }    
     
    return total;
} 
     
static unsigned int
getSizeFromFormatAndObjects(PyObject *iter, const char* format) 
{
    /* Returns the size needed for converting the Python iterator to a
     * buffer according to a DIM format. The format can have an unlimited
     * number of ending variables of the same type.
     */
     
    unsigned int format_size=0, iter_size=0, obj_nr=0, lastobj_size=0, total_size=0;
    const char itemtypes[]="ICLSFDXiclsfdx";
    char *temp_format;
    PyObject *last;
    
    format_size = strlen(format);
    if (!strchr(itemtypes, format[format_size-1])) {
        // this means we can deduct the size directly from format
        return getSizeFromFormat(format);
    }
    // we have an open ended format string
    if (!PySequence_Check(iter)) {
        print("Python object is not a sequence");
        return 0;    
    }
    // Get the size of all fixed number of arguments (if they exist)
    if (format_size>1) {
        temp_format = new char[format_size];
        strncpy(temp_format, format, format_size-1);
        temp_format[format_size-1]='\0';     
        total_size = getSizeFromFormat(temp_format);
        // Get the total number of objects for which the size was calculated
        obj_nr = getElemNrFromFormat(temp_format);
        delete temp_format;        
    } 
    
    // Get the total size of the sequence
    iter_size=PySequence_Size(iter);
    
    if (iter_size <= obj_nr) {
        /* Means the iterator does not contain enough elements
         * This should never be the case but i'm returning the total size 
         * for the fixed parameters
         */
         return total_size;
    }
    // Get the size of the objects that follows
    switch (toupper(format[format_size-1])) {
            case 'L' : 
            case 'I' : lastobj_size = _DIM_INT_LEN   ; break;            
            case 'F' : lastobj_size = _DIM_FLOAT_LEN ; break;
            case 'D' : lastobj_size = _DIM_DOUBLE_LEN; break;
            case 'X' : lastobj_size = _DIM_XTRA_LEN  ; break;
            case 'S' : lastobj_size = _DIM_SHORT_LEN ; break;
            case 'C' :
                /* this is tricky: we only have a string of unlimited size
                 * I have to get the string length and add it.
                 * TODO: subsequent objects are ignored.
                 */
                last = PySequence_GetItem(iter, obj_nr);
                if (!PyString_Check(last)) {
                   print("Invalid Python object expected a string");
                   return 0;
                }
                total_size += PyString_Size(last);                                
                return total_size;
                break;
            default: 
                print("Bad type char (%c) extracted from %s.", format[format_size-1], format);
                return 0;
    }            
    total_size = total_size + lastobj_size * (iter_size - obj_nr);
    
    return total_size;
}


static int
iterator_to_buffer(PyObject     *iter,   /* list or tuple PyObject */ 
                   char         *buffer, /* buffer to perform conversion */
                   unsigned int size,    /* maximum size of buffer */
                   const char   *format) /* the format to use */
{
    /* This function allocates a new buffer and converts the python
     * iterator to it according to format specified.
     */
    int type=0, mult=0, j;
    PyObject *tmp;
    unsigned int ptr=0, buf_ptr=0, str_size=0, iter_size=0, elem=0;
    int i;
    float f;
    double d;
    short s;
    long l;
    long long x;
    char *str;
    
    while (next_element(format, &ptr, &type, &mult) ){        
        for (j=0; j<mult || (mult==-1 && buf_ptr<size); j++){
            if (PySequence_Check(iter)) {
                tmp = PySequence_GetItem(iter, elem++);
            } else {
                print("Provided python object is not a sequence");
                return 0;
            }
            if (!tmp) {
                print("Iterator object does not hold enough values to match format");                
                return 0;
            }
            switch (type) {
                case _DIM_INT :
                    if (size < buf_ptr + _DIM_INT_LEN)
                        goto shortbuffer; 
                    i = PyInt_AsLong(tmp);
                    memcpy(&buffer[buf_ptr], &i, _DIM_INT_LEN);                        
                    buf_ptr += _DIM_INT_LEN;
                    break;
                case _DIM_LONG :
                    if (size < buf_ptr + _DIM_LONG_LEN)
                        goto shortbuffer; 
                    l = PyInt_AsLong(tmp);
                    memcpy(&buffer[buf_ptr], &l, _DIM_LONG_LEN);                         
                    buf_ptr += _DIM_LONG_LEN;
                    break;
                case _DIM_STRING:
                         str = PyString_AsString(tmp);                         
                         if ( strlen(str) < (unsigned int)mult || mult==-1 ) {
                             str_size = strlen(str)+1;
                             if ((buf_ptr+str_size)>size) {
                                  // this means the python string is to 
                                  // big to be completly converted
                                  str_size=size-buf_ptr;
                             }
                         } else {
                             // Truncating string
                             str_size = mult;
                         }
                         memcpy(&buffer[buf_ptr], str, str_size);                         
                         buf_ptr += _DIM_CHAR_LEN * str_size;
                         mult = 0;
                         break;
                case _DIM_FLOAT: 
                    if (size < buf_ptr + _DIM_FLOAT_LEN)
                        goto shortbuffer;                    
                    f = PyFloat_AsDouble(tmp);
                    memcpy(&buffer[buf_ptr], &f, _DIM_FLOAT_LEN);
                    buf_ptr += _DIM_FLOAT_LEN;                         
                    break;
                case _DIM_DOUBLE:
                    if (size < buf_ptr + _DIM_DOUBLE_LEN)
                        goto shortbuffer;                     
                    d = PyFloat_AsDouble(tmp);
                    memcpy(&buffer[buf_ptr], &d, _DIM_DOUBLE_LEN);
                    buf_ptr += _DIM_DOUBLE_LEN;                          
                    break;
                case _DIM_XTRA:
                    if (size < buf_ptr + _DIM_XTRA_LEN)
                        goto shortbuffer;
                    x = PyLong_AsLongLong(tmp);
                    memcpy(&buffer[buf_ptr], &x, _DIM_XTRA_LEN);
                    buf_ptr += _DIM_XTRA_LEN;                        
                    break;
                case _DIM_SHORT: 
                    if (size < buf_ptr + _DIM_SHORT_LEN)
                        goto shortbuffer;
                    s = PyInt_AsLong(tmp);
                    memcpy(&buffer[buf_ptr], &s, _DIM_SHORT_LEN);
                    buf_ptr += _DIM_SHORT_LEN;                         
                    break;
                default: 
                    print("bad character %c. Type is %d\n", format[ptr], type);
                    return 0;
            }
        }
    }    
    iter_size = PySequence_Size(iter);
    if ( iter_size != elem ) {
        // Check to see if all the objects from the iterator were used
        print("WARNING: Iterator object holds more objects than format specifies");           
    }
    // print("Converting iterator to buffer");
    // printDimBuf(buffer, size);
    // printPyObject(iter);
    return 1;
    
 shortbuffer:
     print(" WARNING: The provided buffer is not big enough to hold all the conversion objects. Truncating...");
     return 1;
}


static int
iterator_to_allocated_buffer(PyObject  *iter, /* list or tuple PyObject */
                   const char   *format,  /* the format to use */ 
                   char         **buffer, /* buffer to perform conversion */
                   unsigned int *size)    /* maximum size of buffer */
                   
{
    /* This function allocates a new buffer and converts the python
     * iterator objects according to 'format'.
     * Input: PyTuple or PyList
     * Output: pointer to the newly created buffer and its size
     */          
     *buffer = NULL;
     *size = 0;
      
     *size = getSizeFromFormatAndObjects(iter, format);
     if (!*size) {
         // could not figure out the size of the needed buffer
         return 0;
     }
     *buffer= new char[*size];
     if (iterator_to_buffer(iter, *buffer, *size, format)) {
         // the call succeded
         return 1;
     } else {
         return 0;
			}    
}