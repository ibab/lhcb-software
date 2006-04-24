#ifndef _RTL_H
#define _RTL_H

#define byte_offset(a,b,c) {a *ptr=0; c = (int)&(ptr->b);}

template <class T, class Q> static inline T* add_ptr(T* a, Q b)  {
  return (T*)((void*)((char*)(a)+(long)(b)));
}

#ifdef __cplusplus
#include <cstdlib>
#include <cstdarg>
namespace RTL {}
extern "C" {
#else
#include <stdio.h>
#endif
  struct _IOSB {
    unsigned short condition;
    unsigned short count;
    unsigned int   information;
  };
  typedef _IOSB IOSB_t;
#ifdef _WIN32
  typedef __int64       int64_t;
#else
  typedef long long int int64_t;
#endif
  typedef void* lib_rtl_handle_t;
#ifndef _RTL_INTERNAL_H
  typedef void* lib_rtl_thread_t;
  typedef void* lib_rtl_lock_t;
  typedef void* lib_rtl_event_t;
#endif
  struct lib_rtl_gbl  {
    void*             address;
    void*             addaux;
  };
  typedef lib_rtl_gbl* lib_rtl_gbl_t;
  typedef int (*RTL_ast_t)(void*);
  typedef int (*lib_rtl_rundown_handler_t)(void*);
  inline bool lib_rtl_is_success(int status) { return status&1; }

  enum {
    LIB_RTL_DEFAULT = 0,
    LIB_RTL_ERRNO = 1,
    LIB_RTL_OS,
    LIB_RTL_INFINITE = 0
  };

  /// Access to error code
  int lib_rtl_get_error();
  /// Access to error code from socket library
  int lib_rtl_socket_error();
  /// Access to process ID
  int lib_rtl_pid();
  /// Invoke debugger
  int lib_rtl_start_debugger();
  /// Get process name
  int lib_rtl_get_process_name(char* process, size_t len);
  /// Get system node name
  int lib_rtl_get_node_name(char* node, size_t len);

  /// disable intercepts
  int lib_rtl_disable_intercept();
  /// enable intercpets
  int lib_rtl_enable_intercept();

  /// Declare exit handler
  int lib_rtl_declare_exit(int (*hdlr)(void*) ,void* param);
  /// Remove exit handler. Argument must match declaration by pointer !
  int lib_rtl_remove_exit(int (*hdlr)(void*), void* param);
  /// Declare image rundown handler
  int lib_rtl_declare_rundown( int (*hdlr)(void*) ,void* param);
  /// Remove image rundown handler. Argument must match declaration by pointer !
  int lib_rtl_remove_rundown( int (*hdlr)(void*) ,void* param);

  /// Thread execution call signature
  typedef int (*lib_rtl_thread_routine_t)(void*);
  void* lib_rtl_thread_id();
  /// Start new thread.
  int lib_rtl_start_thread(lib_rtl_thread_routine_t exec, void* thread_arg, lib_rtl_thread_t* handle);
  /// Stop and kill executing thread
  int lib_rtl_delete_thread(lib_rtl_thread_t handle);
  /// Joind (and wait for finishing) executing thread
  int lib_rtl_join_thread(lib_rtl_thread_t handle);
  /// Suspend executing thread from execution
  int lib_rtl_suspend_thread(lib_rtl_thread_t handle);

  /// Create named lock. if the lock_name is 0 the lock is priovate to the currect process.
  int lib_rtl_create_lock (const char* lock_name, lib_rtl_lock_t* lock_handle);
  /// Delete lock
  int lib_rtl_delete_lock (lib_rtl_lock_t lock_handle);
  /// Cancle lock
  int lib_rtl_cancel_lock (lib_rtl_lock_t lock_handle);
  /// Aquire lock
  int lib_rtl_lock        (lib_rtl_lock_t lock_handle);
  /// Release lock
  int lib_rtl_unlock      (lib_rtl_lock_t lock_handle);
  /// Access lock count (linux only)
  int lib_rtl_lock_value  (lib_rtl_lock_t handle, int* value);

  /// Create named event for local process (name==0: event is unnamed)
  int lib_rtl_create_event (const char* name, lib_rtl_event_t* event_flag);
  /// Delete event
  int lib_rtl_delete_event(lib_rtl_event_t event_flag);
  /// Set local event flag
  int lib_rtl_set_event(lib_rtl_event_t event_flag);
  /// Set global event flag
  int lib_rtl_set_global_event(const char* name);
  /// Clear event flag
  int lib_rtl_clear_event(lib_rtl_event_t event_flag);
  /// Wait for event flag
  int lib_rtl_wait_for_event(lib_rtl_event_t event_flag);
  /// Wait for event flag with timeout
  int lib_rtl_timedwait_for_event(lib_rtl_event_t event_flag, int milliseconds);
  /// Wait asynchronously for an event flag
  int lib_rtl_wait_for_event_a(lib_rtl_event_t flag, lib_rtl_thread_routine_t action, void* param);

  /// Create named global section
  int lib_rtl_create_section(const char* sec_name, size_t size, lib_rtl_gbl_t* handle);
  /// Map global section a a specific address
  int lib_rtl_map_section(const char* sec_name, size_t size, lib_rtl_gbl_t* address);
  /// Delete named global section
  int lib_rtl_delete_section(lib_rtl_gbl_t handle);
  /// Unmap global section: address is quadword: void*[2]
  int lib_rtl_unmap_section(lib_rtl_gbl_t handle);
  /// Flush global section to disk file
  int lib_rtl_flush_section(lib_rtl_gbl_t handle);

  /// Time callback signature
  typedef int (*lib_rtl_timer_routine_t)(void*);
  /// Set timer with asynchronous callback. Process must be in alertable state
  int lib_rtl_set_timer(int milli_seconds, lib_rtl_timer_routine_t ast, void* ast_param, unsigned int* timer_id);
  /// Stop timer. Asynchronous callback will not be called
  int lib_rtl_kill_timer(int timer_id);

  /// Get tick count since image startup
  unsigned int lib_rtl_get_ticks();

  int lib_rtl_ffc (int* start, int* len, const void* base, int* position);
  int lib_rtl_ffs (int* start, int* len, const void* base, int* position);
  int lib_rtl_run_ast (RTL_ast_t astadd, void* bm, int);

  /// Issue system error message
  int lib_rtl_signal_message(int, const char* fmt, ...);
  /// Sleep specified time
  int lib_rtl_sleep(int millisecs);
  /// Sleep specified time (sub-millisecond range)
  int lib_rtl_usleep(int microsecs);

  /// System error message (from status)
  const char* lib_rtl_error_message(int status);
  /// Printout redirection
  size_t lib_rtl_printf(const char* fmt, ...);
  /// Install RTL printer 
  void lib_rtl_install_printer(size_t (*func)(void*, const char*, va_list args), void* param);

  /// Small helper function to properly retrun from main program.
  int lib_rtl_default_return();

  /// Cancel I/O request
  int lib_rtl_cancel_io(int channel);


  /** Utility routines  */
  /// Allocate std::map<int, void*> for usage in static 
  /// storage with properly sequenced exit handler
  void* lib_rtl_alloc_int_pointer_map();


#ifdef __cplusplus
}

