#uses "fwDIM"
#uses "ctrlUtils.cpp"

string dimMapName() {
  string sys = getSystemName();
  string dimMap = "DimPublish"+substr(sys,0,strlen(sys)-1);
  fwDim_createConfig(dimMap);
  return dimMap;
}

string tagName(string dp, string part, string itm)  {
  string tmp, det;
  int idx = strpos(dp,"|");
  int idq = strpos(dp,".fsm.currentState");
  tmp = substr(dp,idx+1,idq-idx-1);
  int idp = strpos(tmp,"|");
  if ( idp >= 0 )  {
    tmp = substr(tmp,0,idp);
  }
  idx = strpos(tmp,part+"_"+itm+"_");
  if ( idx>=0 )  {
    det = substr(tmp,strlen(part)+strlen(itm)+1);
  }
  else if ( strpos(tmp,"_Domain_") < 0 ) {
    idx = strpos(tmp,"_"+itm);
    det = substr(tmp,0,idx);
  }
  else {
    return "";
  }
  if ( det != part )
    return part+"."+itm+"."+det;
  return part+"."+itm;
}

int subscribeSubdetectors(string sys, string part)  {
  dyn_string hv, dcs, daq, dai;
  string tmp, val, t, det, tag;
  string cfg = dimMapName();
  int i;
  // hv: ECS:LHCb_HV|ECAL_HV|ECAL_HV.fsm.currentState:_original.._value
  // dcs:ECS:LHCb_DCS|ECAL_DCS|ECAL_DCS.fsm.currentState:_original.._value
  // daq:ECS:LHCb_DAQ|ECAL_DAQ|ECAL_DAQ.fsm.currentState:_original.._value
  // dai:ECS:LHCb_DAI|ECAL_DAI|ECAL_DAI.fsm.currentState:_original.._value
  hv  = dpNames(sys+":"+part+"_HV|*.fsm.currentState");
  dcs = dpNames(sys+":"+part+"_DCS|*.fsm.currentState");
  daq = dpNames(sys+":"+part+"_DAQ|*.fsm.currentState");
  dai = dpNames(sys+":"+part+"_DAI|*.fsm.currentState");
  for(i = 1; i<=dynlen(hv); ++i) {
    tag = tagName(hv[i],part,"HV");
    if ( strlen(tag) > 0 ) fwDim_publishService(cfg,tag, hv[i],"C",0);
  }
  for(i = 1; i<=dynlen(dcs); ++i) {
    tag = tagName(dcs[i],part,"DCS");
    if ( strlen(tag) > 0 ) fwDim_publishService(cfg,tag, dcs[i],"C",0);
  }
  for(i = 1; i<=dynlen(daq); ++i) {
    tag = tagName(daq[i],part,"DAQ");
    if ( strlen(tag) > 0 ) fwDim_publishService(cfg,tag, daq[i],"C",0);
  }
  for(i = 1; i<=dynlen(daq); ++i) {
    tag = tagName(dai[i],part,"DAI");
    if ( strlen(tag) > 0 ) fwDim_publishService(cfg,tag, dai[i],"C",0);
  }
  return 1;
}

