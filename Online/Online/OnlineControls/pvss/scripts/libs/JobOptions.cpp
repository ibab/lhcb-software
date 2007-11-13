
// Names to datapoints
string jo_dpPartitionName;
string JobOptionsVersion     = "1.0";
string JobOptionsPartition_t = "JobOptionsPartition";
string JobOptionsActivity_t  = "JobOptionsActivity";
string JobOptionsTaskType_t  = "JobOptionsTaskType";

int    jo_debug = 1;
int    jo_doExit = 0;
int    jo_NO_EDIT = 0;
int    jo_PARTITION_EDIT = 1;
int    jo_NODE_EDIT = 2;
int    jo_TASK_EDIT = 3;

// Values
string jo_partitionName;
string jo_partitionID;
string jo_activityName;
string jo_tell1Boards;



string JobOptions_sysName()  {
  return JobOptionsSystem+":";
  //return "BBB"+JobOptionsSystem+":";
}
/// Access system ID of the job options system
int JobOptions_sysID()  {
  return getSystemId(JobOptions_sysName());
}

// Print setup parameters with prefix tag
void JobOptions_printSetup(string tag)  {
  string text;
  sprintf(text,"%s> %-32s = %s",tag,"Partition ID",jo_partitionID);
  DebugN(text);
  sprintf(text,"%s> %-32s = %s",tag,"Activity Name",jo_activityName);
  DebugN(text);
  sprintf(text,"%s> %-32s = %s",tag,"Tell1 Boards",jo_tell1Boards);
  DebugN(text);
}

/** Issue error message
  * @param rc        [IN]    Error code (=return value)
  * @param msg       [IN]    Error message
  *
  * @author M.Frank
  */

int JobOptions_error(int rc, string msg)  {
  if ( strlen(jo_dpPartitionName)>0 )  {
    if ( dynlen(getLastError()) > 0 )
      DebugN("Partition:"+jo_dpPartitionName+">"+msg+" Err="+rc+","+getLastError());
    else
      DebugN("Partition:"+jo_dpPartitionName+">"+msg+" Err="+rc);
  }
  else  {
    DebugN(msg+" Err="+rc+","+getLastError());
    string err = msg;
    if ( dynlen(getLastError()) > 0 )
      msg = msg+" Err="+rc+","+getLastError();
    ChildPanelOn("vision/MessageInfo1","Error occurred",makeDynString(err),50,50);
  }
  return rc;
}

/** Write job options to file
  * @param fname     [IN]    Output file name
  * @param opts      [IN]    Option content
  *
  * @author M.Frank
  */
int JobOptions_write(string fname, string defs, string tell1, string opts)  {
  string fn = "C:/Users/frankm/options/"+fname;
  DebugN("  ---->Writing options file:"+fname);
  file f = fopen(fn,"w");
  if ( 0 != f ) {
    fprintf(f,defs+tell1+opts);
    fclose(f);
    return 0;
  } 
  return JobOptions_error(ferror(f),"Cannot open options file:"+fn);
}

/** Configure setup (dereference refs to partition dependent info)
  * Retrieve basic datapoint values, which do not change for
  * a given active partition.
  *
  * @param partitionName     [IN]    Partition name
  *
  * @author M.Frank
  */
int JobOptions_config(string partitionName)  {
  string base = JobOptions_sysName()+JobOptionsPartition_t + "_" + partitionName;
  jo_dpPartitionName = base;
  int rc = dpGet(base + ".PartitionID", jo_partitionID,
    base + ".Activity",    jo_activityName,
    base + ".Tell1Boards", jo_tell1Boards);
  if ( 0 == rc )  {
    jo_partitionName = partitionName;
    jo_partitionID   = dpSubStr(jo_partitionID, DPSUB_SYS_DP_EL);
    jo_activityName  = dpSubStr(jo_activityName,DPSUB_SYS_DP_EL);
    jo_tell1Boards   = dpSubStr(jo_tell1Boards, DPSUB_SYS_DP_EL);
    JobOptions_printSetup("JobOptions_config");
    return rc;
  }
  return JobOptions_error(rc,"Error in retrieving JobOptions setup for Partition:"+partitionName);
}

/** Configure setup (dereference refs to partition dependent info)
  * Retrieve basic datapoint values, which do not change for
  * a given active partition.
  *
  * @param partitionName     [IN]    Partition name
  *
  * @author M.Frank
  */
