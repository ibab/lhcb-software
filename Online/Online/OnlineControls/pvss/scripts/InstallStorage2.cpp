#uses "ctrlUtils.cpp"
#uses "StreamTaskMgr.cpp"
#uses "StreamStatusDisplay.cpp"

aamain()   {
  string stream = "Storage";
  string ctrl_node = "storectl01";
  StreamTaskMgr_connectTaskManager(stream);
}

main()   {
  string cfg;
  string stream = "Storage";
  string ctrl_node = "storectl01";
  int num_recv_tasks_per_slice_and_node = 70;
  int num_strm_tasks_per_slice_and_node = 25;
  ctrlUtils_stopAllTree();
  int res = dpGet(stream+"Alloc.Setup.NumRecvSlices",num_recv_tasks_per_slice_and_node,
                  stream+"Alloc.Setup.NumStrmSlices",num_strm_tasks_per_slice_and_node);
  if ( 0 != res )   {
    ctrlUtils_checkErrors(res);
    return 1;
  }
  // We assume here 8 additional tasks for the infrastructure....
  num_recv_tasks_per_slice_and_node += 8;
  num_strm_tasks_per_slice_and_node += 8;
  StreamTaskMgr_installStream(stream);

  StreamTaskMgr_createAllTree(stream,
                              num_recv_tasks_per_slice_and_node,
                              num_strm_tasks_per_slice_and_node,
                              "", 0);  
  StreamTaskMgr_connectTaskManager(stream);
  cfg = ctrlUtils_dimMapName();
  fwInstallation_addManager("WCCOAdim","always", 30, 3, 3, "-num 40 -dim_dp_config "+cfg+" -dim_dns_node "+ctrl_node);

  cfg = ctrlUtils_dimMapName("DimPublish");
  fwDim_createConfig(cfg);
  fwInstallation_addManager("WCCOAdim","always", 30, 3, 3, "-num 41 -dim_dp_config "+cfg+" -dim_dns_node "+ctrl_node);

  // Status display server
  StreamStatusDisplay_install();
  // Install python managers
  ctrlUtils_installPythonManager(58,"WCCOAStorage","../python/WCCOA_Storage.py");
  ctrlUtils_installPythonManager(59,"WCCOABigBrother","../python/Online/RunDB/SummarySrv.py");
  // Finally generate all FSMs
  ctrlUtils_genAllFSM();
  ctrlUtils_startAllTree();
  exit(0);
}
