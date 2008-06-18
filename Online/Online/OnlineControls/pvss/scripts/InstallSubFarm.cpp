#uses "ctrlUtils.cpp"
#uses "ProcessorFarm.cpp"
#uses "StreamTaskMgr.cpp"

void uninstallAllFarm(string name) {
  StreamTaskMgr_uninstallStream(name);
  fwFsmTree_removeNode("FSM",name,1);
  ctrlUtils_deleteAllTree(name);
  ProcessorFarm_uninstallDataTypes();
  ProcessorFarm_uninstallStreamTypes();
}

main()   {  
  string name = "Reco_"+strtoupper(getHostname());
  uninstallAllFarm(name);
  StreamTaskMgr_install();
  DebugN("Stream Control  type installation finished.");
  ProcessorFarm_installDataTypes();  
  DebugN("Procesor Farm   type installation finished.");
}
