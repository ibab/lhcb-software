#ifndef _RTL_H
#define _RTL_H

#define byte_offset(a,b,c) {a *ptr=0; c = (long)&(ptr->b);}

template <class T, class Q> static inline T* add_ptr(T* a, Q b)  {
  return (T*)((void*)((char*)(a)+(long)(b)));
}

#ifdef __cplusplus
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#ifndef _WIN32
#include <semaphore.h>
#include <unistd.h>
#endif
#ifndef __CXX_CONST
#define __CXX_CONST const
#endif
namespace RTL {}
extern "C" {
#else
#include <stdio.h>
#include <time.h>
#ifndef __CXX_CONST
#define __CXX_CONST
#endif
#endif
  struct _IOSB {
    unsigned short condition;
    unsigned short count;
    unsigned int   information;
  };
  typedef _IOSB IOSB_t;
#if defined(_WIN32)
  typedef __int64       int64_t;
#ifdef __cplusplus
  namespace { 
    inline FILE* popen(__CXX_CONST char* cmd, __CXX_CONST char* typ)
    { return ::_popen(cmd,typ); }
  }
#else
#define popen _popen
#endif
#elif !defined(__int8_t_defined)
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
    long              size;
  };
  typedef lib_rtl_gbl* lib_rtl_gbl_t;
  typedef int (*RTL_ast_t)(void*);
  typedef int (*lib_rtl_rundown_handler_t)(void*);
  inline bool lib_rtl_is_success(int status) { return status&1; }

  /// Enumeration of printout levels
  enum {
    LIB_RTL_VERBOSE = 1,
    LIB_RTL_DEBUG,
    LIB_RTL_INFO,
    LIB_RTL_WARNING,
    LIB_RTL_ERROR,
    LIB_RTL_FATAL,
    LIB_RTL_ALWAYS,
    LIB_RTL_ERRNO = 100,
    LIB_RTL_OS = 200,
    LIB_RTL_DEFAULT = 0,
    LIB_RTL_INFINITE = 0
  };

  /**RTL: Online runtime library wrapper for OS specific functions.
   *
   *   All RTL routines return 1 if successful and
   *   0 if an OS specific error occurred.
   *   The error code can be examined using the fucntion:
   *   lib_rtl_get_error() or lib_rtl_socket_error() in the
   *   event the error occurred on calling a function
   *   from the socket library.
   *
   *    @author M.Frank
   *
   */

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
  /// Access data interface name
  int lib_rtl_get_datainterface_name(char* node, size_t len);
  /// Safe wrapper around getenv
  __CXX_CONST char* lib_rtl_getenv(__CXX_CONST char* value);
  /// disable intercepts
  int lib_rtl_disable_intercept();
  /// enable intercpets
  int lib_rtl_enable_intercept();

  /// Set signal handling output level
  void lib_rtl_signal_log(int value);
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
  /// Access the thread identifier of the current thread
  void* lib_rtl_thread_id();
  /// Check if the thread identified by the handle is the current thread
  int lib_rtl_is_current_thread(lib_rtl_thread_t handle);

  /// Start new thread.
  int lib_rtl_start_thread(lib_rtl_thread_routine_t exec, void* thread_arg, lib_rtl_thread_t* handle);
  /// Stop and kill executing thread
  int lib_rtl_delete_thread(lib_rtl_thread_t handle);
  /// Joind (and wait for finishing) executing thread
  int lib_rtl_join_thread(lib_rtl_thread_t handle);
  /// Suspend executing thread from execution
  int lib_rtl_suspend_thread(lib_rtl_thread_t handle);
  /// Cancel thread execution
  int lib_rtl_cancel_thread(lib_rtl_thread_t handle);
  /// Send signal to thread
  int lib_rtl_kill_thread(lib_rtl_thread_t handle, int sig);

  /// Create named lock. if the lock_name is 0 the lock is priovate to the currect process.
  int lib_rtl_create_lock (__CXX_CONST char* lock_name, lib_rtl_lock_t* lock_handle);
#ifndef _WIN32
  /// Create lock for local process (if handle is in shared memory lock can be shared)
  int lib_rtl_create_lock2 (sem_t* handle, lib_rtl_lock_t* lock_handle, bool initialize=false);
