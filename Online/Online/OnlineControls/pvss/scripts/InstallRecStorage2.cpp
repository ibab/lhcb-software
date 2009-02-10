#uses "ctrlUtils.cpp"
#uses "StreamTaskMgr.cpp"
#uses "StreamStatusDisplay.cpp"

main()   {
  string node;
  string stream = "Storage";
  string ctrl_node = "storectl01";
  ctrlUtils_stopAllTree();
  StreamTaskMgr_installStream(stream);
  //StreamTaskMgr_createAllTree(stream, 25, 50, "", 0);  
  // Only need 4!!!!
  for(int i=0; i<4; ++i)  {
    sprintf(node,"Slice%02X",i);
    StreamTaskMgr_createTree(stream,node,25,50,"",0,0,1);
  }
  fwFsmTree_refreshTree();
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
