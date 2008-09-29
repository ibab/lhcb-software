#uses "ctrlUtils.cpp"
#uses "ProcessorFarm.cpp"

//=============================================================================
int recctrl_createFsmTaskTree(bool refresh=1)  {
  string dev, name, farm, node = ctrlUtils_addFsmTreeNode("FSM", "Reco_CtrlSys", "FSM_Holder", 1);
  for(int i=0; i<4; ++i)  {
    sprintf(name,"Reco_Ctrl%02X",i);
    farm = ctrlUtils_addFsmTreeNode(node, name, "FSM_FarmCtrl", 1);
    sprintf(name,"Farm_Alloc%02X",i);
    if ( !dpExists(name) )  {
      dpCreate(name,"FSM_Alloc");
    }  
    dpSet(name+".State","NOT_ALLOCATED");
    fwFsmTree_addNode(farm, name, "FSM_Alloc", 0);
    sprintf(name,"Storage_Alloc%02X",i);
    if ( !dpExists(name) )  {
      dpCreate(name,"FSM_Alloc");
    }  
    dpSet(name+".State","NOT_ALLOCATED");
    fwFsmTree_addNode(farm, name, "FSM_Alloc", 0);
    sprintf(dev,"RECFARM:Farm_Reco%02X",i);
    fwFsmTree_addNode(farm, dev, "FSM_Farm", 1);
    for(int j=0; j<16; ++j)  {
      sprintf(dev,"RECSTORAGE:Storage_Slice%02X",j);
      fwFsmTree_addNode(farm, dev, "FSM_Slice", 1);
    }
  }
  if ( refresh ) fwFsmTree_refreshTree();
  DebugN("All Done.");
  return 1;
}

void createTree() {
  DebugN("Hello");
  fwFsmTree_stopTree();
  fwFsmTree_removeNode("FSM","Reco_CtrlSys",1);
  recctrl_createFsmTaskTree();
  DebugN("Generating FSMs");
  fwFsmTree_generateAll();
  DebugN("Stop/Restart FSM tree");
  //fwFsmTree_startTree();
  DebugN("Refresh FSM tree");
  fwFsmTree_refreshTree();
  DebugN("All Done.");
}

void main() {
  //ProcessorFarm_uninstallDataTypes();
  //ProcessorFarm_installDataTypes();
  createTree();
}
