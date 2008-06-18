#uses "ctrlUtils.cpp"

ProcessorFarm_installAllocator(string stream, bool refresh=0)  {
  string stream_node = ctrlUtils_addFsmTreeNode("FSM", "Data"+stream, "StreamCluster", 1);
  string stream_alloc = stream+"Alloc";
  if ( !dpExists(stream_alloc) )  {
    dpCreate(stream_alloc,"StreamControl");
  }
  string pref = stream;
  string st = ctrlUtils_addFsmTreeNode(stream_node,stream_alloc,"StreamControl",0);
  fwFsmTree_setNodePanel("Data"+stream,"ProcessorFarm/AllocationControl.pnl");
  fwFsmTree_setNodePanel(stream_alloc,"ProcessorFarm/AllocationControl.pnl");
  fwFsmTree_refreshTree();
}
//=============================================================================
int ProcessorFarm_uninstallStream(string stream)  {
  DebugN("Delete tree:"+"Data"+stream);
  fwFsmTree_removeNode("FSM","Data"+stream,1);
  fwFsmTree_refreshTree();
  DebugN("All Done.");
  return 1;
}

ProcessorFarmInstallation_createFarm(string farmName,int numFarm, bool refresh=0)  {
  dyn_string subfarms = dpNames("*:"+farmName+"_*","FarmSubInfo");
  string panel = "ProcessorFarm/FarmConfigurator.pnl";
  string name, node, dev;
  sprintf(name,"%s_Farm%02X",farmName,numFarm);
  node = ctrlUtils_addFsmTreeNode("FSM", name, "FSM_Farm", 1);
  DebugN(name,node);
  fwFsmTree_setNodePanel(node,panel);
/*
  dev = node+"_Config";
  ctrlUtils_addFsmTreeNode(node, dev, "StreamConfigurator", 0);
  fwFsmTree_setNodeLabel(dev,"Configurator");
  fwFsmTree_setNodePanel(dev,panel);
  if ( !dpExists(dev) )  {
    dpCreate(dev,"StreamConfigurator");
  }  
  dpSet(dev+".State","NOT_READY");
*/
  for(int i=1; i<=dynlen(subfarms); ++i)  {
    dyn_string items = strsplit(subfarms[i],":");
    name = subfarms[i]+"::"+items[2];
    fwFsmTree_addNode(node, name, "FSM_Slice", 0);
  }
  if ( refresh ) fwFsmTree_refreshTree();
  DebugN("All Done.");
}

ProcessorFarmInstallation_createSlice(string farmName,bool refresh=0)  {
  string stream = m_streamName.text;
  int    numSlice = m_numSlices.text;
  if ( !m_useSlicing.state(0) ) numSlice = -1; 
  if ( m_farmLoop.state(0) ) {
    dyn_string subfarms = m_sliceName.items();
    DebugN("Subfarms to create:",subfarms);
    for(int i=1; i<=dynlen(subfarms)-1; ++i)
      ProcessorFarm_createTree(stream,subfarms[i],numSlice,m_numTasksPerNode.text,0);
    farmName = subfarms[dynlen(subfarms)];
    DebugN("Subfarms to create:",subfarms);
  }
  ProcessorFarm_createTree(stream,farmName,numSlice,m_numTasksPerNode.text,refresh);
}

ProcessorFarmInstallation_createIOSlice(string slice, bool refresh=0)  {
  string stream = m_streamName.text;
  int num_storageSlices = m_numStorageSlices.text;
  DebugN("Creating IO Slice "+slice+" with "+num_storageSlices+" slices.");
  StreamTaskMgr_createTree(stream+"IO",slice,num_storageSlices,num_storageSlices,"",refresh,0);
}

void ProcessorFarmInstallation_AboutBox()  {
    string text = "Processor farm setup:\n\n"+
                  "Smart utilities for data streaming applications\n"+
                  "in the LHCb experiment.\n"+
                  "\n\n"+
                  "Copyright by CERN/LHCb";
    ChildPanelOnModal("visionUtils/AboutBox",
                      "About",
                      makeDynString(text),50,50);
}

