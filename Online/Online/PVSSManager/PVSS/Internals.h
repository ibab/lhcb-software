// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/PVSS/Internals.h,v 1.4 2007-03-02 19:53:46 frankb Exp $
//  ====================================================================
//  Internals.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: Internals.h,v 1.4 2007-03-02 19:53:46 frankb Exp $
#ifndef PVSSMANAGER_PVSS_INTERNALS_H
#define PVSSMANAGER_PVSS_INTERNALS_H

// PVSS forward declarations
class DpIdentifier;
#include <ctime>

/*
 * PVSS namespace 
 */
namespace PVSS  {

  // Forward declarations
  class IAPIManager;
  class HotLinkCallback;
  class DeviceManager;
  class DevTypeManager;
  class DevType;
  class CfgManager;
  class CfgType;
  class DevAnswer;
  class Array;

  /**@union Values Internals.h PVSS/Internals.h
    *
    *  Helper structure to transfer data items
    *
    *  @author  M.Frank
    *  @version 1.0
    */
  union Values {
    DpIdentifier*   dpid;
    unsigned char*  uchar;
    char*           schar;
    unsigned int*   uint;
    int*            sint;
    unsigned short* ushort;
    short*          sshort;
    float*          real;
    char*           text;
    time_t*         time;
    bool*           boolean;
    Array*          array;
    const void*     pvoid;
    Values(const void* p) : pvoid(p) {}
  };

  typedef void (*pvss_function_t)(void*);

  /// Create API manager
  IAPIManager* pvss_create_manager(const char* dll, const char* fun);

  /** Access to general information about the PVSS system: System.cpp      */
  
  /// Access system id by name
  int systemID(const char* name);
  /// Access default system identifier
  int defaultSystemID();
  /// Access default system name
  const char* defaultSystemName();
  
  /** OS dependent wrappers: Platform.cpp                                  */

  /// Sleep a specified amount of time
  int  pvss_sleep(int millisecs);
  /// Access DLL load error
  const char* pvss_load_error();
  /// Load function from DLL
  pvss_function_t pvss_load_function(const char* lib, const char* fun);
  /// Start native thread
  int pvss_start_thread(void (*fun)(void*), void* arg);
  /// Terminate native thread
  int pvss_end_thread(int exit_code);
  /// Delete PVSS lock object
  int pvss_delete_lock(void* handle);
  /// Create lock to protect PVSS API manager from multithreaded trouble
  int pvss_create_lock(void** handle);
  /// Release lock object
  int pvss_release_lock(void* handle);
  /// Take lock object
  int pvss_take_lock(void* handle);
  /// Access to global lock object
  void* pvss_global_lock();
 
  /** PVSS configuration access: DpHelpers.cpp                             */

  /// load PVSS configurations with callbacks
  int pvss_load_configurations(CfgManager* mgr, int id,
    void (*addCfg)   (CfgManager* m,int id, const char* n),
    void (*addAttr)  (CfgManager* m,int id, const char* n),
    void (*addDetail)(CfgManager* m,int id, const char* n));

  /// Load device types 
  int pvss_load_device_types(DevTypeManager* mgr, int id,
    DevType* (*addDev) (DevTypeManager* m,int id, const char* n),
    void     (*addElem)(DevType* m,int id, int typ, const char* n));

  /** PVSS data point utilities: DpHelpers.cpp                             */

  /// Check wrapper DpIdentifier
  void pvss_setup_null_dp(const DpIdentifier* data, size_t len);
  /// Lookup datapoint name by datatpoint identifier
  bool pvss_lookup_name(const DpIdentifier& id,char*& nam);
  /// Lookup datapoint identifier by it's name
  bool pvss_lookup_dpid(const char* nam, DpIdentifier& id);
  /// Lookup datapoint set by wildcard name
  bool pvss_lookup_dpidset(const char* wildname,DpIdentifier*& array,long& count,int typeId=0);
  /// Create PVSS data point
  bool pvss_create_device(const char* name, int type, int systemId, DevAnswer* answer);
  /// Delete PVSS data point
  bool pvss_delete_device(const DpIdentifier& id, int systemId, DevAnswer* answer);


  /** DpIdentList handling: DpIdentList.cpp                                */

  /// Create new datapoint identifier list. Must be dropped by the user
  void pvss_list_create(void*& ctxt);
  /// Drop datapoint identifier list
  bool pvss_list_drop(void*& ctxt);
  /// Add identifier to list
  bool pvss_list_add(void* ctxt,const DpIdentifier& dpid);
  /// Add result of wildcard search to the list
  bool pvss_list_add_wild(void* ctxt,const char* wild_name,DpIdentifier*& array,long& count,int typeId=0);

  /** DpIdValueList handling: DpIdValueList.cpp                            */

  /// Create new datapoint ID - Value list. Must be dropped by the user
  void pvss_val_list_create(void*& ctxt);
  /// Drop exisiting datapoint ID - Value list
  void pvss_val_list_drop(void*& ctxt);
  /// Add/Append identifier-value pair to list
  void pvss_val_list_add(void*& ctxt,int data_type,const DpIdentifier& dpid,const Values& value);
  /// Set item in an existing identifier-value list. The loop is initiated with listctxt=0
  void pvss_val_list_set(void* ctxt,void*& listctxt, int data_type,const DpIdentifier& dpid,const Values& value);

  /** Data point connect/disconnect handling: DpConnect.cpp                 */

  /// Connect and listen to changes of an existing datapoint
  bool pvss_list_connect(void*& ctxt,HotLinkCallback* cb, void*& hotlink);
  /// Disconnect from a connection point
  bool pvss_list_disconnect(void* ctxt,void* hotlink);

  // dpset transactions: Used by the WriteTransaction interface
  bool pvss_exec_dpset(void*& context,DevAnswer* answer, bool keep_list);

  /** Read transaction handles: used by device reader. DpRead.cpp           */  
  /// Execute read transaction on DpIdentlist passed by context. Caller(=cb) gets notified for changes
  bool pvss_exec_read(void*& context,DevAnswer* answer,HotLinkCallback* cb,bool keep_list);

}

#endif // PVSSMANAGER_PVSS_INTERNALS_H
