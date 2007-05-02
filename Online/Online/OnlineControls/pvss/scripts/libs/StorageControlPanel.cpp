//=============================================================================
bool StorageControlPanel_checkErrors(int res)  {
  if ( res != 0 || dynlen(getLastError()) > 0 )  {
    string err = " Err="+res+","+getLastError();
    DebugN(" Err="+res+","+getLastError());
    ChildPanelOn("vision/MessageInfo1","Error occurred",makeDynString(err),50,50);
  }
  return res == 0;
}
//=============================================================================
void StorageControlPanel_setListItems(string shape_name, dyn_string values)  {
  shape s = getShape(shape_name);
  s.deleteAllItems();
  for(int k=1; k<=dynlen(values); k++) 
    s.appendItem(values[k]);
}
//=============================================================================
dyn_string StorageControlPanel_getStrings(string dp, string type)  {
  dyn_string items = dpNames(dp,type);
  dyn_string values, result;
  int res = dpGet(items,values);
  if ( StorageControlPanel_checkErrors(res) )  {
    for(int i=1; i<=dynlen(values); i++) {
      if ( strlen(values[i])>0 ) dynAppend(result, values[i]);
    }
  }
  dynSortAsc(values);
  return result;
}
//=============================================================================
dyn_string StorageControlPanel_getStringVectors(string dp,string type)  {
  dyn_dyn_string slots;
  dyn_string values, items = dpNames(dp,type);
  int res = dpGet(items,slots);
  if ( StorageControlPanel_checkErrors(res) )  {
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
int StorageControlPanel_showPanel(string storage,string partition,string ref,string panel,int offset=130)  {
  //DebugN("StorageControlPanel_showPanel:",storage,partition);
  dyn_string params = makeDynString("$partition:"+partition,"$storage:"+storage,"$offset:"+offset);
  //DebugN("StorageControlPanel_showPanel:",myModuleName(), myPanelName(),"Offset=",offset,params);
  int res = addSymbol(myModuleName(), myPanelName(), panel, ref, params, 0, offset, 0, 1., 1.);
  return StorageControlPanel_checkErrors(res);
}
//=============================================================================
void StorageControlPanel_hidePanel(string ref)  {
  removeSymbol(myModuleName(), myPanelName(), ref);
}
//=============================================================================
//
//  Node/partition summary display
//
//=============================================================================
void StorageControlPanel_addTasks(dyn_string tasks,string node,string& val)  {
  for(int i=1, n=dynlen(tasks); i<=n; ++i)  {
    dyn_string itms = strsplit(tasks[i],"/");
    if ( itms[1] == node )
      val = val + itms[1] + "::" + itms[2]+"  [" + itms[3] + "]\n";
  }
}
//=============================================================================
//void StorageControlPanel_initStoragePartitionNodeDisplay(string storage, string partition, string name, string node)  {
void StorageControlPanel_initXXXX(string storage, string partition, string name, string node)
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
  StorageControlPanel_checkErrors(res);
  string val = "";
  StorageControlPanel_addTasks(recvInfraTasks,node,val);
  StorageControlPanel_addTasks(strmInfraTasks,node,val);
  m_class0TasksFrame.text = "Class 0 Tasks of Partition: "+partition;
  m_class0Tasks.backCol   = "_Transparent";
  m_class0Tasks.text = val;
  m_class0Tasks.readOnly = 1;
  m_class0Tasks.backCol = "white";
  //DebugN("Class 0 tasks:",val);
  val = "";
  StorageControlPanel_addTasks(recvReceiverTasks,node,val);
  StorageControlPanel_addTasks(recvSenderTasks,node,val);
  StorageControlPanel_addTasks(strmReceiverTasks,node,val);
  StorageControlPanel_addTasks(strmWriterTasks,node,val);
  m_class1TasksFrame.text = "Class 1 Tasks of Partition: "+partition;
  m_class1Tasks.backCol = "_Transparent";
  m_class1Tasks.text = val;
  m_class1Tasks.readOnly = 1;
  m_class1Tasks.backCol = "white";

  //DebugN("Class 0 tasks:",val);
}
//=============================================================================
//
//  Storage control summary display
//
//=============================================================================
void StorageControlPanel_CheckDisplay(string dp, string value)  {
  int offset = isDollarDefined("$offset") ? getDollarValue("$offset") : 130;
  // DebugN("Rebuilding displays.....",m_storageName.text,m_displayedPartitions.text,offset);
  StorageControlPanel_removeDisplay(m_storageName.text,m_displayedPartitions.text);
  m_displayedPartitions.text = StorageControlPanel_showDisplay(m_storageName.text,offset);
}
//=============================================================================
string StorageControlPanel_initDisplay(string storage, string partition, int offset=130)  {
  // DebugN("Connect to :",storage+".State");
  m_storageName.text       = storage;
  //m_storageName.visible    = 1;
  m_storageName.backCol    = "yellow";
  //m_command.visible        = 1;
  m_command.text           = "Refresh";
  m_partition.visible      = 0;
  m_displayedPartitions.visible = 0;
  int res = dpConnect("StorageControlPanel_CheckDisplay",storage+".State");
  StorageControlPanel_checkErrors(res);
}
//=============================================================================
string StorageControlPanel_showDisplay(string storage, int offset=130)  {
  dyn_string partitions = StorageControlPanel_getStrings(storage+"_Partition*.Name","StoragePartition");
  string parts = "";
  for (int i=1, n=dynlen(partitions); i<=n; ++i)  {
    string partition = partitions[i];
    int off = (i-1)*150+offset;
    dyn_string params = makeDynString("$storage:"+storage,"$partition:"+partition,"$offset:"+offset);
    int res = addSymbol(myModuleName(), myPanelName(), "StorageControl/PartitionDisplay", partition,
              params,0,off,0,1,1);
    StorageControlPanel_checkErrors(res);
    parts = parts + partition + " ";
  }
  m_displayedPartitions.text = parts;
  return parts;
}
//=============================================================================
string StorageControlPanel_removeDisplay(string storage, string partitions)  {
  // DebugN("Disconnect from :",storage+".State");
  int res;// = dpDisconnect("StorageControlPanel_CheckDisplay",storage+".State");
  StorageControlPanel_checkErrors(res);
  dyn_string items  = strsplit(partitions," ");
  for (int i=1; i<=dynlen(items); ++i)  {
    string item = strltrim(strrtrim(items[i]," ")," ");
    res = removeSymbol(myModuleName(), myPanelName(), item);
    //DebugN("Remove partition display:"+item);
  }
}
//=============================================================================
void StorageControlPanel_initPartitionDisplay(string storage, string partition)  {
  string name = StorageControlPanel_getPartition(storage,partition);
  //DebugN("StorageControlPanel_initPartitionDisplay:",storage,partition,name);
  if ( strlen(name)>0 )  {
    StorageControlPanel_initStoragePartitionDisplay(storage,partition,name);
  }
}
//=============================================================================
void StorageControlPanel_initStoragePartitionDisplay(string storage, string partition, string name)  {
  int flag;
  string val, dp = dpSubStr(name,DPSUB_DP);
  dyn_int    strmMult;
  dyn_string rcvInfra, recvTasks, recvInfraTasks, recvSenderTasks, recvReceiverTasks;
  dyn_string strmInfra, strmTypes, strmTasks, strmInfraTasks, strmReceiverTasks, strmWriterTasks;
  dyn_string recv_slices = StorageControlPanel_getStringVectors(dp+".RecvSlices","StoragePartition");
  dyn_string strm_slices = StorageControlPanel_getStringVectors(dp+".StreamSlices","StoragePartition");
  m_frame.text     = "DAQ Partition: "+partition;
  StorageControlPanel_setListItems("m_recvSlots",recv_slices);
  StorageControlPanel_setListItems("m_strmSlots",strm_slices);
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
  StorageControlPanel_checkErrors(res);
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
//
//  ALLOCATION DISPLAY
//
//=============================================================================
void StorageControlPanel_initAllocPanel(string name,string partition)  {
  m_storageName.text = name;
  m_partition.text = partition;
  StorageControlPanel_setupAllocWidgets();
  m_partition.visible = 0;
  m_storageName.visible = 1;
  m_storageName.backCol = "yellow";
  m_partition.visible = 1;
  m_partition.backCol = "yellow";
  m_partitionName.visible = 1;
  m_partitionName.text = "Partition:"+partition;
  m_recvSlots.enabled = 0;
  m_strmSlots.enabled = 0;
  m_recvCheck.visible = 0;
  m_strmCheck.visible = 0;
  StorageControlPanel_initAllocData(partition);
}
//=============================================================================
void StorageControlPanel_setupAllocWidgets()  {
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
void StorageControlPanel_initAllocData(string partition)  {
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
  StorageControlPanel_checkErrors(res);
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
void StorageControlPanel_checkRecvAllocation()  {
}
//=============================================================================
void StorageControlPanel_checkStrmAllocation()  {
}
//=============================================================================
int StorageControlPanel_AllocSave(string partition)  {
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
    StorageControlPanel_checkErrors(res);
    StorageControlPanel_initAllocData(partition);
    return res;
  }
  return 0;
}
//=============================================================================
int StorageControlPanel_AllocOk()  {
  int res = StorageControlPanel_AllocSave();  
  if ( res == 0 )  {
    PanelOff();
  }
}
//=============================================================================
void StorageControlPanel_AllocCancel(string partition)  {
  StorageControlPanel_initAllocData(partition);
  PanelOff();
}
//=============================================================================
void StorageControlPanel_AllocReset(string partition)  {
  StorageControlPanel_initAllocData(partition);
}
//=============================================================================
//
//  Partition content panel
//
//=============================================================================
void StorageControlPanel_init(string storage, string partition)  {
  // DebugN("Storage:"+storage+" Partition:"+partition);
  m_storageName.text = storage;
  m_partition.text   = partition;
  StorageControlPanel_setupWidgets();
  m_partition.visible = 0;
  m_storageName.visible = 0;

  m_storageName.visible = 1;
  m_storageName.backCol = "yellow";
  m_partition.visible = 1;
  m_partition.backCol = "yellow";

  m_partitionName.text = "Partition:"+partition;
  m_partitionName.visible = 1;
  m_partitionList.enabled = 1;
  if ( strlen(partition)>0 )  {
    if ( !StorageControlPanel_showPartition(storage,partition) )  {
      LayerOff(2);
      LayerOff(3);
      LayerOff(4);
    }
  }
  else
    StorageControlPanel_showSummary(storage);
  int res = dpConnect("StorageControlPanel_ActionDone",storage+".State");
  StorageControlPanel_checkErrors(res);
}
//=============================================================================
void StorageControlPanel_setupWidgets()  {
  m_partitionName.text        = "Partition: ----";
  m_partitionName.toolTipText = "Name of the selected partition";

  m_partitionList.toolTipText = "Allocated partition slots";

  m_allSlotFrame.text         = "Used Storage Slots";
  m_allSlotFrame.toolTipText  = "Information about used slots in the storage system";

  m_freeSlotFrame.text        = "Free Storage Slots";
  m_freeSlotFrame.toolTipText = "Information about free slots in the storage system";

  m_showSummary.text          = "Select Partition";
  m_showSummary.toolTipText   = "Go back and show partition summary for further selection";  

  m_freePartition.text        = "Free Partition";
  m_freePartition.toolTipText = "Free partition selected in the partition list";

  m_inputFrame.text           = "Input Parameters";
  m_inputFrame.toolTipText    = "Parameters used to define this storage configuration.";
}
//=============================================================================
void StorageControlPanel_ActionDone(string dp)  {
  string result = "";
  int res = dpGet(dp,result);
  //DebugN("StorageControlPanel_ActionDone:"+dp+" --> "+result);
  // READY/StorageSystem/LHCb/203
  dyn_string itms = strsplit(result,"/");
  //DebugN("StorageControlPanel_ActionDone:"+itms);
  if ( dynlen(itms)>=3 )  {
    if ( itms[2] == m_storageName.text && itms[3] == m_partition.text )  {
      DebugN("StorageControlPanel_ActionDone:"+result+" GOT IT!!!");
    }
  }
}
//=============================================================================
void StorageControlPanel_initTable(dyn_string new_cols) {
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
void StorageControlPanel_adjustTable(dyn_string cols) {
  for(int j=0; j<dynlen(cols); ++j)  m_table.adjustColumn(j);
}
//=============================================================================
void StorageControlPanel_showSummary()  {
  string name = m_storageName.text;
  LayerOn(1);
  LayerOn(3);
  m_table.visible = 1;
  m_tableSteer.visible = 1;
  m_showSummary.visible= 0;
  StorageControlPanel_setListItems("m_partitionList",StorageControlPanel_getStrings(name+"_Partition*.Name","StoragePartition"));
  m_allSlotFrame.enabled = 1;
  StorageControlPanel_setListItems("m_allRecvSlots",StorageControlPanel_getStringVectors(name+"_Partition*.RecvSlices","StoragePartition"));
  StorageControlPanel_setListItems("m_allStrmSlots",StorageControlPanel_getStringVectors(name+"_Partition*.StreamSlices","StoragePartition"));

  m_freeSlotFrame.enabled = 1;
  StorageControlPanel_setListItems("m_freeRecvSlots",StorageControlPanel_getStringVectors(name+".RecvSlices","StorageControl"));
  StorageControlPanel_setListItems("m_freeStrmSlots",StorageControlPanel_getStringVectors(name+".StreamSlices","StorageControl"));
  DebugN("Show Storage summary information.");
  LayerOff(2);
}
//=============================================================================
bool StorageControlPanel_showReceivingTasks(string name)   {
  dyn_string values;
  int res = dpGet(name+"_RunInfo.HLTFarm.receiverTasks", values);
  if ( res == 0 ) {
    dyn_string cols = makeDynString("HLT Node","Rcv Node","Receiver Task Name"," Task Type");
    StorageControlPanel_initTable(cols);
    for(int i=1; i<=dynlen(values); ++i)  {
      dyn_string itms = strsplit(values[i],"/");
      m_table.appendLine(cols[1],itms[1],cols[2],itms[2],cols[3],itms[3],cols[4],itms[4]);
    }
    StorageControlPanel_adjustTable(cols);
  }
  return StorageControlPanel_checkErrors(res);
}
//=============================================================================
bool StorageControlPanel_showRecvSenderTasks(string name)   {
  dyn_string values;
  int res = dpGet(name+"_RunInfo.Storage.recvSenders", values);
  if ( res == 0 )  {
    dyn_string cols = makeDynString("Recv Node","Sender Task Name","Task Type","Stream Node","Recv Task");
    StorageControlPanel_initTable(cols);
    for(int i=1; i<=dynlen(values); ++i)  {
      dyn_string itms = strsplit(values[i],"/");
      m_table.appendLine(cols[1],itms[1],cols[4],itms[4],cols[2],itms[2],cols[3],itms[3],cols[5],itms[5]);
    }
    StorageControlPanel_adjustTable(cols);
  }
  return StorageControlPanel_checkErrors(res);
}
//=============================================================================
bool StorageControlPanel_showInfrastructure(string name)  {
  dyn_string values;
  int res = dpGet(name,values);
  if ( res == 0 )  {
    dyn_string cols = makeDynString("Node","Task Name","Task Type");
    StorageControlPanel_initTable(cols);
    for(int i=1; i<=dynlen(values); ++i)  {
      dyn_string itms = strsplit(values[i],"/");
      m_table.appendLine(cols[1],itms[1],cols[2],itms[2],cols[3],itms[3]);
    }
    StorageControlPanel_adjustTable(cols);
  }
  return StorageControlPanel_checkErrors(res);
}
//=============================================================================
string StorageControlPanel_getPartition(string storage,string name)  {
  dyn_string items = dpNames(storage+"_Partition*.Name","StoragePartition");
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
string StorageControlPanel_getPartitionNameFromSlice(string storage,string name)  {
  // DebugN("StorageControlPanel_getPartitionNameFromSlice:"+storage+" Name:"+name);
  dyn_string items = dpNames(storage+"_Partition*.FSMSlice","StoragePartition");
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
void StorageControlPanel_freePartition(string storage,string name) {
  int res = dpSet(storage+".Command","free('"+name+"')");
  StorageControlPanel_checkErrors(res);
}
//=============================================================================
bool StorageControlPanel_showPartition(string storage,string name) {
  string partition = StorageControlPanel_getPartition(storage,name);
  if ( strlen(partition)>0 )  {
    int flag;
    string val, dp = dpSubStr(partition,DPSUB_DP);
    // DebugN("Storage:"+storage);
    // DebugN("Name:   "+name);
    // DebugN("DP:     "+dp);
    dyn_int    strmMult;
    dyn_string rcvInfra, strmInfra, strmTypes;
    dyn_string recv_slices = StorageControlPanel_getStringVectors(dp+".RecvSlices","StoragePartition");
    dyn_string strm_slices = StorageControlPanel_getStringVectors(dp+".StreamSlices","StoragePartition");
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
    StorageControlPanel_setListItems("m_allRecvSlots",recv_slices);
    StorageControlPanel_setListItems("m_allStrmSlots",strm_slices);
    int res = dpGet(name+"_RunInfo.Storage.recvInfrastructure",rcvInfra,
      name+"_RunInfo.Storage.streamInfrastructure",strmInfra,
      name+"_RunInfo.Storage.streamTypes",strmTypes,
      name+"_RunInfo.Storage.streamMultiplicity",strmMult,
      name+"_RunInfo.Storage.storeFlag",flag
      );
    StorageControlPanel_checkErrors(res);
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
    StorageControlPanel_showReceivingTasks(name);
    return true;
  }
  return false;
}
//=============================================================================
void StorageControlPanel_radioSelectionChanged(int button, string part_name)  {
  //DebugN(part_name+" >> " + m_tableSteer.number()+" -- "+button);
  switch(button) {
    case 0:  StorageControlPanel_showReceivingTasks(part_name);
      return;
    case 1:  StorageControlPanel_showRecvSenderTasks(part_name);
      return;
    case 2:  StorageControlPanel_showInfrastructure(part_name+"_RunInfo.Storage.recvInfrastructureTasks");
      return;
    case 3:  StorageControlPanel_showInfrastructure(part_name+"_RunInfo.Storage.streamReceivers");
      return;
    case 4:  StorageControlPanel_showInfrastructure(part_name+"_RunInfo.Storage.streamWriters");
      return;
    case 5:  StorageControlPanel_showInfrastructure(part_name+"_RunInfo.Storage.streamInfrastructureTasks");
      return;
  }
}
//=============================================================================
void StorageControlPanel_install()  {
  if ( dynlen(dpTypes("StorageControl")) ==0 ) {
    dyn_dyn_string names;
    dyn_dyn_int types;
    names[1]  = makeDynString ("StorageControl","","","");
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
    StorageTaskMgr_checkErrors(result);
  }   
  else
    DebugN("Data type StorageControl are already defined....Nothing to do.");

  if ( dynlen(dpTypes("StoragePartition")) ==0 ) {
    dyn_dyn_string names;
    dyn_dyn_int types;
    names[1]  = makeDynString ("StoragePartition","","","");
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
    StorageTaskMgr_checkErrors(result);
  }   
  else  {
    DebugN("Data type StoragePartition are already defined....Nothing to do.");
  }
}
//=============================================================================
void StorageControlPanel_uninstall()  {
  if ( dynlen(dpTypes("StorageControl")) > 0 ) {
    int result = dpTypeDelete("StorageControl");
    StorageControlPanel_checkErrors(result);
  }  
  else  {
    DebugN("Data type StorageControl does not exist....Nothing to do.");
  }
  if ( dynlen(dpTypes("StoragePartition")) > 0 ) {
    int result = dpTypeDelete("StoragePartition");
    StorageControlPanel_checkErrors(result);
  }
  else  {
    DebugN("Data type StoragePartition does not exist....Nothing to do.");
  }
}
