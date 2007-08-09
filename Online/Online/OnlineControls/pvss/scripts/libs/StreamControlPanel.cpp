//=============================================================================
bool StreamControlPanel_checkErrors(int res)  {
  if ( res != 0 || dynlen(getLastError()) > 0 )  {
    string err = " Err="+res+","+getLastError();
    DebugN(" Err="+res+","+getLastError());
    ChildPanelOn("vision/MessageInfo1","Error occurred",makeDynString(err),50,50);
  }
  return res == 0;
}
//=============================================================================
void StreamControlPanel_setListItems(string shape_name, dyn_string values)  {
  shape s = getShape(shape_name);
  s.deleteAllItems();
  for(int k=1; k<=dynlen(values); k++) 
    s.appendItem(values[k]);
}
//=============================================================================
dyn_string StreamControlPanel_getStrings(string dp, string type)  {
  dyn_string items = dpNames(dp,type);
  dyn_string values, result;
  int res = dpGet(items,values);
  if ( StreamControlPanel_checkErrors(res) )  {
    for(int i=1; i<=dynlen(values); i++) {
      if ( strlen(values[i])>0 ) dynAppend(result, values[i]);
    }
  }
  dynSortAsc(values);
  return result;
}
//=============================================================================
dyn_string StreamControlPanel_getStringVectors(string dp,string type)  {
  dyn_dyn_string slots;
  dyn_string values, items = dpNames(dp,type);
  int res = dpGet(items,slots);
  if ( StreamControlPanel_checkErrors(res) )  {
    for(int i=1; i<=dynlen(slots); i++) {
      for(int j=1; j<=dynlen(slots[i]); j++) {
        dynAppend(values,slots[i][j]);
      }
    }
  }
  dynSortAsc(values);
  return values;
}
//=============================================================================
int StreamControlPanel_showPanel(string stream,string partition,string ref,string panel,int offsety=130,int offsetx=0)  {
  //DebugN("StreamControlPanel_showPanel:",stream,partition);
  dyn_string params = makeDynString("$partition:"+partition,"$stream:"+stream,"$offset:"+offsety);
  //DebugN("StreamControlPanel_showPanel:",myModuleName(), myPanelName(),"Offset=",offsety,params);
  int res = addSymbol(myModuleName(), myPanelName(), panel, ref, params, offsetx, offsety, 0, 1., 1.);
  return StreamControlPanel_checkErrors(res);
}
//=============================================================================
void StreamControlPanel_hidePanel(string ref)  {
  removeSymbol(myModuleName(), myPanelName(), ref);
}
//=============================================================================
//
//  Node/partition summary display
//
//=============================================================================
void StreamControlPanel_addTasks(dyn_string tasks,string node,string& val)  {
  for(int i=1, n=dynlen(tasks); i<=n; ++i)  {
    dyn_string itms = strsplit(tasks[i],"/");
    if ( itms[1] == node )
      val = val + itms[1] + "::" + itms[2]+"  [" + itms[3] + "]\n";
  }
}
//=============================================================================
void StreamControlPanel_initXXXX(string stream, string partition, string name, string node)
{
  string strm_upper = strtoupper(stream);
  if ( strpos(strm_upper,"STORAGE") >= 0 )
    StreamControlPanel_initStorageTasksPanel(stream,partition,name,node);
  else
    StreamControlPanel_initMonitoringTasksPanel(stream,partition,name,node);
}
//=============================================================================
void StreamControlPanel_initMonitoringTasksPanel(string stream, string partition, string name, string node)
{
  string val, dp = dpSubStr(name,DPSUB_DP);
  dyn_string relayInfraTasks, relayTasks, senderTasks;
  dyn_string monInfraTasks, monTasks, monReceivers;
  int res = dpGet(
    partition+"_RunInfo.MonFarm.relayTasks",relayTasks,
    partition+"_RunInfo.MonFarm.senderTasks",senderTasks,
    partition+"_RunInfo.MonFarm.relayInfrastructureTasks",relayInfraTasks,
    partition+"_RunInfo.MonFarm.monInfrastructureTasks",monInfraTasks,
    partition+"_RunInfo.MonFarm.monReceivers",monReceivers,
    partition+"_RunInfo.MonFarm.monTasks",monTasks
    );
  StreamControlPanel_checkErrors(res);
  string val = "";
  StreamControlPanel_addTasks(relayInfraTasks,node,val);
  StreamControlPanel_addTasks(monInfraTasks,node,val);
  m_class0TasksFrame.text = "Class 0 Tasks of Partition: "+partition;
  m_class0Tasks.backCol   = "_Transparent";
  m_class0Tasks.text = val;
  m_class0Tasks.readOnly = 1;
  m_class0Tasks.backCol = "white";
  val = "";
  StreamControlPanel_addTasks(senderTasks,node,val);
  StreamControlPanel_addTasks(relayTasks,node,val);
  StreamControlPanel_addTasks(monReceivers,node,val);
  StreamControlPanel_addTasks(monTasks,node,val);
  m_class1TasksFrame.text = "Class 1 Tasks of Partition: "+partition;
  m_class1Tasks.backCol = "_Transparent";
  m_class1Tasks.text = val;
  m_class1Tasks.readOnly = 1;
  m_class1Tasks.backCol = "white";
}
//=============================================================================
void StreamControlPanel_initStorageTasksPanel(string stream, string partition, string name, string node)
{
  string val, dp = dpSubStr(name,DPSUB_DP);
  dyn_string recvInfraTasks, recvSenderTasks, recvReceiverTasks;
  dyn_string strmInfraTasks, strmReceiverTasks, strmWriterTasks;
  int res = dpGet(
    partition+"_RunInfo.HLTFarm.receiverTasks",recvReceiverTasks,
    partition+"_RunInfo.Storage.recvSenders",recvSenderTasks,
    partition+"_RunInfo.Storage.recvInfrastructureTasks",recvInfraTasks,
    partition+"_RunInfo.Storage.streamInfrastructureTasks",strmInfraTasks,
    partition+"_RunInfo.Storage.streamReceivers",strmReceiverTasks,
    partition+"_RunInfo.Storage.streamWriters",strmWriterTasks
    );
  StreamControlPanel_checkErrors(res);
  string val = "";
  StreamControlPanel_addTasks(recvInfraTasks,node,val);
  StreamControlPanel_addTasks(strmInfraTasks,node,val);
  m_class0TasksFrame.text = "Class 0 Tasks of Partition: "+partition;
  m_class0Tasks.backCol   = "_Transparent";
  m_class0Tasks.text = val;
  m_class0Tasks.readOnly = 1;
  m_class0Tasks.backCol = "white";
  //DebugN("Class 0 tasks:",val);
  val = "";
  StreamControlPanel_addTasks(recvReceiverTasks,node,val);
  StreamControlPanel_addTasks(recvSenderTasks,node,val);
  StreamControlPanel_addTasks(strmReceiverTasks,node,val);
  StreamControlPanel_addTasks(strmWriterTasks,node,val);
  m_class1TasksFrame.text = "Class 1 Tasks of Partition: "+partition;
  m_class1Tasks.backCol = "_Transparent";
  m_class1Tasks.text = val;
  m_class1Tasks.readOnly = 1;
  m_class1Tasks.backCol = "white";

  //DebugN("Class 0 tasks:",val);
}
//=============================================================================
//
//  Stream control summary display
//
//=============================================================================
void StreamControlPanel_CheckDisplay(string dp, string value)  {
  int offset = isDollarDefined("$offset") ? getDollarValue("$offset") : 130;
  // DebugN("Rebuilding displays.....",m_streamName.text,m_displayedPartitions.text,offset);
  StreamControlPanel_removeDisplay(m_streamName.text,m_displayedPartitions.text);
  m_displayedPartitions.text = StreamControlPanel_showDisplay(m_streamName.text,offset);
}
//=============================================================================
string StreamControlPanel_initDisplay(string stream, string partition, int offset=130)  {
  // DebugN("Connect to :",stream+".State");
  m_streamName.text        = stream;
  //m_streamName.visible     = 1;
  m_streamName.backCol     = "yellow";
  //m_command.visible        = 1;
  m_command.text           = "Refresh";
  m_partition.visible      = 0;
  m_displayedPartitions.visible = 0;
  int res = dpConnect("StreamControlPanel_CheckDisplay",stream+".State");
  StreamControlPanel_checkErrors(res);
}
//=============================================================================
string StreamControlPanel_showDisplay(string stream, int offset=130)  {
  dyn_string partitions = StreamControlPanel_getStrings(stream+"_Partition*.Name","StreamPartition");
  string parts = "";
  for (int i=1, n=dynlen(partitions); i<=n; ++i)  {
    string partition = partitions[i];
    int off = (i-1)*150+offset;
    dyn_string params = makeDynString("$stream:"+stream,"$partition:"+partition,"$offset:"+offset);
    int res = addSymbol(myModuleName(), myPanelName(), "StreamControl/PartitionDisplay", partition,
              params,0,off,0,1,1);
    StreamControlPanel_checkErrors(res);
    parts = parts + partition + " ";
  }
  m_displayedPartitions.text = parts;
  return parts;
}
//=============================================================================
string StreamControlPanel_removeDisplay(string stream, string partitions)  {
  // DebugN("Disconnect from :",stream+".State");
  int res;// = dpDisconnect("StreamControlPanel_CheckDisplay",stream+".State");
  StreamControlPanel_checkErrors(res);
  dyn_string items  = strsplit(partitions," ");
  for (int i=1; i<=dynlen(items); ++i)  {
    string item = strltrim(strrtrim(items[i]," ")," ");
    res = removeSymbol(myModuleName(), myPanelName(), item);
    //DebugN("Remove partition display:"+item);
  }
}
//=============================================================================
void StreamControlPanel_initPartitionDisplay(string stream, string partition)  {
  string name = StreamControlPanel_getPartition(stream,partition);
  DebugN("StreamControlPanel_initPartitionDisplay:",stream,partition,name);
  if ( strlen(name)>0 )  {
    if ( strpos(stream,"Moni") >= 0 ) {
      StreamControlPanel_initMoniPartitionDisplay(stream,partition,name);
    }
    else  {
      StreamControlPanel_initStreamPartitionDisplay(stream,partition,name);
    }
  }
}
//=============================================================================
void StreamControlPanel_initStreamPartitionDisplay(string stream, string partition, string name)  {
  int flag;
  string val, dp = dpSubStr(name,DPSUB_DP);
  dyn_int    strmMult;
  dyn_string rcvInfra, recvTasks, recvInfraTasks, recvSenderTasks, recvReceiverTasks;
  dyn_string strmInfra, strmTypes, strmTasks, strmInfraTasks, strmReceiverTasks, strmWriterTasks;
  dyn_string recv_slices = StreamControlPanel_getStringVectors(dp+".RecvSlices","StreamPartition");
  dyn_string strm_slices = StreamControlPanel_getStringVectors(dp+".StreamSlices","StreamPartition");
  m_frame.text     = "DAQ Partition: "+partition;
  StreamControlPanel_setListItems("m_recvSlots",recv_slices);
  StreamControlPanel_setListItems("m_strmSlots",strm_slices);
  int res = dpGet(
    partition+"_RunInfo.Storage.recvInfrastructure",rcvInfra,
    partition+"_RunInfo.HLTFarm.receiverTasks",recvReceiverTasks,
    partition+"_RunInfo.Storage.streamInfrastructure",strmInfra,
    partition+"_RunInfo.Storage.streamTypes",strmTypes,
    partition+"_RunInfo.Storage.streamMultiplicity",strmMult,
    partition+"_RunInfo.Storage.recvSenders",recvSenderTasks,
    partition+"_RunInfo.Storage.recvInfrastructureTasks",recvInfraTasks,
    partition+"_RunInfo.Storage.streamInfrastructureTasks",strmInfraTasks,
    partition+"_RunInfo.Storage.streamReceivers",strmReceiverTasks,
    partition+"_RunInfo.Storage.streamWriters",strmWriterTasks,
    partition+"_RunInfo.Storage.storeFlag",flag
    );
  StreamControlPanel_checkErrors(res);
  string val = "Flag:\nReceiving layer infrastructure:\nStreaming layer infrastructure:\nOutput streams:\t\t";
  val = val + "\nReceiving layer:\t\nStreaming layer:\t";
  m_desc.text = val;
  val = flag+"\n";
  for(int i=1; i<=dynlen(rcvInfra); ++i)  
    val = val + rcvInfra[i] + "  ";
  val = val + "\n";
  for(int i=1; i<=dynlen(strmInfra); ++i)  
    val = val + strmInfra[i] + "  ";
  val = val + "\n";
  for(int i=1; i<=dynlen(strmTypes); ++i)
    val = val + strmTypes[i] + " ["+strmMult[i]+"]  ";
  val = val + "\n" + dynlen(recv_slices) + " slots used with ";
  if ( dynlen(recvInfraTasks) > 0 )
    val = val +dynlen(recvInfraTasks) + " infrastructure task(s) ";
  if ( dynlen(recvReceiverTasks) > 0 )
    val = val + dynlen(recvReceiverTasks) + " receiver(s) ";
  if ( dynlen(recvSenderTasks) > 0 )
    val = val + dynlen(recvSenderTasks) + " sender(s) ";
  val = val + "\n" + dynlen(strm_slices) + " slots used with ";
  if ( dynlen(strmInfraTasks) > 0 )
    val = val + dynlen(strmInfraTasks) + " infra task(s) ";
  if ( dynlen(strmReceiverTasks) > 0 )
    val = val + dynlen(strmReceiverTasks) + " receiver(s) ";
  if ( dynlen(strmWriterTasks) > 0 )
    val = val + dynlen(strmWriterTasks) + " file writer(s) ";
  m_params.text = val;
  m_desc.backCol   = "_Button";
  m_params.backCol = "_Button";
  m_desc.backCol   = "_Transparent";
  m_params.backCol = "_Transparent";
  m_displayedPartitions.visible = 0;
  m_partition.visible = 0;
}
//=============================================================================
void StreamControlPanel_initMoniPartitionDisplay(string stream, string partition, string name)  {
  int flag;
  string val, dp = dpSubStr(name,DPSUB_DP);
  dyn_int    monMult;
  dyn_string monStreams, monTypes, monInfra, monTasks, monInfraTasks, monReceivers, senderTasks;
  dyn_string relayTasks, relayInfra, relayInfraTasks;
  dyn_string relay_slices = StreamControlPanel_getStringVectors(dp+".RecvSlices","StreamPartition");
  dyn_string moni_slices  = StreamControlPanel_getStringVectors(dp+".StreamSlices","StreamPartition");
  m_frame.text = "DAQ Partition: "+partition;
  StreamControlPanel_setListItems("m_recvSlots",relay_slices);
  StreamControlPanel_setListItems("m_strmSlots",moni_slices);
  int res = dpGet(
    partition+"_RunInfo.MonFarm.senderTasks",             senderTasks,
    partition+"_RunInfo.MonFarm.monTypes",                monTypes,
    partition+"_RunInfo.MonFarm.monStreams",              monStreams,
    partition+"_RunInfo.MonFarm.monMultiplicity",         monMult,
    partition+"_RunInfo.MonFarm.monInfrastructure",       monInfra,
    partition+"_RunInfo.MonFarm.monInfrastructureTasks",  monInfraTasks,
    partition+"_RunInfo.MonFarm.monReceivers",            monReceivers,
    partition+"_RunInfo.MonFarm.monTasks",                monTasks,
    partition+"_RunInfo.MonFarm.relayTasks",              relayTasks,
    partition+"_RunInfo.MonFarm.relayInfrastructure",     relayInfra,
    partition+"_RunInfo.MonFarm.relayInfrastructureTasks",relayInfraTasks,
    partition+"_RunInfo.MonFarm.monFlag",                 flag
    );
  StreamControlPanel_checkErrors(res);
  string dsc = "Flag:\nInfrastructure:\nMonitoring types:\t";
  string val = flag+"\nRelay:";
  for(int i=1; i<=dynlen(relayInfra); ++i)  
    val = val + relayInfra[i] + "  ";
  val = val + "  Monitoring:";
  for(int i=1; i<=dynlen(monInfra); ++i)  
    val = val + monInfra[i] + "  ";
  val = val + "\n";
  for(int i=1; i<=dynlen(monTypes); ++i)  {
    if ( i>1 && ((i-1)%3) == 0 )  {
      val = val + "\n";
      dsc = dsc + "\n";
    }
    val = val + monTypes[i] + " ["+monStreams[i]+"/"+monMult[i]+"] ";
  }
  dsc = dsc + "\nRelay setup:\t\nMonitoring setup:\t\n";
  val = val + "\n" + dynlen(relay_slices) + " slots used with ";
  if ( dynlen(senderTasks) > 0 )
    val = val + dynlen(senderTasks) + " sender(s) and ";
  if ( dynlen(relayInfraTasks) > 0 )
    val = val + dynlen(relayInfraTasks) + " infrastructure task(s) ";
  val = val + "\n" + dynlen(moni_slices) + " slots used with ";
  if ( dynlen(monReceivers) > 0 )
    val = val + dynlen(monReceivers) + " receiver(s) ";
  if ( dynlen(monInfraTasks) > 0 )
    val = val +dynlen(monInfraTasks) + " infrastructure task(s) ";
  
  m_desc.text = dsc;
  m_params.text = val;
  m_desc.backCol   = "_Button";
  m_params.backCol = "_Button";
  m_desc.backCol   = "_Transparent";
  m_params.backCol = "_Transparent";
  m_displayedPartitions.visible = 0;
  m_partition.visible = 0;
}
//=============================================================================
//
//  ALLOCATION DISPLAY
//
//=============================================================================
void StreamControlPanel_initAllocPanel(string name,string partition)  {
  m_streamName.text = name;
  m_partition.text = partition;
  StreamControlPanel_setupAllocWidgets();
  m_partition.visible = 0;
  m_streamName.visible = 1;
  m_streamName.backCol = "yellow";
  m_partition.visible = 1;
  m_partition.backCol = "yellow";
  m_partitionName.visible = 1;
  m_partitionName.text = "Partition:"+partition;
  m_recvSlots.enabled = 0;
  m_strmSlots.enabled = 0;
  m_recvCheck.visible = 0;
  m_strmCheck.visible = 0;
  StreamControlPanel_initAllocData(partition);
}
//=============================================================================
void StreamControlPanel_setupAllocWidgets()  {
  m_recvFrame.toolTipText         = "Input parameters to allocate slots on the receiving layer .";
  m_recvNumSlotText.toolTipText   = "Number of slots in the receiving layer.";
  m_recvSlots.toolTipText         = m_recvNumSlotText.toolTipText;
  m_recvStrategyText.toolTipText  = "Parameter for allocation strategy. Don't change unless you know what you do.";
  m_recvStrategy.toolTipText      = m_recvStrategyText.toolTipText;
  m_recvInfraText.toolTipText     = "Task types to be started for the receiving layer infrastructure.";
  m_recvInfraTaskTypes.toolTipText= m_recvInfraText.toolTipText;
  m_strmFrame.toolTipText         = "Input parameters to allocate slots on the streaming layer .";
  m_strmStrategyText.toolTipText  = "Parameter for allocation strategy. Don't change unless you know what you do.";
  m_strmStrategy.toolTipText      = m_strmStrategyText.toolTipText;
  m_strmInfraText.toolTipText     = "Task types to be started for the streaming layer infrastructure.";
  m_strmInfraTaskTypes.toolTipText= m_strmInfraText.toolTipText;
  m_strmTypesText.toolTipText     = "Output stream types and multiplicities. Form: Type/Mult";
  m_strmTypes.toolTipText         = m_strmTypesText.toolTipText;
  m_Reset.toolTipText             = "Discard changes and reset to original values";
  m_Cancel.toolTipText            = "Discard changes and close panel";
  m_Save.toolTipText              = "Save changes to datapoints but keep panel open.";
  m_Ok.toolTipText                = "Save changes to datapoints and close panel.";
}
//=============================================================================
void StreamControlPanel_initAllocData(string partition)  {
  int recv_slots = 1;
  int strm_slots = 0;
  string info = partition+"_RunInfo.";
  dyn_int strm_mult;
  dyn_string recv_infra, strm_infra, strm_types;
  int res = dpGet(info+"HLTFarm.nSubFarms",recv_slots,
                  info+"Storage.recvInfrastructure", recv_infra,
                  info+"Storage.streamInfrastructure", strm_infra,
                  info+"Storage.streamMultiplicity", strm_mult,
                  info+"Storage.streamTypes", strm_types);
  StreamControlPanel_checkErrors(res);
  string s= "";
  for(int i=1, n=dynlen(strm_mult); i<=n; ++i)  {
    strm_slots = strm_slots + strm_mult[i];
    s = s + strm_types[i] + "/" + strm_mult[i];
    if ( i < n ) s = s+ "\n";
  }
  m_strmTypes.text = s;
  s = "";
  for(int i=1, n=dynlen(strm_infra); i<=n; ++i)  {
    s = s + strm_infra[i];
    if ( i < n ) s = s+ "\n";
  }
  m_recvInfraTaskTypes.text = s;
  s = "";
  for(int i=1, n=dynlen(recv_infra); i<=n; ++i)  {
    s = s + recv_infra[i];
    if ( i < n ) s = s+ "\n";
  }
  m_strmInfraTaskTypes.text = s;
  m_partitionName.text = "Partition:"+partition;
  m_recvSlots.text = recv_slots;
  m_recvStrategy.sbMinimum = 1;
  m_recvStrategy.sbMaximum = 50;
  m_recvStrategy.sbStep = 1;
  m_recvStrategy.text = 2;

  m_strmSlots.text = strm_slots;
  m_strmStrategy.sbMinimum = 1;
  m_strmStrategy.sbMaximum = 20;
  m_strmStrategy.sbStep = 1;
  m_strmStrategy.text = 2;
}
//=============================================================================
void StreamControlPanel_checkRecvAllocation()  {
}
//=============================================================================
void StreamControlPanel_checkStrmAllocation()  {
}
//=============================================================================
int StreamControlPanel_AllocSave(string partition)  {
  // Are you sure ....
  string info = partition+"_RunInfo.";
  dyn_float df;
  dyn_string ds;
  string msg = "Are you sure you want to save the parameters ?";
  ChildPanelOnReturn("vision/MessageInfo","Please Confirm",
                     makeDynString("$1:"+msg,"$2:Cancel","$3:Ok"),50,50,ds,df);
  if ( ds[1] == 0 )   {
    string s0 = m_strmTypes.text, s1 = m_recvInfraTaskTypes.text, s2 = m_strmInfraTaskTypes.text;
    strreplace(s0,"\n"," ");
    while ( strpos(s0,"  ")>0 ) strreplace(s0,"  "," ");
    strreplace(s0,"/"," ");
    strreplace(s1,"\n"," ");
    while ( strpos(s1,"  ")>0 ) strreplace(s1,"  "," ");
    strreplace(s2,"\n"," ");
    while ( strpos(s2,"  ")>0 ) strreplace(s2,"  "," ");
    dyn_string strm_typs  = strsplit(s0," ");
    dyn_string recv_infra = strsplit(s1," ");
    dyn_string strm_infra = strsplit(s2," ");
    if ( dynlen(strm_typs)%2 != 0 )  {
      ChildPanelOn("vision/MessageInfo1","Bad input",makeDynString("Parameter 'Stream Types' is incorrect"),50,50);
      return 1;
    }
    dyn_int mult;
    dyn_string types;
    for(int i=1, n=dynlen(strm_typs); i<=n; i=i+2)  {
      dynAppend(types,strm_typs[i]);
      dynAppend(mult,strm_typs[i+1]);
    }
    int res = dpSet(info+"Storage.recvInfrastructure", recv_infra,
                    info+"Storage.streamInfrastructure", strm_infra,
                    info+"Storage.streamMultiplicity", mult,
                    info+"Storage.streamTypes", types);
    StreamControlPanel_checkErrors(res);
    StreamControlPanel_initAllocData(partition);
    return res;
  }
  return 0;
}
//=============================================================================
int StreamControlPanel_AllocOk()  {
  int res = StreamControlPanel_AllocSave();  
  if ( res == 0 )  {
    PanelOff();
  }
}
//=============================================================================
void StreamControlPanel_AllocCancel(string partition)  {
  StreamControlPanel_initAllocData(partition);
  PanelOff();
}
//=============================================================================
void StreamControlPanel_AllocReset(string partition)  {
  StreamControlPanel_initAllocData(partition);
}
//=============================================================================
//
//  Partition content panel
//
//=============================================================================
void StreamControlPanel_init(string stream, string partition)  {
  // DebugN("Stream:"+stream+" Partition:"+partition);
  m_streamName.text = stream;
  m_partition.text   = partition;
  StreamControlPanel_setupWidgets();
  m_partition.visible = 0;
  m_streamName.visible = 0;

  m_streamName.visible = 1;
  m_streamName.backCol = "yellow";
  m_partition.visible = 1;
  m_partition.backCol = "yellow";

  m_partitionName.text = "Partition:"+partition;
  m_partitionName.visible = 1;
  m_partitionList.enabled = 1;
  if ( strlen(partition)>0 )  {
    if ( !StreamControlPanel_showPartition(stream,partition) )  {
      LayerOff(2);
      LayerOff(3);
      LayerOff(4);
    }
  }
  else
    StreamControlPanel_showSummary(stream);
  int res = dpConnect("StreamControlPanel_ActionDone",stream+".State");
  StreamControlPanel_checkErrors(res);
}
//=============================================================================
void StreamControlPanel_setupWidgets()  {
  m_partitionName.text        = "Partition: ----";
  m_partitionName.toolTipText = "Name of the selected partition";

  m_partitionList.toolTipText = "Allocated partition slots";

  m_allSlotFrame.text         = "Used Stream Slots";
  m_allSlotFrame.toolTipText  = "Information about used slots in the stream system";

  m_freeSlotFrame.text        = "Free Stream Slots";
  m_freeSlotFrame.toolTipText = "Information about free slots in the stream system";

  m_showSummary.text          = "Select Partition";
  m_showSummary.toolTipText   = "Go back and show partition summary for further selection";  

  m_freePartition.text        = "Free Partition";
  m_freePartition.toolTipText = "Free partition selected in the partition list";

  m_inputFrame.text           = "Input Parameters";
  m_inputFrame.toolTipText    = "Parameters used to define this stream configuration.";
}
//=============================================================================
void StreamControlPanel_ActionDone(string dp)  {
  string result = "";
  int res = dpGet(dp,result);
  //DebugN("StreamControlPanel_ActionDone:"+dp+" --> "+result);
  // READY/StreamSystem/LHCb/203
  dyn_string itms = strsplit(result,"/");
  //DebugN("StreamControlPanel_ActionDone:"+itms);
  if ( dynlen(itms)>=3 )  {
    if ( itms[2] == m_streamName.text && itms[3] == m_partition.text )  {
      DebugN("StreamControlPanel_ActionDone:"+result+" GOT IT!!!");
    }
  }
}
//=============================================================================
void StreamControlPanel_initTable(dyn_string new_cols) {
  m_table.deleteAllLines();
  for( int j=m_table.columnCount()-1; j>=0; j-- )
    m_table.deleteColumn(j);
  for(int j=0; j<dynlen(new_cols); ++j )  {
    m_table.insertColumn(m_table.columnCount());
    m_table.columnHeader(j,new_cols[j+1]);
    m_table.columnName(j,new_cols[j+1]);
  }
}
//=============================================================================
void StreamControlPanel_adjustTable(dyn_string cols) {
  for(int j=0; j<dynlen(cols); ++j)  m_table.adjustColumn(j);
}
//=============================================================================
void StreamControlPanel_showSummary()  {
  string name = m_streamName.text;
  LayerOn(1);
  LayerOn(3);
  m_table.visible = 1;
  m_tableSteer.visible = 1;
  m_showSummary.visible= 0;
  StreamControlPanel_setListItems("m_partitionList",StreamControlPanel_getStrings(name+"_Partition*.Name","StreamPartition"));
  m_allSlotFrame.enabled = 1;
  StreamControlPanel_setListItems("m_allRecvSlots",StreamControlPanel_getStringVectors(name+"_Partition*.RecvSlices","StreamPartition"));
  StreamControlPanel_setListItems("m_allStrmSlots",StreamControlPanel_getStringVectors(name+"_Partition*.StreamSlices","StreamPartition"));

  m_freeSlotFrame.enabled = 1;
  StreamControlPanel_setListItems("m_freeRecvSlots",StreamControlPanel_getStringVectors(name+".RecvSlices","StreamControl"));
  StreamControlPanel_setListItems("m_freeStrmSlots",StreamControlPanel_getStringVectors(name+".StreamSlices","StreamControl"));
  DebugN("Show Stream summary information.");
  LayerOff(2);
}
//=============================================================================
bool StreamControlPanel_showReceivingTasks(string name)   {
  dyn_string values;
  int res = dpGet(name+"_RunInfo.HLTFarm.receiverTasks", values);
  if ( res == 0 ) {
    dyn_string cols = makeDynString("HLT Node","Rcv Node","Receiver Task Name"," Task Type");
    StreamControlPanel_initTable(cols);
    for(int i=1; i<=dynlen(values); ++i)  {
      dyn_string itms = strsplit(values[i],"/");
      m_table.appendLine(cols[1],itms[1],cols[2],itms[2],cols[3],itms[3],cols[4],itms[4]);
    }
    StreamControlPanel_adjustTable(cols);
  }
  return StreamControlPanel_checkErrors(res);
}
//=============================================================================
bool StreamControlPanel_showRecvSenderTasks(string name)   {
  dyn_string values;
  int res = dpGet(name+"_RunInfo.Storage.recvSenders", values);
  if ( res == 0 )  {
    dyn_string cols = makeDynString("Recv Node","Sender Task Name","Task Type","Stream Node","Recv Task");
    StreamControlPanel_initTable(cols);
    for(int i=1; i<=dynlen(values); ++i)  {
      dyn_string itms = strsplit(values[i],"/");
      m_table.appendLine(cols[1],itms[1],cols[4],itms[4],cols[2],itms[2],cols[3],itms[3],cols[5],itms[5]);
    }
    StreamControlPanel_adjustTable(cols);
  }
  return StreamControlPanel_checkErrors(res);
}
//=============================================================================
bool StreamControlPanel_showInfrastructure(string name)  {
  dyn_string values;
  int res = dpGet(name,values);
  if ( res == 0 )  {
    dyn_string cols = makeDynString("Node","Task Name","Task Type");
    StreamControlPanel_initTable(cols);
    for(int i=1; i<=dynlen(values); ++i)  {
      dyn_string itms = strsplit(values[i],"/");
      m_table.appendLine(cols[1],itms[1],cols[2],itms[2],cols[3],itms[3]);
    }
    StreamControlPanel_adjustTable(cols);
  }
  return StreamControlPanel_checkErrors(res);
}
//=============================================================================
string StreamControlPanel_getPartition(string stream,string name)  {
  dyn_string items = dpNames(stream+"_Partition*.Name","StreamPartition");
  dyn_string values;
  int res = dpGet(items,values);
  if ( 0 == res )  {
    for(int i=1; i<=dynlen(values); i++) {
      if ( name == values[i] )  {
        string dp = dpSubStr(items[i],DPSUB_DP);
        return dp;
      }
    }
  }
  return "";
}
//=============================================================================
string StreamControlPanel_getPartitionNameFromSlice(string stream,string name)  {
  // DebugN("StreamControlPanel_getPartitionNameFromSlice:"+stream+" Name:"+name);
  dyn_string items = dpNames(stream+"_Partition*.FSMSlice","StreamPartition");
  dyn_string values;
  int res = dpGet(items,values);
  if ( 0 == res )  {
    for(int i=1; i<=dynlen(values); i++) {
      if ( name == values[i] )  {
        string val, dp = dpSubStr(items[i],DPSUB_DP)+".Name";
        dpGet(dp,val);
        return val;
      }
    }
  }
  return "";
}
//=============================================================================
void StreamControlPanel_freePartition(string stream,string name) {
  int res = dpSet(stream+".Command","free('"+name+"')");
  StreamControlPanel_checkErrors(res);
}
//=============================================================================
bool StreamControlPanel_showPartition(string stream,string name) {
  string partition = StreamControlPanel_getPartition(stream,name);
  if ( strlen(partition)>0 )  {
    int flag;
    string val, dp = dpSubStr(partition,DPSUB_DP);
    // DebugN("Stream:"+stream);
    // DebugN("Name:   "+name);
    // DebugN("DP:     "+dp);
    dyn_int    strmMult;
    dyn_string rcvInfra, strmInfra, strmTypes;
    dyn_string recv_slices = StreamControlPanel_getStringVectors(dp+".RecvSlices","StreamPartition");
    dyn_string strm_slices = StreamControlPanel_getStringVectors(dp+".StreamSlices","StreamPartition");
    LayerOn(2);
    LayerOff(3);
    m_partition.text = name;
    m_partitionName.text = "Partition:"+name;
    m_tableSteer.text(0) = "Show HLT-receiving layer tasks";
    m_tableSteer.text(1) = "Show receiving - streaming layer tasks";
    m_tableSteer.text(2) = "Show receiving layer infrastructure tasks";
    m_tableSteer.text(3) = "Show streaming layer receiver tasks";
    m_tableSteer.text(4) = "Show streaming layer writer tasks";
    m_tableSteer.text(5) = "Show streaming layer infrastructure tasks";
    m_showSummary.visible = 1;
    StreamControlPanel_setListItems("m_allRecvSlots",recv_slices);
    StreamControlPanel_setListItems("m_allStrmSlots",strm_slices);
    int res = dpGet(name+"_RunInfo.Storage.recvInfrastructure",rcvInfra,
      name+"_RunInfo.Storage.streamInfrastructure",strmInfra,
      name+"_RunInfo.Storage.streamTypes",strmTypes,
      name+"_RunInfo.Storage.streamMultiplicity",strmMult,
      name+"_RunInfo.Storage.storeFlag",flag
      );
    StreamControlPanel_checkErrors(res);
    // DebugN("rcvInfra: "+rcvInfra);
    // DebugN("strmInfra:"+strmInfra);
    // DebugN("strmTypes:"+strmTypes);
    // DebugN("strmMult: "+strmMult);
    val = "Flag:\nReceiving layer infrastructure:\nStreaming layer infrastructure:\nOutput streams:\t\t";
    m_inputDesc.text = val;
    val = flag+"\n";
    for(int i=1; i<=dynlen(rcvInfra); ++i)  
      val = val + rcvInfra[i] + "  ";
    val = val + "\n";
    for(int i=1; i<=dynlen(strmInfra); ++i)  
      val = val + strmInfra[i] + "  ";
    val = val + "\n";
    for(int i=1; i<=dynlen(strmTypes); ++i)
      val = val + strmTypes[i] + " ["+strmMult[i]+"]  ";;
    m_inputParams.text = val;
    StreamControlPanel_showReceivingTasks(name);
    return true;
  }
  return false;
}
//=============================================================================
void StreamControlPanel_radioSelectionChanged(int button, string part_name)  {
  //DebugN(part_name+" >> " + m_tableSteer.number()+" -- "+button);
  switch(button) {
    case 0:  StreamControlPanel_showReceivingTasks(part_name);
      return;
    case 1:  StreamControlPanel_showRecvSenderTasks(part_name);
      return;
    case 2:  StreamControlPanel_showInfrastructure(part_name+"_RunInfo.Storage.recvInfrastructureTasks");
      return;
    case 3:  StreamControlPanel_showInfrastructure(part_name+"_RunInfo.Storage.streamReceivers");
      return;
    case 4:  StreamControlPanel_showInfrastructure(part_name+"_RunInfo.Storage.streamWriters");
      return;
    case 5:  StreamControlPanel_showInfrastructure(part_name+"_RunInfo.Storage.streamInfrastructureTasks");
      return;
  }
}
//=============================================================================
void StreamControlPanel_install()  {
  if ( dynlen(dpTypes("StreamControl")) ==0 ) {
    dyn_dyn_string names;
    dyn_dyn_int types;
    names[1]  = makeDynString ("StreamControl","","","");
    names[2]  = makeDynString ("","ActivePartitions","","");
    names[3]  = makeDynString ("","Command","","");
    names[4]  = makeDynString ("","InUse","","");
    names[5]  = makeDynString ("","PartitionDesc","","");
    names[6]  = makeDynString ("","RecvNodes","","");
    names[7]  = makeDynString ("","RecvSlices","","");
    names[8]  = makeDynString ("","State","","");
    names[9]  = makeDynString ("","StreamNodes","","");
    names[10] = makeDynString ("","StreamSlices","","");
    types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
    types[2]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[3]  = makeDynInt (0,DPEL_STRING,0,0);
    types[4]  = makeDynInt (0,DPEL_DYN_INT,0,0);
    types[5]  = makeDynInt (0,DPEL_DYN_DPID,0,0);
    types[6]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[7]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[8]  = makeDynInt (0,DPEL_STRING,0,0);
    types[9]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[10] = makeDynInt (0,DPEL_DYN_STRING,0,0);
    int result = dpTypeCreate(names,types);
    StreamTaskMgr_checkErrors(result);
  }   
  else {
    DebugN("Data type StreamControl are already defined....Nothing to do.");
  }
  if ( dynlen(dpTypes("StreamPartition")) ==0 ) {
    dyn_dyn_string names;
    dyn_dyn_int types;
    names[1]  = makeDynString ("StreamPartition","","","");
    names[2]  = makeDynString ("","FSMSlice","","");
    names[3]  = makeDynString ("","InUse","","");
    names[4]  = makeDynString ("","Name","","");
    names[5]  = makeDynString ("","RecvNodes","","");
    names[6]  = makeDynString ("","RecvSlices","","");
    names[7]  = makeDynString ("","StreamNodes","","");
    names[8]  = makeDynString ("","StreamSlices","","");
    types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
    types[2]  = makeDynInt (0,DPEL_STRING,0,0);
    types[3]  = makeDynInt (0,DPEL_INT,0,0);
    types[4]  = makeDynInt (0,DPEL_STRING,0,0);
    types[5]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[6]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[7]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[8]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
    int result = dpTypeCreate(names,types);
    StreamTaskMgr_checkErrors(result);
  }   
  else  {
    DebugN("Data type StreamPartition are already defined....Nothing to do.");
  }
}
//=============================================================================
void StreamControlPanel_uninstall()  {
  if ( dynlen(dpTypes("StreamControl")) > 0 ) {
    dyn_string objs = dpNames("*","StreamControl");
    for(int i=1; i<=dynlen(objs);++i) dpDelete(objs[i]);
    int result = dpTypeDelete("StreamControl");
    StreamControlPanel_checkErrors(result);
  }  
  else  {
    DebugN("Data type StreamControl does not exist....Nothing to do.");
  }
  if ( dynlen(dpTypes("StreamPartition")) > 0 ) {
    dyn_string objs = dpNames("*","StreamPartition");
    for(int i=1; i<=dynlen(objs);++i) dpDelete(objs[i]);
    int result = dpTypeDelete("StreamPartition");
    StreamControlPanel_checkErrors(result);
  }
  else  {
    DebugN("Data type StreamPartition does not exist....Nothing to do.");
  }
}
