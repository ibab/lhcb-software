#uses "ctrlUtils.cpp"
#uses "RecoFarm.cpp"
#uses "StreamTaskMgr.cpp"
void uninstallAllReco(string stream) {
  StreamTaskMgr_uninstallStream(stream);
  StreamTaskMgr_uninstallStream(stream+"IO");
  ctrlUtils_deleteAllTree(stream);
  ctrlUtils_deleteAllTree(stream+"IO");  
  RecoFarm_uninstallDataTypes();
  RecoFarm_uninstallStreamTypes();
}

main()   {
  uninstallAllReco("Reco");
  StreamTaskMgr_install();
  DebugN("Stream Control       type installation finished.");
  RecoFarm_install();
  DebugN("Reconstruction Farm  type installation finished.");
}
