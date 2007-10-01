#uses "JobOptions.cpp"
#uses "StreamTaskMgr.cpp"
#uses "StreamControlPanel.cpp"
main()   {
  string stream = "Storage";
  StreamTaskMgr_installStream(stream);
  StreamTaskMgr_createAllTree(stream,5,40,5,15,"",0);  
  // Finally generate all FSMs
  //StreamTaskMgr_genAllFSM(stream);
  exit(0);
}
