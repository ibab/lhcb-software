/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/
#ifndef ONLINE_FINITESTATEMACHINE_TYPEDOBJECT_H
#define ONLINE_FINITESTATEMACHINE_TYPEDOBJECT_H

// Framework include files
#include "FiniteStateMachine/config.h"

// C/C++ include files
#include <string>

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine {

  // Forward declarations
  class Type;

  /**@class TypedObject  TypedObject.h FiniteStateMachine/TypedObject.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class TypedObject    {
  public:
    enum { 
      NOLOG=0,
      VERBOSE,
      DEBUG,
      INFO,
      WARNING,
      ERROR,
      FATAL,
      ALWAYS
    };
    typedef size_t (*dump_function_t)(void*, int severity, const char*, const char*);

  protected:
    /// Object name
    std::string m_name;
    /// Pointer to FSM type structure the state belongs to
    const Type* m_type;

  public:
    /** Class Constructor
     *
     * @arg nam [string, read-only]  State name
     * @arg typ [pointer, read-only] FSM type
     */
    TypedObject(const Type *typ, const std::string& nam);
    
    /// Standatrd destructor  
    virtual ~TypedObject();
    
    /// Accessor: object name
    const std::string& name()   const { return m_name; }

    /// Accessor: object name as c-string (shortcut for printing)
    const char* c_name()   const { return m_name.c_str(); }

    /// Accessor: object type - pointer to FSMtype structure the object belongs
    const Type* type()   const        { return m_type; }

    /// Pointer safe name access - return [UNKNOWN] if pointer is NULL
    static const char* c_name(const TypedObject* object);

    /** Calls the display action
     *  @arg severity   [int,read-only]      Display severity flag
     *  @arg fmt        [string,read-only]   Format string for ellipsis args
     *  @return Status code indicating success or failure
     */
    static Status display(int severity, const char* src, const char* fmt, ...);

    /// Set new print level. Returns the old print level
    static int setPrintLevel(int new_level);
    /// Customize printer function
    static void setPrinter(void* print_arg, dump_function_t fcn);
  };   //  End class TypedObject
}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_STATE_H