int subscribeConditions(string sys, string part)  {
  string cfg = dimMapName();
  fwDim_publishService(cfg,part+".RunInfoCond.VELO.MotionSystem.ResolPosRC",sys+":"+part+"_RunInfoCond.VELO.MotionSystem.ResolPosRC","F",0);
  fwDim_publishService(cfg,part+".RunInfoCond.VELO.MotionSystem.ResolPosLA",sys+":"+part+"_RunInfoCond.VELO.MotionSystem.ResolPosLA","F",0);
  fwDim_publishService(cfg,part+".RunInfoCond.VELO.MotionSystem.ResolPosY", sys+":"+part+"_RunInfoCond.VELO.MotionSystem.ResolPosY","F",0);
  fwDim_publishService(cfg,part+".RunInfoCond.Magnet.Set.Current",       sys+":"+part+"_RunInfoCond.Magnet.Set.Current","F",0);
  fwDim_publishService(cfg,part+".RunInfoCond.Magnet.Set.Polarity",      sys+":"+part+"_RunInfoCond.Magnet.Set.Polarity","I",0);
  fwDim_publishService(cfg,part+".RunInfoCond.Magnet.Measured.Current",  sys+":"+part+"_RunInfoCond.Magnet.Measured.Current","F",0);
  fwDim_publishService(cfg,part+".RunInfoCond.Magnet.Measured.Polarity", sys+":"+part+"_RunInfoCond.Magnet.Measured.Polarity","I",0);
  fwDim_publishService(cfg,part+".RunInfoCond.LHC.FillNumber",           sys+":"+part+"_RunInfoCond.LHC.FillNumber","I",0);
  fwDim_publishService(cfg,part+".RunInfoCond.LHC.Mode",                 sys+":"+part+"_RunInfoCond.LHC.Mode","C",0);
  fwDim_publishService(cfg,part+".RunInfoCond.LHC.State",                sys+":"+part+"_RunInfoCond.LHC.State","C",0);
  fwDim_publishService(cfg,part+".RunInfoCond.LHC.Energy",               sys+":"+part+"_RunInfoCond.LHC.Energy","I",0);
  return 1;
}

