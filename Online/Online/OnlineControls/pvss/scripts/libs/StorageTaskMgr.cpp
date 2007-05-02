//#uses "libFSM.ctl"
#uses "fwFsmTreeDisplay.ctl"
#uses "StorageControlPanel.cpp"

const int num_recvClass0 = 5;
const int num_recvClass1 = 30;
const int num_strmClass0 = 5;
const int num_strmClass1 = 15;
const int num_recvNodes = 5;
const int num_strmNodes = 5;
//=============================================================================
bool StorageTaskMgr_checkErrors(int res)  {
  if ( res != 0 || dynlen(getLastError()) > 0 )  {
    string err = " Err="+rc+","+getLastError();
    DebugN(" Err="+rc+","+getLastError());
    ChildPanelOn("vision/MessageInfo1","Error occurred",makeDynString(err),50,50);
  }
  return res == 0;
}
//=============================================================================
int StorageTaskMgr_installTaskType()  {
  string type="FSM_DimTask";
  if ( dynlen(dpTypes(type)) ==0 ) {
    dyn_dyn_string names;
    dyn_dyn_int types;
    names[1]  = makeDynString (type,"","","");
    names[2]  = makeDynString ("","Name","","");
    names[3]  = makeDynString ("","Type","","");
    names[4]  = makeDynString ("","Node","","");
    names[5]  = makeDynString ("","Command","","");
    names[6]  = makeDynString ("","State","","");
    names[7]  = makeDynString ("","Messages","","");
    names[8]  = makeDynString ("","","severity","");
    names[9]  = makeDynString ("","","msg","");
    names[10] = makeDynString ("","","source","");
    names[11] = makeDynString ("","Class","","");
    names[12] = makeDynString ("","InUse","","");
    names[13] = makeDynString ("","Priority","","");
    names[14] = makeDynString ("","FMC_Start","","");
    types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
    types[2]  = makeDynInt (0,DPEL_STRING,0,0);
    types[3]  = makeDynInt (0,DPEL_STRING,0,0);
    types[4]  = makeDynInt (0,DPEL_STRING,0,0);
    types[5]  = makeDynInt (0,DPEL_STRING,0,0);
    types[6]  = makeDynInt (0,DPEL_STRING,0,0);
    types[7]  = makeDynInt (0,DPEL_STRUCT,0,0);
    types[8]  = makeDynInt (0,0,DPEL_INT,0);
    types[9]  = makeDynInt (0,0,DPEL_STRING,0);
    types[10] = makeDynInt (0,0,DPEL_STRING,0);
    types[11] = makeDynInt (0,DPEL_INT,0,0);
    types[12] = makeDynInt (0,DPEL_INT,0,0);
    types[13] = makeDynInt (0,DPEL_INT,0,0);
    types[14] = makeDynInt (0,DPEL_STRING,0,0);
    int result = dpTypeCreate(names,types);
    StorageTaskMgr_checkErrors(result);
    DebugN("Installation of data type "+type+" finished.");
    return;
  }
  DebugN("Data type "+type+" are already defined....Nothing to do.");
}
//=============================================================================
int StorageTaskMgr_uninstallTaskType(string type)  {
  if ( dynlen(dpTypes(type)) > 0 ) {
    dyn_string dps = dpNames("*",type);
    if ( dynlen(dps) > 0 )  {
      for (int i=1, n=dynlen(dps); i<=n; ++i)  {
        int res = dpDelete(dps[i]);
        DebugN("Deleted datapoint:"+dps[i]);
        StorageControlPanel_checkErrors(res);
      }
    }
    int result = dpTypeDelete(type);
    StorageControlPanel_checkErrors(result);
    DebugN("Data type "+type+" removed.");
    return;
  }  
  DebugN("Data type "+type+" does not exist....Nothing to do.");
}
//=============================================================================
int StorageTaskMgr_install()  {
  StorageTaskMgr_installTaskType();
}
//=============================================================================
int StorageTaskMgr_uninstall()  {
  StorageTaskMgr_uninstallTaskType();
}
//=============================================================================
int StorageTaskMgr_installStorage(string storage)  {
    string storage_node = fwFsmTree_addNode("FSM", "Data"+storage, "StorageCluster", 1);
    if ( !dpExists(storage) )  {
      dpCreate(storage,"StorageControl");
    }
    string pref = "";
    if ( substr(storage,0,4)=="Test")  {
      pref = "Test";
    }
    if ( !dpExists(pref+"JobOptions") )  {
      dpCreate(pref+"JobOptions","JobOptionsControl");
    }
    string st = fwFsmTree_addNode(storage_node,storage,"StorageControl",0);
    string st = fwFsmTree_addNode(storage_node,pref+"JobOptions","JobOptionsControl",0);
    fwFsmTree_refreshTree();
}
//=============================================================================
int StorageTaskMgr_uninstallStorage(string storage)  {
  DebugN("Delete tree:"+"Data"+storage);
  fwFsmTree_removeNode("FSM","Data"+storage,1);
  fwFsmTree_refreshTree();
  DebugN("All Done.");
}

