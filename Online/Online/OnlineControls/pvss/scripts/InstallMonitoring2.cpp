#uses "JobOptions.cpp"
#uses "StreamTaskMgr.cpp"
#uses "StreamControlPanel.cpp"
main()   {
  string stream = "Monitoring";
  StreamTaskMgr_installStream(stream);
  StreamTaskMgr_createTree(stream,"Slice00",5,40,5,15,"STORAGE:Storage",10);  
  // Finally generate all FSMs
  //StreamTaskMgr_genAllFSM(stream);
  exit(0);
}
