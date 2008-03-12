#uses "ctrlUtils.cpp"
//=============================================================================
int RecoFarm_createTree(string stream, string slice,int numSet,int numTasks,bool refresh=0)  {
  dyn_string sets;
  string name;
  DebugN(stream+" "+slice+" "+numSet);
  for(int i=0; i<numSet; ++i)  {
    sprintf(name,"%s%02d",slice,i);
    dynAppend(sets,name);
  }
  DebugN(sets);
  // ctrlUtils_createFsmTaskTree(stream,slice,sets,numTasks,refresh);
}
//=============================================================================
int RecoFarm_deleteTree(string stream, string slice_name, int refresh=1)  {
  string node = stream+"_"+slice_name;
  DebugN("Delete tree:"+node);
  fwFsmTree_removeNode("FSM",node,1);
  dyn_string tasks = dpNames(node+"*","FSM_DimTask");
  for(int i=1, n=dynlen(tasks); i<=n; ++i)  {
    if ( dpExists(tasks[i]) ) dpDelete(tasks[i]);
  }
  if ( refresh ) ctrlUtils_refreshDEN();
  DebugN("All Done...Deleted tree and "+dynlen(tasks)+" Tasks.");
}
//=============================================================================
int RecoFarm_install()  {
  dyn_dyn_string names;
  dyn_dyn_int types;
  names[1]  = makeDynString ("RecoFarmInfo","","","");
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
  names[1]  = makeDynString ("RecoSubFarmInfo","","","");
  names[2]  = makeDynString ("","Name","","");
  names[3]  = makeDynString ("","Nodes","","");
  names[4]  = makeDynString ("","NodesCPU","","");
  names[5]  = makeDynString ("","UsedBy","","");
  types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
  types[2]  = makeDynInt (0,DPEL_STRING,0,0);
  types[3]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
  types[4]  = makeDynInt (0,DPEL_DYN_INT,0,0);
  types[5]  = makeDynInt (0,DPEL_STRING,0,0);
  ctrlUtils_installDataType(names,types);
  names[1]  = makeDynString ("RecoFarmSlice","","","");
  names[2]  = makeDynString ("","Name","","");
  names[3]  = makeDynString ("","InUse","","");
  names[4]  = makeDynString ("","Activity","","");
  names[5]  = makeDynString ("","Command","","");
  names[6]  = makeDynString ("","State","","");
  names[7]  = makeDynString ("","SubFarms","","");
  names[8]  = makeDynString ("","FSMSlice","","");
  names[9]  = makeDynString ("","Tasks","","");
  names[10] = makeDynString ("","RunInfo","","");
  names[11] = makeDynString ("","general","","");
  names[12] = makeDynString ("","","partName","");
  types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
  types[2]  = makeDynInt (0,DPEL_STRING,0,0);
  types[3]  = makeDynInt (0,DPEL_INT,0,0);
  types[4]  = makeDynInt (0,DPEL_STRING,0,0);
  types[5]  = makeDynInt (0,DPEL_STRING,0,0);
  types[6]  = makeDynInt (0,DPEL_STRING,0,0);
  types[7]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
  types[8]  = makeDynInt (0,DPEL_STRING,0,0);
  types[9]  = makeDynInt (0,DPEL_DYN_STRING,0,0);
  types[10] = makeDynInt (0,DPEL_STRING,0,0);
  types[11] = makeDynInt (0,DPEL_STRUCT,0,0);
  types[12] = makeDynInt (0,0,DPEL_STRING,0);
  ctrlUtils_installDataType(names,types);
  names[1]  = makeDynString ("RecoFarmActivity","","","");
  names[2]  = makeDynString ("","Name","","");
  names[3]  = makeDynString ("","Farm","","");
  names[4]  = makeDynString ("","","Infrastructure","");
  names[5]  = makeDynString ("","","TaskTypes","");
  names[6]  = makeDynString ("","","ReceiverType","");
  names[7]  = makeDynString ("","","SenderType","");
  names[8]  = makeDynString ("","Storage","","");
  names[9]  = makeDynString ("","","storeFlag","");
  names[10] = makeDynString ("","","recvInfrastructure","");
  names[11] = makeDynString ("","","recvTypes","");
  names[12] = makeDynString ("","","recvMultiplicity","");
  names[13] = makeDynString ("","","recvStrategy","");
  names[14] = makeDynString ("","","streamInfrastructure","");
  names[15] = makeDynString ("","","streamTypes","");
  names[16] = makeDynString ("","","streamMultiplicity","");
  names[17] = makeDynString ("","","strmStrategy","");
  types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
  types[2]  = makeDynInt (0,DPEL_STRING,0,0);
  types[3]  = makeDynInt (0,DPEL_STRUCT,0,0);
  types[4]  = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[5]  = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[6]  = makeDynInt (0,0,DPEL_STRING,0);
  types[7]  = makeDynInt (0,0,DPEL_STRING,0);
  types[8]  = makeDynInt (0,DPEL_STRUCT,0,0);
  types[9]  = makeDynInt (0,0,DPEL_INT,0);
  types[10] = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[11] = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[12] = makeDynInt (0,0,DPEL_DYN_INT,0);
  types[13] = makeDynInt (0,0,DPEL_INT,0);
  types[14] = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[15] = makeDynInt (0,0,DPEL_DYN_STRING,0);
  types[16] = makeDynInt (0,0,DPEL_DYN_INT,0);
  types[17] = makeDynInt (0,0,DPEL_INT,0);
  ctrlUtils_installDataType(names,types);
  return 1;
}
//=============================================================================
int RecoFarm_uninstallDataTypes()  {
  ctrlUtils_uninstallDataType("RecoFarmInfo");
  ctrlUtils_uninstallDataType("RecoSubFarmInfo");
  ctrlUtils_uninstallDataType("RecoFarmAllocator");
  ctrlUtils_uninstallDataType("RecoFarmSlice");
  ctrlUtils_uninstallDataType("RecoFarmActivity");
  return 1;
}
//=============================================================================
int RecoFarm_uninstallStreamTypes()  {
  ctrlUtils_uninstallDataType("FSM_DimTask");
  ctrlUtils_uninstallDataType("StreamConfigurator");
  ctrlUtils_uninstallDataType("StreamControl");
  ctrlUtils_uninstallDataType("StreamPartition");
  ctrlUtils_uninstallDataType("StreamTaskCreator");
  return 1;
}
//=============================================================================
//
//   Activity editor
// 
//=============================================================================
void  RecoFarm_setupWidgets()  {
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

int RecoFarm_initEditor(string activity)  {
  RecoFarm_setupWidgets();
  m_activityName.text = activity;
  return RecoFarm_initData(activity);
}

int RecoFarm_initData(string activity)  {
  string recvType, sendType, dp = "RecoFarmActivity_"+activity;
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

int RecoFarm_editorCancel(string activity)  {
  return 1;
}

int RecoFarm_editorReset(string activity)  {
  return RecoFarm_initData(activity);
}

int RecoFarm_editorSave(string activity)  {
  return 1;
}

int RecoFarm_editorOk(string activity)  {
  RecoFarm_editorSave(activity);
  return 1;
}