//=============================================================================
int ProcessorFarm_createTree(string stream,string farm,int numSlice,int numTasks,bool refresh=0)  {
  dyn_string sets, set, names;
  string name;
  string panel = "ProcessorFarm/ActivityEditor.pnl";
  DebugN(stream+" Farm:"+farm+" No.Slices:"+numSlice+" No.Tasks per Node:"+numTasks);
  names = dpNames(stream+"_"+farm+".Nodes","FarmSubInfo");
  dynAppend(sets,farm);
  DebugN("Subfarms: ["+stream+"_"+farm+".Nodes] "+names);
  for(int i=1, n=dynlen(names); i<=n; ++i)  {
    dpGet(names[i],set);
    for(int j=1, m=dynlen(set); j<=m; ++j)
      dynAppend(sets,strsplit(set[j],'/')[1]);
  }
  DebugN(sets);
  if ( numSlice > 0 )  {
    for(int k=0; k<numSlice; ++k)  {
      sprintf(name,"Slice%02X",k);
      ctrlUtils_createFsmTaskTree(stream,name,sets,numTasks,refresh,1,1);
      fwFsmTree_setNodePanel(stream+'_'+farm+"_"+name,panel);
      fwFsmTree_setNodePanel(stream+'_'+farm+"_"+name+"_Config",panel);
    }
  }
  else  {
    name = stream;
    ctrlUtils_createFsmTaskTree(name,farm,sets,numTasks,refresh,1,0);
    fwFsmTree_setNodePanel(name+'_'+farm,panel);
    fwFsmTree_setNodePanel(name+'_'+farm+"_Config",panel);
  }
}
//=============================================================================
int ProcessorFarm_deleteTree(string stream, string slice_name, int refresh=1)  {
  dyn_string subfarms;
  if ( m_farmLoop.state(0) ) 
    dynAppend(subfarms,m_sliceName.items());
  else
    dynAppend(subfarms,slice_name);
  for(int j=1; j<=dynlen(subfarms); ++j)  {
    string node = stream+"_"+subfarms[j];
    DebugN("Delete tree:"+node);
    fwFsmTree_removeNode("FSM",node,1);
    dyn_string tasks = dpNames(node+"*","FSM_DimTask");
    for(int i=1, n=dynlen(tasks); i<=n; ++i)  {
      if ( dpExists(tasks[i]) ) dpDelete(tasks[i]);
    }
    DebugN("All Done...Deleted tree and "+dynlen(tasks)+" Tasks.");
  }
  if ( refresh ) ctrlUtils_refreshDEN();
  DebugN("All Done.");
}
//=============================================================================
int ProcessorFarm_installDataTypes()  {
  dyn_dyn_string names;
  dyn_dyn_int types;
  names[1]  = makeDynString ("FarmInfo","","","");
  names[2]  = makeDynString ("","Name","","");
  names[3]  = makeDynString ("","SubFarms","","");
  names[4]  = makeDynString ("","InUse","","");
  names[5]  = makeDynString ("","Command","","");
  names[6]  = makeDynString ("","State","","");
  types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
  types[2]  = makeDynInt (0,DPEL_STRING,0,0);
  types[3]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
  types[4]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
  types[5]  = makeDynInt (0,DPEL_STRING,0,0);
  types[6]  = makeDynInt (0,DPEL_STRING,0,0);
  ctrlUtils_installDataType(names,types);
  names[1]  = makeDynString ("FarmSubInfo","","","");
  names[2]  = makeDynString ("","Name","","");
  names[3]  = makeDynString ("","Nodes","","");
  names[4]  = makeDynString ("","UsedBy","","");
  names[5]  = makeDynString ("","Activity","","");
  names[6]  = makeDynString ("","RunInfo","","");
  types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
  types[2]  = makeDynInt (0,DPEL_STRING,0,0);
  types[3]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
  types[4]  = makeDynInt (0,DPEL_STRING,0,0);
  types[5]  = makeDynInt (0,DPEL_STRING,0,0);
  types[6]  = makeDynInt (0,DPEL_STRING,0,0);
  ctrlUtils_installDataType(names,types);
  names[1]  = makeDynString ("FarmSlice","","","");
  names[2]  = makeDynString ("","InUse","","");
  names[3]  = makeDynString ("","FSMSlice","","");
  names[4]  = makeDynString ("","RunInfo","","");
  types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
  types[2]  = makeDynInt (0,DPEL_INT,0,0);
  types[3]  = makeDynInt (0,DPEL_STRING,0,0);
  types[4]  = makeDynInt (0,DPEL_STRING,0,0);
  ctrlUtils_installDataType(names,types);
  names[1]  = makeDynString ("FarmActivity","","","");
  names[2]  = makeDynString ("","Name","","");
  names[3]  = makeDynString ("","Farm","","");
  names[4]  = makeDynString ("","","Infrastructure","");
  names[5]  = makeDynString ("","","Worker","");
  names[6]  = makeDynString ("","Storage","","");
  names[7]  = makeDynString ("","","storeFlag","");
  names[8]  = makeDynString ("","","recvInfrastructure","");
  names[9]  = makeDynString ("","","recvTypes","");
  names[10] = makeDynString ("","","recvMultiplicity","");
  names[11] = makeDynString ("","","recvStrategy","");
  names[12] = makeDynString ("","","streamInfrastructure","");
  names[13] = makeDynString ("","","streamTypes","");
  names[14] = makeDynString ("","","streamMultiplicity","");
  names[15] = makeDynString ("","","strmStrategy","");
  names[16] = makeDynString ("","","dataDirectory","");
  names[17] = makeDynString ("","Control","","");
  names[18] = makeDynString ("","","Infrastructure","");
  types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
  types[2]  = makeDynInt (0,DPEL_STRING,0,0);
  types[3]  = makeDynInt (0,DPEL_STRUCT,0,0);
  types[4]  = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[5]  = makeDynInt (0,0,DPEL_STRING,0);
  types[6]  = makeDynInt (0,DPEL_STRUCT,0,0);
  types[7]  = makeDynInt (0,0,DPEL_INT,0);
  types[8]  = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[9]  = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[10] = makeDynInt (0,0,DPEL_DYN_INT,0);
  types[11] = makeDynInt (0,0,DPEL_INT,0);
  types[12] = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[13] = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[14] = makeDynInt (0,0,DPEL_DYN_INT,0);
  types[15] = makeDynInt (0,0,DPEL_INT,0);
  types[16] = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[17] = makeDynInt (0,DPEL_STRUCT,0,0);
  types[18] = makeDynInt (0,0,DPEL_DYN_STRING,0);
  ctrlUtils_installDataType(names,types);
  names[1]  = makeDynString ("FarmRunInfo","","","");
  names[2]  = makeDynString ("","general","","");
  names[3]  = makeDynString ("","","partName","");
  names[4]  = makeDynString ("","","partId","");
  names[5]  = makeDynString ("","","activePartId","");
  names[6]  = makeDynString ("","","runType","");
  names[7]  = makeDynString ("","HLTFarm","","");
  names[8]  = makeDynString ("","","nSubFarms","");
  names[9]  = makeDynString ("","","subFarms","");
  names[10] = makeDynString ("","","nodeList","");
  names[11] = makeDynString ("","Storage","","");
  names[12] = makeDynString ("","","storeFlag","");
  names[13] = makeDynString ("","","dataDirectory","");
  names[14] = makeDynString ("","","fileName","");
  names[15] = makeDynString ("","","recvStrategy","");
  names[16] = makeDynString ("","","recvInfraStructure","");
  names[17] = makeDynString ("","","strmStrategy","");
  names[18] = makeDynString ("","","streamInfrastructure","");
  names[19] = makeDynString ("","","streamMultiplicity","");
  names[20] = makeDynString ("","","streamTypes","");
  types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
  types[2]  = makeDynInt (0,DPEL_STRUCT,0,0);
  types[3]  = makeDynInt (0,0,DPEL_STRING,0,0);
  types[4]  = makeDynInt (0,0,DPEL_INT,0,0);
  types[5]  = makeDynInt (0,0,DPEL_INT,0,0);
  types[6]  = makeDynInt (0,0,DPEL_STRING,0,0);
  types[7]  = makeDynInt (0,DPEL_STRUCT,0,0);
  types[8]  = makeDynInt (0,0,DPEL_INT,0);
  types[9]  = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[10] = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[11] = makeDynInt (0,DPEL_STRUCT,0,0);
  types[12] = makeDynInt (0,0,DPEL_INT,0);
  types[13] = makeDynInt (0,0,DPEL_STRING,0);
  types[14] = makeDynInt (0,0,DPEL_STRING,0);
  types[15] = makeDynInt (0,0,DPEL_INT,0);
  types[16] = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[17] = makeDynInt (0,0,DPEL_INT,0);
  types[18] = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[19] = makeDynInt (0,0,DPEL_DYN_INT,0);
  types[20] = makeDynInt (0,0,DPEL_DYN_STRING,0);
  ctrlUtils_installDataType(names,types);
  return 1;
}
//=============================================================================
int ProcessorFarm_uninstallDataTypes()  {
  ctrlUtils_uninstallDataType("FarmInfo");
  ctrlUtils_uninstallDataType("FarmSubInfo");
  ctrlUtils_uninstallDataType("FarmAllocator");
  ctrlUtils_uninstallDataType("FarmSlice");
  ctrlUtils_uninstallDataType("FarmActivity");
  ctrlUtils_uninstallDataType("FarmRunInfo");
  return 1;
}
//=============================================================================
int ProcessorFarm_uninstallStreamTypes()  {
  ctrlUtils_uninstallDataType("FSM_DimTask");
  ctrlUtils_uninstallDataType("StreamConfigurator");
  ctrlUtils_uninstallDataType("StreamControl");
  ctrlUtils_uninstallDataType("StreamPartition");
  ctrlUtils_uninstallDataType("StreamTaskCreator");
  return 1;
}
//=============================================================================
//
//  ALLOCATION DISPLAY
//
//=============================================================================
void ProcessorFarmAlloc_CheckAllocPanel(string dp, string value)  {
  string partition;
  m_runInfoDP.text = substr(dp,0,strpos(dp,".Activity"));
  if ( dpExists(value) )  {
    if ( 0 == dpGet(m_runInfoDP.text+".UsedBy",partition) )  {
      if ( strlen(partition)>0 )  {
        m_runInfoDP.text = value;
        m_partition.text = partition;
        m_partitionName.text = "Partition:"+partition;
        ProcessorFarmAlloc_initAllocPanel(m_runInfoDP.text);
        return;
      }
    }
  }
  DebugN("ProcessorFarmAlloc_CheckAllocPanel>",dp,value);
  m_partition.text = "";
  m_farmNotInUse.text="The farm\n"+m_farmName.text+" in\n"+dp+"\nis currently not used.";
  LayerOff(2);
  LayerOn(3);
}