int JobOptions_execute()  {
  int pid;
  string activity, tsk;
  dyn_dyn_string   tasks;
  dyn_bool         reqTell1, reqDefs;
  dyn_string       boards, addrs, task_set, tell1_set, def_set, options, activities;
  int rc = dpGet(jo_partitionID, pid, 
    jo_activityName, activity,
    jo_tell1Boards, boards, 
    jo_tell1Boards, addrs);
//    jo_tell1Boards+".Name", boards, 
//    jo_tell1Boards+".IP", addrs);
  if ( 0 == rc )  {
    string dps = JobOptions_sysName()+JobOptionsActivity_t+"_"+activity+"_*";
    activities = dpNames(dps);
    DebugN("Search for:"+dps);
    //DebugN(rc+" PartitionID:"+pid);
    DebugN(rc+" Activity   :"+activity);
    //DebugN(rc+" Boards     :"+boards);
    //DebugN(rc+" Addresses  :"+addrs);
    for(int i=1, n=dynlen(activities)+1; i<n; ++i)
      activities[i] = activities[i]+".";
    DebugN("activities:",activities,dynlen(activities));
    if ( dynlen(activities) > 0 )  {
      rc = dpGet(activities,tasks);
      DebugN("Tasks:",tasks,dynlen(tasks));
      if ( 0 == rc )  {
        for(int i=1, n=dynlen(tasks)+1; i<n; ++i)
          dynAppend(task_set,tasks[i]);
        dynUnique(task_set);
        if ( dynlen(task_set) > 0 )  {
          for(int i=1, n=dynlen(task_set)+1; i<n; ++i)  {
            dynAppend(def_set,task_set[i]+".NeedDefaults");
            dynAppend(tell1_set,task_set[i]+".NeedTell1s");
            task_set[i] = task_set[i] + ".Options";
          }
          DebugN("Task_Set: "+task_set);
          DebugN("Tell1_Set:"+tell1_set);
          DebugN("Def_Set:  "+def_set);
          rc = dpGet(task_set,options);
          rc = dpGet(tell1_set, reqTell1);
          rc = dpGet(def_set, reqDefs);
          if ( 0 == rc )  {
            string env = 
              "// ---------------- General partition information:  \n"+
              "OnlineEnv.PartitionID   = "+pid+";\n"+
              "OnlineEnv.PartitionName = \""+jo_partitionName+"\";\n"+
              "OnlineEnv.Activity      = \""+activity+"\";\n\n";
            string tell1s = 
              "// ---------------- Tell1 board information:  \n"+
              "OnlineEnv.Tell1Boards    = {";
            for(int i=1, n=dynlen(boards); i<=n; ++i)  	{
              tell1s = tell1s + "\n  \""+boards[i]+"\", \""+addrs[i]+"\", \"\"";
              if ( i < n ) tell1s = tell1s + ",";
            }
            tell1s = tell1s + "\n};\n";
            for(int i=1, n=dynlen(options)+1; i<n; ++i)  	{
              tsk = substr(dpSubStr(task_set[i],DPSUB_DP),strlen(JobOptionsTaskType_t)+1);
              JobOptions_write(tsk+".opts",
                tell1_set[i] ? tell1s : "",
                def_set[i] ? env : "",
                "// ---------------- Task options:\n"+options[i]+"\n");
            }
            return rc;
          }
          return JobOptions_error(rc,"Failed to load options for tasks");
        }
        return JobOptions_error(rc,"No tasks were mathcing the selection!");
      }
    }
    return JobOptions_error(rc,"Failed to retrieve activities for tasks");
  }
  return JobOptions_error(rc,"Error initializing JobOptions");
}

/// Set new state
void JobOptions_setState(string new_state)  {
  dpSet(jo_dpPartitionName + ".State",new_state);
}

/** Callback when command changed.
  * Execute all required actions, then update the state of the object accordingly
  *
  * @param dpe       [IN]    Name of the datapoint element, which changed
  * @param cmd       [IN]    Value of the datapoint element
  *
  * @author M.Frank
  */
void JobOptions_callback(string dpe, string cmd)  {
  DebugN("DP:"+dpe+" value:"+cmd);
  if ( cmd == "configure" )  {
    int rc = JobOptions_execute();
    if ( 0 != rc )  {
      JobOptions_error(rc,"Failed to generate job options.");
      JobOptions_setState("ERROR");
      return;
    }
    JobOptions_setState("READY");
  }
  else if ( cmd == "unload" )  {
    // Disconnect datapoint ... stop liostening to commands
    dpDisconnect("JobOptions_callback",dpe);
    JobOptions_setState("OFFLINE");
    jo_doExit = 1;
  }
  else if ( cmd == "reset" )  {
    JobOptions_setState("NOT_READY");
  }
  else if ( cmd == "start" )  {
    JobOptions_setState("RUNNING");
  }
  else if ( cmd == "stop" )  {
    JobOptions_setState("READY");
  }
}
/** Listen to command from the FSM
  *
  * @author M.Frank
  */
