#uses "lbECS_TCK.ctl"

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
int Checkpoint_connectTaskManager(string cfg, string name)  {
  dyn_string recvNodes, strmNodes;
  fwDim_createConfig(cfg);
  return 1;
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_install()  {
  string cfg = "DimPublishCHECKPOINT";//ctrlUtils_dimMapName();
  dyn_dyn_string names;
  dyn_dyn_int types;
  string dp = "CheckpointService";
  names[1]  = makeDynString ("GaudiCheckpointing","","","");
  names[2]  = makeDynString ("","Command","","");
  names[3]  = makeDynString ("","State","","");
  names[4]  = makeDynString ("","Messages","","");
  names[5]  = makeDynString ("","Status","","");
  types[1]  = makeDynInt (DPEL_STRUCT,0,0,0);
  types[2]  = makeDynInt (0,DPEL_STRING,0,0);
  types[3]  = makeDynInt (0,DPEL_STRING,0,0);
  types[4]  = makeDynInt (0,DPEL_STRING,0,0);
  types[5]  = makeDynInt (0,DPEL_STRING,0,0);
  ctrlUtils_installDataType(names,types);
  if ( !dpExists(dp) )  {
    dpCreate(dp,"GaudiCheckpointing");
  }
  fwDim_createConfig(cfg);
  if ( !dpExists("CHECKPOINT01_StreamTaskCreator") ) dpCreate("CHECKPOINT01_StreamTaskCreator","StreamTaskCreator");
  fwDim_unSubscribeCommandsByDp(cfg,"CHECKPOINT01_StreamTaskCreator*");
  fwDim_unSubscribeServicesByDp(cfg,"CHECKPOINT01_StreamTaskCreator*");
  dpSet("CHECKPOINT01_StreamTaskCreator.Name","CHECKPOINT01");
  fwDim_subscribeCommand(cfg,"/FMC/CHECKPOINT01/task_manager/start","CHECKPOINT01_StreamTaskCreator.Start", 1);
  fwDim_subscribeCommand(cfg,"/FMC/CHECKPOINT01/task_manager/stop","CHECKPOINT01_StreamTaskCreator.Stop", 1);
  fwDim_subscribeCommand(cfg,"/FMC/CHECKPOINT01/task_manager/kill","CHECKPOINT01_StreamTaskCreator.Kill", 1);
  DebugN("Connect /FMC/CHECKPOINT01/task_manager to CHECKPOINT01_StreamTaskCreator");

  fwDim_unSubscribeCommandsByDp(cfg,dp+"*");
  fwDim_unSubscribeServicesByDp(cfg,dp+"*");
  fwDim_subscribeService(cfg,"/FMC/CHECKPOINT01/logger/gaudi/log",
                         dp+".Messages",
                         "Connected to output logger",
                         0,1,0);
  DebugN("Checkpoint_install> All Done.");
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_uninstall()  {
  string cfg = "DimPublishCHECKPOINT";//ctrlUtils_dimMapName();
  string dp  = "CheckpointService";
  ctrlUtils_uninstallDataType("GaudiCheckpointing");
  fwDim_unSubscribeCommandsByDp(cfg,dp+"*");
  fwDim_unSubscribeServicesByDp(cfg,dp+"*");
}

//=============================================================================
// Callback when a new output message arrives.
// Perform some message analysis to detect possible problems.
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void CheckpointMessage_callback(string dpe, string text)  {
  int pos = strpos(text,"finished. Process now exiting.");
  int start = strpos(text,"GaudiCheckpoint.exe("); 
  int end = strpos(text,"\n");
  if ( end <= 0 ) end = strlen(text);
  if ( start > 0 )  {
    start = strpos(text,"):");
    m_messages.append(substr(text,start+3,end-start-3));
  }
  else  {
    m_messages.append(substr(text,0,end));
  } 
  if ( pos > 0 )   {
    //m_result.foreCol = "black";
    //m_result.backCol = "FwStateOKPhysics";
    m_result.text = "  >>> Checkpointing FINISHED";
    m_messages.append(m_result.text);    
    Checkpoint_enableCommands(true);
    return;
  }
  pos = strpos(text,"Exit(-1)");
  if ( pos > 0 )  {
    m_result.text = "  >>> Checkpointing FAILED";
    m_messages.append("  >>> Checkpointing FAILED");
    Checkpoint_enableCommands(true);
  }
  else if ( (pos=strpos(text,"*** Break *** segmentation violation"))>0 ) {
    m_result.text = "  >>> Checkpointing FAILED";
    m_messages.append("  >>> Checkpointing FAILED");
    Checkpoint_enableCommands(true);
  }    
  else if ( (pos=strpos(text," Segmentation fault "))>0 ) {
    m_result.text = "  >>> Checkpoint TEST FAILED";
    m_messages.append("  >>> Checkpoint TEST FAILED");
    Checkpoint_enableCommands(true);
  }    
  else if ( (pos=strpos(text,"CHECKPOINT FILE DOES NOT EXIST"))>0 ) {
    m_result.text = "  >>> Checkpoint TEST FAILED";
    m_messages.append("  >>> Checkpoint TEST FAILED");
    Checkpoint_enableCommands(true);
  }    
}

//=============================================================================
// Initialize DB tags constants from RunInfo datapoint. 
//
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_dbTagsFromRunInfo()   {
  if(dpExists(Checkpoint_RunInfoDp+".Trigger.condDBTag"))      {
    dpGet(Checkpoint_RunInfoDp+".Trigger.condDBTag", Checkpoint_CondDB);
  }
  if(dpExists(Checkpoint_RunInfoDp+".Trigger.DDDBTag"))      {
    dpGet(Checkpoint_RunInfoDp+".Trigger.DDDBTag", Checkpoint_DDDB);
  }
}

//=============================================================================
// Initialize the partition selector according to the recipe cache.
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_initPartitionSelector()   {
  mapping m;
  dyn_string recipes = dpNames("*:RecipeCache/Trigger/*","_FwRecipeCache");

  for(int i=1, n=dynlen(recipes); i<=n; ++i)  {
    int    pos = strpos(recipes[i],"/Trigger/");
    string s   = substr(recipes[i],pos+9);
    s   = substr(s,0,strpos(s,"/"));
    Checkpoint_Partition = s;
    m[s] = 1;
  }
  m_partSelector.items = mappingKeys(m);
  m_partSelector.text = Checkpoint_Partition;
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_readL0Types()  {
  dyn_string L0Types, FullL0Types;
  dyn_string dps = dpNames("TRGL0DU:RecipeCache/L0DU_Algorithms/*","_FwRecipeCache");
  for(int pos=0, i=1; i <= dynlen(dps); i++)    {
    dyn_string items = strsplit(dps[i],":");
    FullL0Types[i] = items[2];
    strreplace(FullL0Types[i], "RecipeCache/L0DU_Algorithms/","");
    pos = strpos(FullL0Types[i],"_0x");
    L0Types[i] = substr(FullL0Types[i], 0, pos);
  }
  DebugN("L0 types:"+L0Types);
  DebugN("Full L0 types:"+FullL0Types);
  //findL0Types();
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
string Checkpoint_directory()  {
  string f=m_outdir.text, pgm=m_hltProgram.text;
  if ( m_flavour.text == "Moore" && m_hltType.text == "PassThrough" )  {
    f = f + "/" + m_hltType.text;
    f = f + "/" + m_online.text;
    f = f + "/" + m_hltProgram.text;
    return f;
  }
  if ( strlen(Checkpoint_AppType) > 0 )  {
    f = f + "/" + Checkpoint_AppType;
  }
  if ( strlen(m_app.text) > 0 )  {
    f = f + "/" + m_app.text;
  }
  if ( !(pgm == "OnlineBrunel" || pgm == "DataQuality") )   {
    if ( strlen(m_hltType.text) > 0 )  {
      f = f + "/" + m_hltType.text;
    }
  }
  if ( Checkpoint_useDBtags )  {
    f = f +   "/" + m_condDB.text + "/" + m_ddDB.text;
  }
  if ( strlen(m_condMap.text) > 0 )   {
    f = f + "/" + baseName(m_condMap.text);
  }
  if ( m_flavour.text == "Moore" && strlen(m_hltProgram.text) > 0 )  {
    f = f + "/" + m_hltProgram.text;
  }
  return f;
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_update()  {
  string f = Checkpoint_directory() + "/Checkpoint.data";
  m_output.text          = f;  
  m_result.text          = "";
  m_result.backCol       = "_Transparent";
  m_result.foreCol       = "_WindowText";
  m_condMap.visible      = Checkpoint_useDBtags;
  m_condDB.visible       = Checkpoint_useDBtags;
  m_ddDB.visible         = Checkpoint_useDBtags;
  m_condDBLabel.visible  = Checkpoint_useDBtags;
  m_ddDBLabel.visible    = Checkpoint_useDBtags;
}

//=============================================================================
// Load database tag selector from disk content
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_showdbTags()  {
  dyn_string items;
  if ( Checkpoint_CondDB == "" )  {
    items = Checkpoint_dbTags("LHCBCOND");
    m_condDB.items = items;
    Checkpoint_CondDB = dynlen(items)>0 ? items[1] : "";
    m_condDB.text = Checkpoint_CondDB;
  }
  else   {
    m_condDB.text = Checkpoint_CondDB;
    m_condDB.editable = false;
    m_condDB.items = makeDynString(Checkpoint_CondDB);
  }
  if ( Checkpoint_DDDB == "" )  {
    items = Checkpoint_dbTags("DDDB");  
    m_ddDB.items = items;
    Checkpoint_DDDB = dynlen(items)>0 ? items[1] : "";
    m_ddDB.text  = Checkpoint_DDDB;
  }
  else   {
    m_ddDB.items = makeDynString(Checkpoint_DDDB);
    m_ddDB.text = Checkpoint_DDDB;
    m_ddDB.editable = false;
  }
  if ( strlen(Checkpoint_CondDB)> 0 )  {
    int pos = strpos(Checkpoint_CondDB,"|");
    if ( pos > 0 )  {
      m_condDB.text = substr(Checkpoint_CondDB,0,pos);
      m_ddDB.text   = substr(Checkpoint_CondDB,pos+1);
    }
  }
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
dyn_string Checkpoint_dbTags(string prefix)   {
  dyn_string tags;
  string dir = "g:\\online\\hlt\\conditions";
  if(_UNIX)  {
    dir = "/group/online/hlt/conditions";
  }
  tags = getFileNames(dir,prefix+"*.db");
  for(int i = 1; i <= dynlen(tags); i++)  {
    strreplace(tags[i],prefix+"_","");
    strreplace(tags[i],".db","");
  }
  dynSortAsc(tags);
  return tags;
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_About(string app_type)   {
  dyn_float df;
  dyn_string ds;
  string msg = "LHCb Online:\n\nPanel to create checkpoints\nfor "+app_type+" processes.";
  msg = msg + "\n\nComments and suggestions to M.Frank CERN/LHCb";
  ChildPanelOnReturn("visionUtils/AboutBox","About",
                     makeDynString(msg),
                     150,150,ds,df);  
}

//=============================================================================
// Enable commands
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_enableCommands(bool val)   {
  bool edit = val;
  // Input fields
  if ( !Checkpoint_Editable ) edit = false;
  m_app.editable       = edit;
  m_outdir.editable    = edit;
  m_hltType.editable   = edit;
  m_output.editable    = edit;
  m_condMap.editable   = edit;
  m_online.editable    = edit;
  m_mooreVsn.editable  = edit;
  m_tck.editable       = edit;
  m_tckList.enabled    = edit;
  // DB tags
  m_condDB.enabled     = val;
  m_ddDB.enabled       = val;
  // Buttons:
  m_close.enabled      = val;
  m_create.enabled     = val;
  m_test.enabled       = val;
  m_reload.enabled     = val;
  m_gzip.enabled       = val;
  m_options.enabled    = val;

  m_enable.visible     = !val;
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_waitTest(string utgid)  {
  if ( strlen(utgid) > 12 ) { // require some minimal length!
    dyn_float df;
    dyn_string ds;
    string msg = "Note:\n\nThe process started normally, but may not end by itself.\n"+
                 "Press OK to kill process with the UTGID:\n"+utgid+
                 "\nThe process MUST be killed to avoid orphanes.";
    Checkpoint_enableCommands(false);
    ChildPanelOnReturn("visionUtils/ErrorBox","Kill child",
                       makeDynString("$1:"+msg,"$2:Ok"),
                       150,550,ds,df);
    dpSet("STORAGE:CHECKPOINT01_StreamTaskCreator.Kill","-s 9 "+utgid);
    m_messages.append(">>>> Process "+utgid+" killed.....");
    m_result.text = "  >>> Checkpoint action completed.";
    Checkpoint_enableCommands(true);
  }
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_createZip()  {
  string tag = m_condDB.text + "_" + m_ddDB.text + "_zip";
  string utgid = m_app.text+"_"+tag;
  string args = "-E /tmp/logGaudi.fifo -O /tmp/logGaudi.fifo";
  args = args + " -n online";
  args = args + " -u "+utgid;
  args = args + " -D CHECKPOINT_FILE=Checkpoint.data";
  args = args + " -D CHECKPOINT_DIR="+Checkpoint_directory();
  args = args + " /group/online/dataflow/cmtuser/checkpoints/cmds/zip_checkpoint.sh";
  DebugN(args);
  m_messages.text = "";
  dpSet("STORAGE:CHECKPOINT01_StreamTaskCreator.Start",args);
  Checkpoint_waitTest(utgid);
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_initMenu(string partition, string triggerConf)  {
  string dir = "/group/online/dataflow/cmtuser/checkpoints";
  Checkpoint_Partition = partition;
  if ( _WIN32 ) {
    strreplace(Checkpoint_SwPath,"/group/","G:/");
  }

  m_flavour.visible               = true;
  m_partSelector.items            = makeDynString(partition);
  m_partSelector.text             = partition;
  m_partSelector.editable         = false;

  m_outdir.text                   = dir;
  m_close.text                    = "Close";
  m_close.toolTipText             = "Close panel.";
  m_messages.wordWrap             = "NoWrap";
  m_result.text                   = "";
  m_result.backCol                = "_Transparent";
  m_result.foreCol                = "_WindowText";

  m_partSelector.toolTipText      = "Select partition name.";
  m_partSelectorLabel.toolTipText = m_partSelector.toolTipText;
  m_hltProgram.toolTipText        = "Select the application type";
  m_hltProgramLabel.toolTipText   = m_hltProgram.toolTipText;
  m_triggerConf.toolTipText       = "Select trigger configuration by nick name.";
  m_triggerConfLabel.toolTipText  = m_triggerConf.toolTipText;
  m_mooreLabel.toolTipText        = "Select Moore software version";
  m_mooreVsn.toolTipText          = m_mooreLabel.toolTipText;
  m_appLabel.toolTipText          = "Select here the app.software version used to create the checkpoint";
  m_app.toolTipText               = m_appLabel.toolTipText;
  m_hltTypeLabel.toolTipText      = "Select the TCK family to create the checkpoint.";
  m_hltType.toolTipText           = m_hltTypeLabel.toolTipText;
  m_condDBLabel.toolTipText       = "Select the conditions database tag to create the checkpoint.";
  m_condDB.toolTipText            = m_condDBLabel.toolTipText;  
  m_condMapLabel.toolTipText      = "File with run-dependent conditions items";
  m_condMap.toolTipText           = "File with run-dependent conditions items";
  m_ddDBLabel.toolTipText         = "Select the detector database tag to create the checkpoint.";
  m_ddDB.toolTipText              = m_ddDBLabel.toolTipText;  
  m_outdirLabel.toolTipText       = "Output directory for checkpoint files.";
  m_outdir.toolTipText            =  m_outdirLabel.toolTipText;
  m_output.toolTipText            = "Final output file name for the checkpoint.";  
  m_output.editable               = false;
  m_onlineLabel.toolTipText       = "Online version corresponding to this Moore version";
  m_online.toolTipText            = m_onlineLabel.toolTipText;
  m_suggestions.toolTipText       = "If you really want changes, contact me....";
  m_clear.toolTipText             = "Press to clear the message display.";
  m_create.toolTipText            = "Press to create checkpoint file according to selected parameters.";
  m_test.toolTipText              = "Press to start up process using checkpoint file.";
  m_options.toolTipText           = "Press to generate checkpoint job options.";
  m_close.toolTipText             = "Press to close the panel.";

  m_scan.text                     = strlen(triggerConf)>0 ? "" : "1";
  m_enable.toolTipText            = "Press to re-enable commands.";
  m_enable.visible                = false;
}

//=============================================================================
// Panel initialization routine
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_initPanel(string partition, string triggerConf, string flavour, string app_type)  {
  m_flavour.text = flavour;
  LayerOff(3);
  Checkpoint_initMenu(partition, triggerConf);
  if ( flavour == "Moore" )
    MooreCheckpoint_init(partition, triggerConf, app_type);
  else
    BrunelCheckpoint_init(partition, triggerConf, app_type);
}

//=============================================================================
// Callback when the state of the partition selector changed.
//
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_partitionChanged()  {
  dyn_string empty;
  if ( m_partSelector.editable )  {
    Checkpoint_Partition = m_partSelector.text;
    Checkpoint_initValuesFromRecipeCache(Checkpoint_Partition,"");
  }
}

//=============================================================================
// Callback when the state of the trigger configuration selector changed.
//
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_changeTriggerConf()  {
  if ( m_partSelector.editable )  {
    Checkpoint_initValuesFromRecipeCache(Checkpoint_Partition,m_triggerConf.text);
  }
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_reloadApp(string app)  {
  dyn_string tcks;
  if ( app != m_app.text ) m_app.text = app;
  if ( m_partSelector.editable )  {
    Checkpoint_readCmtFile();
    Checkpoint_readTckFile(tcks);
    Checkpoint_showTCKs(tcks);
  }
  Checkpoint_update();
}

//=============================================================================
// Initialize constants from RunInfo datapoint. 
//
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_initValuesFromRunInfo()   {
  int tckHex;
  dpGet(Checkpoint_RunInfoDp+".Trigger.mooreVersion", Checkpoint_MooreVsn,
        Checkpoint_RunInfoDp+".Trigger.mooreOnlineVersion", Checkpoint_AppVsn,
        Checkpoint_RunInfoDp+".Trigger.HLTType", Checkpoint_HLTType,
        Checkpoint_RunInfoDp+".Trigger.L0Type", Checkpoint_L0Type,
        Checkpoint_RunInfoDp+".Trigger.TCKLabel", Checkpoint_TCKLabel,
        Checkpoint_RunInfoDp+".Trigger.swPath", Checkpoint_SwPath,
        Checkpoint_RunInfoDp+".Trigger.TCK", tckHex,
        Checkpoint_RunInfoDp+".general.runType", Checkpoint_RunType);
  if(dpExists(Checkpoint_RunInfoDp+".Trigger.LumiTrigger"))    {
    dpGet(Checkpoint_RunInfoDp+".Trigger.LumiTrigger", Checkpoint_LumiEnabled,
          Checkpoint_RunInfoDp+".Trigger.LumiPars", Checkpoint_LumiPars,
          Checkpoint_RunInfoDp+".Trigger.BeamGasTrigger", Checkpoint_BeamGasEnabled);
    if(dpExists(Checkpoint_RunInfoDp+".Trigger.LumibbRate"))      {
      dpGet(Checkpoint_RunInfoDp+".Trigger.LumibbRate", Checkpoint_LumiBB);
    }
  }
  Checkpoint_dbTagsFromRunInfo();
  Checkpoint_CondDB = "";
  Checkpoint_DDDB = "";
  m_triggerConf.visible = false;
  m_triggerConfLabel.visible = false;
}

//=============================================================================
// Initialialize panel constants from the recipe chache
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_initValuesFromRecipeCache(string partition, string triggerConf)  {
  string name, sys=getSystemName(), cfg=triggerConf, tmp, s=m_scan.text;
  dyn_string pars, vals, infos, recipes;
  int i, len, pos, tckHex, scan = strlen(s)>0 ? 1 : 0;

  name = "*:RecipeCache/Trigger/"+partition+"/";
  recipes = dpNames(name+"*","_FwRecipeCache");
  Checkpoint_CondDB = "";
  Checkpoint_DDDB = "";

  tmp = "/"+partition+"/";
  len = strlen(tmp);
  //DebugN("recipes["+name+"]:"+recipes);
  for(int i=1; i<=dynlen(recipes); ++i)  {
    pos = strpos(recipes[i],tmp);
    s   = substr(recipes[i],pos+len);
    if ( strlen(triggerConf)==0 )  {
      sys = dpSubStr(recipes[i],DPSUB_SYS);
      triggerConf = s;
    }
    else if ( strpos(recipes[i],triggerConf)>0 )  {
      sys = dpSubStr(recipes[i],DPSUB_SYS);
    }
    recipes[i] = s;
  }
  if ( scan > 0 || strlen(cfg)==0 )   {
    m_triggerConf.items = recipes;
  }
  else  {
    m_triggerConf.items = makeDynString(triggerConf);
    m_triggerConf.editable = false;
    Checkpoint_Editable = false;
  }
  m_triggerConf.text  = triggerConf;
  name = sys+"RecipeCache/Trigger/"+partition+"/"+triggerConf;    

  dpGet(name+".DataPoints.DPNames",infos,
        name+".Values.DPENames",pars,
        name+".Values.DPEValues",vals);
  Checkpoint_AppVsn = "";
  Checkpoint_CondDB = "";
  Checkpoint_RunInfoDp = infos[1];
  DebugN("Partition:"+partition+" triggerConf:"+triggerConf+" Sys:"+sys+
         " Scan:"+scan+" "+name+" Got "+dynlen(pars)+" parameters");

  for(int i = 1; i <= dynlen(pars); i++)    {
    if(pars[i] == Checkpoint_RunInfoDp+".Trigger.onlineVersion") {
      m_online.text = vals[i];
      DebugN("New online version is:"+vals[i]);
    }
    else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.gaudiVersion")  {      
    }
    else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.mooreVersion")
      Checkpoint_MooreVsn = vals[i];
    else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.mooreOnlineVersion")
      Checkpoint_AppVsn = vals[i];
    else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.HLTType")
      Checkpoint_HLTType = vals[i];
    else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.TCKLabel")
      Checkpoint_TCKLabel = vals[i];
    else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.swPath")
      Checkpoint_SwPath = vals[i];
    else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.L0Type")
      Checkpoint_L0Type = vals[i];
    else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.TCK")
      tckHex = vals[i];
    else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.runType")
      Checkpoint_RunType = vals[i];
    else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.LumiTrigger")
      Checkpoint_LumiEnabled = vals[i];
    else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.BeamGasTrigger")
      Checkpoint_BeamGasEnabled = vals[i];
    else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.LumibbRate")
      Checkpoint_LumiBB = vals[i];
    else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.condDBTag")
      Checkpoint_CondDB = vals[i];
    else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.DDDBTag")
      Checkpoint_DDDB = vals[i];
  }

  if ( strlen(Checkpoint_AppVsn) == 0 ) Checkpoint_AppVsn = Checkpoint_MooreVsn;
  DebugN("Checkpoint_DDDB:"+Checkpoint_DDDB+" Checkpoint_CondDB:"+Checkpoint_CondDB);
  m_mooreVsn.items   = makeDynString(Checkpoint_MooreVsn);
  m_mooreVsn.text    = Checkpoint_MooreVsn;

  m_app.items   = makeDynString(Checkpoint_AppVsn);
  m_app.text    = Checkpoint_AppVsn;

  m_hltType.items = makeDynString(Checkpoint_HLTType);
  m_hltType.text  = Checkpoint_HLTType;

  sprintf(name,"%s (%08x)",Checkpoint_TCKLabel,tckHex);
  m_tck.text = name;
  m_tckList.items = makeDynString(name);
  // Nothing better yet for the moment

  Checkpoint_showdbTags();
  bool val = false;
  m_mooreVsn.editable = val;
  m_app.editable      = val;
  m_outdir.editable   = val;
  m_hltType.editable  = val;
  m_output.editable   = val;
  m_online.editable   = val;
  m_tck.editable      = val;
  m_tckList.enabled   = val;
  m_enable.visible    = val;
  m_reload.visible    = val;
  //m_create.enabled  = val;
  //m_options.enabled = val;
}

//=============================================================================
// Initialialize panel constants from the recipe chache
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_initValuesFromMapping(string partition, string triggerConf)  {
  string name, sys=getSystemName();
  mapping parameters;
  int tckHex;
  float lumi;
  lbECS_getHltPars(parameters,"ECS:"+partition+"_RunInfo",triggerConf);
  //DebugN("Run Parameters:"+parameters);

  Checkpoint_Editable = false;
  m_triggerConf.items = makeDynString(triggerConf);
  m_triggerConf.editable = false;
  m_triggerConf.text  = triggerConf;
  //DebugN("Partition:"+partition+" triggerConf:"+triggerConf+" Sys:"+sys);
  m_online.text             = parameters["OnlineV"];
  Checkpoint_MooreVsn       = parameters["MooreV"];
  Checkpoint_AppVsn         = parameters["MooreOnlineV"];
  Checkpoint_TCKLabel       = parameters["TCKLabel"];
  Checkpoint_L0Type         = parameters["L0Type"];
  Checkpoint_HLTType        = parameters["HLTType"];
  Checkpoint_DDDB           = parameters["DDDBTag"];
  Checkpoint_CondDB         = parameters["CondDBTag"];
  Checkpoint_CondMap        = parameters["DynCondList"];
  Checkpoint_LumiBB         = parameters["LumiBBRate"];
  Checkpoint_BeamGasEnabled = parameters["BeamGasTrigger"] == "FALSE" ? 0 : 1;
  Checkpoint_LumiEnabled    = parameters["LumiTrigger"] == "FALSE" ? 0 : 1;
  tckHex                    = parameters["TCK"];
  //else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.swPath")
  //  Checkpoint_SwPath = vals[i];
  //else if(pars[i] == Checkpoint_RunInfoDp+".Trigger.runType")
  //  Checkpoint_RunType = vals[i];
  DebugN("Checkpoint_DDDB:"+Checkpoint_DDDB+" Checkpoint_CondDB:"+Checkpoint_CondDB);
  m_mooreVsn.items   = makeDynString(Checkpoint_MooreVsn);
  m_mooreVsn.text    = Checkpoint_MooreVsn;

  m_app.items        = makeDynString(Checkpoint_AppVsn);
  m_app.text         = Checkpoint_AppVsn;

  m_hltType.items    = makeDynString(Checkpoint_HLTType);
  m_hltType.text     = Checkpoint_HLTType;

  m_condMap.text     = Checkpoint_CondMap;
  m_condMap.editable = false;

  sprintf(name,"%s (%08x)",Checkpoint_TCKLabel,tckHex);
  m_tck.text = name;
  m_tckList.items = makeDynString(name);
  // Nothing better yet for the moment
  Checkpoint_LumiPars = makeDynFloat(Checkpoint_LumiBB,0,0,0);

  Checkpoint_showdbTags();
  bool val = false;
  m_mooreVsn.editable  = val;
  m_app.editable       = val;
  m_outdir.editable    = val;
  m_hltType.editable   = val;
  m_output.editable    = val;
  m_online.editable    = val;
  m_mooreVsn.editable  = val;
  m_tck.editable       = val;
  m_tckList.enabled    = val;
  m_enable.visible     = val;
  m_reload.visible     = val;
  //m_create.enabled   = val;
  //m_options.enabled  = val;
  Checkpoint_update();
}


//=============================================================================
// Reload panel items from file
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_reload()  {
  Checkpoint_findVersions();
  Checkpoint_reloadApp(m_app.text);
}

//=============================================================================
// Read the TCK content of a given Moore version
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
int Checkpoint_readTckFile(dyn_string& TCKs)   {
  file f;
  dyn_string items, types;
  string path, moorev, fname, s, tckDatav;
  
  moorev = m_app.text;
  path = Checkpoint_SwPath+"/MOORE/"+moorev+"/InstallArea/TCK/manifest";
  dynClear(TCKs);
  dynClear(types);
  f = fopen(path,"r");
  if(f == 0)  {
    DebugN("TCK File not found: "+path);
    return 0;
  }
  while(!feof(f))  {
    fgets(s,2000,f);
    s = strrtrim(s);
    s = strltrim(s);
    strreplace(s, " : ",":");
    dynAppend(TCKs, s);
  }
  fclose(f);
  for(int i = 1; i <= dynlen(TCKs); i++)  {
    items = strsplit(TCKs[i],":");
    if(dynlen(items))    {
      if(!dynContains(types,items[1]))
        dynAppend(types, items[1]);
    }
  }
  dynSortAsc(types);
  m_hltType.items = types;
  m_hltType.text  = types[1];
}

//=============================================================================
// Find all availible Moore versions from the hlt group area
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_findVersions()    {
  string path;
  dyn_string files, filesRev;
  path = Checkpoint_SwPath;
  if(_WIN32)   {
    strreplace(path,"/sw/","P:/");
    strreplace(path,"/group/","G:/");
    Checkpoint_SwPath = path;
  }
  /// Scan MOORE versions
  if ( m_flavour.text == "Moore" )  {
    files = getFileNames(path+"/MOORE","*",FILTER_DIRS);
    for(int i = 1; i <= dynlen(files); i++)  {
      if((strpos(files[i],"MOORE_") != 0) && (strpos(files[i],"Moore_") != 0))    {
        dynRemove(files, i);
        i--;
      }
    }
    if(!dynlen(files))
      files = getFileNames(path+"/MOORE/*","*");
    for(int i = 1; i <= dynlen(files); i++)  {
      if((strpos(files[i],"MOORE_") != 0) && (strpos(files[i],"Moore_") != 0))    {
        dynRemove(files, i);
        i--;
      }
    }
  }
  // Now scan Other stuff versions
  else {
    DebugN("Find program versions for application type:"+Checkpoint_AppVsn);
    path = "/group/online/dataflow/cmtuser";
    if(_WIN32)   {
      strreplace(path,"/group/","G:/");
    }
    files = getFileNames(path,Checkpoint_AppVsn+"*",FILTER_DIRS);
  }
  dynSortAsc(files);
  for(int i = 1, n=dynlen(files); i <= dynlen(files); i++)  {
    filesRev[i] = files[n];
    n--;
  }
  m_app.items = filesRev;
  m_app.text = Checkpoint_AppVsn;
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_showTCKs(dyn_string& TCKs)   {
  dyn_string labels;
  string hltType = m_hltType.text;
  string ltck    = m_tck.text;
  string label   = ltck;
  
  dyn_string ltcks = dynPatternMatch(hltType+":*",TCKs);
  for(int i = 1; i <= dynlen(ltcks); i++)  {
    dyn_string items = strsplit(ltcks[i], ":");
    dynAppend(labels, items[4]+" ("+items[2]+")");
  }
  m_tckList.items = labels;
  if(dynlen(labels))  {
    if(!dynContains(labels, ltck))
      label = labels[1];
  }
  else   {
    label = "";
  }
  m_tck.text = label;
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
int Checkpoint_readCmtFile()    {
  file f;
  string path, dirname, fname, s, onlinev;
  int i, pos, pos1;
  dyn_string items;
  
  dirname = m_app.text;
  fname = "cmt/project.cmt";
  path = Checkpoint_SwPath+"/MOORE/"+dirname+"/"+fname;
  if(_WIN32)   {
    strreplace(path,"/sw/","P:/");
    strreplace(path,"/group/","G:/");
  }

  f = fopen(path,"r");
  if(f == 0)  {
    DebugN("CMT Project File not found: "+path);
    return 0;
  }
  while(!feof(f))  {
    fgets(s,2000,f);
    if(strpos(s,"#") == 0)
      continue;
    pos = strpos(s,"ONLINE_v");
    pos1 = strpos(s,"Online_v");
    if(pos >= 0)    {
      onlinev = substr(s,pos);
      onlinev = strrtrim(onlinev);
    }
    if(pos1 >= 0)    {
      onlinev = substr(s,pos1);
      onlinev = strrtrim(onlinev);
    }
  }
  fclose(f);
  m_online.text = onlinev;
  return 1;
}

//=============================================================================
// Write task initialization options to initiate the checkpoint
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
string Checkpoint_createOptions(int mode=0)  {
  int    pos;
  string fname = Checkpoint_directory() + "/";
  string text, lumi_param, tckhex, pyopts;
  bool res = mkdir(Checkpoint_directory(),"777");
  
  if ( _WIN32 ) strreplace(fname,"/group/","G:/");
  for(int i=1; i<=dynlen(Checkpoint_LumiPars); ++i)  {
    lumi_param = lumi_param + Checkpoint_LumiPars[i];
    if ( i<dynlen(Checkpoint_LumiPars) ) lumi_param = lumi_param + ",";
  }
  tckhex = m_tck.text;
  pos = strpos(tckhex,"(")+1;
  tckhex = substr(tckhex,pos,strpos(tckhex,")")-pos);
  text = 
      "// ---------------- General partition information:  \n" +
      "OnlineEnv.PartitionID        = 1;\n" +
      "OnlineEnv.PartitionIDName    = \"0001\";\n"  +
      "OnlineEnv.PartitionName      = \""+Checkpoint_Partition+"\";\n"  +
      "OnlineEnv.Activity           = \"COLLISION\";\n"  +
      "OnlineEnv.TAE                = 0;\n"  +
      "OnlineEnv.OutputLevel        = 4;\n"  +
      "OnlineEnv.HltArchitecture    = \""+m_hltType.text+"\";\n"  +
      "OnlineEnv.CalibArchitecture  = \"None\";\n"  +
      "// ---------------- Trigger parameters:    \n"  +
      "OnlineEnv.DeferHLT           = 0;\n"  +
      "OnlineEnv.passThroughDelay   = 0;\n"  +
      "OnlineEnv.AcceptRate         = 1.0;\n"  +
      "OnlineEnv.InitialTCK         = \""+tckhex+"\";\n"  +
      "OnlineEnv.MooreStartupMode   = "+mode+";\n" +
      "OnlineEnv.CondDBTag          = \""+m_condDB.text+"\";\n"  +
      "OnlineEnv.DDDBTag            = \""+m_ddDB.text+"\";\n"  +
      "OnlineEnv.ConditionsMapping  = \""+m_condMap.text+"\";\n"  +
      "OnlineEnv.LumiTrigger        = "+Checkpoint_LumiEnabled+";\n"  +
      "OnlineEnv.BeamGasTrigger     = "+Checkpoint_BeamGasEnabled+";\n"  +
      "OnlineEnv.LumiPars           = {"+lumi_param+"};\n"  +
      "OnlineEnv.L0Type             = \""+Checkpoint_L0Type+"\";\n"  +
      "OnlineEnv.HLTType            = \""+m_hltType.text+"\";\n"  +
      "OnlineEnv.MooreOnlineVersion = \""+m_app.text+"\";\n"  +
      "OnlineEnv.OnlineVersion      = \""+m_online.text+"\";\n"  +
      "OnlineEnv.MooreVersion       = \""+m_mooreVsn.text+"\";\n"  +
      "// ---------------- Reconstruction parameters:    \n"  +
      "OnlineEnv.RecoStartupMode    = 1;\n" +
      "OnlineEnv.OnlineBrunelVersion= \""+m_app.text+"\";\n" +
      "// \n";
  file f = fopen(fname + "OnlineEnv.opts","w");
  fprintf(f,text);
  fclose(f);
  m_messages.append(">>>Python option file prepared:"+fname+ "OnlineEnvBase.py");
  strreplace(text,";\n","\n");
  strreplace(text,"{","[");
  strreplace(text,"}","]");
  strreplace(text,"// ","# ");
  strreplace(text,"OnlineEnv.","");
  f = fopen(fname + "OnlineEnvBase.py","w");
  fprintf(f,text);
  fclose(f);
  f = fopen(fname + "OnlineEnv.py","w");
  fprintf(f,"from OnlineEnvBase import *\n\nfrom OnlineConfig import *\n\n");
  fclose(f);
  m_messages.append(">>>Option file prepared:"+fname+ "OnlineEnv.opts");
  return tckhex;
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
string Checkpoint_submitCreateCheckpoint(string tck)  {
  string tag   = m_condDB.text + "_" + m_ddDB.text;
  string utgid = m_app.text+"_"+tag+"_"+tck+"_create";
  string part  = m_partSelector.text;
  string hlt   = m_hltType.text;
  string args = "-E /tmp/logGaudi.fifo -O /tmp/logGaudi.fifo";
  args = args + " -n online";
  args = args + " -u "+utgid;
  args = args + " -D APP_STARTUP_OPTS=-checkpoint";
  args = args + " -D CREATE_CHECKPOINT=1";
  args = args + " -D CHECKPOINT_FILE="+Checkpoint_directory()+"/Checkpoint.data";
  args = args + " -D CHECKPOINT_DIR="+Checkpoint_directory();
  args = args + " /group/online/dataflow/cmtuser/checkpoints/cmds/Application_checkpoint.sh ";
  args = args + m_app.text + " storectl01 " + part + "  " + m_hltProgram.text;
  DebugN(args);
  dpSet("STORAGE:CHECKPOINT01_StreamTaskCreator.Start",args);
  return utgid;
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
string Checkpoint_submitTestCheckpoint()  {
  string tag = m_condDB.text + "_" + m_ddDB.text;
  string tckhex = m_tck.text, utgid;
  string part   = m_partSelector.text;
  string hlt    = m_hltType.text;
  int    pos    = strpos(tckhex,"(")+1;
  tckhex = substr(tckhex,pos,strpos(tckhex,")")-pos);
  utgid = m_app.text+"_"+tag+"_"+tckhex+"_test";
  string args = "-E /tmp/logGaudi.fifo -O /tmp/logGaudi.fifo";
  args = args + " -n online";
  args = args + " -u "+utgid;
  args = args + " -D APP_STARTUP_OPTS=-restore";
  args = args + " -D TEST_CHECKPOINT=1";
  args = args + " -D CHECKPOINT_FILE="+Checkpoint_directory()+"/Checkpoint.data";
  args = args + " -D CHECKPOINT_DIR="+Checkpoint_directory();
  args = args + " /group/online/dataflow/cmtuser/checkpoints/cmds/Application_checkpoint.sh ";
  args = args + m_app.text + " storectl01 " + part + "  " + m_hltProgram.text + " 1 ";
  DebugN(args);
  dpSet("STORAGE:CHECKPOINT01_StreamTaskCreator.Start",args);
  return utgid;
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_createCheckpoint()  {
  string utgid, tck = Checkpoint_createOptions(100);
  m_messages.text = "";
  utgid = Checkpoint_submitCreateCheckpoint(tck);
  Checkpoint_enableCommands(false);
  Checkpoint_waitTest(utgid);
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void Checkpoint_testCheckpoint()  {
  m_messages.text = "";
  string tck = Checkpoint_createOptions(2);
  string utgid = Checkpoint_submitTestCheckpoint();
  Checkpoint_waitTest(utgid);
}

//=============================================================================
// Panel initialization routine
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void MooreCheckpoint_init(string partition, string triggerConf, string app_typ)   {
  LayerOn(2);

  m_hltProgram.items              = makeDynString("Moore1","Moore2","PassThrough","OnlineBrunel","DataQuality");
  if ( strlen(app_typ) == 0 )
    m_hltProgram.text             = "Moore1";
  else
    m_hltProgram.text             = app_typ;
  Checkpoint_AppType              = m_flavour.text;
  Checkpoint_HLTProgram           = m_hltProgram.text;
  if ( partition == "ANY" )  {
    m_partSelector.editable = true;
    Checkpoint_initPartitionSelector();
    Checkpoint_initValuesFromRecipeCache(Checkpoint_Partition,triggerConf);
  }
  else if ( strlen(partition)>0 )  {
    //Checkpoint_initValuesFromRecipeCache(partition,triggerConf);
    Checkpoint_initValuesFromMapping(partition,triggerConf);
  }
  else  {
    Checkpoint_initValuesFromRunInfo();
    Checkpoint_reloadApp(Checkpoint_AppVsn);
  }
  if(!dynlen(Checkpoint_LumiPars))  {
    Checkpoint_LumiPars[1] = 0;
    Checkpoint_LumiPars[2] = 0;
    Checkpoint_LumiPars[3] = 0;
    Checkpoint_LumiPars[4] = 0;
  }
  m_appLabel.text = "Moore Online version";
  if ( 0 == Checkpoint_msgConnected )  {
    int rc = dpConnect("CheckpointMessage_callback","STORAGE:CheckpointService.Messages");
    if ( 0 != rc )  {
      DebugN("Failed to connect to datapoint STORAGE:CheckpointService.Messages. rc="+rc);
    }
    Checkpoint_msgConnected = 1;
  }
}

//=============================================================================
// Panel initialization routine
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void BrunelCheckpoint_init(string partition, string triggerConf, string app_typ)   {
  Checkpoint_initMenu(partition, triggerConf);
  LayerOff(2);

  Checkpoint_AppType              = app_typ;// aka "OnlineBrunel";
  Checkpoint_HLTProgram           = m_hltProgram.text;
  if ( partition == "ANY" )  {
    m_partSelector.editable = true;
    Checkpoint_initPartitionSelector();
    Checkpoint_initValuesFromRecipeCache(Checkpoint_Partition,triggerConf);
  }
  else if ( strlen(partition)>0 )  {
    //Checkpoint_initValuesFromRecipeCache(partition,triggerConf);
    Checkpoint_initValuesFromMapping(partition,triggerConf);
  }
  else  {
    Checkpoint_initValuesFromRunInfo();
    Checkpoint_reloadApp(Checkpoint_AppVsn);
  }
  if(!dynlen(Checkpoint_LumiPars))  {
    Checkpoint_LumiPars[1] = 0;
    Checkpoint_LumiPars[2] = 0;
    Checkpoint_LumiPars[3] = 0;
    Checkpoint_LumiPars[4] = 0;
  }
  Checkpoint_Editable      = true;
  Checkpoint_AppVsn        = app_typ;//"OnlineBrunel";
  Checkpoint_findVersions();

  m_appLabel.text            = "Brunel Online version";
  m_app.editable             = true;
  m_hltProgram.items         = makeDynString("Moore1","Moore2","PassThrough","OnlineBrunel","DataQuality");
  m_hltProgram.text          = Checkpoint_HLTProgram;
  //m_hltType.visible          = false;
  //m_hltTypeLabel.visible     = false;
  Checkpoint_update();

  if ( 0 == Checkpoint_msgConnected )  {
    int rc = dpConnect("CheckpointMessage_callback","STORAGE:CheckpointService.Messages");
    if ( 0 != rc )  {
      DebugN("Failed to connect to datapoint STORAGE:CheckpointService.Messages. rc="+rc);
    }
    Checkpoint_msgConnected = 1;
  }
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
void BrunelCheckpoint_submitCreateCheckpoint()  {
  string tag = m_condDB.text + "_" + m_ddDB.text;
  string args = "-E /tmp/logGaudi.fifo -O /tmp/logGaudi.fifo";
  args = args + " -n online";
  args = args + " -u "+m_app.text+"_"+tag+" -D PARTITION=None";
  args = args + " -D APP_STARTUP_OPTS=-checkpoint";
  args = args + " -D CREATE_CHECKPOINT=1";
  args = args + " -D LOGFIFO=/tmp/logGaudi.fifo";
  args = args + " -D CHECKPOINT_FILE="+Checkpoint_directory()+"/Checkpoint.data";
  args = args + " -D CHECKPOINT_DIR="+Checkpoint_directory();
  args = args + " /group/online/dataflow/cmtuser/"+m_app.text+"/OnlineBrunelSys/scripts/runBrunel.sh ";
  args = args + m_app.text + " Class1 RecBrunel storectl01";
  DebugN(args);
  dpSet("STORAGE:CHECKPOINT01_StreamTaskCreator.Start",args);
}

//=============================================================================
// 
// @author  M.Frank
// @version 1.0
//=============================================================================
string BrunelCheckpoint_submitTestCheckpoint()  {
  string tag = m_condDB.text + "_" + m_ddDB.text;
  string utgid = m_app.text+"_"+tag;
  string args = "-E /tmp/logGaudi.fifo -O /tmp/logGaudi.fifo";
  args = args + " -n online";
  args = args + " -u "+utgid+" -D PARTITION=LHCb";
  args = args + " -D APP_STARTUP_OPTS=-restart";
  args = args + " -D TEST_CHECKPOINT=1";
  args = args + " -D LOGFIFO=/tmp/logGaudi.fifo";
  args = args + " -D CHECKPOINT_FILE="+Checkpoint_directory()+"/Checkpoint.data";
  args = args + " -D CHECKPOINT_DIR="+Checkpoint_directory();
  args = args + " /group/online/dataflow/cmtuser/"+m_app.text+"/OnlineBrunelSys/scripts/runBrunel.sh ";
  args = args + m_app.text + " Class1 RecBrunel storectl01";
  DebugN(args);
  m_messages.text = "";
  dpSet("STORAGE:CHECKPOINT01_StreamTaskCreator.Start",args);
  return utgid;
}
