#uses "ctrlUtils.cpp"
#uses "StreamControlPanel.cpp"

//=============================================================================
//
//  ALLOCATION DISPLAY
//
//=============================================================================
void RecoControlPanel_CheckAllocPanel(string dp, string value)  {
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
    RecoControlPanel_initAllocPanel(stream,partition);
    return;
  }
  else if ( dpExists(value) )   {
    m_sliceNotInUse.visible = 0;
    StreamControl_trace("Rebuilding Stream allocation panel with runInfo:"+value+ "Partition:"+slice);
    RecoControlPanel_initAllocPanel(sys,slice);
    m_streamName.text = substr(slice,0,strpos(slice,"_Slice"));
    return;
  }
  m_sliceNotInUse.visible = 1;
  StreamControl_trace("System:"+sys+" DP:"+dp+" Value:"+value+" Slice:"+slice+" Stream:"+stream);
  setValue("m_sliceNotInUse","foreCol","red");
  setValue("m_sliceNotInUse","text","The streaming slice\n"+slice+"\nin system "+sys+"\nis currently not used.");
  LayerOff(2);
}
//=============================================================================
void RecoControlPanel_startAllocPanel(string slice, string stream)  {
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
    dpConnect("RecoControlPanel_CheckAllocPanel",dp);
    return;
  }
  StreamControl_trace("RecoControlPanel_startAllocPanel> The Datapoint:"+dp+" does not exist.");
  setValue("m_sliceNotInUse","backCol","_Transparent");
  setValue("m_sliceNotInUse","foreCol","red");
  setValue("m_sliceNotInUse","text","The streaming slice\n"+slice+"\nin system "+sys+"\nis currently not used.");
  LayerOff(2);
  m_sliceNotInUse.visible = 1;
}
//=============================================================================
void RecoControlPanel_initAllocPanel(string name, string partition)  {
  string font = "Arial,8,-1,5,50,0,0,0,0,0";
  m_streamName.text               = name;
  m_streamName.visible            = 1;
  m_streamName.backCol            = "yellow";

  m_partition.text                = partition;
  m_partition.visible             = 1;
  m_partition.backCol             = "yellow";

  m_partitionName.visible         = 1;
  m_partitionName.text            = "Partition:"+partition;

  m_runInfoText.visible           = 1;
  m_runInfoText.text              = "Run info:"+partition;

  m_sliceNotInUse.visible         = 0;

  m_strmFrame.toolTipText         = "Input parameters send data from the storage layer.";
  m_ctrlInfraText.toolTipText     = "Task types to be started for the storage layer infrastructure.";
  m_strmTypesText.visible         = 1;
  m_ctrlInfraTypes.toolTipText    = m_ctrlInfraText.toolTipText;
  m_ctrlInfraTypes.font           = font;
  m_ctrlInfraTypes.visible        = 1;

  m_strmTypesText.toolTipText     = "Input stream types and multiplicities. Form: Type/Mult";
  m_strmTypes.toolTipText         = m_strmTypesText.toolTipText;
  m_strmTypes.font                = font;

  m_relayFrame.toolTipText        = "Input parameters to the data relay.";
  m_relayInfraText.toolTipText    = "Relay layer infrastructure tasks.";
  m_relayInfraTypes.toolTipText   = m_relayInfraText.toolTipText;
  m_relayInfraTypes.font          = font;

  m_workerFrame.toolTipText       = "Input parameters to instrument a worker node.";
  m_workerStrategyText.toolTipText= "Allocation strategy paramer to allocate slots on the worker frames.";
  m_workerStrategy.toolTipText    = m_workerStrategyText.toolTipText;

  m_workerInfraText.toolTipText   = "Worker node infrastrcuture tasks.";
  m_workerInfraTypes.toolTipText  = m_workerInfraText.toolTipText;
  m_workerInfraTypes.font         = font;

  m_workerTypeText.toolTipText    = "Process executing the work load on the farm.";
  m_workerType.toolTipText        = m_workerTypeText.toolTipText;
  m_workerType.font               = font;

  m_options.toolTipText           = "Invoke job options editor.";
  m_Reset.toolTipText             = "Discard changes and reset to original values";
  m_Cancel.toolTipText            = "Discard changes and close panel";
  m_Save.toolTipText              = "Save changes to datapoints but keep panel open.";
  m_Ok.toolTipText                = "Save changes to datapoints and close panel.";

  RecoControlPanel_initAllocData(name,partition);
  LayerOn(2);
}
//=============================================================================
void RecoControlPanel_initAllocData(string stream, string partition)  {
  int recvStrategy = 2, strmStrategy = 2;
  string dp, worker, info = "LBECS:"+partition+"_RunInfo";
  dyn_string rcvInfra, strmInfra, ctrlInfra, nodeInfra, relayInfra, strmTypes;

  int res = 0;
  dp = strtoupper(stream)+":"+m_runInfoDP.text;
  if ( !dpExists(dp) ) {
    res = -1;
    StreamControl_trace("RecoControlPanel_initAllocData> dpExists(dp)",res,dp);
  }
  if ( 0 == res ) res = dpGet(dp,info);
  if ( !dpExists(info) )   {
    StreamControl_trace("RecoControlPanel_initAllocData> dpExists(dp)",res,dp,info);
    res = -1;
  }
  if ( 0 == res )  {
    res = dpGet(info+".Reco.recvInfrastructure", rcvInfra,
		info+".Reco.strmInfrastructure", strmInfra,
		info+".Reco.ctrlInfrastructure", ctrlInfra,
		info+".Reco.nodeInfrastructure", nodeInfra,
		info+".Reco.relayInfrastructure",relayInfra,
		info+".Reco.streams",            strmTypes,
		info+".Reco.worker",             worker,
		info+".Reco.recvStrategy",       recvStrategy,
		info+".Reco.strmStrategy",       strmStrategy);
  }
  StreamControl_trace("RecoControlPanel_initAllocData > RunInfo:"+info+" Stream:"+stream+" Partition:"+partition);
  if ( 0 != res )  {
    ctrlUtils_checkErrors(res);
    return;
  }
  string s= "";
  for(int i=1, n=dynlen(strmTypes); i<=n; ++i)  {
    s = s + strmTypes[i];
    if ( i < n ) s = s+ "\n";
  }
  m_strmTypes.text = s;
  s = "";
  for(int i=1, n=dynlen(relayInfra); i<=n; ++i)  {
    s = s + relayInfra[i];
    if ( i < n ) s = s+ "\n";
  }
  m_relayInfraTypes.text = s;
  s = "";
  for(int i=1, n=dynlen(ctrlInfra); i<=n; ++i)  {
    s = s + ctrlInfra[i];
    if ( i < n ) s = s+ "\n";
  }
  m_ctrlInfraTypes.text = s;
  s = "";
  for(int i=1, n=dynlen(nodeInfra); i<=n; ++i)  {
    s = s + nodeInfra[i];
    if ( i < n ) s = s+ "\n";
  }
  m_workerInfraTypes.text = s;
  m_workerType.text = worker;
  if ( strmStrategy <= 0 ) strmStrategy = 1;
  m_partitionName.text = "Partition:"+partition;
  m_runInfoText.text   = "Run info:"+info;
  m_workerStrategy.sbMinimum = 1;
  m_workerStrategy.sbMaximum = 50;
  m_workerStrategy.sbStep = 1;
  m_workerStrategy.text = strmStrategy;
}
//=============================================================================
int RecoControlPanel_AllocSave(string stream, string partition)  {
  string info;
  dyn_float df;
  dyn_string ds, items;
  string msg = "Are you sure you want to save the parameters ?";
  ChildPanelOnReturn("vision/MessageInfo","Please Confirm",
                     makeDynString("$1:"+msg,"$2:Cancel","$3:Ok"),50,50,ds,df);
  if ( ds[1] == 0 )   {
    dyn_dyn_string result;
    dyn_string data = makeDynString(m_strmTypes.text,
				    m_ctrlInfraTypes.text,
				    m_relayInfraTypes.text,
				    m_workerInfraTypes.text);
    for(int i=1, n=dynlen(data); i<=n; ++i) {
      strreplace(data[i],"\n"," ");
      while ( strpos(data[i],"  ")>0 ) strreplace(data[i],"  "," ");
      while ( strpos(data[i]," /")>0 ) strreplace(data[i]," /","/");
      while ( strpos(data[i],"/ ")>0 ) strreplace(data[i],"/ ","/");
      items = strsplit(data[i]," ");
      for(int j=1, m=dynlen(items); j<m; ++j) {
        if ( i>1 && dynlen(strsplit(items[j],"/")) != 2 )  {
	  ChildPanelOn("vision/MessageInfo1","Bad input",
		       makeDynString("Parameter '"+data[i]+"' is incorrect "+dynlen(items)),50,50);
	  return 1;
	}
      }
      dynAppend(result,items);
    }
    int res = dpGet(strtoupper(stream)+":"+m_runInfoDP.text,info);
    if ( !dpExists(info) ) res = -1;
    if ( 0 == res )  {
      int strategy = m_workerStrategy.text;
      res = dpSet(info+".Reco.streams",            result[1],
		  info+".Reco.ctrlInfrastructure", result[2],
		  info+".Reco.relayInfrastructure",result[3],
		  info+".Reco.nodeInfrastructure", result[4],
		  info+".Reco.worker",             m_workerType.text,
		  info+".Reco.strmStrategy",       strategy);
    }
    ctrlUtils_checkErrors(res);
    RecoControlPanel_initAllocData(stream,partition);
    return res;
  }
  return 0;
}
//=============================================================================
void RecoControlPanel_showJobOptions()  {
  ChildPanelOn("JobOptions/JobOptions","JobOptions",makeDynString(),100,100);
}
//=============================================================================
int RecoControlPanel_AllocOk(string stream, string partition)  {
  int res = RecoControlPanel_AllocSave(stream,partition);  
  if ( res == 0 ) PanelOff();
}
//=============================================================================
void RecoControlPanel_AllocCancel(string stream, string partition)  {
  RecoControlPanel_initAllocData(stream, partition);
  PanelOff();
}
//=============================================================================
void RecoControlPanel_AllocReset(string stream, string partition)  {
  RecoControlPanel_initAllocData(stream, partition);
}