//=============================================================================
void ProcessorFarmAlloc_startAllocPanel(string farm, string partition)  {
  dyn_string items = dpNames("*:"+farm,"FarmSubInfo");
  string info, sys = strtoupper(partition), sysId = getSystemId(sys+":");
  if ( dynlen(items)>0 )  {
    info = items[1];
  }
  else if ( getSystemId(sys+":") > 0 )  {
    info = sys+":"+farm;
  }
  else if ( getSystemId(sys+":") < 0 )  {
    sys = getSystemName();
    sys = substr(sys,0,strlen(sys)-1);
    info = sys+":"+farm;
  }
  DebugN("ProcessorFarmAlloc_startAllocPanel> DP is:"+info);
  m_Ok.visible = 0;
  m_Cancel.visible = 0;
  m_farmName.text = farm;
  m_runInfoDP.text = info;
  if ( dpExists(info+".Activity") )  {
    dpConnect("ProcessorFarmAlloc_CheckAllocPanel",info+".Activity");
    return;
  }
  m_farmNotInUse.text="The farm \n"+farm+"\nin system "+sys+"\nis currently not used.";
  LayerOff(2);
  LayerOn(3);
}
void ProcessorFarmAlloc_initStandalone(string rundp)  {
  ProcessorFarmAlloc_initAllocPanel(rundp);
  setValue("m_Ok","visible",0);
  setValue("m_Cancel","visible",0);
  setValue("m_farmName","visible",0);
  setValue("m_partition","visible",0);
  setValue("m_partitionName","visible",0);
}
//=============================================================================
void ProcessorFarmAlloc_initAllocPanel(string rundp)  {
  string font = "Arial,8,-1,5,50,0,0,0,0,0";
  ProcessorFarmAlloc_setupAllocWidgets();
  m_runInfoDP.text          = rundp;
  m_runInfoDP.backCol       = "_Transparent";
  m_farmName.visible        = 1;
  m_farmName.backCol        = "yellow";
  m_partition.visible       = 1;
  m_partition.backCol       = "yellow";
  m_partitionName.visible   = 1;
  m_farmNotInUse.foreCol    = "red";
  m_farmNotInUse.backCol    = "_Transparent";
  m_ctrlInfra.font          = font;
  m_farmInfra.font          = font;
  m_farmWorker.font         = font;
  m_newDpName.font          = font;
  m_newDpName.visible       = 0;
  m_newDpNameText.visible   = 0;
  m_farmWorkerText.visible  = 1;
  m_newDpNameText.text      = "Enter new activity name:\nThen press save to fill values.";
  m_ctrlInfraText.text      = "Controls processes      \nForm: Type/Class:\n[Example: Adder/Class1]";
  m_farmInfraText.text      = "Infrastructure processes\nForm: Type/Class:\n[Example: MBM/Class0]";
  m_farmWorkerText.text     = "Farm Worker process     \nForm: Type/Class:\n[Example: Brunel/Class1]";
  ProcessorFarmAlloc_initAllocData(rundp);
  LayerOff(3);
  LayerOn(2);
}
//=============================================================================
void ProcessorFarmAlloc_setupAllocWidgets()  {
  m_ctrlFrame.toolTipText         = "Input parameters define tasks in the Controls PC.";
  m_ctrlInfraText.toolTipText     = "Task types to be started in the Controls PC.";
  m_ctrlInfra.toolTipText         = m_ctrlInfraText.toolTipText;
  m_farmFrame.toolTipText         = "Input parameters to start tasks on the farm nodes.";
  m_farmInfraText.toolTipText     = "Task types to be started on the farm node infrastructure.";
  m_farmInfra.toolTipText= m_farmInfraText.toolTipText;
  m_farmWorkerText.toolTipText    = "Farm Worker process type. Form: Type/Class";
  m_farmWorker.toolTipText        = m_farmWorkerText.toolTipText;
  m_Reset.toolTipText             = "Discard changes and reset to original values";
  m_Cancel.toolTipText            = "Discard changes and close panel";
  m_Save.toolTipText              = "Save changes to datapoints but keep panel open.";
  m_Ok.toolTipText                = "Save changes to datapoints and close panel.";
}
//=============================================================================
void ProcessorFarmAlloc_initAllocData(string rundp)  {
  string worker_type, dp = rundp;
  dyn_int strmMult;
  dyn_string farmInfra, ctrlInfra, strmTypes, streams;
  int res = -1;
  if ( dpExists(dp) )  {
    res = dpGet(dp+".Control.Infrastructure",ctrlInfra,
                dp+".Farm.Infrastructure",farmInfra,
                dp+".Farm.Worker",worker_type);
  }
  if ( 0 != res )  {
    ctrlUtils_checkErrors(res);
    return;
  }
  m_farmWorker.text = worker_type;
  string s= "";
  for(int i=1, n=dynlen(farmInfra); i<=n; ++i)  {
    s = s + farmInfra[i];
    if ( i < n ) s = s+ "\n";
  }
  m_farmInfra.text = s;
  s = "";
  for(int i=1, n=dynlen(ctrlInfra); i<=n; ++i)  {
    s = s + ctrlInfra[i];
    if ( i < n ) s = s+ "\n";
  }
  m_ctrlInfra.text = s;
}
//=============================================================================
void ProcessorFarmAlloc_showJobOptions()  {
  ChildPanelOn("JobOptions/JobOptions","JobOptions",makeDynString(),100,100);
}
//=============================================================================
int ProcessorFarmAlloc_Save()  {
  string newDp = m_newDpName.text;
  m_newDpName.text = "";
  m_newDpName.visible = 0;
  m_newDpNameText.visible = 0;  
  if ( ctrlUtils_confirm("Are you sure you want to save the parameters ?") )   {
    string dp = m_runInfoDP.text, farmWorker = m_farmWorker.text, s1 = m_ctrlInfra.text, s2 = m_farmInfra.text;
    strreplace(s1,"\n"," ");
    while ( strpos(s1,"  ")>0 ) strreplace(s1,"  "," ");
    strreplace(s2,"\n"," ");
    while ( strpos(s2,"  ")>0 ) strreplace(s2,"  "," ");
    dyn_string ctrlInfra = strsplit(s1," ");
    dyn_string farmInfra = strsplit(s2," ");

    if ( ctrlUtils_checkTasks(ctrlInfra,2) &&
         ctrlUtils_checkTasks(farmInfra,2) &&
         ctrlUtils_checkTask(farmWorker,2) &&
         dpExists(dp) )   {
      int res = 0;
      if ( strlen(newDp)>0 )  {
        dp = "FarmActivity_"+newDp;
        if ( !dpExists(dp) ) res = dpCreate(dp,"FarmActivity");
      }
      res = dpSet(dp+".Control.Infrastructure",ctrlInfra,
                  dp+".Farm.Infrastructure",farmInfra,
                  dp+".Farm.Worker",farmWorker);
      ctrlUtils_checkErrors(res);
      ProcessorFarmAlloc_initAllocData(m_runInfoDP.text);
      return res;
    }
  }
  return 0;
}
//=============================================================================
int ProcessorFarmAlloc_Ok()  {
  int res = ProcessorFarmAlloc_Save();  
  if ( res == 0 )  {
    PanelOff();
  }
}
//=============================================================================
void ProcessorFarmAlloc_Cancel()  {
  ProcessorFarmAlloc_initAllocData(m_runInfoDP.text);
  PanelOff();
}
//=============================================================================
void ProcessorFarmAlloc_Reset()  {
  ProcessorFarmAlloc_initAllocData(m_runInfoDP.text);
}
//=============================================================================
//
//   Run Information editor
// 
//=============================================================================
void ProcessorFarmRunInfo_openPanel(string name,int offx=5, int offy=35)  {
  string activity_name = dpSubStr(name,DPSUB_DP);
  DebugN("openRunInfoPanel> Building panel using:"+name);
  ChildPanelOn("ProcessorFarm/RunInfoEditor","RunInfoEditor",makeDynString("$1:NONE","$2:"+name),100,100);
}
void ProcessorFarmRunInfo_openMultiPanel(string name,int offx=5, int offy=35)  {
  string activity_name = dpSubStr(name,DPSUB_DP);
  DebugN("openRunInfoPanel> Building panel using:"+name);
  ChildPanelOn("ProcessorFarm/MultiRunInfoEditor","MultiRunInfoEditor",makeDynString("$1:NONE","$2:"+name),100,100);
}
//=============================================================================
void ProcessorFarmRunInfo_createPanel(string name,int offx=5, int offy=35)  {
  string activity_name = dpSubStr(name,DPSUB_DP);
  DebugN("createRunInfoPanel> Building panel using:"+name);
  addSymbol(myModuleName(),myPanelName(),"ProcessorFarm/RunInfoEditor.pnl","m_runInfoPanel",
            makeDynString("$1:NONE","$2:"+name),offx,offy,0,0,0);
}
//=============================================================================
void ProcessorFarmRunInfo_init(string runinfo)  {
  setValue("m_Ok","visible",1);
  setValue("m_Ok","text","Quit");
  m_runInfoDP.text             = runinfo;
  m_runInfoDP.toolTipText      = "Current run information datapoint.";
  m_runInfoDP.backCol          = "_Transparent";
  m_numSubFarmText.backCol     = "_Transparent";
  m_numSubFarmText.text        = "Number of subfarms:";
  m_numSubFarmText.toolTipText = "Number of subfarms to be used by this partition.";
  m_numSubFarm.toolTipText     = m_numSubFarmText.toolTipText;
  m_partNameText.backCol       = "_Transparent";
  m_partNameText.text          = "Partition name:";
  m_partNameText.toolTipText   = "Partition identifier of this run information datapoint.";
  m_partName.toolTipText       = m_partNameText.toolTipText;
  m_runTypeText.backCol        = "_Transparent";
  m_runTypeText.text           = "Run type:";
  m_runTypeText.toolTipText    = "Run type name of this run information datapoint.";
  m_runType.toolTipText        = m_runTypeText.toolTipText;
  m_newDpName.visible          = 0;
  m_newDpNameText.visible      = 0;
  m_newDpNameText.backCol      = "_Transparent";
  m_newDpNameText.text         = "Enter new datapoint name:\nThen press Save.";
  m_newDpNameText.toolTipText  = "Enter here the name of the new datapoint. Then press Save to store.";
  m_newDpName.toolTipText      = m_newDpNameText.toolTipText;
  ProcessorFarmRunInfo_load(runinfo);
}
void ProcessorFarmRunInfo_load(string runinfo)  {
  string runtype, partname;
  int partid, nsubfarm;
  if ( dpExists(runinfo) )  {
    int res = dpGet(runinfo+".HLTFarm.nSubFarms",nsubfarm,
                    runinfo+".general.partName",partname,
                    runinfo+".general.activePartId",partid,
                    runinfo+".general.runType",runtype);
    if ( 0 == res )  {
      m_partIdText.text = "Partition ID:"+partid;
      m_partName.text = partname;
      m_numSubFarm.text = nsubfarm;
      m_runType.text = runtype;
      LayerOn(2);
      return;
    }        
    ctrlUtils_checkErrors(res);
    return;
  }
  LayerOff(2);
  m_runInfoDP.text = "";
  ChildPanelOn("visionUtils/ErrorBox","Error occurred",
               makeDynString("$1:"+"The datapoint\n"+runinfo+"\n does not exist!"),50,50);
}
//=============================================================================
int ProcessorFarmRunInfo_Save()  {
  string newDp = m_newDpName.text;
  m_newDpName.text = "";
  m_newDpName.visible = 0;
  m_newDpNameText.visible = 0;  
  int res = 0;
  if ( ctrlUtils_confirm("Are you sure you want to save the parameters ?") )   {
    string runinfo = m_runInfoDP.text;
    string runtype = m_runType.text, partname = m_partName.text;
    int nsubfarm = m_numSubFarm.text;
    if ( dpExists(runinfo) )   {
      if ( strlen(newDp)>0 )  {
        runinfo = newDp;
        if ( !dpExists(runinfo) ) res = dpCreate(runinfo,"FarmRunInfo");
      }
      if ( 0 == res )  {
        res = dpSet(runinfo+".HLTFarm.nSubFarms",nsubfarm,
                    runinfo+".general.partName",partname,
                    runinfo+".general.runType",runtype);
        ctrlUtils_checkErrors(res);
        ProcessorFarmRunInfo_load(m_runInfoDP.text);
        return res;
      }
      ctrlUtils_checkErrors(res);
      return;
    }
    ChildPanelOn("visionUtils/ErrorBox","Error occurred",
                 makeDynString("$1:"+"The datapoint\n"+runinfo+"\n does not exist!"),50,50);
  }
  return res;
}
//=============================================================================
int ProcessorFarmRunInfo_Delete()  {
  int res = 0;
  if ( ctrlUtils_confirm("Are you sure you want to delete the parameters ?") )   {
    string runinfo = m_runInfoDP.text;
    if ( dpExists(runinfo) )   {
      int res = dpDelete(runinfo);
      m_runInfoDP.text = "";
      ctrlUtils_checkErrors(res);
      LayerOff(2);
      return;
    }
    ChildPanelOn("visionUtils/ErrorBox","Error occurred",
                 makeDynString("$1:"+"The datapoint\n"+runinfo+"\n does not exist!"),50,50);
  }
  return res;
}
//=============================================================================
int ProcessorFarmRunInfo_Ok()  {
  int res = ProcessorFarmRunInfo_Save();  
  if ( res == 0 )  {
    PanelOff();
  }
}
//=============================================================================
void ProcessorFarmRunInfo_Cancel()  {
  ProcessorFarmRunInfo_load(m_runInfoDP.text);
  PanelOff();
}
//=============================================================================
void ProcessorFarmRunInfo_Reset()  {
  ProcessorFarmRunInfo_load(m_runInfoDP.text);
}
//=============================================================================
//
//   Activity editor
// 
//=============================================================================
void  ProcessorFarm_setupWidgets()  {
  m_ioFrame.toolTipText                = "Section to fill I/O parameters for data transfer to/from storage.";
  m_storageReaderText.toolTipText      = "Enter here the storage reader task type.";
  m_storageReader.toolTipText          = m_storageReaderText.toolTipText;
  m_storageWriterText.toolTipText      = "Enter here the storage writer task type.";
  m_storageWriter.toolTipText          = m_storageWriterText.toolTipText;
  m_storageSenderText.toolTipText      = "Enter here the storage sender task type.";
  m_storageSender.toolTipText          = m_storageSenderText.toolTipText;
  m_storageReceiverText.toolTipText    = "Enter here the storage receiver task type.";
  m_storageReceiver.toolTipText        = m_storageReceiverText.toolTipText;
  m_farmSenderText.toolTipText         = "Enter here the farm sender task type.";
  m_farmSender.toolTipText             = m_farmSenderText.toolTipText;
  m_farmReceiverText.toolTipText       = "Enter here the farm receiver task type.";
  m_farmReceiver.toolTipText           = m_farmReceiverText.toolTipText;
  m_farmInfrastructureText.toolTipText = "Enter here the infrastructure task types to execute on each node.";
  m_farmInfrastructure.toolTipText     = m_farmInfrastructureText.toolTipText;
  m_farmTasksText.toolTipText          = "Enter here the task types to execute on each farm node.";
  m_farmTasks.toolTipText              = m_farmTasksText.toolTipText;
  m_activityName.toolTipText           = "Currently selected activity";
  m_Reset.toolTipText                  = "Discard changes and reset to original values";
  m_Cancel.toolTipText                 = "Discard changes and close panel";
  m_Save.toolTipText                   = "Save changes to datapoints but keep panel open.";
  m_Ok.toolTipText                     = "Save changes to datapoints and close panel.";
}