int startFSM_DimTask(string domain, string name)    {
  DebugN("Starting task:"+domain+" "+name);
  /*
int fwFM_startProc(string nodeName, string command)
{
  //if (!dpExists(nodeName+"/StTaskManager")) dpCreate(nodeName+"/StTaskManager","StTaskManager");
  string name="fwFMC_"+nodeName+".StTaskManager.settings.start";
  dpSetWait(name,command);
  return(0);
}
*/
}
int StorageTaskMgr_createNodeTasks(string node, string name, int howmany, int first) {
  string dev_name;
  for(int j=first; j<howmany+first; j++)  {
    sprintf(dev_name,"%s_%03d",name,j);
    if ( !dpExists(dev_name) )  {
      dpCreate(dev_name,"FSM_DimTask");
    }
    string task = fwFsmTree_addNode(node, dev_name, "FSM_DimTask", 0);
    DebugN("Create Node:"+dev_name+ " -> " + task);
  }
  return 1;
}
//=============================================================================
int StorageTaskMgr_createNode(string storage_node,string name,int num_class0,int num_class1)  {
  string recv_node = fwFsmTree_addNode(storage_node, name, "FSM_Node", 0);
  if ( recv_node != "" )  {
    string node  = fwFsmTree_addNode(recv_node, name+"_Class0", "FSM_Class0", 0);
    DebugN("    Create Node:"+name+"_Class0 -> " + node);
    if ( node != "" )  {
      fwFsmTree_setNodeLabel(name+"_Class0", "Class 0");
      StorageTaskMgr_createNodeTasks(node,name,num_class0,0);
    }
    node  = fwFsmTree_addNode(recv_node, name+"_Class1", "FSM_Class1", 0);
    DebugN("    Create Node:"+name+"_Class1 -> " + node);
    if ( node != "" )  {
      fwFsmTree_setNodeLabel(name+"_Class1", "Class 1");
      StorageTaskMgr_createNodeTasks(node,name,num_class1,num_class0);
    }
  }
  return 1;
}
//=============================================================================
int StorageTaskMgr_createTree(string storage, string slice_name, int refresh=1)  {
  dyn_string recvNodes, strmNodes;
  // Get storage configuration from the corresponding StorageControl datapoint
  int res = dpGet(storage+".RecvNodes",recvNodes,
                  storage+".StreamNodes",strmNodes);
  if ( 0 == res )  {
    string node = storage+"_"+slice_name;
    string storage_node = fwFsmTree_addNode("FSM", node, "FSM_Control", 1);
    if ( storage_node != "" )   {
      for(int i=1; i<=dynlen(recvNodes); ++i)  {
        string name = recvNodes[i];
        StorageTaskMgr_createNode(storage_node,node+"_"+name,num_recvClass0,num_recvClass1);
        fwFsmTree_setNodeLabel(node+"_"+name,name);
      }
      for(int i=1; i<=dynlen(strmNodes); ++i)  {
        string name = strmNodes[i];
        StorageTaskMgr_createNode(storage_node,node+"_"+name,num_strmClass0,num_strmClass1);
        fwFsmTree_setNodeLabel(node+"_"+name,name);
      }
    }  
    if ( refresh ) fwFsmTree_refreshTree();
    DebugN("All Done.");
  }
  StorageTaskMgr_checkErrors(res);
  return 1;
}
//=============================================================================
int StorageTaskMgr_createAllTree(string storage)  {
  for(int i=0; i<16; ++i)  {    
    string node;
    sprintf(node,"Slice%02d",i);
    StorageTaskMgr_createTree(storage,node,0);
  }
  fwFsmTree_refreshTree();
  DebugN("All Done.");
}
//=============================================================================
int StorageTaskMgr_deleteTree(string storage, string slice_name, int refresh=1)  {
  string node = storage+"_"+slice_name;
  DebugN("Delete tree:"+node);
  fwFsmTree_removeNode("FSM",node,1);
  if ( refresh ) fwFsmTree_refreshTree();
  DebugN("All Done.");
}
//=============================================================================
int StorageTaskMgr_deleteAllTree(string storage)  {
  for(int i=0; i<16; ++i)  {    
    string node;
    sprintf(node,"Slice%02d",i);
    StorageTaskMgr_deleteTree(storage,node,0);
  }
  fwFsmTree_refreshTree();
  DebugN("All Done.");
}
//=============================================================================
int StorageTaskMgr_genFSM(string storage, string slice_name)  {
  string node = storage+"_"+slice_name;
  DebugN("Generate FSM:"+node);
  fwFsmTree_generateTreeNode(node);
  DebugN("All Done.");
}
//=============================================================================
int StorageTaskMgr_genAllFSM(string storage)  {
  fwFsmTree_generateAll();
  DebugN("All Done.");
}
//=============================================================================
int StorageTaskMgr_startTree(string storage, string slice_name)  {
  string node = storage+"_"+slice_name;
  DebugN("Start Tree:"+node);
  fwFsmTree_startTreeNode(node);
  DebugN("All Done.");
}
//=============================================================================
int StorageTaskMgr_startAllTree(string storage)  {
  fwFsmTree_startTree();
  DebugN("All Done.");
}
//=============================================================================
int StorageTaskMgr_stopTree(string storage, string slice_name)  {
  string node = storage+"_"+slice_name;
  DebugN("Start Tree:"+node);
  fwFsmTree_stopTreeNode(node);
  DebugN("All Done.");
}
//=============================================================================
int StorageTaskMgr_stopAllTree(string storage)  {
  fwFsmTree_stopTree();
  DebugN("All Done.");
}
//=============================================================================
void StorageTaskMgr_clearTasks(string slice)  {
  dyn_string  dp_names;
  dyn_anytype dp_values;
  dyn_string dps = dpNames(slice+"_*","FSM_DimTask");
  for(int j=1, n=dynlen(dps); j<=n; ++j)  {
    string n = dps[j];
    DebugN("Disable tree node:"+n);
    fwCU_disableObj(n,n,1);
    fwFsmTree_setNodeLabel(n,"Disabled");
    dynAppend(dp_names,n+".Name");
    dynAppend(dp_values,(string)"Unused");
    dynAppend(dp_names,n+".Class");
    dynAppend(dp_values,(int)-1);
    dynAppend(dp_names,n+".InUse");
    dynAppend(dp_values,(int)0);
    dynAppend(dp_names,n+".Node");
    dynAppend(dp_values,(string)"");
    dynAppend(dp_names,n+".Priority");
    dynAppend(dp_values,(int)0);
    dynAppend(dp_names,n+".State");
    dynAppend(dp_values,(string)"UNKNOWN");
    dynAppend(dp_names,n+".Type");
    dynAppend(dp_values,(string)"");
    dynAppend(dp_names,n+".Command");
    dynAppend(dp_values,(string)"");
  }
  dpSet(dp_names, dp_values);
}
//=============================================================================
void StorageTaskMgr_setupTasks(string slice,dyn_string& tasks, int class, 
                               dyn_string& dp_names, dyn_string& dp_values, 
                               mapping& cl0_map, mapping& cl1_map, int offset)
{
  for(int j=1, n=dynlen(tasks); j<=n; ++j)  {
    dyn_string items = strsplit(tasks[j],"/");
    string tmp;
    string mach = items[1];
    string task = items[2];
    string type = items[3];
    if ( class == 0 )  {
      if ( !mappingHasKey(cl0_map, mach) )  {
        cl0_map[mach] = 0;
      }
      sprintf(tmp,"%03d",cl0_map[mach]);
      cl0_map[mach] = cl0_map[mach] + 1;
    }
    else {
      if ( !mappingHasKey(cl1_map, mach) ) {
        cl1_map[mach] = offset;
      }
      sprintf(tmp,"%03d",cl1_map[mach]);
      cl1_map[mach] = cl1_map[mach] + 1;
    }
    string sub_tree_node = slice + "_" + mach + "_" + tmp;    
    DebugN("Enable node: "+sub_tree_node+" as "+type+"_"+tmp);
    fwFsmTree_setNodeLabel(sub_tree_node,type+"_"+tmp);
    fwCU_enableObj(sub_tree_node,sub_tree_node,1);
    dynAppend(dp_names,sub_tree_node+".Name");
    dynAppend(dp_values,(string)task);
    dynAppend(dp_names,sub_tree_node+".Class");
    dynAppend(dp_values,(int)0);
    dynAppend(dp_names,sub_tree_node+".InUse");
    dynAppend(dp_values,(int)1);
    dynAppend(dp_names,sub_tree_node+".Node");
    dynAppend(dp_values,(string)mach);
    dynAppend(dp_names,sub_tree_node+".Priority");
    dynAppend(dp_values,(int)10);
    dynAppend(dp_names,sub_tree_node+".State");
    dynAppend(dp_values,(string)"UNKNOWN");
    dynAppend(dp_names,sub_tree_node+".Type");
    dynAppend(dp_values,(string)type);
    dynAppend(dp_names,sub_tree_node+".Command");
    dynAppend(dp_values,(string)"");
  }
}
//=============================================================================
int StorageTaskMgr_manipTree(string storage, string slice_name)  {
  int node_count = 0;
  string slice = storage + "_" + slice_name;
  dyn_string itms1 = dpNames(storage+"_Partition*.FSMSlice","StoragePartition");
  dyn_string itms2 = dpNames(storage+"_Partition*.Name","StoragePartition");
  dyn_string val1, val2;
  dpGet(itms1,val1);
  dpGet(itms2,val2);
  DebugN("Storage:"+storage+"  Slice:"+slice_name);
  for(int i=1, n=dynlen(itms1); i<=n; ++i)  {
    if ( strlen(val1[i]) > 0 && val1[i] == slice )  {
      string pname = val2[i]+"_RunInfo.Storage.";
      string part = dpSubStr(itms1[i],DPSUB_DP);
      dyn_string recvInfraTasks, strmInfraTasks;
      dyn_string recvSenders, strmReceivers, strmWriters;
      dpGet(pname+"recvInfrastructureTasks",recvInfraTasks, 
            pname+"streamInfrastructureTasks",strmInfraTasks,
            pname+"streamReceivers",strmReceivers,
            pname+"streamWriters",strmWriters,
            pname+"recvSenders",recvSenders);
      DebugN("Slice:"+itms1[i]+" "+val1[i]+" "+val2[i]+" "+part+"   "+pname);
      DebugN("      "+recvInfraTasks);
      DebugN("      "+strmInfraTasks);
      DebugN("      "+strmReceivers);
      DebugN("      "+strmWriters);
      DebugN("      "+recvSenders);
      mapping cl0_map, cl1_map;
      int task_cnt = 0;
      dyn_string  dp_names;
      dyn_anytype dp_values;
      StorageTaskMgr_clearTasks(slice);
      StorageTaskMgr_setupTasks(slice,recvInfraTasks,0,dp_names,dp_values,cl0_map,cl1_map,0);
      StorageTaskMgr_setupTasks(slice,strmInfraTasks,0,dp_names,dp_values,cl0_map,cl1_map,0);
      StorageTaskMgr_setupTasks(slice,recvSenders,1,dp_names,dp_values,cl0_map,cl1_map,num_recvClass0);
      StorageTaskMgr_setupTasks(slice,strmReceivers,1,dp_names,dp_values,cl0_map,cl1_map,num_strmClass0);
      StorageTaskMgr_setupTasks(slice,strmWriters,1,dp_names,dp_values,cl0_map,cl1_map,num_strmClass0);
      // DebugN("5 - names:"+dynlen(dp_names)+"   values:"+dynlen(dp_values));
      dpSet(dp_names, dp_values);
    }
  }
  fwFsmTree_refreshTree();
  return 1;
}

