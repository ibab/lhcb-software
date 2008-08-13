#uses "ctrlUtils.cpp"
#uses "RecoFarm.cpp"
#uses "StreamTaskMgr.cpp"
main()   {
  string name;
  string stream = "Reco";
  string ctrl_node = "mona08";
  ctrlUtils_stopAllTree();

  StreamTaskMgr_installStream(stream);
  StreamTaskMgr_installStream(stream+"IO");

  for(int i=0; i<3;++i) {
    sprintf(name,"Slice%02d",i);
    StreamTaskMgr_createTree(stream+"IO",name, 45,45, "", 0, 0, 0);  
    RecoFarm_createTree(stream, name, 7, 45, 0);
  }
  ctrlUtils_refreshDEN();
  ctrlUtils_genAllFSM();
  ctrlUtils_stopAllTree();
  ctrlUtils_startAllTree();

  // Finally generate all FSMs
  fwInstallation_addManager("PVSS00dim","always", 30, 3, 3, "-num 40 -dim_dp_config "+ctrlUtils_dimMapName()+" -dim_dns_node "+ctrl_node);
  ctrlUtils_installPythonManager(58,"PVSS00Reco","../python/PVSS00RecoFarm.py");
  exit(0);
}