int ProcessorFarm_initEditor(string activity)  {
  ProcessorFarm_setupWidgets();
  m_activityName.text = activity;
  return ProcessorFarm_initData(activity);
}

int ProcessorFarm_initData(string activity)  {
  string recvType, sendType, dp = "FarmActivity_"+activity;
  dyn_string infra, tasks;
  int res;
  res = dpGet(dp+".Infrastructure",infra,
              dp+".TaskTypes",tasks,
              dp+".ReceiverType",recvType,
              dp+".SenderType",sendType);
  if ( 0 == res )  {
     string txt;
     m_farmReceiver.text = recvType;
     m_farmSender.text = sendType;
     txt = "";
     for(int i=1; i<=dynlen(infra); ++i) 
       txt = txt + infra[i] + "\n";
     m_farmInfrastructure.text = substr(txt,0,strlen(txt)-1);
     txt = "";
     for(int i=1; i<=dynlen(tasks); ++i) 
       txt = txt + tasks[i] + "\n";
     m_farmTasks.text = substr(txt,0,strlen(txt)-1);
    return 1;
  }
  ctrlUtils_checkErrors(res,"Failed to access datapoint "+dp);     
  return 0;   
}

int ProcessorFarm_editorCancel(string activity)  {
  return 1;
}

int ProcessorFarm_editorReset(string activity)  {
  return ProcessorFarm_initData(activity);
}

int ProcessorFarm_editorSave(string activity)  {
  return 1;
}

int ProcessorFarm_editorOk(string activity)  {
  ProcessorFarm_editorSave(activity);
  return 1;
}
