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
  StreamControl_trace("StreamControlPanel_getStrings:"+dp+" ["+type+"]");
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
  StreamControl_trace("StreamControlPanel_showPanel:"+stream+" "+partition+" "+ref+" "+panel);
  dyn_string params = makeDynString("$partition:"+partition,"$stream:"+stream,"$offset:"+offsety);
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
void StreamControlPanel_initTasksPanel(string stream, string partition, string name, string node)
{
  StreamControl_trace("StreamControlPanel_initMonitoringTasksPanel:"+stream+" "+partition+" "+name+" "+node);
  string font = "Arial,8,-1,5,50,0,0,0,0,0";
  string val, dp = dpSubStr(name,DPSUB_DP);
  dyn_string dataSenders, recvInfrastructure, recvReceivers, recvSenders;
  dyn_string strmInfrastructure, strmReceivers, strmSenders;
  int res = dpGet(
    name+".DataSenders",dataSenders,
    name+".RecvInfrastructure",recvInfrastructure,
    name+".RecvReceivers",recvReceivers,
    name+".RecvSenders",recvSenders,
    name+".StreamInfrastructure",strmInfrastructure,
    name+".StreamReceivers",strmReceivers,
    name+".StreamSenders",strmSenders
    );
  StreamControlPanel_checkErrors(res);
  string val = "";
  StreamControlPanel_addTasks(dataSenders,node,val);
  StreamControlPanel_addTasks(recvInfrastructure,node,val);
  StreamControlPanel_addTasks(recvReceivers,node,val);
  StreamControlPanel_addTasks(recvSenders,node,val);
  StreamControlPanel_addTasks(strmInfrastructure,node,val);
  StreamControlPanel_addTasks(strmReceivers,node,val);
  StreamControlPanel_addTasks(strmSenders,node,val);
  m_class1TasksFrame.text = "Tasks for "+stream+" of Partition "+partition+" on "+node;
  m_class1Tasks.backCol = "_Transparent";
  m_class1Tasks.backCol = "white";
  m_class1Tasks.readOnly = 1;
  m_class1Tasks.font = font;  
  m_class1Tasks.text = val;
}
//=============================================================================
void StreamControl_trace(string msg)  {
  // DebugN(msg);
}
//=============================================================================
//
//  Stream control summary display
//
//=============================================================================
void StreamControlPanel_CheckDisplay(string dp, string value)  {
  int offset = isDollarDefined("$offset") ? getDollarValue("$offset") : 130;
  StreamControl_trace("Rebuilding displays.....",m_streamName.text+" "+m_displayedPartitions.text+" "+offset);
  StreamControlPanel_removeDisplay(m_streamName.text,m_displayedPartitions.text);
  m_displayedPartitions.text = StreamControlPanel_showDisplay(m_streamName.text,offset);
}
//=============================================================================
string StreamControlPanel_initDisplay(string stream, string partition, int offset=130)  {
  StreamControl_trace("Connect to :"+stream+".State");
  m_streamName.text        = stream;
  //m_streamName.visible     = 1;
  m_streamName.backCol     = "yellow";
  //m_command.visible        = 1;
  m_command.text           = "Refresh";
  m_partition.visible      = 0;
  m_displayedPartitions.visible = 0;
  int res = dpConnect("StreamControlPanel_CheckDisplay",stream+"Alloc.State");
  StreamControlPanel_checkErrors(res);
}
//=============================================================================
string StreamControlPanel_showDisplay(string stream, int offset=130)  {
  dyn_string partitions = StreamControlPanel_getStrings(stream+"_Slice*.Name","StreamPartition");
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
  StreamControl_trace("StreamControlPanel_removeDisplay:"+stream+" "+partitions);
  int res = 0;// = dpDisconnect("StreamControlPanel_CheckDisplay",stream+".State");
  StreamControlPanel_checkErrors(res);
  dyn_string items  = strsplit(partitions," ");
  for (int i=1; i<=dynlen(items); ++i)  {
    string item = strltrim(strrtrim(items[i]," ")," ");
    res = removeSymbol(myModuleName(), myPanelName(), item);
    StreamControl_trace("Remove partition display:"+item);
  }
}
//=============================================================================
void StreamControlPanel_initPartitionDisplay(string stream, string partition)  {
  string name = StreamControlPanel_getPartition(stream,partition);
  StreamControl_trace("StreamControlPanel_initPartitionDisplay:"+stream+" "+partition+" "+name);
  int flag;
  string     rundp_name, val, dp = dpSubStr(name,DPSUB_DP);
  dyn_int    strmMult;
  dyn_string rcvInfra, recvTasks, recvInfraTasks, recvSenderTasks, recvReceiverTasks;
  dyn_string strmInfra, strmTypes, strmTasks, strmInfraTasks, strmReceiverTasks, strmWriterTasks;
  dyn_string recv_slices = StreamControlPanel_getStringVectors(dp+".RecvSlices","StreamPartition");
  dyn_string strm_slices = StreamControlPanel_getStringVectors(dp+".StreamSlices","StreamPartition");
  m_frame.text     = "Partition: "+partition;
  StreamControlPanel_setListItems("m_recvSlots",recv_slices);
  StreamControlPanel_setListItems("m_strmSlots",strm_slices);
  rundp_name = "LBECS:"+partition;
  int res = dpGet(
    rundp_name+"_RunInfo.Storage.recvInfrastructure",rcvInfra,
    rundp_name+"_RunInfo.Storage.streamInfrastructure",strmInfra,
    rundp_name+"_RunInfo.Storage.streamTypes",strmTypes,
    rundp_name+"_RunInfo.Storage.streamMultiplicity",strmMult,
    rundp_name+"_RunInfo.Storage.storeFlag",flag
    );
  StreamControlPanel_checkErrors(res);
  res = dpGet(
    dp+".RecvReceivers",recvReceiverTasks,
    dp+".RecvSenders",recvSenderTasks,
    dp+".RecvInfrastructure",recvInfraTasks,
    dp+".StreamInfrastructure",strmInfraTasks,
    dp+".StreamReceivers",strmReceiverTasks,
    dp+".StreamSenders",strmWriterTasks
    );
  StreamControlPanel_checkErrors(res);
  string val = "Flag:\nReceive infrastructure:\nStream infrastructure:\nOutput streams:\t\t";
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
    val = val + dynlen(strmWriterTasks) + " senders(s) ";
  m_params.text = val;
  m_desc.backCol   = "_Button";
  m_params.backCol = "_Button";
  m_desc.backCol   = "_Transparent";
  m_params.backCol = "_Transparent";
}
//=============================================================================
//
//  ALLOCATION DISPLAY
//
//=============================================================================
void StreamControlPanel_CheckAllocPanel(string dp, string value)  {
  string partition = "";
  string stream = m_streamName.text;
  if ( dpExists(value+".general.partName") )  {
    LayerOn(2);
    setValue("m_sliceNotInUse","visible",0);
    dpGet(value+".general.partName",partition);
    StreamControl_trace("Rebuilding Stream allocation panel with runInfo:"+value);
    StreamControlPanel_initAllocPanel(stream,partition);
    return;
  }
  LayerOff(2);
  setValue("m_sliceNotInUse","visible",1);
}
//=============================================================================
void StreamControlPanel_startAllocPanel(string slice, string stream)  {
  string run_info;
  string sys = getSystemName();
  sys = substr(sys,0,strlen(sys)-1);
  m_streamName.text = stream;
  setValue("m_sliceNotInUse","text","The streaming slice\n"+slice+"\nin system "+sys+"\nis currently not used.");
  setValue("m_sliceNotInUse","backCol","_Transparent");
  setValue("m_sliceNotInUse","foreCol","red");
  if (dpExists(slice+".RunInfo"))  {
    dpConnect("StreamControlPanel_CheckAllocPanel",slice+".RunInfo");
    return;
  }
  LayerOff(2);
  setValue("m_sliceNotInUse","visible",1);  
}
//=============================================================================
void StreamControlPanel_initAllocPanel(string name, string partition)  {
  string font = "Arial,8,-1,5,50,0,0,0,0,0";
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
  m_runInfoText.visible = 1;
  m_runInfoText.text = "Run info:"+partition;
  m_recvSlots.enabled = 0;
  m_strmSlots.enabled = 0;
  m_recvCheck.visible = 0;
  m_strmCheck.visible = 0;
  m_strmSlots.font = font;
  m_recvInfraTaskTypes.font = font;
  m_strmInfraTaskTypes.font = font;
  m_strmTypes.font = font;
  StreamControlPanel_initAllocData(name,partition);
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
void StreamControlPanel_initAllocData(string stream, string partition)  {
  int recv_slots = 1, strm_slots = 0, recv_strategy = 2, strm_strategy = 2;
  string dp, info = "LBECS:"+partition+"_RunInfo";
  dyn_int strm_mult;
  dyn_string recv_infra, strm_infra, strm_types;
  int res = 0;
  if ( strpos(stream,"Storage") >= 0 )  {
    dp = strtoupper(stream)+":"+m_runInfoDP.text;
    if ( !dpExists(dp) ) res = -1;
    if ( 0 == res ) res = dpGet(dp,info);
    if ( !dpExists(info) ) res = -1;
    if ( 0 == res )  {
      res = dpGet(info+".HLTFarm.nSubFarms",recv_slots,
                  info+".Storage.recvInfrastructure", recv_infra,
                  info+".Storage.streamInfrastructure", strm_infra,
                  info+".Storage.streamMultiplicity", strm_mult,
                  info+".Storage.streamTypes", strm_types,
                  info+".Storage.recvStrategy", recv_strategy,
                  info+".Storage.strmStrategy", strm_strategy);
    }
  }
  else if ( strpos(stream,"Monitoring") >= 0 )  {
    dp = strtoupper(stream)+":"+m_runInfoDP.text;
    if ( !dpExists(dp) ) res = -1;
    if ( 0 == res ) res = dpGet(dp,info);
    if ( !dpExists(info) ) res = -1;
    if ( 0 == res )  {
      res = dpGet(info+".MonFarm.relayInfrastructure", recv_infra,
                  info+".MonFarm.monInfrastructure", strm_infra,
                  info+".MonFarm.monMultiplicity", strm_mult,
                  info+".MonFarm.monTypes", strm_types,
                  info+".MonFarm.recvStrategy", recv_strategy,
                  info+".MonFarm.strmStrategy", strm_strategy);
    }
  }
  else  {
    StreamControlPanel_checkErrors(res);
    return;
  }
  recv_slots = 0;
  if ( 0 == res )  {
    for(int i=1, n=dynlen(strm_mult); i<n; ++i) 
      recv_slots = recv_slots + strm_mult[i];   
  }
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
  m_strmInfraTaskTypes.text = s;
  s = "";
  for(int i=1, n=dynlen(recv_infra); i<=n; ++i)  {
    s = s + recv_infra[i];
    if ( i < n ) s = s+ "\n";
  }
  m_recvInfraTaskTypes.text = s;
  m_partitionName.text = "Partition:"+partition;
  m_runInfoText.text = "Run info:"+info;
  m_recvSlots.text = recv_slots;
  m_recvStrategy.sbMinimum = 1;
  m_recvStrategy.sbMaximum = 50;
  m_recvStrategy.sbStep = 1;
  m_recvStrategy.text = recv_strategy;

  m_strmSlots.text = strm_slots;
  m_strmStrategy.sbMinimum = 1;
  m_strmStrategy.sbMaximum = 20;
  m_strmStrategy.sbStep = 1;
  m_strmStrategy.text = strm_strategy;
}
//=============================================================================
void StreamControlPanel_checkRecvAllocation()  {
}
//=============================================================================
void StreamControlPanel_checkStrmAllocation()  {
}
//=============================================================================
int StreamControlPanel_AllocSave(string stream, string partition)  {
  // Are you sure ....
  string info;
  dyn_float df;
  dyn_string ds;
  string msg = "Are you sure you want to save the parameters ?";
  ChildPanelOnReturn("vision/MessageInfo","Please Confirm",
                     makeDynString("$1:"+msg,"$2:Cancel","$3:Ok"),50,50,ds,df);
  if ( ds[1] == 0 )   {
    string s0 = m_strmTypes.text, s1 = m_recvInfraTaskTypes.text, s2 = m_strmInfraTaskTypes.text;
    int recv_strategy = m_recvStrategy.text;
    int strm_strategy = m_strmStrategy.text;
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
    int res = -1;
    if ( strpos(stream,"Storage") >= 0 )  {
      res = dpGet(strtoupper(stream)+":"+m_runInfoDP.text,info);
      if ( !dpExists(info) ) res = -1;
      if ( 0 == res )  {
        res = dpSet(info+".Storage.recvInfrastructure", recv_infra,
                    info+".Storage.streamInfrastructure", strm_infra,
                    info+".Storage.streamMultiplicity", mult,
                    info+".Storage.streamTypes", types,
                    info+".Storage.recvStrategy", recv_strategy,
                    info+".Storage.strmStrategy", strm_strategy);
      }
    }
    else if ( strpos(stream,"Monitoring") >= 0 )  {
      res = dpGet(strtoupper(stream)+":"+m_runInfoDP.text,info);
      if ( !dpExists(info) ) res = -1;
      if ( 0 == res )  {
        res = dpSet(info+".MonFarm.relayInfrastructure", strm_infra,
                    info+".MonFarm.monInfrastructure", recv_infra,
                    info+".MonFarm.monMultiplicity", mult,
                    info+".MonFarm.monTypes", types,
                    info+".MonFarm.recvStrategy", recv_strategy,
                    info+".MonFarm.strmStrategy", strm_strategy);
      }
    }
    StreamControlPanel_checkErrors(res);
    StreamControlPanel_initAllocData(stream,partition);
    return res;
  }
  return 0;
}
//=============================================================================
int StreamControlPanel_AllocOk(string stream, string partition)  {
  int res = StreamControlPanel_AllocSave(stream,partition);  
  if ( res == 0 )  {
    PanelOff();
  }
}
//=============================================================================
void StreamControlPanel_AllocCancel(string stream, string partition)  {
  StreamControlPanel_initAllocData(stream, partition);
  PanelOff();
}
//=============================================================================
void StreamControlPanel_AllocReset(string stream, string partition)  {
  StreamControlPanel_initAllocData(stream, partition);
}
//=============================================================================
//
//  Partition content panel
//
//=============================================================================
void StreamControlPanel_init(string stream, string partition)  {
  StreamControl_trace("Stream:"+stream+" Partition:"+partition);
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
  StreamControl_trace("StreamControlPanel_getPartition:"+stream+" Name:"+name);
  dyn_string items = dpNames(stream+"_Slice*.Name","StreamPartition");
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
  dyn_string items = dpNames(stream+"_Slice*.FSMSlice","StreamPartition");
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
  if ( dynlen(dpTypes("StreamConfigurator")) ==0 ) {
    dyn_dyn_string names;
    dyn_dyn_int    types;
    names[1]  = makeDynString ("StreamConfigurator","","","");
    names[2]  = makeDynString ("","Command","","");
    names[3]  = makeDynString ("","State","","");
    types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
    types[2]  = makeDynInt (0,DPEL_STRING,0,0);
    types[3]  = makeDynInt (0,DPEL_STRING,0,0);
    int result = dpTypeCreate(names,types);
    StreamTaskMgr_checkErrors(result);
  }   
  else {
    DebugN("Data type StreamConfigurator are already defined....Nothing to do.");
  }
  if ( dynlen(dpTypes("StreamPartition")) ==0 ) {
    dyn_dyn_string names;
    dyn_dyn_int types;
    names[1]  = makeDynString ("StreamPartition","","","");
    names[2]  = makeDynString ("","FSMSlice","","");
    names[3]  = makeDynString ("","InUse","","");
    names[4]  = makeDynString ("","Name","","");
    names[5]  = makeDynString ("","RunInfo","","");
    names[6]  = makeDynString ("","RecvNodes","","");
    names[7]  = makeDynString ("","RecvSlices","","");
    names[8]  = makeDynString ("","StreamNodes","","");
    names[9]  = makeDynString ("","StreamSlices","","");
    names[10] = makeDynString ("","DataSenders","","");
    names[11] = makeDynString ("","RecvInfrastructure","","");
    names[12] = makeDynString ("","RecvReceivers","","");
    names[13] = makeDynString ("","RecvSenders","","");
    names[14] = makeDynString ("","StreamInfrastructure","","");
    names[15] = makeDynString ("","StreamReceivers","","");
    names[16] = makeDynString ("","StreamSenders","","");

    types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
    types[2]  = makeDynInt (0,DPEL_STRING,0,0);
    types[3]  = makeDynInt (0,DPEL_INT,0,0);
    types[4]  = makeDynInt (0,DPEL_STRING,0,0);
    types[5]  = makeDynInt (0,DPEL_STRING,0,0);
    types[6]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[7]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[8]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[9]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[10] = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[11] = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[12] = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[13] = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[14] = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[15] = makeDynInt (0,DPEL_DYN_STRING,0,0);
    types[16] = makeDynInt (0,DPEL_DYN_STRING,0,0);
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