#include <string>
/*
 *
 */
namespace RTL  {

  /// Process name (UTGID)
  const std::string& processName();
  /// Fully qualified node name
  const std::string& nodeName();
  /// Short node name (=computer name without domain)
  const std::string& nodeNameShort();

  /**@class CLU rtl.h RTL/rtl.h
    * 
    * Small helper class to interprete command line options
    *
    * @author  M.Frank
    * @version 1.0
    */
  class CLI  {
  private:
    int    m_argc;
    char** m_argv;
    void (*m_help)();
    int getopt(const char* name, int nchars, void* value, const char* fmt, int (*deformat)(const char*, const char*,...));

  public:
    CLI(int argc, char** argv, void (*help)()) : m_argc(argc), m_argv(argv), m_help(help) {}
    const char* getopt(const char* name, int nchars);
    int getopt(const char* name, int nchars, char& value);
    int getopt(const char* name, int nchars, unsigned char& value);
    int getopt(const char* name, int nchars, short& value);
    int getopt(const char* name, int nchars, unsigned short& value);
    int getopt(const char* name, int nchars, int& value);
    int getopt(const char* name, int nchars, unsigned int& value);
    int getopt(const char* name, int nchars, long& value);
    int getopt(const char* name, int nchars, unsigned long& value);
    int getopt(const char* name, int nchars, float& value);
    int getopt(const char* name, int nchars, double& value);
    int getopt(const char* name, int nchars, std::string& value);
  };
}
#endif
#endif // _RTL_H
