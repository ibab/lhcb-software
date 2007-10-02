#uses "JobOptions.cpp"
#uses "StreamTaskMgr.cpp"
#uses "StreamControlPanel.cpp"
main()   {
  //JobOptions_installControl();
  //DebugN("JobOptions Control   type installation finished.");
  StreamTaskMgr_installTaskType();
  DebugN("Task type            type installation finished.");
  StreamControlPanel_install();
  DebugN("Stream Control       type installation finished.");
}
