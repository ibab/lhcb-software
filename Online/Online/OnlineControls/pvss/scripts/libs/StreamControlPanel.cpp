#uses "ctrlUtils.cpp"

//=============================================================================
int StreamControlPanel_showPanel(string stream,string partition,string ref,string panel,int offsety=130,int offsetx=0)  {
  StreamControl_trace("StreamControlPanel_showPanel:"+stream+" "+partition+" "+ref+" "+panel);
  dyn_string params = makeDynString("$partition:"+partition,"$stream:"+stream,"$offset:"+offsety);
  int res = addSymbol(myModuleName(), myPanelName(), panel, ref, params, offsetx, offsety, 0, 1., 1.);
  return ctrlUtils_checkErrors(res);
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
  if ( dpExists(name+".DataSenders") ) {
    StreamControl_trace("StreamControlPanel_initTasksPanel:"+stream+" "+partition+" "+name+" "+node);
    string font = "Arial,8,-1,5,50,0,0,0,0,0";
    string val, dp = dpSubStr(name,DPSUB_DP);
    dyn_string dataSenders, recvInfrastructure, recvReceivers, recvSenders;
    dyn_string strmInfrastructure, strmReceivers, strmSenders;
    int res = dpGet(name+".DataSenders",dataSenders,
		    name+".RecvInfrastructure",recvInfrastructure,
		    name+".RecvReceivers",recvReceivers,
		    name+".RecvSenders",recvSenders,
		    name+".StreamInfrastructure",strmInfrastructure,
		    name+".StreamReceivers",strmReceivers,
		    name+".StreamSenders",strmSenders
		    );
    ctrlUtils_checkErrors(res);
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
    return;
  }
  m_class1Tasks.visible = 0;
  m_class1TasksFrame.visible = 0;
}
//=============================================================================
void StreamControl_trace(string msg)  {
  DebugN(msg);
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
  string dp = stream+"Alloc.State";
  if ( dpExists(dp) )  {
    int res = dpConnect("StreamControlPanel_CheckDisplay",dp);
    ctrlUtils_checkErrors(res);
    return;
  }
  StreamControlPanel_CheckDisplay("","");
}
//=============================================================================
string StreamControlPanel_showDisplay(string stream, int offset=130)  {
  dyn_string partitions = ctrlUtils_getStrings(stream+"_Slice*.Name","StreamPartition");
  StreamControl_trace("StreamControlPanel_showDisplay> Stream:"+stream+ " Partitions:"+partitions);
  string parts = "";
  for (int i=1, n=dynlen(partitions); i<=n; ++i)  {
    string partition = partitions[i];
    int off = (i-1)*150+offset;
    dyn_string params = makeDynString("$stream:"+stream,"$partition:"+partition,"$offset:"+offset);
    int res = addSymbol(myModuleName(), myPanelName(), "StreamControl/PartitionDisplay", partition,
              params,0,off,0,1,1);
    ctrlUtils_checkErrors(res);
    parts = parts + partition + " ";
  }
  m_displayedPartitions.text = parts;
  return parts;
}
//=============================================================================
string StreamControlPanel_removeDisplay(string stream, string partitions)  {
  StreamControl_trace("StreamControlPanel_removeDisplay:"+stream+" "+partitions);
  int res = 0;// = dpDisconnect("StreamControlPanel_CheckDisplay",stream+".State");
  ctrlUtils_checkErrors(res);
  dyn_string items  = strsplit(partitions," ");
  for (int i=1; i<=dynlen(items); ++i)  {
    string item = strltrim(strrtrim(items[i]," ")," ");
    res = removeSymbol(myModuleName(), myPanelName(), item);
    StreamControl_trace("Remove partition display:"+item);
  }
}
//=============================================================================
void StreamControlPanel_initPartitionDisplay(string stream, string partition)  {
  int flag, res;
  dyn_int    strmMult;
  dyn_string rcvInfra, recvTasks, recvInfraTasks, recvSenderTasks, recvReceiverTasks;
  dyn_string strmInfra, strmTypes, strmTasks, strmInfraTasks, strmReceiverTasks, strmWriterTasks, monStreams;
  dyn_string recv_slices, strm_slices;
  string rundp_name, val, dp, dp_type, name;
  name = StreamControlPanel_getPartition(stream,partition);
  dp_type = dpExists(name) ? dpTypeName(name) : "StreamPartition";
  dp = dpSubStr(name,DPSUB_DP);
  StreamControl_trace("StreamControlPanel_initPartitionDisplay:"+stream+" Partition:"+partition+" Name:"+name+" Typ:"+dp_type+" DP:"+dp);
  recv_slices = ctrlUtils_getStringVectors(dp+".RecvSlices",dp_type);
  strm_slices = ctrlUtils_getStringVectors(dp+".StreamSlices",dp_type);
  bool isMonitoring = strtoupper(stream) == "MONITORING";
  m_frame.text     = "Partition: "+partition;
  ctrlUtils_setListItems("m_recvSlots",recv_slices);
  ctrlUtils_setListItems("m_strmSlots",strm_slices);
  res = dpGet(dp+".RunInfo",rundp_name,
              dp+".RecvReceivers",recvReceiverTasks,
              dp+".RecvSenders",recvSenderTasks,
              dp+".RecvInfrastructure",recvInfraTasks,
              dp+".StreamInfrastructure",strmInfraTasks,
              dp+".StreamReceivers",strmReceiverTasks,
              dp+".StreamSenders",strmWriterTasks
             );
  ctrlUtils_checkErrors(res,"Datapoint:"+dp);
  if ( strtoupper(stream) == "STORAGE" )  {
    res = dpGet(
      rundp_name+".Storage.recvInfrastructure",rcvInfra,
      rundp_name+".Storage.streamInfrastructure",strmInfra,
      rundp_name+".Storage.streamTypes",strmTypes,
      rundp_name+".Storage.streamMultiplicity",strmMult,
      rundp_name+".Storage.storeFlag",flag
      );
  }
  else   if ( isMonitoring )  {
    res = dpGet(
      rundp_name+".MonFarm.relayInfrastructure",rcvInfra,
      rundp_name+".MonFarm.monInfrastructure",strmInfra,
      rundp_name+".MonFarm.monTypes",strmTypes,
      rundp_name+".MonFarm.monStreams",monStreams,
      rundp_name+".MonFarm.monMultiplicity",strmMult,
      rundp_name+".MonFarm.monFlag",flag
      );
  }
  else  {
    dyn_string rcvTypes;
    res = 0;//dpGet(partition+".Activity",rundp_name);
    if ( 0 == res )  {
      res = dpGet(rundp_name+".Storage.recvInfrastructure", rcvInfra,
                  rundp_name+".Storage.streamInfrastructure", strmInfra,
                  rundp_name+".Storage.streamTypes", strmTypes,
                  rundp_name+".Storage.recvTypes", rcvTypes,
                  rundp_name+".Storage.storeFlag", flag);
      if ( 0 == res )  {
        dynAppend(rcvInfra,rcvTypes);
      }
    }
  }
  ctrlUtils_checkErrors(res);
  string val = stream+" Flag:\nReceive infrastructure:\nStream infrastructure:\nOutput streams:\t\t";
  for(int i=1; i<=dynlen(strmTypes); ++i)
    if ( i%3 == 0 ) val = val + "\n";
  val = val + "\nReceiving layer:\t\nStreaming layer:\t";
  m_desc.text = val;
  val = flag+"\n";
  for(int i=1; i<=dynlen(rcvInfra); ++i)  
    val = val + rcvInfra[i] + "  ";
  val = val + "\n";
  for(int i=1; i<=dynlen(strmInfra); ++i)  
    val = val + strmInfra[i] + "  ";
  val = val + "\n";
  for(int i=1; i<=dynlen(strmTypes); ++i)  {
    val = val + strmTypes[i] + "/";
    if ( dynlen(strmMult)>0 ) val = val + strmMult[i];
    if ( isMonitoring ) val = val + "/" + monStreams[i];
    val = val + " ";
    if ( i%3 == 0 ) val = val + "\n";
  }
  val = val + "\n" + dynlen(recv_slices) + " slots used";
  if ( dynlen(recvInfraTasks) > 0 )
    val = val + " with " + dynlen(recvInfraTasks) + " infrastructure task(s) ";
  if ( dynlen(recvReceiverTasks) > 0 )
    val = val + dynlen(recvReceiverTasks) + " receiver(s) ";
  if ( dynlen(recvSenderTasks) > 0 )
    val = val + dynlen(recvSenderTasks) + " sender(s) ";
  val = val + "\n" + dynlen(strm_slices) + " slots used";
  if ( dynlen(strmInfraTasks) > 0 )
    val = val + " with " + dynlen(strmInfraTasks) + " infra task(s) ";
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
  string sys = dpSubStr(dp,DPSUB_SYS);
  string slice = dpSubStr(dp,DPSUB_DP_EL);
  sys = substr(sys,0,strlen(sys)-1);
  slice = substr(slice,0,strpos(slice,"."));
  if ( dpExists(value+".general.partName") )  {
    m_sliceNotInUse.visible = 0;
    dpGet(value+".general.partName",partition);
    StreamControl_trace("Rebuilding Stream allocation panel with runInfo:"+value+ "Partition:"+value);
    StreamControlPanel_initAllocPanel(stream,partition);
    return;
  }
  else if ( dpExists(value) )   {
    m_sliceNotInUse.visible = 0;
    StreamControl_trace("Rebuilding Stream allocation panel with runInfo:"+value+ "Partition:"+slice);
    StreamControlPanel_initAllocPanel(sys,slice);
    m_streamName.text = substr(slice,0,strpos(slice,"_Slice"));
    //m_partition.enabled = 0;
    return;
  }
  m_sliceNotInUse.visible = 1;
  StreamControl_trace("System:"+sys+" DP:"+dp+" Value:"+value+" Slice:"+slice+" Stream:"+stream);
  setValue("m_sliceNotInUse","foreCol","red");
  setValue("m_sliceNotInUse","text","The streaming slice\n"+slice+"\nin system "+sys+"\nis currently not used.");
  LayerOff(2);
}
//=============================================================================
void StreamControlPanel_startAllocPanel(string slice, string stream)  {
  string run_info;
  string sys = strtoupper(stream);
  string sysId = getSystemId(sys+":");
  string dp = sys+":"+slice+".RunInfo";
  if ( getSystemId(sys+":") < 0 )  {
    sys = getSystemName();
    sys = substr(sys,0,strlen(sys)-1);
    dp = sys+":"+slice+".RunInfo";
  }
  StreamControl_trace("System ID:"+sysId+" System:"+sys+" DP:"+dp+" Slice:"+slice+" Stream:"+stream);
  m_streamName.text = stream;
  if ( dpExists(dp) )  {
    dpConnect("StreamControlPanel_CheckAllocPanel",dp);
    return;
  }
  StreamControl_trace("StreamControlPanel_startAllocPanel> The Datapoint:"+dp+" does not exist.");
  setValue("m_sliceNotInUse","backCol","_Transparent");
  setValue("m_sliceNotInUse","foreCol","red");
  setValue("m_sliceNotInUse","text","The streaming slice\n"+slice+"\nin system "+sys+"\nis currently not used.");
  LayerOff(2);
  m_sliceNotInUse.visible = 1;
}
//=============================================================================
void StreamControlPanel_initAllocPanel(string name, string partition)  {
  string font = "Arial,8,-1,5,50,0,0,0,0,0";
  m_streamName.text = name;
  m_partition.text = partition;
  StreamControlPanel_setupAllocWidgets();
  m_partition.visible       = 0;
  m_streamName.visible      = 1;
  m_streamName.backCol      = "yellow";
  m_partition.visible       = 1;
  m_partition.backCol       = "yellow";
  m_partitionName.visible   = 1;
  m_partitionName.text      = "Partition:"+partition;
  m_runInfoText.visible     = 1;
  m_runInfoText.text        = "Run info:"+partition;
  m_recvCheck.visible       = 0;
  m_strmCheck.visible       = 0;
  m_sliceNotInUse.visible   = 0;
  m_recvInfraTaskTypes.font = font;
  m_strmInfraTaskTypes.font = font;
  m_strmTypes.font          = font;
  m_strmTypesText.visible   = 1;
  StreamControlPanel_initAllocData(name,partition);
  LayerOn(2);
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
  int recv_slots = 1, strm_slots = 0, recvStrategy = 2, strmStrategy = 2;
  string dp, info = "LBECS:"+partition+"_RunInfo";
  dyn_int strmMult;
  dyn_string rcvInfra, strmInfra, strmTypes, streams;
  bool isStorage = strpos(stream,"Storage") >= 0;
  bool isMonitoring = strpos(stream,"Monitoring") >= 0;
  int res = 0;
  dp = strtoupper(stream)+":"+m_runInfoDP.text;
  if ( !dpExists(dp) ) {
    res = -1;
    StreamControl_trace("StreamControlPanel_initAllocData> dpExists(dp)",res,dp);
  }
  if ( 0 == res ) res = dpGet(dp,info);
  if ( !dpExists(info) )   {
    StreamControl_trace("StreamControlPanel_initAllocData> dpExists(dp)",res,dp,info);
    res = -1;
  }
  if ( isStorage )  {
    if ( 0 == res )  {
      res = dpGet(info+".HLTFarm.nSubFarms",recv_slots,
                  info+".Storage.recvInfrastructure", rcvInfra,
                  info+".Storage.streamInfrastructure", strmInfra,
                  info+".Storage.streamMultiplicity", strmMult,
                  info+".Storage.streamTypes", strmTypes,
                  info+".Storage.recvStrategy", recvStrategy,
                  info+".Storage.strmStrategy", strmStrategy);
    }
    m_strmTypesText.text = "OutputStreams/Multiplicity:\n[Example: B2pipi/2,BBincl/5]";
  }
  else if ( isMonitoring )  {
    if ( 0 == res )  {
      res = dpGet(info+".MonFarm.relayInfrastructure", rcvInfra,
                  info+".MonFarm.monInfrastructure", strmInfra,
                  info+".MonFarm.monMultiplicity", strmMult,
                  info+".MonFarm.monTypes", strmTypes,
                  info+".MonFarm.monStreams", streams,
                  info+".MonFarm.recvStrategy", recvStrategy,
                  info+".MonFarm.strmStrategy", strmStrategy);
    }
    m_strmTypesText.text = "Mon Type/Multiplicity/Input:\n[Example: BBincl/5/Stream2]";
  }
  else  {
    dyn_string rcvTypes;
    res = dpGet(info+".Storage.recvInfrastructure", rcvInfra,
                info+".Storage.streamInfrastructure", strmInfra,
                info+".Storage.streamTypes", strmTypes,
                info+".Storage.recvTypes", rcvTypes,
                info+".Storage.recvStrategy", recvStrategy,
                info+".Storage.strmStrategy", strmStrategy);
    if ( 0 == res )  {
      dynAppend(rcvInfra,rcvTypes);
      for(int i=1; i<=dynlen(strmTypes); ++i) dynAppend(strmMult,1);
      m_Reset.visible = 1;
      m_Save.visible = 0;
    }
    if ( 0 != res ) LayerOff(2);
    m_strmTypesText.text = "Data streamers:";
  }
  StreamControl_trace("StreamControlPanel_initAllocData > RunInfo:"+info+" Stream:"+stream+" Partition:"+partition);
  if ( 0 != res )  {
    ctrlUtils_checkErrors(res);
    return;
  }
  recv_slots = 0;
  if ( 0 == res )  {
    for(int i=1, n=dynlen(strmMult); i<n; ++i) 
      recv_slots = recv_slots + strmMult[i];   
  }
  string s= "";
  for(int i=1, n=dynlen(strmMult); i<=n; ++i)  {
    strm_slots = strm_slots + strmMult[i];
    s = s + strmTypes[i] + "/" + strmMult[i];
    if ( isMonitoring ) s = s +"/" + streams[i];
    if ( i < n ) s = s+ "\n";
  }
  m_strmTypes.text = s;
  s = "";
  for(int i=1, n=dynlen(strmInfra); i<=n; ++i)  {
    s = s + strmInfra[i];
    if ( i < n ) s = s+ "\n";
  }
  m_strmInfraTaskTypes.text = s;
  s = "";
  for(int i=1, n=dynlen(rcvInfra); i<=n; ++i)  {
    s = s + rcvInfra[i];
    if ( i < n ) s = s+ "\n";
  }
  if ( recvStrategy <= 0 ) recvStrategy = 1;
  if ( strmStrategy <= 0 ) strmStrategy = 1;
  m_recvInfraTaskTypes.text = s;
  m_partitionName.text = "Partition:"+partition;
  m_runInfoText.text = "Run info:"+info;
  m_recvSlots.text = recv_slots;
  m_recvStrategy.sbMinimum = 1;
  m_recvStrategy.sbMaximum = 50;
  m_recvStrategy.sbStep = 1;
  m_recvStrategy.text = recvStrategy;

  m_strmSlots.text = strm_slots;
  m_strmStrategy.sbMinimum = 1;
  m_strmStrategy.sbMaximum = 20;
  m_strmStrategy.sbStep = 1;
  m_strmStrategy.text = strmStrategy;
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
    
    bool isStorage = strpos(stream,"Storage") >= 0;
    bool isMonitoring = strpos(stream,"Monitoring") >= 0;
    string s0 = m_strmTypes.text, s1 = m_recvInfraTaskTypes.text, s2 = m_strmInfraTaskTypes.text;
    int recvStrategy = m_recvStrategy.text;
    int strmStrategy = m_strmStrategy.text;
    //strreplace(s0,"\n"," ");
    //while ( strpos(s0,"  ")>0 ) strreplace(s0,"  "," ");
    strreplace(s1,"\n"," ");
    while ( strpos(s1,"  ")>0 ) strreplace(s1,"  "," ");
    strreplace(s2,"\n"," ");
    while ( strpos(s2,"  ")>0 ) strreplace(s2,"  "," ");
    dyn_string strm_typs  = strsplit(s0,"\n");
    dyn_string rcvInfra = strsplit(s1," ");
    dyn_string strmInfra = strsplit(s2," ");
    dyn_int mult;
    dyn_string items, types, strm;
    
    // DebugN("Length:",dynlen(strm_typs),strm_typs);
    
    for(int i=1, n=dynlen(strm_typs); i<=n; ++i)  {
      items = strsplit(strm_typs[i],"/");
      if ( isStorage && dynlen(items) != 2 )  {
        ChildPanelOn("vision/MessageInfo1","Bad input",makeDynString("Parameter 'Stream Types' is incorrect"),50,50);
        return 1;
      }
      if ( isMonitoring && dynlen(items) != 3 )  {
        ChildPanelOn("vision/MessageInfo1","Bad input",makeDynString("Parameter 'Stream Types' is incorrect"),50,50);
        return 1;
      }
      dynAppend(types,items[1]);
      dynAppend(mult,items[2]);
      if ( isMonitoring ) dynAppend(strm,items[3]);
    }
    int res = -1;
    if ( isStorage )  {
      res = dpGet(strtoupper(stream)+":"+m_runInfoDP.text,info);
      if ( !dpExists(info) ) res = -1;
      if ( 0 == res )  {
        res = dpSet(info+".Storage.recvInfrastructure", rcvInfra,
                    info+".Storage.streamInfrastructure", strmInfra,
                    info+".Storage.streamMultiplicity", mult,
                    info+".Storage.streamTypes", types,
                    info+".Storage.recvStrategy", recvStrategy,
                    info+".Storage.strmStrategy", strmStrategy);
      }
    }
    else if ( strpos(stream,"Monitoring") >= 0 )  {
      res = dpGet(strtoupper(stream)+":"+m_runInfoDP.text,info);
      if ( !dpExists(info) ) res = -1;
      if ( 0 == res )  {
        res = dpSet(info+".MonFarm.relayInfrastructure", rcvInfra,
                    info+".MonFarm.monInfrastructure", strmInfra,
                    info+".MonFarm.monMultiplicity", mult,
                    info+".MonFarm.monTypes", types,
                    info+".MonFarm.monStreams", strm,
                    info+".MonFarm.recvStrategy", recvStrategy,
                    info+".MonFarm.strmStrategy", strmStrategy);
      }
    }
    ctrlUtils_checkErrors(res);
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
  ctrlUtils_checkErrors(res);
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
  StreamControl_trace("StreamControlPanel_ActionDone:"+dp+" --> "+result);
  // READY/StreamSystem/LHCb/203
  dyn_string itms = strsplit(result,"/");
  StreamControl_trace("StreamControlPanel_ActionDone:"+itms);
  if ( dynlen(itms)>=3 )  {
    if ( itms[2] == m_streamName.text && itms[3] == m_partition.text )  {
      StreamControl_trace("StreamControlPanel_ActionDone:"+result+" GOT IT!!!");
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
  ctrlUtils_setListItems("m_partitionList",ctrlUtils_getStrings(name+"_Slice*.Name","StreamPartition"));
  m_allSlotFrame.enabled = 1;
  ctrlUtils_setListItems("m_allRecvSlots",ctrlUtils_getStringVectors(name+"_Slice*.RecvSlices","StreamPartition"));
  ctrlUtils_setListItems("m_allStrmSlots",ctrlUtils_getStringVectors(name+"_Slice*.StreamSlices","StreamPartition"));

  m_freeSlotFrame.enabled = 1;
  ctrlUtils_setListItems("m_freeRecvSlots",ctrlUtils_getStringVectors(name+".RecvSlices","StreamControl"));
  ctrlUtils_setListItems("m_freeStrmSlots",ctrlUtils_getStringVectors(name+".StreamSlices","StreamControl"));
  StreamControl_trace("Show Stream summary information.");
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
  return ctrlUtils_checkErrors(res);
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
  return ctrlUtils_checkErrors(res);
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
  return ctrlUtils_checkErrors(res);
}
//=============================================================================
string StreamControlPanel_getPartition(string stream,string name,int dp_type=DPSUB_DP)  {
  StreamControl_trace("StreamControlPanel_getPartition:"+stream+" Name:"+name);
  dyn_string items = dpNames(stream+"_Slice*.Name","StreamPartition");
  if ( dynlen(items)>0 ) {
    dyn_string values;
    int res = dpGet(items,values);
    if ( 0 == res )  {
      for(int i=1; i<=dynlen(values); i++) {
	if ( name == values[i] )  {
	  string dp = dpSubStr(items[i],dp_type);
	  return dp;
	}
      }
    }
  }
  return "";
}
//=============================================================================
string StreamControlPanel_getPartitionNameFromSlice(string stream,string name)  {
  StreamControl_trace("StreamControlPanel_getPartitionNameFromSlice:"+stream+" Name:"+name);
  dyn_string items = dpNames(stream+"_Slice*.FSMSlice","StreamPartition");
  if ( dynlen(items)>0 ) {
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
  }
  return "";
}
//=============================================================================
void StreamControlPanel_freePartition(string stream,string name) {
  int res = dpSet(stream+".Command","free('"+name+"')");
  ctrlUtils_checkErrors(res);
}
//=============================================================================
bool StreamControlPanel_showPartition(string stream,string name) {
  string partition = StreamControlPanel_getPartition(stream,name);
  string dp_type = dpExists(name) ? dpTypeName(name) : "StreamPartition";
  if ( strlen(partition)>0 )  {
    int flag;
    string val, dp = dpSubStr(partition,DPSUB_DP);
    // StreamControl_trace("Stream:"+stream);
    // StreamControl_trace("Name:   "+name);
    // StreamControl_trace("DP:     "+dp);
    dyn_int    strmMult;
    dyn_string rcvInfra, strmInfra, strmTypes;
    dyn_string recv_slices = ctrlUtils_getStringVectors(dp+".RecvSlices",dp_type);
    dyn_string strm_slices = ctrlUtils_getStringVectors(dp+".StreamSlices",dp_type);
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
    ctrlUtils_setListItems("m_allRecvSlots",recv_slices);
    ctrlUtils_setListItems("m_allStrmSlots",strm_slices);
    int res = dpGet(name+"_RunInfo.Storage.recvInfrastructure",rcvInfra,
      name+"_RunInfo.Storage.streamInfrastructure",strmInfra,
      name+"_RunInfo.Storage.streamTypes",strmTypes,
      name+"_RunInfo.Storage.streamMultiplicity",strmMult,
      name+"_RunInfo.Storage.storeFlag",flag
      );
    ctrlUtils_checkErrors(res);
    // StreamControl_trace("rcvInfra: "+rcvInfra);
    // StreamControl_trace("strmInfra:"+strmInfra);
    // StreamControl_trace("strmTypes:"+strmTypes);
    // StreamControl_trace("strmMult: "+strmMult);
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
  //StreamControl_trace(part_name+" >> " + m_tableSteer.number()+" -- "+button);
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
