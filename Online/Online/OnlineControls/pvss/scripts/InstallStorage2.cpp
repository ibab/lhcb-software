#uses "JobOptions.cpp"
#uses "StreamTaskMgr.cpp"
#uses "StreamControlPanel.cpp"
main()   {
  string stream = "Storage";
  StreamTaskMgr_installStream(stream);
  StreamTaskMgr_createTree(stream,"Slice00",5,40,5,15,"",0);  
  // Finally generate all FSMs
  //StreamTaskMgr_genAllFSM(stream);
  exit(0);
}