int JobOptions_listen()  {
  string cmd = jo_dpPartitionName + ".Command";
  int rc = dpConnect("JobOptions_callback",cmd);	
  if ( 0 == rc )  {
    DebugN("connected to command:"+cmd);
    return rc;
  }
  return JobOptions_error(rc,"Failed to connect to command:"+cmd);
}
/// Collect datapoint items ready for display
dyn_string JobOptions_items(string match)  {
  string typ = match + "_*";
  int    typ_len = strlen(typ)-1;
  dyn_string items = dpNames(JobOptions_sysName()+typ);
  for(int i=1, n=dynlen(items); i<=n; ++i)
    items[i] = substr(dpSubStr(items[i],DPSUB_DP),typ_len);
  return items;
}
/// Return all known partition names as a dyn_string
dyn_string JobOptions_partitions()  {
  return JobOptions_items(JobOptionsPartition_t);
}
/// Return all node types as a dyn_string
dyn_string JobOptions_nodeTypes(string match="")  {
  return JobOptions_items(JobOptionsActivity_t+match);
}
/// Return all task types as a dyn_string
dyn_string JobOptions_taskTypes()  {
  return JobOptions_items(JobOptionsTaskType_t);
}
void JobOptions_typeCreate(dyn_dyn_string& names, dyn_dyn_int& types)  {
  string typ = names[1][1];
  if ( dynlen(dpTypes(typ)) == 0 )  {
    int rc = dpTypeCreate(names,types);
    if ( rc != 0 )
      JobOptions_error(rc,"Failed to create datapoint type:"+typ);
    else
      DebugN (typ+" datapoint type created, result: "+rc);
    dynClear(names);
    dynClear(types);
  }
  else {
    DebugN (typ+" datapoint type already exists ... Nothing to do.");
  }
}
void JobOptions_typeDelete(string typ)  {
  if ( dynlen(dpTypes(typ)) == 1 )  {
    dyn_string objs = dpNames("*",typ);
    for(int i=1;i<=dynlen(objs);++i)dpDelete(objs[i]);
    int rc = dpTypeDelete(typ);
    if ( rc != 0 )
      JobOptions_error(rc,"Failed to delete datapoint type:"+typ);
    else
      DebugN (typ+" datapoint type deleted, result: "+rc);
  }
  else {
    DebugN (typ+" datapoint type does not exists ... Nothing to do.");
  }
}
/// Install manager
int JobOptions_installManager(int num, string name, string script)  {
  string path = PROJ_PATH+"/bin/"+name;
  file f = fopen(path,"w");
  fprintf(f,"#!/bin/bash\n. `dirname $0`/PVSS00api.sh -NAME "+name+" -DLL PVSSInterface -FUN pvss_pymain $*\n\n");
  fclose(f);  
  system("/bin/chmod +x "+path);
  fwInstallation_addManager(name,"always", 30, 3, 3, "-num "+num+" -SCRIPT "+script);
}

