#uses "fwUi"
#uses "ctrlUtils.cpp"
#uses "fwExternalAlertHandler/fwExternalAlertHandlerUser.ctl"

int g_hartBeatCount = 0;
time g_currTime;
string g_notRunning = "FarmMonitor inactive - server on ecs03 not running.";

//=============================================================================
string farmMonitor_dimMapName() {
  string sys = getSystemName();
  string dimMap;
  dimMap = "FarmMonitor"+substr(sys,0,strlen(sys)-1);
  return dimMap;
}

int farmMonitor_init(string dpn)  {
  string cfg = farmMonitor_dimMapName();
  //ctrlUtils_uninstallDataType("FarmMonitor_Alarm_t",1);
  if ( 0 == dynlen(dpTypes("FarmMonitor_Alarm*")) ) {
    dyn_dyn_string names;
    dyn_dyn_int types;
    names[1]  = makeDynString ("FarmMonitor_Alarm_t","","","");
    names[2]  = makeDynString ("","Alarms","","");
    names[3]  = makeDynString ("","HartBeat","","");
    names[4]  = makeDynString ("","LastUpdate","","");
    names[5]  = makeDynString ("","Command","","");
    types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
    types[2]  = makeDynInt (0,DPEL_STRING,0,0);
    types[3]  = makeDynInt (0,DPEL_INT,0,0);
    types[4]  = makeDynInt (0,DPEL_TIME,0,0);
    types[5]  = makeDynInt (0,DPEL_INT,0,0);
    ctrlUtils_installDataType(names,types);
  }
  if ( !dpExists(dpn) ) {
    int res = dpCreate(dpn,"FarmMonitor_Alarm_t");
    if ( res != 0 ) {
      ctrlUtils_checkErrors(res);
      return 0;
    }
  }
  dpSet(dpn+".Alarms","");
  farmMonitor_handle_alarm(dpn,"CLEARALL","");
  if ( farmMonitor_setupDim(dpn) != 1 ) {
    ctrlUtils_trace("farmMonitor: Failed to setup DIM connection to farm monitoring.");
    return 0;
  }
  return farmMonitor_connect(dpn);
}

int farmMonitor_setupDim(string dpn) {
  string cfg = farmMonitor_dimMapName();
  dyn_int    dim_immed, dim_tmo,  dim_flg;
  dyn_string dps_svcs,  dim_svcs, dim_defaults, dim_cmds, dps_cmds, cmds_fmt;

  fwDim_createConfig(cfg);
  fwInstallation_addManager("PVSS00dim","always", 30, 3, 3, "-dim_dp_config "+cfg+" -dim_dns_node ecs03");

  fwDim_unSubscribeServicesByDp(cfg,"*:"+dpn+".*",1);
  fwDim_unSubscribeCommandsByDp(cfg,"*:"+dpn+".*",1);

  dynAppend(dim_svcs,"/"+dpn+"/Alarms");
  dynAppend(dps_svcs,dpn+".Alarms");
  dynAppend(dim_defaults,"");
  dynAppend(dim_tmo,0);
  dynAppend(dim_flg,0);
  dynAppend(dim_immed,1);
  dynAppend(dim_svcs,"/"+dpn+"/HartBeat");
  dynAppend(dps_svcs,dpn+".HartBeat");
  dynAppend(dim_defaults,0);
  dynAppend(dim_tmo,0);
  dynAppend(dim_flg,0);
  dynAppend(dim_immed,1);
  fwDim_subscribeServices(cfg,dim_svcs,dps_svcs,dim_defaults,dim_tmo,dim_flg,dim_immed,1);
  dynAppend(dim_cmds,"/"+dpn+"/Command");
  dynAppend(dps_cmds,dpn+".Command");
  dynAppend(cmds_fmt,"I1");
  fwDim_subscribeCommands(cfg,dim_cmds,dps_cmds,1,cmds_fmt);
  return 1;
}

int farmMonitor_connect(string dpn) {
  string cmd = dpn + ".Alarms";
  int rc = dpConnect("farmMonitor_alarm_callback",cmd);	
  if ( 0 == rc )  {
    ctrlUtils_trace("farmMonitor: Connected to alarm data point:"+cmd);
    cmd = dpn + ".HartBeat";
    rc = dpConnect("farmMonitor_hartbeat_callback",cmd);	
    if ( 0 == rc )  {
      ctrlUtils_trace("farmMonitor: Connected to hartbeat data point:"+cmd);
      return 1;
    }
    ctrlUtils_checkErrors(rc,"farmMonitor: Failed to connect to hartbeat datapoint:"+cmd);
    return 0;
  }
  ctrlUtils_checkErrors(rc,"farmMonitor: Failed to connect to alarm datapoint:"+cmd);
  return 0;
}

