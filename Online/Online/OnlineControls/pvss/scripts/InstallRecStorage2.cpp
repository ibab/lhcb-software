#uses "ctrlUtils.cpp"
#uses "StreamTaskMgr.cpp"
#uses "StreamStatusDisplay.cpp"

main()   {
  string stream = "Storage";
  string ctrl_node = "storectl01";
  ctrlUtils_stopAllTree();
  StreamTaskMgr_installStream(stream);
  StreamTaskMgr_createAllTree(stream, 25, 25, "", 0);  
  StreamTaskMgr_connectTaskManager(stream);
  // Finally generate all FSMs
  ctrlUtils_genAllFSM();
  ctrlUtils_startAllTree();
  fwInstallation_addManager("PVSS00dim","always", 30, 3, 3, "-num 40 -dim_dp_config "+ctrlUtils_dimMapName()+" -dim_dns_node "+ctrl_node);
  ctrlUtils_installPythonManager(58,"PVSS00Storage","../python/PVSS00RecStorage.py");
  // Status display server
  StreamStatusDisplay_install();
  exit(0);
}
