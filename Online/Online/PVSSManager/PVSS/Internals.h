// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/PVSS/Internals.h,v 1.1 2007-03-01 10:40:00 frankb Exp $
//  ====================================================================
//  CfgDetail.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $ID: $
#ifndef PVSSMANAGER_PVSS_INTERNALS_H
#define PVSSMANAGER_PVSS_INTERNALS_H

// PVSS forward declarations
class DpIdentifier;
#include <ctime>

namespace PVSS  {
  class IAPIManager;
  class HotLinkCallback;
  class DeviceManager;
  class DevTypeManager;
  class DevType;
  class CfgManager;
  class CfgType;
  class DevAnswer;
  class Array;
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

  typedef void (*pvss_function_t)(void);

  IAPIManager* pvss_create_manager(const char* dll, const char* fun);

  int systemID(const char* name);
  int defaultSystemID();
  const char* defaultSystemName();
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

  int pvss_load_configurations(CfgManager* mgr, int id,
    void (*addCfg)   (CfgManager* m,int id, const char* n),
    void (*addAttr)  (CfgManager* m,int id, const char* n),
    void (*addDetail)(CfgManager* m,int id, const char* n));

  int pvss_load_device_types(DevTypeManager* mgr, int id,
    DevType* (*addDev) (DevTypeManager* m,int id, const char* n),
    void     (*addElem)(DevType* m,int id, int typ, const char* n));


  void pvss_setup_null_dp(const DpIdentifier* data, size_t len);
  bool pvss_create_device(const char* name, int type, int systemId, DevAnswer* answer);
  bool pvss_delete_device(const DpIdentifier& id, int systemId, DevAnswer* answer);

  bool pvss_lookup_name(const DpIdentifier& id,char*& nam);
  bool pvss_lookup_dpid(const char* nam, DpIdentifier& id);
  bool pvss_lookup_dpidset(const char* wildname,DpIdentifier*& array,long& count,int typeId=0);

  // DpIdentList handling: DpIdentList.cpp
  void pvss_list_create(void*& ctxt);
  bool pvss_list_drop(void*& ctxt);
  bool pvss_list_add(void* ctxt,const DpIdentifier& dpid);
  bool pvss_list_add_wild(void* ctxt,const char* wild_name,DpIdentifier*& array,long& count,int typeId=0);

  // DpIdValueList handling: DpIdValueList.cpp
  void pvss_val_list_create(void*& ctxt);
  void pvss_val_list_drop(void*& ctxt);
  void pvss_val_list_add(void*& ctxt,int data_type,const DpIdentifier& dpid,const Values& value);
  void pvss_val_list_set(void* ctxt,void*& listctxt, int data_type,const DpIdentifier& dpid,const Values& value);

  // dp connect/disconnect handling
  bool pvss_list_connect(void*& ctxt,HotLinkCallback* cb, void*& hotlink);
  bool pvss_list_disconnect(void* ctxt,void* hotlink);

  // dpset transactions: Used by the WriteTransaction interface
  bool pvss_exec_dpset(void*& context,DevAnswer* answer, bool keep_list);

  // dpget transactions: Used by the ReadTransaction interface
  void pvss_start_dpget(void*& context);
  void pvss_drop_dpget(void*& context);
  bool pvss_exec_dpget(void*& context,DevAnswer* answer);
  void pvss_add_dpget(void*& ctxt,int data_type,void (*ldf)(void*,const void*), const DpIdentifier& dpid,void* value);

  // Read transaction handles: used by device reader
  bool pvss_exec_read(void*& context,DevAnswer* answer,HotLinkCallback* cb,bool keep_list);
  void pvss_add_read(void*& ctxt,const DpIdentifier& dpid);
}
#endif // PVSSMANAGER_PVSS_INTERNALS_H
