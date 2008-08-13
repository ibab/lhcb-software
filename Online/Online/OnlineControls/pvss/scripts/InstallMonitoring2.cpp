#uses "ctrlUtils.cpp"
#uses "StreamTaskMgr.cpp"

main()   {
  int num_relay_tasks = 45;
  int num_sender_tasks = 10;
  int num_worker_tasks = 20;
  string stream = "Monitoring";
  string ctrl_node   = strtoupper(getHostname());
  string storage_sys = "STORAGE:Storage";
  ctrlUtils_stopAllTree();
  if ( ctrl_node == "MONA06" ) {
    num_worker_tasks = 15;
    stream = "Monitoring2";
    StreamTaskMgr_installStream(stream);
    StreamTaskMgr_createAllTree(stream,num_relay_tasks,num_worker_tasks,storage_sys,num_sender_tasks);  
  }
  else if ( ctrl_node == "MONA09" ) {
    num_worker_tasks = 8;
    stream = "Reconstruction";
    StreamTaskMgr_installStream(stream);
    StreamTaskMgr_createTree(stream,"Slice00",num_relay_tasks,num_worker_tasks,storage_sys,num_sender_tasks);
    StreamTaskMgr_createTree(stream,"Slice01",num_relay_tasks,num_worker_tasks,storage_sys,num_sender_tasks);
    StreamTaskMgr_createTree(stream,"Slice02",num_relay_tasks,num_worker_tasks,storage_sys,num_sender_tasks);
  }
  else {
    StreamTaskMgr_installStream(stream);
    StreamTaskMgr_createAllTree(stream,num_relay_tasks,num_worker_tasks,storage_sys,num_sender_tasks);  
  }
  StreamTaskMgr_connectTaskManager(stream);
  // Finally generate all FSMs
  ctrlUtils_genAllFSM();
  ctrlUtils_startAllTree();
  fwInstallation_addManager("PVSS00dim","always", 30, 3, 3, "-num 40 -dim_dp_config DimStorage -dim_dns_node "+ctrl_node);
  ctrlUtils_installPythonManager(59,"PVSS00Monitoring","../python/PVSS00Monitoring.py");
  exit(0);
}