#endif
  /// Delete lock
  int lib_rtl_delete_lock (lib_rtl_lock_t lock_handle);
  /// Cancel lock
  int lib_rtl_cancel_lock (lib_rtl_lock_t lock_handle);
  /// Aquire lock
  int lib_rtl_lock        (lib_rtl_lock_t lock_handle);
  /// Try to aquire lock; returns immediately with return code 2 if unsuccessful.
  int lib_rtl_trylock     (lib_rtl_lock_t lock_handle);
  /// Wait for lock flag with timeout given in milliseconds
  int lib_rtl_lock_timedwait(lib_rtl_lock_t lock_handle, int milliseconds);
  /// Release lock
  int lib_rtl_unlock      (lib_rtl_lock_t lock_handle);
  /// Access lock count (linux only)
  int lib_rtl_lock_value  (lib_rtl_lock_t handle, int* value);

  /// Create named event for local process (name==0: event is unnamed)
  int lib_rtl_create_event (__CXX_CONST char* name, lib_rtl_event_t* event_flag);
#ifndef _WIN32
  /// Create named event for local process (name==0: event is unnamed)
  int lib_rtl_create_event2 (sem_t* handle, lib_rtl_event_t* event_flag);
#endif
  /// Delete event
  int lib_rtl_delete_event(lib_rtl_event_t event_flag);
  /// Set local event flag
  int lib_rtl_set_event(lib_rtl_event_t event_flag);
  /// Clear event flag
  int lib_rtl_clear_event(lib_rtl_event_t event_flag);
  /// Wait for event flag
  int lib_rtl_wait_for_event(lib_rtl_event_t event_flag);
  /// Wait for event flag with timeout
  int lib_rtl_timedwait_for_event(lib_rtl_event_t event_flag, int milliseconds);
  /// Wait asynchronously for an event flag
  int lib_rtl_wait_for_event_a(lib_rtl_event_t flag, lib_rtl_thread_routine_t action, void* param);
  /// Try to get event flag if set; returns immediately with return code 2 if event is not set.
  int lib_rtl_try_event(lib_rtl_event_t event_handle);

  /// Create named global section
  int lib_rtl_create_section(__CXX_CONST char* sec_name, size_t size, lib_rtl_gbl_t* handle, bool keep=false);
  /// Map global section a a specific address. If the size is NULL, map the entire section
  int lib_rtl_map_section(__CXX_CONST char* sec_name, size_t size, lib_rtl_gbl_t* address);
  /// Delete named global section
  int lib_rtl_delete_section(lib_rtl_gbl_t handle);
  /// Unmap global section: address is quadword: void*[2]
  int lib_rtl_unmap_section(lib_rtl_gbl_t handle);
  /// Flush global section to disk file
  int lib_rtl_flush_section(lib_rtl_gbl_t handle);


  /// Creates a pipe and executes a command.
  FILE* lib_rtl_pipe_open(__CXX_CONST char* command, __CXX_CONST char* mode);
  /// Waits for new command processor and closes stream on associated pipe.
  int lib_rtl_pipe_close(FILE* stream);

  /// Time callback signature
  typedef int (*lib_rtl_timer_routine_t)(void*);
  /// Set timer with asynchronous callback. Process must be in alertable state
  int lib_rtl_set_timer(int milli_seconds, lib_rtl_timer_routine_t ast, void* ast_param, unsigned long* timer_id);
  /// Stop timer. Asynchronous callback will not be called
  int lib_rtl_kill_timer(long timer_id);

  /// Get tick count since image startup
  unsigned int lib_rtl_get_ticks();

  int lib_rtl_ffc (int* start, int* len, __CXX_CONST void* base, int* position);
  int lib_rtl_ffs (int* start, int* len, __CXX_CONST void* base, int* position);
  int lib_rtl_run_ast (RTL_ast_t astadd, void* bm, int);

  /// Issue system error message
  int lib_rtl_signal_message(int, __CXX_CONST char* fmt, ...);
  /// Sleep specified time
  int lib_rtl_sleep(int millisecs);
  /// Sleep specified time (sub-millisecond range)
  int lib_rtl_usleep(int microsecs);

  /// System error message (from status)
  __CXX_CONST char* lib_rtl_error_message(int status);
  /// Printout redirection
  size_t lib_rtl_printf(__CXX_CONST char* fmt, ...);
  /// Printout redirection
  size_t lib_rtl_output(int level, __CXX_CONST char* fmt, ...);
  /// Install RTL printer 
