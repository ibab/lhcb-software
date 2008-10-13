#uses "ProcessorFarm.cpp"
#uses "StreamTaskMgr.cpp"
#uses "fwDIM"

createPartition(string name,int i) {
  string nn, nam;
  sprintf(nn,"%s_Reco%02X",name,i);
  sprintf(nam,"Part%d",i);
  if ( !dpExists(nn) ) dpCreate(nn,"FarmRunInfo");
  dpSet(nn+".general.partName",nn,
	nn+".general.partId",1<<i,
	nn+".general.activePartId",1<<i,
	nn+".general.runType","RECFARM:FarmActivity_REPRO_1",
	nn+".HLTFarm.nSubFarms",0,
	nn+".HLTFarm.subFarms",makeDynString());
}

createSlice(string name,int i) {
  string nn;
  sprintf(nn,"%s_Slice%02X",name,i);
  if ( !dpExists(nn) ) dpCreate(nn,"FarmSlice");
  dpSet(nn+".InUse",0,
	nn+".FSMSlice","",
	nn+".RunInfo","");
}

createActivity(string name) {
  string nn = "FarmActivity_"+name;
  if ( !dpExists(nn) ) dpCreate(nn,"FarmActivity");
  dpSet(nn+".Name",name,
	nn+".Farm.Infrastructure",makeDynString("RecMBM/Class0","RecFarmRecv/Class2","RecFarmSend/Class2"),
	nn+".Farm.Worker","RecBrunel_vXYrZ/Class1",
        nn+".Control.Infrastructure",makeDynString("Adder/Class1"),
        nn+".Storage.recvInfrastructure",makeDynString("RecStorageMBM/Class0","RecStorageRead/Class2"),
        nn+".Storage.streamInfrastructure",makeDynString("RecStorageMBM/Class0","RecStorageWrite/Class2")
	);
}

removeInstallation(string typ) {
  string nam, cfg=ctrlUtils_dimMapName();
  fwDim_deleteConfig(cfg);
  fwDim_createConfig(cfg);
  fwFsmTree_removeNode("FSM","Data"+typ,1);
  fwFsmTree_removeNode("FSM",typ+"_Slices",1);
  for(int i=0; i<16;++i) {
    sprintf(nam,"%s_Reco%02X",typ,i);
    fwFsmTree_removeNode("FSM",nam,1);
  }
  fwFsmTree_refreshTree();
  ProcessorFarm_uninstallDataTypes();
  ProcessorFarm_uninstallStreamTypes();
}

main() {
  string sys, nam, typ="Farm", cfg=ctrlUtils_dimMapName();
  string ctrl_node = strtoupper(getHostname());
  dyn_string farms = dpNames("*:"+typ+"_*.Name","FarmSubInfo");
  dyn_string names, used;

  removeInstallation("Reco");
  removeInstallation("Farm");

  StreamTaskMgr_install();
  DebugN("Stream Control  type installation finished.");
  ProcessorFarm_installDataTypes();  
  DebugN("Procesor Farm   type installation finished.");

  for(int i=1; i<=dynlen(farms); ++i) {
    dpGet(farms[i],nam);
    if ( strlen(nam)>0 ) {
      string sys = strsplit(farms[i],":")[1];
      DebugN("Nam:"+nam);
      nam = strsplit(nam,"_")[2];
      dynAppend(names,nam+"/"+sys);
      dynAppend(used,"");
    }
  }
  if ( !dpExists(typ) ) dpCreate(typ,"FarmInfo");
  dpSet(typ+".SubFarms",names,
        typ+".InUse",used,
	typ+".State","NOT_READY",
	typ+".Name",typ,
	typ+".Command","");
  createActivity("REPRO_1");
  createActivity("REPRO_2");
  createActivity("REPRO_3");
  for(int i=0; i<16;++i) {
    createSlice(typ,i);
    createPartition(typ,i);
  }
  DebugN("Known subfarms:"+names);
  ctrlUtils_stopAllTree();

  StreamTaskMgr_installStream(typ);
  ProcessorFarm_installAllocator(typ,0);
  for(int i=0; i<5;++i) {
    ProcessorFarmInstallation_createFarm(typ,"Reco",i,0);
  }
  // Finally generate all FSMs
  ctrlUtils_refreshDEN();
  ctrlUtils_genAllFSM();
  ctrlUtils_stopAllTree();
  ctrlUtils_startAllTree();

  fwInstallation_addManager("PVSS00dim","always", 30, 3, 3, "-num 40 -dim_dp_config "+ctrlUtils_dimMapName()+" -dim_dns_node "+ctrl_node);
  ctrlUtils_installPythonManager(58,"PVSS00Farm","../python/PVSS00Farm.py");
  exit(0);
}
