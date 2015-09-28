#uses "ctrlUtils.cpp"
//=============================================================================
//
//  ALLOCATION DISPLAY
//
//=============================================================================
void SliceEditor_initPanel(string name)  {
  string font = "Arial,8,-1,5,50,0,0,0,0,0";
  DebugN("SliceEditor_initPanel> "+name);
  m_system.text                 = name;
  m_system.visible              = 1;
  m_system.backCol              = "yellow";
  m_systemName.visible          = 1;
  m_systemName.text             = "System:";
  m_recvFrame.toolTipText       = "Input parameters to allocate slots on the receiving layer .";
  m_layer1NodesText.toolTipText = "Nodes of Layer 1 (Dispatch layer)";
  m_layer1Nodes.toolTipText     = m_layer1NodesText.toolTipText;
  m_layer1NodesText.backCol     = "_Transparent";
  m_layer2NodesText.toolTipText = "Nodes of Layer 2 (Execution layer)";
  m_layer2Nodes.toolTipText     = m_layer2NodesText.toolTipText;
  m_layer2NodesText.backCol     = "_Transparent";

  m_layer1Garage.toolTipText    = "Nodes of Layer 1, which are currrently not in use.";
  m_layer1GarageText.toolTipText= m_layer1Garage.toolTipText;
  m_layer1GarageText.backCol    = "_Transparent";

  m_layer2Garage.toolTipText    = "Nodes of Layer 2, which are currrently not in use.";
  m_layer2GarageText.toolTipText= m_layer2Garage.toolTipText;
  m_layer2GarageText.backCol    = "_Transparent";

  m_Cancel.text                 = "Close";
  m_Cancel.toolTipText          = "Discard changes and close panel";

  m_addRecvSlicesText.backCol   = "_Transparent";
  m_addStreamSlicesText.backCol = "_Transparent";

  SliceEditor_SetAddingMode(1,0);
  SliceEditor_SetAddingMode(2,0);

  SliceEditor_init(name);
}
//=============================================================================
void SliceEditor_init(string sys)  {
  string dp1=strtoupper(sys)+":"+sys+"Alloc.RecvNodes";
  string dp2=strtoupper(sys)+":"+sys+"Alloc.StreamNodes";
  string dp3=strtoupper(sys)+":"+sys+"Alloc.RecvSlices";
  string dp4=strtoupper(sys)+":"+sys+"Alloc.StreamSlices";
  string dp5=dp1+"Garage";
  string dp6=dp2+"Garage";

  dyn_string nodes1, nodes2, slots1, slots2, garage1, garage2;
  if ( 0 == dpGet(dp1,nodes1,dp2,nodes2,dp3,slots1,dp4,slots2,dp5,garage1,dp6,garage2) )  {
    m_layer1Nodes.deleteAllItems();
    for(int i=1,n=dynlen(nodes1); i<=n; ++i)
      m_layer1Nodes.appendItem(nodes1[i]);
    m_layer1Slots.deleteAllItems();
    for(int i=1,n=dynlen(slots1); i<=n; ++i)
      m_layer1Slots.appendItem(slots1[i]);

    m_layer2Nodes.deleteAllItems();
    for(int i=1,n=dynlen(nodes2); i<=n; ++i)
      m_layer2Nodes.appendItem(nodes2[i]);
    m_layer2Slots.deleteAllItems();
    for(int i=1,n=dynlen(slots2); i<=n; ++i)
      m_layer2Slots.appendItem(slots2[i]);

    m_layer1Garage.deleteAllItems();
    for(int i=1,n=dynlen(garage1); i<=n; ++i)
      m_layer1Garage.appendItem(garage1[i]);
    m_layer2Garage.deleteAllItems();
    for(int i=1,n=dynlen(garage2); i<=n; ++i)
      m_layer2Garage.appendItem(garage2[i]);

  }
}
//=============================================================================
void SliceEditor_popup(string sys,int which)  {
  int answer, res = 0;
  dyn_string txt = makeDynString(  "PUSH_BUTTON, Add..., 1, 1",
                                   "PUSH_BUTTON, Disable, 2, 1",
                                   "PUSH_BUTTON, Cancel, 3, 1");
                                   //"PUSH_BUTTON, Delete, 4, 1");
  popupMenu(txt, answer);
  if ( answer == 1 )          // Adding a new node
    SliceEditor_SetAddingMode(which,1);
  else if ( answer == 2 )     // Moving node to garage
    res = SliceEditor_DisableSlices(sys,which);
  else if ( answer == 3 )     // Cancel
    return;
  else if ( answer == 4 )     // Removing a node
    res = SliceEditor_RemoveSlices(sys,which);
  if ( res != 0 ) ctrlUtils_checkErrors(res);
  SliceEditor_init(sys);
}

