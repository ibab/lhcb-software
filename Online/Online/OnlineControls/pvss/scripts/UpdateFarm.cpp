#uses "ProcessorFarm.cpp"
#uses "StreamTaskMgr.cpp"

main() {
  string sys, nam, typ="Farm", cfg=ctrlUtils_dimMapName();
  string ctrl_node = strtoupper(getHostname());
  dyn_string farms = dpNames("*:"+typ+"_*.Name","FarmSubInfo");
  dyn_string names, used;
  DebugN("Got farms:"+farms);
  for(int i=1; i<=dynlen(farms); ++i) {
    nam = "";
    if ( 0 == dpGet(farms[i],nam) ) {
      if ( strlen(nam)>0 ) {
	string sys = strsplit(farms[i],":")[1];
	DebugN("Got subfarm with name:"+sys+" --> "+nam);
	nam = strsplit(nam,"_")[2];
	dynAppend(names,nam+"/"+sys);
	dynAppend(used,"");
      }
    }
  }
  if ( !dpExists(typ) ) dpCreate(typ,"FarmInfo");
  dpSet(typ+".SubFarms",names, typ+".InUse",used);
  DebugN("Known subfarms:"+names);
  ctrlUtils_stopAllTree();
  for(int i=0; i<5;++i) {
    sprintf(nam,"%s_Reco%02X",typ,i);
    fwFsmTree_removeNode("FSM",nam,1);
  }
  for(int i=0; i<5;++i) {
    ProcessorFarmInstallation_createFarm(typ,"Reco",i,0);
  }
  // Finally generate all FSMs
  ctrlUtils_refreshDEN();
  ctrlUtils_genAllFSM();
  ctrlUtils_stopAllTree();
  ctrlUtils_startAllTree();
  exit(0);
}
