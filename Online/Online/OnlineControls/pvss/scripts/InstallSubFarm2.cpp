#uses "ctrlUtils.cpp"
#uses "ProcessorFarm.cpp"
#uses "StreamTaskMgr.cpp"
main()   {
  string name = "Reco";
  string ctrl_node = strtoupper(getHostname());
  ctrlUtils_stopAllTree();

  ProcessorFarm_createTree(name,ctrl_node,-1,16,0);

  ctrlUtils_refreshDEN();
  ctrlUtils_genAllFSM();
  ctrlUtils_stopAllTree();
  ctrlUtils_startAllTree();

  // Finally generate all FSMs
  fwInstallation_addManager("PVSS00dim","always", 30, 3, 3, "-num 40 -dim_dp_config DimStorage -dim_dns_node "+ctrl_node);
  ctrlUtils_installPythonManager(58,"PVSS00SubFarm","../python/PVSS00SubFarm.py");
  exit(0);
}