int subscribeRunInfo(string sys, string part)  {
  string cfg = dimMapName();
  fwDim_publishService(cfg,part+".RunInfo.status",                 sys+":"+part+"_RunInfo.status",               "I",0);
  fwDim_publishService(cfg,part+".RunInfo.general.fillNumber",     sys+":"+part+"_RunInfo.general.fillNumber",           "I",0);
  fwDim_publishService(cfg,part+".RunInfo.general.runNumber",      sys+":"+part+"_RunInfo.general.runNumber",            "I",0);
  fwDim_publishService(cfg,part+".RunInfo.general.TAE",            sys+":"+part+"_RunInfo.general.TAE",                  "I",0);
  fwDim_publishService(cfg,part+".RunInfo.general.L0Gap",          sys+":"+part+"_RunInfo.general.L0Gap",                "I",0);
  fwDim_publishService(cfg,part+".RunInfo.general.acceptRate",     sys+":"+part+"_RunInfo.general.acceptRate",           "I",0);
  fwDim_publishService(cfg,part+".RunInfo.general.activePartId",   sys+":"+part+"_RunInfo.general.activePartId",         "I",0);
  fwDim_publishService(cfg,part+".RunInfo.general.dataType",       sys+":"+part+"_RunInfo.general.dataType",             "C",0);
  fwDim_publishService(cfg,part+".RunInfo.general.runType",        sys+":"+part+"_RunInfo.general.runType",              "C",0);
  fwDim_publishService(cfg,part+".RunInfo.general.runNTriggers",   sys+":"+part+"_RunInfo.general.runNTriggers",         "I",0);
  fwDim_publishService(cfg,part+".RunInfo.TFC.OdinData",           sys+":"+part+"_RunInfo.TFC.OdinData",                 "C",0);
  fwDim_publishService(cfg,part+".RunInfo.TFC.OdinMEPRequest",     sys+":"+part+"_RunInfo.TFC.OdinMEPRequest",           "C",0);
  fwDim_publishService(cfg,part+".RunInfo.TFC.nTriggers",          sys+":"+part+"_RunInfo.TFC.nTriggers",        "I",0);
  fwDim_publishService(cfg,part+".RunInfo.TFC.deadTime",           sys+":"+part+"_RunInfo.TFC.deadTime",         "F",0);
  fwDim_publishService(cfg,part+".RunInfo.TFC.liveTime",           sys+":"+part+"_RunInfo.TFC.liveTime",         "F",0);
  fwDim_publishService(cfg,part+".RunInfo.TFC.triggerRate",        sys+":"+part+"_RunInfo.TFC.triggerRate",      "F",0);
  fwDim_publishService(cfg,part+".RunInfo.TFC.runLiveTime",        sys+":"+part+"_RunInfo.TFC.runLiveTime",      "F",0);
  fwDim_publishService(cfg,part+".RunInfo.TFC.runDeadTime",        sys+":"+part+"_RunInfo.TFC.runDeadTime",      "F",0);
  fwDim_publishService(cfg,part+".RunInfo.TFC.runTriggerRate",     sys+":"+part+"_RunInfo.TFC.runTriggerRate",   "F",0);
  fwDim_publishService(cfg,part+".RunInfo.SubDetectors.ethMTU",    sys+":"+part+"_RunInfo.SubDetectors.ethMTU",          "I",0);
  fwDim_publishService(cfg,part+".RunInfo.Trigger.HLTType",        sys+":"+part+"_RunInfo.Trigger.HLTType",              "C",0);
  fwDim_publishService(cfg,part+".RunInfo.Trigger.L0Type",         sys+":"+part+"_RunInfo.Trigger.L0Type",               "C",0);
  fwDim_publishService(cfg,part+".RunInfo.Trigger.TCK",            sys+":"+part+"_RunInfo.Trigger.TCK",                  "I",0);
  fwDim_publishService(cfg,part+".RunInfo.Trigger.TCKLabel",       sys+":"+part+"_RunInfo.Trigger.TCKLabel",             "C",0);
  fwDim_publishService(cfg,part+".RunInfo.Trigger.dataflowVersion",sys+":"+part+"_RunInfo.Trigger.dataflowVersion",      "C",0);
  fwDim_publishService(cfg,part+".RunInfo.Trigger.gaudiVersion",   sys+":"+part+"_RunInfo.Trigger.gaudiVersion",         "C",0);
  fwDim_publishService(cfg,part+".RunInfo.Trigger.onlineVersion",  sys+":"+part+"_RunInfo.Trigger.onlineVersion",        "C",0);
  fwDim_publishService(cfg,part+".RunInfo.Storage.storeSlice",     sys+":"+part+"_RunInfo.Storage.storeSlice",           "C",0);
  fwDim_publishService(cfg,part+".RunInfo.HLTFarm.hltNTriggers",   sys+":"+part+"_RunInfo.HLTFarm.hltNTriggers",  "I",0);
  fwDim_publishService(cfg,part+".RunInfo.HLTFarm.hltRate",        sys+":"+part+"_RunInfo.HLTFarm.hltRate",       "F",0);
  fwDim_publishService(cfg,part+".RunInfo.HLTFarm.nSubFarms",      sys+":"+part+"_RunInfo.HLTFarm.nSubFarms",     "I",0);
  fwDim_publishService(cfg,part+".RunInfo.HLTFarm.runHltRate",     sys+":"+part+"_RunInfo.HLTFarm.runHltRate",    "F",0);
  fwDim_publishService(cfg,part+".RunInfo.HLTFarm.subFarms",       sys+":"+part+"_RunInfo.HLTFarm.subFarms",      "",0);
  fwDim_publishService(cfg,part+".RunInfo.HLTFarm.nodeList",       sys+":"+part+"_RunInfo.HLTFarm.nodeList",      "",1);
  return 1;
}

int subscribeDetector(string sys, string part)  {
  string ctrl_node = "storectl01";
  string cfg = dimMapName();
  fwDim_unPublishServices(cfg,part+".*",1);
  subscribeSubdetectors(sys,part);
  subscribeConditions(sys,part);
  subscribeRunInfo(sys,part);
  part = "FEST";
  fwDim_unPublishServices(cfg,part+".*",1);
  subscribeRunInfo(sys,part);
  fwInstallation_addManager("PVSS00dim","always", 30, 3, 3, "-num 41 -dim_dp_config "+dimMapName()+" -dim_dns_node "+ctrl_node);
}

