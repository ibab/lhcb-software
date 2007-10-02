//#uses "libFSM.ctl"
#uses "fwDIM"
#uses "fwFsmTreeDisplay.ctl"
#uses "StreamControlPanel.cpp"

//=============================================================================
bool StreamTaskMgr_checkErrors(int res)  {
  if ( res != 0 || dynlen(getLastError()) > 0 )  {
    string err = " Err="+res+","+getLastError();
    DebugN(" Err="+res+","+getLastError());
    ChildPanelOn("vision/MessageInfo1","Error occurred",makeDynString(err),50,50);
  }
  return res == 0;
}
//=============================================================================
int StreamTaskMgr_installTaskType()  {
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
    StreamTaskMgr_checkErrors(result);
    DebugN("Installation of data type "+type+" finished.");
  }
  else  {
    DebugN("Data type "+type+" are already defined....Nothing to do.");
  }
  type="StreamTaskCreator";
  if ( dynlen(dpTypes(type)) ==0 ) {
    dyn_dyn_string names;
    dyn_dyn_int types;
    names[1]  = makeDynString (type,"","","");
    names[2]  = makeDynString ("","Name","","");
    names[3]  = makeDynString ("","Start","","");
    names[4]  = makeDynString ("","Stop","","");
    names[5]  = makeDynString ("","Kill","","");
    types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
    types[2]  = makeDynInt (0,DPEL_STRING,0,0);
    types[3]  = makeDynInt (0,DPEL_STRING,0,0);
    types[4]  = makeDynInt (0,DPEL_STRING,0,0);
    types[5]  = makeDynInt (0,DPEL_STRING,0,0);
    int result = dpTypeCreate(names,types);
    StreamTaskMgr_checkErrors(result);
    DebugN("Installation of data type "+type+" finished.");
  }
  else  {
    DebugN("Data type "+type+" are already defined....Nothing to do.");
  }
}
//=============================================================================
int StreamTaskMgr_connectTaskManager(string stream)  {
  dyn_string recvNodes, strmNodes;
  string cfg = "DimStorage";
  fwDim_createConfig(cfg);
  StreamTaskMgr_installTaskType();
  int res = dpGet(stream+"Alloc.RecvNodes",recvNodes,stream+"Alloc.StreamNodes",strmNodes);
  if ( 0 == res )  {
    dynAppend(recvNodes,strmNodes);
    for(int i=1; i<=dynlen(recvNodes); ++i)  {
      string name = strtoupper(recvNodes[i]);
      string dp_name = name+"_StreamTaskCreator";
      string svc_name = "/" + name + "/task_manager";
      fwDim_unSubscribeCommandsByDp("DimStorage",dp_name+"*");
      fwDim_unSubscribeServicesByDp("DimStorage",dp_name+"*");
      if ( !dpExists(dp_name) ) dpCreate(dp_name,"StreamTaskCreator");
      dpSet(dp_name+".Name",name);
      fwDim_subscribeCommand(cfg,svc_name+"/start",dp_name+".Start");
      fwDim_subscribeCommand(cfg,svc_name+"/stop",dp_name+".Stop");
      fwDim_subscribeCommand(cfg,svc_name+"/kill",dp_name+".Kill");
      DebugN("Connect "+svc_name+" to "+dp_name);
    }
    DebugN("All Done.");
  }
  StreamTaskMgr_checkErrors(res);
  return 1;
}
//=============================================================================
string StreamTaskMgr_addTreeNode(string parent, string node, string type, int isCU) {
  if ( 0 == fwFsmTree_isNode(node) ) {
    DebugN("Create Node:"+parent+"::"+node+" ["+type+"]");
    return fwFsmTree_addNode(parent, node, type, isCU);
  }
  DebugN("Node "+parent+"::"+node+" Exists already....");
  return node;
}
//=============================================================================
int StreamTaskMgr_uninstallTaskType()  {
  string type="FSM_DimTask";
  if ( dynlen(dpTypes(type)) > 0 ) {
    dyn_string dps = dpNames("*",type);
    if ( dynlen(dps) > 0 )  {
      for (int i=1, n=dynlen(dps); i<=n; ++i)  {
        int res = dpDelete(dps[i]);
        DebugN("Deleted datapoint:"+dps[i]);
        StreamControlPanel_checkErrors(res);
      }
    }
    int result = dpTypeDelete(type);
    StreamControlPanel_checkErrors(result);
    DebugN("Data type "+type+" removed.");
    return;
  }  
  DebugN("Data type "+type+" does not exist....Nothing to do.");
}
//=============================================================================
int StreamTaskMgr_install()  {
  StreamTaskMgr_installTaskType();
}
//=============================================================================
int StreamTaskMgr_uninstall()  {
  StreamTaskMgr_uninstallTaskType();
}
//=============================================================================
int StreamTaskMgr_installStream(string stream)  {
    string stream_node = StreamTaskMgr_addTreeNode("FSM", "Data"+stream, "StreamCluster", 1);
    string stream_alloc = stream+"Alloc";
    if ( !dpExists(stream_alloc) )  {
      dpCreate(stream_alloc,"StreamControl");
    }
    string pref = stream;
    string st = StreamTaskMgr_addTreeNode(stream_node,stream_alloc,"StreamControl",0);
    fwFsmTree_refreshTree();
}
//=============================================================================
int StreamTaskMgr_uninstallStream(string stream)  {
  DebugN("Delete tree:"+"Data"+stream);
  fwFsmTree_removeNode("FSM","Data"+stream,1);
  fwFsmTree_refreshTree();
  DebugN("All Done.");
  return 1;
}
int StreamTaskMgr_generateFSM(string node)   {
  DebugN("Generate FSM:"+node);
  fwFsmTree_generateTreeNode(node);
  DebugN("All Done.");
  return 1;
}
int StreamTaskMgr_startFSMNode(string node)   {
  DebugN("Start Tree:"+node);
  fwFsmTree_startTreeNode(node);
  DebugN("All Done.");
  return 1;
}
int StreamTaskMgr_stopFSMNode(string node)   {
  DebugN("Start Tree:"+node);
  fwFsmTree_stopTreeNode(node);
  DebugN("All Done.");
  return 1;
}
//=============================================================================
int StreamTaskMgr_genStreamFSM(string stream)   
{ return StreamTaskMgr_generateFSM(stream+"Alloc");   }
//=============================================================================
int StreamTaskMgr_startStreamTree(string stream)  
{ return StreamTaskMgr_startFSMNode(stream+"Alloc");  }
//=============================================================================
int StreamTaskMgr_stopStreamTree(string stream)  
{ return StreamTaskMgr_stopFSMNode(stream+"Alloc");  }
//=============================================================================
int StreamTaskMgr_createNodeTasks(string node, string name, int howmany, int first) {
  string dev_name;
  for(int j=first; j<howmany+first; j++)  {
    sprintf(dev_name,"%s_%03d",name,j);
    if ( !dpExists(dev_name) )  {
      dpCreate(dev_name,"FSM_DimTask");
    }
    string task = StreamTaskMgr_addTreeNode(node, dev_name, "FSM_DimTask", 0);
  }
  return 1;
}
//=============================================================================
int StreamTaskMgr_createNode(string stream_node,string name,string type,int num)  {
  string node = StreamTaskMgr_addTreeNode(stream_node, name, type, 0);
  if ( num > 0 && node != "" )  {
    StreamTaskMgr_createNodeTasks(node,name,num,0);
  }
  return 1;
}
//=============================================================================
int StreamTaskMgr_createTree(string stream, 
                              string slice_name, 
                              int num_recvClass0,
                              int num_recvClass1,
                              int num_strmClass0,
                              int num_strmClass1,
                              string monitoringInput,
                              int num_monTasks,
                              int refresh=1)  
{
  dyn_string recvNodes, strmNodes, sendNodes;
  // Get stream configuration from the corresponding StreamControl datapoint
  int res = dpGet(stream+"Alloc.RecvNodes",recvNodes,stream+"Alloc.StreamNodes",strmNodes);
  DebugN(stream+"> Generating tree:"+slice_name+" "+num_recvClass0+"+"+num_recvClass1+" "+
         num_strmClass0+"+"+num_strmClass1+" Monitoring:"+monitoringInput+" "+num_monTasks);
  if ( num_monTasks>0 && 0 == res )   {
    res = dpGet(monitoringInput+"Alloc.RecvNodes",sendNodes);
  }
  if ( 0 == res )  {
    string name, node = stream+"_"+slice_name;
    //string slice_node = StreamTaskMgr_addTreeNode("Data"+stream, stream+"_Slices", "FSM_Holder", 1);
    string slice_node = StreamTaskMgr_addTreeNode("FSM", stream+"_Slices", "FSM_Holder", 1);
    string stream_node = StreamTaskMgr_addTreeNode(stream+"_Slices", node, "FSM_Slice", 1);
    DebugN("Slices:"+slice_node+" Stream:"+stream_node);
    if ( stream_node != "" )   {
      StreamTaskMgr_addTreeNode(stream_node, node+"_Config", "StreamConfigurator", 0);
      fwFsmTree_setNodeLabel(node+"_Config","Configurator");
      for(int i=1; i<=dynlen(sendNodes); ++i)  {
        name = node+"_"+sendNodes[i];
        StreamTaskMgr_createNode(stream_node,name,"FSM_Tasks",num_monTasks);
        fwFsmTree_setNodeLabel(name,sendNodes[i]);
      }
      for(int i=1; i<=dynlen(recvNodes); ++i)  {
        name = node+"_"+recvNodes[i];
        StreamTaskMgr_createNode(stream_node,name,"FSM_Tasks",num_recvClass0+num_recvClass1);
        fwFsmTree_setNodeLabel(name,recvNodes[i]);
      }
      for(int i=1; i<=dynlen(strmNodes); ++i)  {
        name = node+"_"+strmNodes[i];
        StreamTaskMgr_createNode(stream_node,name,"FSM_Tasks",num_strmClass0+num_strmClass1);
        fwFsmTree_setNodeLabel(name,strmNodes[i]);
      }
    }    
    if ( refresh ) fwFsmTree_refreshTree();
    DebugN("All Done.");
  }
  StreamTaskMgr_checkErrors(res);
  return 1;
}
//=============================================================================
int StreamTaskMgr_createAllTree(string stream,
                                int num_recvClass0,
                                int num_recvClass1,
                                int num_strmClass0,
                                int num_strmClass1,
                                string monitoringInput,
                                int num_monTasks)
{
  for(int i=0; i<16; ++i)  {    
    string node;
    sprintf(node,"Slice%02X",i);
    StreamTaskMgr_createTree(stream,node,
                             num_recvClass0,num_recvClass1,
                             num_strmClass0,num_strmClass1,
                             monitoringInput,num_monTasks,0);
  }
  fwFsmTree_refreshTree();
  DebugN("All Done.");
}
//=============================================================================
int StreamTaskMgr_deleteTree(string stream, string slice_name, int refresh=1)  {
  string node = stream+"_"+slice_name;
  DebugN("Delete tree:"+node);
  fwFsmTree_removeNode("FSM",node,1);
  dyn_string tasks = dpNames(node+"*","FSM_DimTask");
  for(int i=1, n=dynlen(tasks); i<=n; ++i)  {
    if ( dpExists(tasks[i]) ) dpDelete(tasks[i]);
  }
  if ( refresh ) fwFsmTree_refreshTree();
  DebugN("All Done...Deleted tree and "+dynlen(tasks)+" Tasks.");
}
//=============================================================================
int StreamTaskMgr_deleteAllTree(string stream)  {
  for(int i=0; i<16; ++i)  {    
    string node;
    sprintf(node,"Slice%02X",i);
    StreamTaskMgr_deleteTree(stream,node,0);
  }
  fwFsmTree_refreshTree();
  DebugN("All Done.");
}
//=============================================================================
int StreamTaskMgr_genFSM(string stream, string slice_name)
{ return StreamTaskMgr_generateFSM(stream+"_"+slice_name);   }
//=============================================================================
int StreamTaskMgr_genAllFSM(string stream)  {
  fwFsmTree_generateAll();
  DebugN("All Done.");
}
//=============================================================================
int StreamTaskMgr_startTree(string stream, string slice_name)  
{ return StreamTaskMgr_startFSMNode(stream+"_"+slice_name);  }
//=============================================================================
int StreamTaskMgr_startAllTree(string stream)  {
  fwFsmTree_startTree();
  DebugN("All Done.");
}
//=============================================================================
int StreamTaskMgr_stopTree(string stream, string slice_name)
{ return StreamTaskMgr_stopFSMNode(stream+"_"+slice_name);  }
//=============================================================================
int StreamTaskMgr_stopAllTree(string stream)  {
  fwFsmTree_stopTree();
  DebugN("All Done.");
}
//=============================================================================
void StreamTaskMgr_clearTasks(string slice)  {
  dyn_string  dp_names;
  dyn_anytype dp_values;
  dyn_string dps = dpNames(slice+"_*","FSM_DimTask");
  for(int j=1, n=dynlen(dps); j<=n; ++j)  {
    string n = dps[j];
    DebugN("Disable tree node:"+n);
    fwCU_disableObj(n,n,1);
    fwFsmTree_setNodeLabel(n,"Disabled");
    dynAppend(dp_names,n+".Name");
    dynAppend(dp_values,(string)"NONE");
    dynAppend(dp_names,n+".Class");
    dynAppend(dp_values,(int)-1);
    dynAppend(dp_names,n+".InUse");
    dynAppend(dp_values,(int)0);
    dynAppend(dp_names,n+".Node");
    dynAppend(dp_values,(string)"NONE");
    dynAppend(dp_names,n+".Priority");
    dynAppend(dp_values,(int)0);
    dynAppend(dp_names,n+".State");
    dynAppend(dp_values,(string)"UNKNOWN");
    dynAppend(dp_names,n+".Type");
    dynAppend(dp_values,(string)"NONE");
    dynAppend(dp_names,n+".Command");
    dynAppend(dp_values,(string)"");
  }
  dpSet(dp_names, dp_values);
}
//=============================================================================
void StreamTaskMgr_setupTasks(string slice,dyn_string& tasks, int class, 
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