int xxStorageTaskMgr_manipTree(string storage, string node_name)  {
  for(int i=0; i<num_recvNodes; ++i)  {
    string recv_node;
    sprintf(recv_node,"%s_Recv%02d",node,i);
    string cl0_node  = recv_node+"_Class0";
    if ( cl0_node != "" )  {
      string dev_name;
      for(int j=0; j<num_recvClass0; j++)  {
        sprintf(dev_name,"%s_%03d",recv_node,j);
        if ( j > 2 )  {
          DebugN("Manipulate Node:"+dev_name);
          fwCU_disableObj(dev_name,dev_name,1);
        }
        if      ( j == 0 )  fwFsmTree_setNodeLabel(dev_name,"MBM");
        else if ( j == 1 )  fwFsmTree_setNodeLabel(dev_name,"ErrSrv");
        else  {
          fwFsmTree_setNodeLabel(dev_name,"Unused");
        }
      }
    }
    DebugN("Create Node:"+recv_node+"_Class1");
    string cl1_node  = recv_node+"_Class1";
    if ( cl1_node != "" )  {
      string dev_name;
      for(int j=0; j<num_class1; j++)  {
        sprintf(dev_name,"StoRecv_%03d",j);
        if ( j > 10 )  {
          DebugN("Manipulate Node:"+dev_name);
          fwCU_disableObj(dev_name,dev_name,1);
        }
      }
    }
  }
  fwFsmTree_refreshTree();
  DebugN("All Done.");
}
int main()  {
  // StorageTaskMgr_install();
  //StorageTaskMgr_deleteTree("StorageSlice");
  //StorageTaskMgr_createTree("StorageSlice");
}