/// Install types
void JobOptions_install() {
  JobOptions_installOptions();
  JobOptions_installControl();
}
void JobOptions_installControl() {
  dyn_dyn_string names;
  dyn_dyn_int types;
  // Create Partition type
  names[1] = makeDynString ("JobOptionsControl","","","");
  names[2] = makeDynString ("","State","","");
  names[3] = makeDynString ("","Command","","");

  types[1] = makeDynInt (DPEL_STRUCT);
  types[2] = makeDynInt (0,DPEL_STRING);
  types[3] = makeDynInt (0,DPEL_STRING);
  JobOptions_typeCreate(names,types);
}
void JobOptions_installOptions() {
  dyn_dyn_string names;
  dyn_dyn_int types;
  // Create Partition type
  names[1] = makeDynString (JobOptionsPartition_t,"","","");
  names[2] = makeDynString ("","State","","");
  names[3] = makeDynString ("","Command","","");
  names[4] = makeDynString ("","PartitionID","","");
  names[5] = makeDynString ("","Activity","","");
  names[6] = makeDynString ("","Tell1Boards","","");
  names[7] = makeDynString ("","Vars","","");

  types[1] = makeDynInt (DPEL_STRUCT);
  types[2] = makeDynInt (0,DPEL_STRING);
  types[3] = makeDynInt (0,DPEL_STRING);
  types[4] = makeDynInt (0,DPEL_DPID);
  types[5] = makeDynInt (0,DPEL_DPID);
  types[6] = makeDynInt (0,DPEL_DPID);
  types[7] = makeDynInt (0,DPEL_STRUCT);
  JobOptions_typeCreate(names,types);
  // Create Activity type
  names[1] = makeDynString (JobOptionsActivity_t,"","","");	
  types[1] = makeDynInt (DPEL_DYN_DPID);
  JobOptions_typeCreate(names,types);
  // Create Task Type type
  names[1] = makeDynString (JobOptionsTaskType_t,"","","");	
  names[2] = makeDynString ("","Options","","");	
  names[3] = makeDynString ("","NeedTell1s","","");	
  names[4] = makeDynString ("","NeedDefaults","","");	
  types[1] = makeDynInt (DPEL_STRUCT);
  types[2] = makeDynInt (0,DPEL_STRING);
  types[3] = makeDynInt (0,DPEL_BOOL);
  types[4] = makeDynInt (0,DPEL_BOOL);
  JobOptions_typeCreate(names,types);
}
/// Uninstall types
void JobOptions_uninstallOptions() {
  JobOptions_typeDelete(JobOptionsPartition_t);
  JobOptions_typeDelete(JobOptionsActivity_t);
  JobOptions_typeDelete(JobOptionsTaskType_t);
}
void JobOptions_uninstallControl() {
  JobOptions_typeDelete("JobOptionsControl");
}
/// Uninstall types for options editor
void JobOptions_uninstall() {
  JobOptions_uninstallControl();
  JobOptions_uninstallOptions();
}
int JobOptionsEditor_isInstalled()  {
  if ( dynlen(dpTypes(JobOptionsTaskType_t,JobOptions_sysID())) == 0 )  {
    return 0;
  }
  return 1;
}
/// Flip between systems
int JobOptionsEditor_setSystem(string name)  {
  JobOptionsSystem = name;
  JobOptionsEditor_init();
}

