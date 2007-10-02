#uses "JobOptions.cpp"
#uses "StreamTaskMgr.cpp"
#uses "StreamControlPanel.cpp"
main()   {
  string stream = "Monitoring";
  string ctrl_node = "mona08";
  StreamTaskMgr_stopAllTree(stream);
  StreamTaskMgr_installStream(stream);
  StreamTaskMgr_createAllTree(stream,5,40,5,15,"STORAGE:Storage",10);  
  StreamTaskMgr_connectTaskManager(stream);
  // Finally generate all FSMs
  StreamTaskMgr_genAllFSM(stream);
  StreamTaskMgr_startAllTree(stream);
  fwInstallation_addManager("PVSS00dim","always", 30, 3, 3, "-num 40 -dim_dp_config DimStorage -dim_dns_node "+ctrl_node);
  JobOptions_installManager(59,"PVSS00Monitoring","../python/PVSS00Monitoring.py");
  exit(0);
}