//=============================================================================
void SliceEditor_popupGarage(string sys, int which) {
  int answer, res = 0; 
  dyn_string txt = makeDynString(  "PUSH_BUTTON, Enable, 1, 1",
                                   "PUSH_BUTTON, Cancel, 2, 1");
  popupMenu(txt, answer);
  if ( answer == 1 )          // Enable the node
    res = SliceEditor_EnableSlices(sys,which);
  else if ( answer == 2 )     // Nothing to do...
    return;
  if ( res != 0 ) ctrlUtils_checkErrors(res);
  SliceEditor_init(sys);
}

//=============================================================================
void SliceEditor_getDP(string sys, int which, string& item, string& dp1, string& dp2)    {
  if ( which == 1 )  {
    item = m_layer1Nodes.selectedText;
    dp1=strtoupper(sys)+":"+sys+"Alloc.RecvNodes";
    dp2=strtoupper(sys)+":"+sys+"Alloc.RecvSlices";
  }
  else  {
    item = m_layer2Nodes.selectedText;
    dp1=strtoupper(sys)+":"+sys+"Alloc.StreamNodes";
    dp2=strtoupper(sys)+":"+sys+"Alloc.StreamSlices";
  }
}

//=============================================================================
int SliceEditor_RemoveSlices(string sys,int which) {
  string dp1, dp2, item;
  dyn_string nodes, slots, res_nodes, res_slots, itms;

  SliceEditor_getDP(sys,which,item,dp1,dp2);
  int res= dpGet(dp1,nodes,dp2,slots);
  if ( 0 == res )  {
    for(int i=1,n=dynlen(slots); i<=n; ++i)  {
      itms=strsplit(slots[i],":");
      if ( itms[1]!=item) dynAppend(res_slots,slots[i]);
    }
    for(int i=1,n=dynlen(nodes); i<=n; ++i)  {
      if ( nodes[i]!=item) dynAppend(res_nodes,nodes[i]);
    }
    res = SliceEditor_Save(sys,dp1,res_nodes,dp2,res_slots);
  }
  return res;
}

//=============================================================================
int SliceEditor_AddSlices(string sys, int which, string node, int num)  {
  string item, dp1, dp2;
  dyn_string nodes, slots;

  SliceEditor_getDP(sys,which,item,dp1,dp2);
  int res= dpGet(dp1,nodes,dp2,slots);
  if ( 0 == res )  {
    dynAppend(nodes,node);
    DebugN("Adding...."+node+" "+num);
    for(int i=1;i<=num;++i)  {
      sprintf(item,"%s:%02d",node,i);
      dynAppend(slots,item);
    }
    res = SliceEditor_Save(sys,dp1,nodes,dp2,slots);
    SliceEditor_init(sys);
  }
  return res;
}

