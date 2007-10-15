#uses "ctrlUtils.cpp"
#uses "StreamTaskMgr.cpp"

main()   {
  string stream = "Monitoring";
  string ctrl_node = "mona08";
  ctrlUtils_stopAllTree();
  StreamTaskMgr_installStream(stream);
  StreamTaskMgr_createAllTree(stream,45,20,"STORAGE:Storage",10);  
  StreamTaskMgr_connectTaskManager(stream);
  // Finally generate all FSMs
  ctrlUtils_genAllFSM();
  ctrlUtils_startAllTree();
  fwInstallation_addManager("PVSS00dim","always", 30, 3, 3, "-num 40 -dim_dp_config DimStorage -dim_dns_node "+ctrl_node);
  ctrlUtils_installPythonManager(59,"PVSS00Monitoring","../python/PVSS00Monitoring.py");
  exit(0);
}