#if !defined(__GCCXML) 
  void lib_rtl_install_printer(size_t (*func)(void*, int, __CXX_CONST char*, va_list args), void* param);
#endif
  /// Formatted time string
#ifdef __cplusplus
  __CXX_CONST char* lib_rtl_timestr(__CXX_CONST char* fmt = "%b %d %H:%M:%S ", const time_t* tp=0);
#else
  __CXX_CONST char* lib_rtl_timestr(__CXX_CONST char* fmt, const time_t* tp);
#endif
#ifdef __cplusplus
  __CXX_CONST char* lib_rtl_gmtimestr(__CXX_CONST char* fmt = "%b %d %H:%M:%S ", const time_t* tp=0);
#else
  __CXX_CONST char* lib_rtl_gmtimestr(__CXX_CONST char* fmt, time_t* tp);
#endif

  /// Small helper function to properly retrun from main program.
  int lib_rtl_default_return();

  /// Cancel I/O request
  int lib_rtl_cancel_io(int channel);


  /** Utility routines  */
  /// Allocate std::map<int, void*> for usage in static 
  /// storage with properly sequenced exit handler
  void* lib_rtl_alloc_int_pointer_map();

  int lib_rtl_event_exithandler();
  int lib_rtl_gbl_exithandler();
  int lib_rtl_lock_exithandler();

  /// POSIX/ISO compiant wrapper around unlink
  int lib_rtl_unlink(__CXX_CONST char* fname);

  /// Read path associated to file name
  int lib_rtl_readlink(__CXX_CONST char* fname, char* path, size_t path_len);

  /// Read path according from file descriptor
  int lib_rtl_file_name(int fd, char* path, size_t path_len);

  /// Access total/free disk space on file system (linux:statvfs call)
  int lib_rtl_diskspace(__CXX_CONST char* name, unsigned long long int* blk_size,
                        unsigned long long int* total_blk,
                        unsigned long long int* availible_blk);
  /// Check for read/write/exectute permissions or existence using bit mask mode (1, 2, 4, 8)
  int lib_rtl_access(__CXX_CONST char *name, int mode);			  

#ifdef __cplusplus
}

#include <string>
#include <map>
/*
 *
 */
namespace RTL  {
#if !defined(__GCCXML) 
  typedef std::map<lib_rtl_event_t, lib_rtl_thread_t> lib_rtl_thread_map_t;
  typedef std::map<std::string, lib_rtl_event_t>      lib_rtl_event_map_t;
  typedef std::map<std::string, lib_rtl_gbl_t>        lib_rtl_gbl_map_t;
  typedef std::map<std::string, lib_rtl_lock_t>       lib_rtl_lock_map_t;
  lib_rtl_event_map_t&  allEventFlags();
  lib_rtl_thread_map_t& waitEventThreads();
  lib_rtl_gbl_map_t&    allSections();
  lib_rtl_lock_map_t&   allLocks();
  /// Read POSIX link
  std::string fileFromLink(const std::string& link);
  /// Access file name from file descriptor
  std::string fileFromDescriptor(int fd);
#endif

  template<class T> union func_cast   {
    void* ptr;
    T     fun;
    explicit func_cast(T t) { fun = t; }
    explicit func_cast(void* t) { ptr = t; }
  };

  /// Reset static storage
  void RTL_reset();
  /// Process name (UTGID)
  const std::string& processName();
  /// Fully qualified node name
  const std::string& nodeName();
  /// Short node name (=computer name without domain)
  const std::string& nodeNameShort();
  /// Access data interface name
  const std::string& dataInterfaceName();
  /// Independent version of strerror
  const char* errorString();
  /// Access error string from integer error code.
  const char* errorString(int status);

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
#undef __CXX_CONST
#endif // _RTL_H