//=============================================================================
int SliceEditor_EnableSlices(string sys, int which)  {
  string item, dp1, dp2;
  dyn_string nodes, slots, res_sav_nodes, res_sav_slots, sav_nodes, sav_slots, itms;

  SliceEditor_getDP(sys,which,item,dp1,dp2);
  if ( which == 1 ) item = m_layer1Garage.selectedText;
  else              item = m_layer2Garage.selectedText;

  int res= dpGet(dp1,nodes,dp2,slots,dp1+"Garage",sav_nodes,dp2+"Garage",sav_slots);
  if ( 0 == res )  {
    DebugN("Moving node to garage position...."+item);
    for(int i=1,n=dynlen(sav_slots); i<=n; ++i)  {
      itms=strsplit(sav_slots[i],":");
      if ( itms[1]==item ) DebugN("Moving node to active position...."+item+"   "+itms[1]);
      if ( itms[1]==item ) dynAppend(slots,sav_slots[i]);
      else                 dynAppend(res_sav_slots,sav_slots[i]);
    }
    for(int i=1,n=dynlen(sav_nodes); i<=n; ++i)  {
      if ( sav_nodes[i]==item )    DebugN("Moving slice to active position...."+item+"   "+sav_nodes[i]);
      if ( sav_nodes[i]==item )    dynAppend(nodes,sav_nodes[i]);
      else                         dynAppend(res_sav_nodes,sav_nodes[i]);
    }

    res = SliceEditor_Save(sys,dp1,nodes,dp2,slots,1);
    if ( 1 == res )  {
      res = dpSet(dp1+"Garage",res_sav_nodes,dp2+"Garage",res_sav_slots);
    }
  }
  return res;
}

//=============================================================================
int SliceEditor_DisableSlices(string sys, int which)  {
  string item, dp1, dp2;
  dyn_string nodes, slots, res_nodes, res_slots, sav_nodes, sav_slots, itms;

  SliceEditor_getDP(sys,which,item,dp1,dp2);
  int res= dpGet(dp1,nodes,dp2,slots,dp1+"Garage",sav_nodes,dp2+"Garage",sav_slots);
  if ( 0 == res )  {
    DebugN("Moving node to garage position...."+item);
    for(int i=1,n=dynlen(slots); i<=n; ++i)  {
      itms=strsplit(slots[i],":");
      if ( itms[1]==item )  DebugN("Moving node to garage position...."+item+"   "+itms[1]);
      if ( itms[1]==item )  dynAppend(sav_slots,slots[i]);
      else                  dynAppend(res_slots,slots[i]);
    }
    for(int i=1,n=dynlen(nodes); i<=n; ++i)  {
      if ( nodes[i]==item ) DebugN("Moving slice to garage position...."+item+"   "+itms[1]);
      if ( nodes[i]==item ) dynAppend(sav_nodes,nodes[i]);
      else                  dynAppend(res_nodes,nodes[i]);
    }

    res = SliceEditor_Save(sys,dp1,res_nodes,dp2,res_slots,1);
    if ( 1 == res )  {
      res = dpSet(dp1+"Garage",sav_nodes,dp2+"Garage",sav_slots);
    }
  }
  return res;
}

//=============================================================================
int SliceEditor_Save(string sys, 
                     string dp_nodes, dyn_string nodes, 
                     string dp_slots, dyn_string slots,
		     int    cont=0)  
{
  // Are you sure ....
  int res = 0;
  string info;
  dyn_float df;
  dyn_string ds;
  string msg = "Are you sure you want to save the parameters ?";
  ChildPanelOnReturn("vision/MessageInfo","Please Confirm",
                     makeDynString("$1:"+msg,"$2:Cancel","$3:Ok"),50,50,ds,df);
  if ( ds[1] == 0 )   {
        //ctrlUtils_checkErrors(res);
    DebugN("Saving parameters....",nodes,slots);
    res = dpSet(dp_nodes,nodes,dp_slots,slots);
    if ( cont )  return 1;
    return res;
  }
  return 0;
}

//=============================================================================
void SliceEditor_AddingDone(int which) {
  SliceEditor_SetAddingMode(which,0);
}

//=============================================================================
void SliceEditor_SetAddingMode(int which,int mode) {
  if ( which == 1 )  {
    m_addRecvSlicesNode.visible = mode;
    m_addRecvSlicesText.visible = mode;
    m_addRecvSlicesNum.visible  = mode;
    m_addRecvSlices.visible     = mode;
    m_doneRecvSlices.visible    = mode;
  }
  else  {
    m_addStreamSlicesNode.visible = mode;
    m_addStreamSlicesText.visible = mode;
    m_addStreamSlicesNum.visible  = mode;
    m_addStreamSlices.visible     = mode;
    m_doneStreamSlices.visible    = mode;
  }
}

//=============================================================================
void SliceEditor_Close(string stream)  {
  SliceEditor_init(stream);
  PanelOff();
}
