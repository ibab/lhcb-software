#uses "StreamTaskMgr.cpp"
main()   {
  StreamTaskMgr_uninstall();
  StreamTaskMgr_install();
  DebugN("Stream Control       type installation finished.");
}
