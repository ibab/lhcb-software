#uses "ctrlUtils.cpp"
#uses "ProcessorFarm.cpp"
#uses "StreamTaskMgr.cpp"

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
      dpCreate(name,"Task_Configurator");
    }  
    dpSet(name+".State","NOT_ALLOCATED");
    fwFsmTree_addNode(farm, name, "FSM_Alloc", 0);
    sprintf(name,"IO_Tasks%02X",i);    
    string iotask = ctrlUtils_addFsmTreeNode(farm, name, "FSM_IOTasks", 1);
    sprintf(dev,"RECFARM:Farm_Reco%02X",i);
    fwFsmTree_addNode(farm, dev, "FSM_Farm", 1);
    for(int j=0; j<16; ++j)  {
      sprintf(dev,"RECSTORAGE:Storage_Slice%02X",j);
      fwFsmTree_addNode(farm, dev, "FSM_Slice", 1);
    }
    sprintf(dev,"IOTaskConfigurator%02X",i);
    if ( !dpExists(dev) )  {
      dpCreate(dev,"IOTaskConfigurator");
    }
    dpSet(dev+".State","NOT_READY");
    fwFsmTree_addNode(iotask, dev, "IOTaskConfigurator", 0);
    
    for(int j=0; j<10; ++j){
      sprintf(dev,"IOTask%02X%02X",i,j);
      if ( !dpExists(dev) )  {
        dpCreate(dev,"FSM_DimTask");
      }
      fwFsmTree_addNode(iotask, dev, "FSM_DimTask", 0);
      dpSet(dev+".State","OFFLINE");
    }
  }
  if ( refresh ) fwFsmTree_refreshTree();
  DebugN("All Done.");
  return 1;
}

int recctrl_installDataTypes(){
  dyn_dyn_string names;
  dyn_dyn_int types;
  names[1] = makeDynString ("FSM_Alloc","","","");
  names[2] = makeDynString ("","State","","");
  types[1] = makeDynInt (DPEL_STRUCT,0,0,0);
  types[2] = makeDynInt (0,DPEL_STRING,0,0);
  ctrlUtils_installDataType(names,types);
  names[1] = makeDynString ("IOTask_Configurator","","","");
  names[2] = makeDynString ("","State","","");
  types[1] = makeDynInt (DPEL_STRUCT,0,0,0);
  types[2] = makeDynInt (0,DPEL_STRING,0,0);
  ctrlUtils_installDataType(names,types);  
}

void createTree() {
  DebugN("Stopping tree...");
  fwFsmTree_stopTree();
  fwFsmTree_removeNode("FSM","Reco_CtrlSys",1);
  DebugN("Installing Subfarm...");
  ProcessorFarm_installSubfarm(1);
  recctrl_createFsmTaskTree();
  DebugN("Adding PVSS00dim manager...");
  fwInstallation_addManager("PVSS00dim","always", 30, 3, 3, "-num 40 -dim_dp_config "+ctrlUtils_dimMapName()+" -dim_dns_node mona07");
  DebugN("Connecting Task Manager...");
  dyn_string nodes = makeDynString("mona0701");
  ProcessorFarm_connectTaskManagerEx(nodes);
  DebugN("Generating FSMs...");
  fwFsmTree_generateAll();
  DebugN("Stop/Restart FSM tree...");
  fwFsmTree_startTree();
  DebugN("Refresh FSM tree...");
  fwFsmTree_refreshTree();
  DebugN("All Done.");
}

void main() {
  createTree();
  return;
}
