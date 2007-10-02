#uses "JobOptions.cpp"
#uses "StreamTaskMgr.cpp"
#uses "StreamControlPanel.cpp"
main()   {
  string stream = "Storage";
  string ctrl_node = "storectl01";
  StreamTaskMgr_stopAllTree(stream);
  StreamTaskMgr_installStream(stream);
  StreamTaskMgr_createAllTree(stream, 5, 40, 5, 15, "", 0);  
  StreamTaskMgr_connectTaskManager(stream);
  // Finally generate all FSMs
  StreamTaskMgr_genAllFSM(stream);
  StreamTaskMgr_startAllTree(stream);
  fwInstallation_addManager("PVSS00dim","always", 30, 3, 3, "-num 40 -dim_dp_config DimStorage -dim_dns_node "+ctrl_node);
  JobOptions_installManager(58,"PVSS00Storage","../python/PVSS00Storage.py");
  exit(0);
}