/// Editor: Initialize widget
int JobOptionsEditor_init()  {
  bool isChild = isDollarDefined("$1") || isDollarDefined("$2");
  //DebugN("--------------------------------------------------------------------------------"+isChild);
  //JobOptions_uninstall();
  //JobOptions_install();
  setValue("m_systemName","text","System:"+JobOptionsSystem);
  setValue("m_systemName","visible",1);
  if ( !JobOptionsEditor_isInstalled() )  {
    DebugN("Not connected to job options system.");
    LayerOff(2);
    LayerOff(3);
    LayerOff(4);
    LayerOff(5);
    LayerOn(6);
    return 1;
  }  
  LayerOn(2);
  LayerOn(3);
  LayerOn(4);
  LayerOff(5);
  LayerOff(6);
  setValue("m_textEditor","visible",true);
  setValue("m_needDefaults","visible",true);
  setValue("m_needTell1Setup","visible",true);
  setValue("m_OK","visible",1);//isChild);
  JobOptionsEditor_setToolTips();
  JobOptionsEditor_closeEditors();
  JobOptionsEditor_showCreator(0);
  return JobOptionsEditor_showPartitions();
}
/// Set tootips to all present graphics components
void JobOptionsEditor_setToolTips()  {
  shape tedit = getShape("m_textEditor");
  tedit.toolTipText            = "Text editor for job options.";
  m_needDefaults.toolTipText   = "Check YES if the tasks requires partition defaults";
  m_needTell1Setup.toolTipText = "Check YES if the tasks requires the list of TELL1 boards";
  m_showTasksTypes.toolTipText = "Show list of known task types.";
  m_showNodeTypes.toolTipText  = "Show list of node type configurations.";
  m_showPartitions.toolTipText = "Show list of known partition configurations.";

  m_OK.toolTipText             = "Exit and close panel";
  m_close.toolTipText	       = "Close text editor without saving possible changes.";
  m_save.toolTipText	       = "Close text editor WITH saving possible changes.";
  m_systemName.toolTipText     = "Click here to change the system for accessing job options.";

  m_list.toolTipText           = "List of found objects. Double click for further expansion.";
  m_listEditor.toolTipText     = "List of found objects. Double click for further expansion.";
  m_createFrame.toolTipText    = "Enter here information to create new objects.";
  m_create.toolTipText         = "Create new object of the requested type.";
  m_newName.toolTipText        = "Enter name of new object to be created.";

  m_partitionInfo.toolTipText  = "Summary information of the selected partition.";
  m_partitionName.toolTipText  = "Name of the selected partition.";
  m_partitionID.toolTipText    = "Partition identifier of the selected partition.";
  m_activity.toolTipText       = "Active activity type of the selected partition.";
  m_state.toolTipText	       = "State of the selected partition.";
  m_logo.toolTipText           = "Yeah! That's us!!!";
  m_logo2.toolTipText          = m_logo.toolTipText;
}
/// Create a new Partition object
int JobOptionsEditor_createObject(string name,string typ) {
  if ( strlen(name) > 0 )  {
    string n = typ+"_"+name;
    dyn_string names;
    dyn_uint ids;
    getSystemNames(names,ids);
    DebugN(names,ids);
    if ( 0 == nameCheck(n) )  {
      if ( !dpExists(n) )  {
        int rc = dpCreate(n,typ,JobOptions_sysID());
        if ( 0 == rc )  {
          return 0;
        }
        return JobOptions_error(rc,"Cannot create new "+typ+" object:"+n+" system:"+JobOptionsSystem+" id:"+id);
      }
      return JobOptions_error(0,"Datapoint "+n+" of type "+typ+" exists already");
    }
    return JobOptions_error(0,"Datapoint name "+n+" is not allowed."+
                            "Are you sure you are on the correct system?");
  }
  return JobOptions_error(rc,"Invalid name for a new "+typ+" object '"+name+"'");
}
/// Delete a Partition object
int JobOptionsEditor_deleteObject(string name,string typ) {
  if ( strlen(name) > 0 )  {
    string n = JobOptions_sysName()+typ+"_"+name;
    if ( dpExists(n) )  {
      int rc = dpDelete(n);
      if ( 0 == rc )  {
        return 0;
      }
      return JobOptions_error(rc,"Cannot delete "+typ+" object:"+n);
    }
    return JobOptions_error(0,"Datapoint "+n+" of type "+typ+" does not exist");
  }
  JobOptions_error(rc,"Invalid name to delete "+typ+" object '"+name+"'");
}
void JobOptionsEditor_showCreator(int panel_type)  {
  if ( jo_debug > 0 ) { DebugN("JobOptionsEditor_showCreator> Panel type:"+panel_type); }
  bool visible = panel_type > 0;
  m_newName.text         = "";
  setInputFocus(myModuleName(), myPanelName(), "m_newName");
  if ( visible ) {
    m_listEditor.visible = false;
    LayerOff(2);
    LayerOn(3);
    m_list.scale(1.,0.85);
  }
  else {
    LayerOff(3);
  }
  switch(panel_type) {
    case jo_PARTITION_EDIT:
      m_createFrame.text = "Create new partition";
      break;
    case jo_NODE_EDIT:
      m_createFrame.text = "Create new node type";
      break;
    case jo_TASK_EDIT:
      m_createFrame.text = "Create new task type";
      break;
  }
}
/// Show/hide text editor object
void JobOptionsEditor_showTextEditor(int val)	  {
  DebugN("JobOptionsEditor_showTextEditor["+JobOptionsVersion+"]> Text editor mode:"+val);
  if ( val ) LayerOn(2);
  else       LayerOff(2);
  if ( val ) setInputFocus(myModuleName(), myPanelName(), "m_textEditor");
}
/// Editor: Open the job options editor
void JobOptionsEditor_openTextEditor() {
  setValue("m_save","visible",true);
  setValue("m_close","visible",true);
  m_needDefaults.text(0)    = "Require defaults";
  m_needTell1Setup.text(0)  = "Require TELL1 list";
  m_list.scale(1.,0.3);
  JobOptionsEditor_showTextEditor(1);
}
/// Editor: Close the job options editor
void JobOptionsEditor_closeTextEditor() {
  JobOptionsEditor_showTextEditor(0);
  m_save.visible = 0;
  m_close.visible = 0;
  if ( !m_listEditor.visible )  {
    m_list.scale(1.,1.);
    m_logo.visible = 1;
    m_logo2.visible = 1;
    m_logoFrame.visible = 1;
  }  
}
/// Editor: Open the list editor
void JobOptionsEditor_openListEditor() {
  m_listEditor.visible  = 1;
  m_save.visible  = 0;
  m_close.visible = 0;
  m_list.scale(1.,0.3);
}
/// Editor: Close the list editor
void JobOptionsEditor_closeListEditor() {
  m_listEditor.visible = 0;
  m_save.visible = 0;
  m_close.visible = 0;
  m_list.scale(1.,1.);
  m_logo.visible = 1;
  m_logo2.visible = 1;
  m_logoFrame.visible = 1;
}
/// Editor: close all editor windows
void JobOptionsEditor_closeEditors() {
  JobOptionsEditor_closeTextEditor();
  JobOptionsEditor_closeListEditor();
  m_partitionInfo.visible = 0;
  m_partitionName.visible = 0;
  m_partitionID.visible	= 0;
  m_activity.visible	= 0;
  m_state.visible	= 0;
  m_list.scale(1.,1.);
  m_logo.visible = 1;
  m_logo.text	= "LHCb";
  m_logo2.visible = 1;
  m_logo2.text	= "  Online";
  m_logoFrame.visible = 1;
  m_logoFrame.text = "";
}
/// Editor: Populate list window with items
void JobOptionsEditor_populateList(dyn_string items)  {
  m_list.deleteAllItems();
  for(int i=1, n=dynlen(items); i<=n; ++i)
    m_list.appendItem(items[i]);
  setInputFocus(myModuleName(), myPanelName(), "m_list");
}
/// Editor: Populate list editor window with items
void JobOptionsEditor_populateListEditor(dyn_string items)  {
  m_listEditor.deleteAllItems();
  for(int i=1, n=dynlen(items); i<=n; ++i)
    m_listEditor.appendItem(items[i]);
  setInputFocus(myModuleName(), myPanelName(), "m_listEditor");
}
void JobOptionsEditor_showFirstLevelItems(dyn_string items)  {
  JobOptionsEditor_closeEditors();
  JobOptionsEditor_showCreator(0);
  JobOptionsEditor_populateList(items);
}
/// Editor: display all partitions in the list window
int JobOptionsEditor_showPartitions()  {
  JobOptionsEditor_showFirstLevelItems(JobOptions_partitions());
  m_resultFrame.text = "List of known node partition configurations";
  return 1;
}
/// Editor: display all node types of a given partition in the list window
int JobOptionsEditor_showNodes()  {
  JobOptionsEditor_showFirstLevelItems(JobOptions_nodeTypes());
  m_resultFrame.text = "List of known node type configurations";
  return 2;
}
/// Editor: display all task types in the list window
int JobOptionsEditor_showTaskTypes()  {
  JobOptionsEditor_showFirstLevelItems(JobOptions_taskTypes());
  m_resultFrame.text = "List of known task types";
  return 3;
}
/// Editor: Show all tasks executing in a node type
int JobOptionsEditor_showNodeTasks(string text)  {
  dyn_string tasks;
  int rc = dpGet(JobOptions_sysName()+JobOptionsActivity_t+"_"+text+".",tasks);
  if ( 0 == rc )  {
    DebugN("JobOptionsEditor_showNodeTasks> "+JobOptionsActivity_t+"_"+text+" has "+dynlen(tasks)+" tasks.");
    int typ_len = strlen(JobOptionsActivity_t)+1;
    m_listEditor.deleteAllItems();
    JobOptionsEditor_openListEditor();
    for(int i=1,n=dynlen(tasks); i<=n; ++i)  {
      string tsk = substr(dpSubStr(tasks[i],DPSUB_DP),typ_len);
      m_listEditor.appendItem(tsk);
    }
    return jo_NODE_EDIT;
  }
  return JobOptions_error(rc,"Cannot access node type"+text);
}
/// Editor: Show all tasks executing in a node type
int JobOptionsEditor_showNodeTypes()  {
  string activity, pid, state;
  string partName = m_list.selectedText;
  string typ = JobOptions_sysName()+JobOptionsPartition_t+"_"+partName;
  DebugN("JobOptionsEditor_showNodeTypes> "+typ);
  int rc = dpGet(typ+".Activity",activity,
    typ+".PartitionID",pid,
    typ+".State",state);
  if ( rc == 0 )  {
    int part_id;
    bool exists_pid = dpExists(pid);
    bool exists_act = dpExists(activity);
    if ( !(exists_act && exists_pid) )  {
      dyn_string ds;
      dyn_float  df;
      string err = "Partition "+partName+" is not properly configured:\n";
      if ( !exists_act ) err = err + "Activity is undefined. ";
      if ( !exists_pid ) err = err + "Partition is undefined.\n";
      err = err + "Do you ant to edit the values ?";
      ChildPanelOnReturn("vision/MessageInfo","Partition:"+partName,makeDynString(err,"Yes","No"),50,50,df,ds);
      if ( df[1] )  {
        JobOptionsEditor_edit2(jo_PARTITION_EDIT,partName,exists_act ? "PartitionID" : "Activity");	
      }
      return jo_NO_EDIT;
    }
    rc = dpGet(activity,activity,pid,part_id);
    if ( rc == 0 )  {
      //JobOptionsEditor_populateListEditor(JobOptions_nodeTypes("_"+partName+"_"+activity));
      JobOptionsEditor_populateListEditor(JobOptions_nodeTypes());
      JobOptionsEditor_openListEditor();
      m_partitionInfo.text	= "Partition info";
      m_partitionName.text      = "Partition:"+m_list.selectedText;
      m_partitionID.text	= "Partition ID:"+part_id;
      m_activity.text		= "Activity:"+activity;
      m_state.text		= "State:"+state;
      m_partitionInfo.visible	= 1;
      m_partitionName.visible	= 1;
      m_partitionID.visible	= 1;
      m_activity.visible	= 1;
      m_state.visible		= 1;
      m_logo.visible            = 0;
      m_logo2.visible           = 0;
      m_logoFrame.visible       = 0;
      return jo_PARTITION_EDIT;
    }
  }
  return JobOptions_error(rc,"Cannot access partition information of:"+m_list.selectedText+"["+typ+"]");
}
/// Editor: Show options for one task type in edit control
int JobOptionsEditor_showTaskType(string text)  {
  string nam = JobOptionsTaskType_t+"_"+text, opts;
  string dp  = JobOptions_sysName()+nam+".";
  bool   tell1, defs;
  int rc = dpGet(dp+"Options",opts,dp+"NeedTell1s",tell1,dp+"NeedDefaults",defs);
  if ( 0 == rc )  {
    m_textEditor.Text	= opts;
    m_needDefaults.state(0) = defs;
    m_needTell1Setup.state(0) = tell1;
    JobOptionsEditor_openTextEditor();
    return jo_TASK_EDIT;
  }
  return JobOptions_error(rc,"Cannot access TaskType:"+text+" DP:"+dp);
}
/// Create a new object depending on editing mode
int JobOptionsEditor_create(int panel_type,string name)  {
  switch(panel_type) {
    case jo_PARTITION_EDIT:
      JobOptionsEditor_createObject(name,JobOptionsPartition_t);
      return JobOptionsEditor_showPartitions();
    case jo_NODE_EDIT:
      JobOptionsEditor_createObject(name,JobOptionsActivity_t);
      return JobOptionsEditor_showNodes();
    case jo_TASK_EDIT:
      JobOptionsEditor_createObject(name,JobOptionsTaskType_t);
      return JobOptionsEditor_showTaskTypes();
  }
  return jo_NO_EDIT;
}
/// Delete an object depending on editing mode
int JobOptionsEditor_delete(int panel_type,string name)  {
  switch(panel_type) {
    case jo_PARTITION_EDIT:
      JobOptionsEditor_deleteObject(name,JobOptionsPartition_t);
      return JobOptionsEditor_showPartitions();
    case jo_NODE_EDIT:
      JobOptionsEditor_deleteObject(name,JobOptionsActivity_t);
      return JobOptionsEditor_showNodes();
    case jo_TASK_EDIT:
      JobOptionsEditor_deleteObject(name,JobOptionsTaskType_t);
      return JobOptionsEditor_showTaskTypes();
  }
  return jo_NO_EDIT;
}
/// Edit an object depending on editing mode
int JobOptionsEditor_edit(int panel_type,string name)  {
  switch(panel_type) {
    case jo_PARTITION_EDIT:  	return JobOptionsEditor_edit2(panel_type,name,"State");
    case jo_NODE_EDIT:
    case jo_TASK_EDIT:          return JobOptionsEditor_edit2(panel_type,name,"");
  }
}
/// Edit an object depending on editing mode
int JobOptionsEditor_edit2(int panel_type,string name,string sub_dp)  {
  string p_name;
  switch(panel_type) {
    case jo_PARTITION_EDIT:
      p_name = JobOptionsPartition_t+"_"+name;
      ChildPanelOn("JobOptions/JobOptionsEditor.pnl",p_name,makeDynString(p_name+"."+sub_dp,p_name),50,50);
      return panel_type;
    case jo_NODE_EDIT:
      p_name = JobOptionsActivity_t+"_"+name;
      ChildPanelOn("JobOptions/JobOptionsEditor.pnl",p_name,makeDynString(p_name+".:_original.._value",p_name),50,50);
      return panel_type;
    case jo_TASK_EDIT:
      return JobOptionsEditor_showInfo(panel_type);
  }
}
/// Editor: Show iformation items in edit control
int JobOptionsEditor_showInfo(int panel_type)  {
  JobOptionsEditor_showCreator(0);
  DebugN("JobOptionsEditor_showInfo> Panel call type is:"+panel_type);
  switch(panel_type) {
    case 1:
      return jo_NO_EDIT == JobOptionsEditor_showNodeTypes() ? panel_type : 10*panel_type;
    case 10:
      JobOptionsEditor_showNodeTasks(m_listEditor.selectedText);
      return 10*panel_type;
    case 2:
      JobOptionsEditor_showNodeTasks(m_list.selectedText);
      return 10*panel_type;
    case 20:
      m_listEditor.visible = 0;
      JobOptionsEditor_showTaskType(m_listEditor.selectedText);
      return 10*panel_type;
    case 100:
    case 200:
    case 300:
      m_listEditor.visible = 0;
      JobOptionsEditor_showTaskType(m_listEditor.selectedText);
      return panel_type;
    case 3:
      return JobOptionsEditor_showTaskType(m_list.selectedText);
  }
  return jo_NO_EDIT;
}
/// Editor: Close editors
int JobOptionsEditor_close(int panel_type)  {
  switch(panel_type) {
    case 3:
      JobOptionsEditor_closeTextEditor();
      return panel_type;
    case 100:
      JobOptionsEditor_showTextEditor(0);
      JobOptionsEditor_showNodeTypes();
      return panel_type/10;	  
    case 20:
    case 200:
      JobOptionsEditor_showTextEditor(0);
      JobOptionsEditor_openListEditor();
      return panel_type/10;
  }
  return jo_NO_EDIT;
}
/// Editor: Save items
int JobOptionsEditor_save(int panel_type)  {
  switch(panel_type) {
    case 3:
      JobOptionsEditor_saveOptions(m_list.selectedText);
      JobOptionsEditor_closeTextEditor();
      return panel_type;
    case 100:
      JobOptionsEditor_saveOptions(m_listEditor.selectedText);
      JobOptionsEditor_showNodeTypes();
      return panel_type/10;	  
    case 20:
    case 200:
      JobOptionsEditor_saveOptions(m_listEditor.selectedText);
      JobOptionsEditor_openListEditor();
      return panel_type/10;
  }
  return jo_NO_EDIT;
}
/// Editor: Save job options for a given task.
void JobOptionsEditor_saveOptions(string text)  {
  string opts  = m_textEditor.Text;
  string dp    = JobOptions_sysName()+JobOptionsTaskType_t+"_"+text+".";
  bool   tell1 = m_needTell1Setup.state(0);
  bool   defs  = m_needDefaults.state(0);
  int rc = dpSet(dp+"Options",opts,dp+"NeedTell1s",tell1,dp+"NeedDefaults",defs);
  if ( 0 == rc )  {
    JobOptionsEditor_showTextEditor(0);
    return;
  }
  JobOptions_error(rc,"Cannot access data point:"+dp);
}
int JobOptionsEditor_listPopup(int panel_type) {
  if ( jo_debug > 0 )  { DebugN("JobOptionsEditor_listPopup> Panel type:"+panel_type); }
  if ( panel_type != 0 )  {
    string name = m_list.selectedText;
    int answer; 
    dyn_string txt = makeDynString(  "PUSH_BUTTON, New..., 1, 1");
    if(strlen(name)>0) {
      dynAppend(txt,"PUSH_BUTTON, Delete, 2, 1");
      dynAppend(txt,"PUSH_BUTTON, Edit,   3, 1");
    }
    popupMenu(txt, answer);
    if ( answer == 1 ) {
      JobOptionsEditor_showCreator(panel_type);
    }
    else if ( answer == 2 && strlen(name) > 0 )  {
      JobOptionsEditor_delete(panel_type,name);
    }
    else if ( answer == 3 && strlen(name) > 0 )  {
      JobOptionsEditor_edit(panel_type,name);
    }
  }
  return panel_type;
}

/** Main CTRL entry point
  *
  * @author M.Frank
  */
main()   {
  string partitionName = "LHCb";
  jo_doExit = 0;
  addGlobal("JobOptionsSystem",STRING_VAR);
  JobOptionsSystem = "STORAGE";
  if ( 0 == JobOptions_config(partitionName) )  {
    if ( 0 == JobOptions_listen() )  {
      DebugN("JobOptions writer active and listening to commands.");
      while(!jo_doExit)  {
        delay(1);
      }
    }
  }
  DebugN("JobOptions writer finished.");
}