void farmMonitor_handle_alarm(string facility, string action, string text) {
  //int fwExternalAlertHandler_sendAlarm(int alarmType, string object, string cause, string alarmClass = "", string helpTxt = "")
  //int fwExternalAlertHandler_deactivateAlarm(int alarmType, string object, string cause)
  //int fwExternalAlertHandler_deactivateAllObjAlarms( string object, dyn_int alarmTypes = makeDynInt())

  int severity = lbEAH_ERROR;
  if ( action == "CLEARALL" ) {
    ctrlUtils_trace("farmMonitor: Clear all pending alamrs for "+facility);
    fwExternalAlertHandler_deactivateAllObjAlarms(facility);
  }
  if ( action == "DECLARE" ) {
    fwExternalAlertHandler_sendAlarm(severity,facility,text);
  }
  else if ( action == "CLEAR" ) {
    fwExternalAlertHandler_deactivateAlarm(severity,facility,text);
  }
}

void farmMonitor_alarm_callback(string dpe, string value)  {
  int code;
  dyn_string items = strsplit(value,"#");
  string facility,message,action,when,subfarm,node,desc,opt, text;
  //DebugTN("farmMonitor: DP:"+dpe+" value:"+value+" "+dynlen(items)+" Items:"+items);
  g_currTime = getCurrentTime();
  if ( dynlen(items)>3 ) {
    facility = items[1];
    message  = items[2];
    action   = items[3];
    if ( action != "CLEARALL" ) {
      when     = items[4];
      code     = items[5];
      subfarm  = items[6];
      node     = items[7];
      desc     = "";
      opt      = "";
      if ( dynlen(items) > 7 ) desc = items[8];
      if ( dynlen(items) > 8 ) opt = items[9];
    }
    text = node+": "+message+" "+desc+" "+opt;
    //DebugTN(facility+":"+action+" "+subfarm+"/"+text);
    farmMonitor_handle_alarm(facility,action,text);
  }
}

void farmMonitor_hartbeat_callback(string dpe, string value)  {
  int rc;
  string dp = dpSubStr(dpe,DPSUB_SYS_DP)+".LastUpdate";
  g_currTime = getCurrentTime();
  rc = dpSet(dp,g_currTime);
  if ( rc != 0 ) {
    ctrlUtils_checkErrors(rc,"farmMonitor: Failed to set datapoint:"+dp);
  }
  g_hartBeatCount = g_hartBeatCount + 1;
  if ( 0 == (g_hartBeatCount%10) ) {
    // ctrlUtils_trace("Got callback from hartbeat:"+formatTime("%d/%m/%Y %H:%M:%S",g_currTime));
  }
}

void farmMonitor_handle_alarms(string dpn)  {
  time t;
  int diff;
  int alm_set = 0;
  delay(5);
  dpSetWait(dpn+".Command",1);
  while(1)  {
    t = getCurrentTime();
    diff = period(t-g_currTime); 
    if ( diff > 60 && alm_set == 0 ) {
      //ctrlUtils_trace("Current time:"+formatTime("%d/%m/%Y %H:%M:%S",t)+
      //	     " Last update:"+formatTime("%d/%m/%Y %H:%M:%S",g_currTime)+
      //	     " Diff:"+diff+": "+g_notRunning);
      fwExternalAlertHandler_sendAlarm(lbEAH_ERROR,"FarmMonitor",g_notRunning);
      alm_set = 1;
    }
    else if ( diff <= 60 && alm_set == 1 ) {
      //ctrlUtils_trace("Current time:"+formatTime("%d/%m/%Y %H:%M:%S",t)+": Release alarm:"+g_notRunning);
      fwExternalAlertHandler_deactivateAlarm(lbEAH_ERROR,"FarmMonitor",g_notRunning);
      alm_set = 0;
    }
    delay(10);
  }
}

void farmMonitor_run()  {
  string dpn = "FarmMonitor_Environment";
  string facility = "FarmMonitor";
  g_currTime = getCurrentTime();

  //fwExternalAlertHandler_deactivateAlarm(lbEAH_ERROR,facility,g_notRunning);
  fwExternalAlertHandler_deactivateAllObjAlarms(facility);
  if ( farmMonitor_init(dpn) != 1 )  {
    ctrlUtils_trace("farmMonitor: FAILED to initialize farmMonitor alarm handler.");
    return;
  }
  ctrlUtils_trace("farmMonitor: Alarm handler starting:"+formatTime("%d/%m/%Y %H:%M:%S",g_currTime));
  //s_ctrlUtil_print_messages = 0;
  farmMonitor_handle_alarms(dpn);
  ctrlUtils_trace("farmMonitor: Alarm handler finished.");
}
