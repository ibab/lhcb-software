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
  m_Cancel.text                 = "Close";
  m_Cancel.toolTipText          = "Discard changes and close panel";
  m_addRecvSlicesNode.visible   = 0;
  m_addRecvSlicesText.visible   = 0;
  m_addRecvSlicesText.backCol   = "_Transparent";
  m_addRecvSlicesNum.visible    = 0;
  m_addRecvSlices.visible       = 0;
  m_addStreamSlicesNode.visible = 0;
  m_addStreamSlicesText.visible = 0;
  m_addStreamSlicesText.backCol = "_Transparent";
  m_addStreamSlicesNum.visible  = 0;
  m_addStreamSlices.visible     = 0;
  m_addRecvSlicesNode.visible   = 0;
  m_addRecvSlicesText.visible   = 0;
  m_addRecvSlicesNum.visible    = 0;
  m_addRecvSlices.visible       = 0;
  m_addStreamSlicesNode.visible = 0;
  m_addStreamSlicesText.visible = 0;
  m_addStreamSlicesNum.visible  = 0;
  m_addStreamSlices.visible     = 0;
  SliceEditor_init(name);
}
//=============================================================================
void SliceEditor_init(string sys)  {
  string dp1=strtoupper(sys)+":"+sys+"Alloc.RecvNodes";
  string dp2=strtoupper(sys)+":"+sys+"Alloc.StreamNodes";
  string dp3=strtoupper(sys)+":"+sys+"Alloc.RecvSlices";
  string dp4=strtoupper(sys)+":"+sys+"Alloc.StreamSlices";
  dyn_string nodes1, nodes2, slots1, slots2;
  if ( 0 == dpGet(dp1,nodes1,dp2,nodes2,dp3,slots1,dp4,slots2) )  {
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
  }
}
//=============================================================================
void SliceEditor_popup(string sys,int which)  {
  int answer, res; 
  dyn_string txt = makeDynString(  "PUSH_BUTTON, Add..., 1, 1","PUSH_BUTTON, Delete, 2, 1");
  popupMenu(txt, answer);
  if ( answer == 1 ) {
    if ( which == 1 )  {
      m_addRecvSlicesNode.visible = 1;
      m_addRecvSlicesText.visible = 1;
      m_addRecvSlicesNum.visible = 1;
      m_addRecvSlices.visible = 1;
    }
    else  {
      m_addStreamSlicesNode.visible = 1;
      m_addStreamSlicesText.visible = 1;
      m_addStreamSlicesNum.visible = 1;
      m_addStreamSlices.visible = 1;
    }
  }
  else if ( answer == 2 )  {
    string dp1, dp2, item;
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
    dyn_string nodes, slots, res_nodes, res_slots, itms;
    res= dpGet(dp1,nodes,dp2,slots);
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
  }
  if ( res != 0 ) ctrlUtils_checkErrors(res);
  SliceEditor_init(sys);
}
//=============================================================================
void SliceEditor_AddSlices(string sys, int which, string node, int num)  {
  string item, dp1, dp2;
  dyn_string nodes, slots;
  if ( which == 1 )  {
    dp1=strtoupper(sys)+":"+sys+"Alloc.RecvNodes";
    dp2=strtoupper(sys)+":"+sys+"Alloc.RecvSlices";
  }
  else  {
    dp1=strtoupper(sys)+":"+sys+"Alloc.StreamNodes";
    dp2=strtoupper(sys)+":"+sys+"Alloc.StreamSlices";
  }
  int res= dpGet(dp1,nodes,dp2,slots);
  if ( 0 == res )  {
    dynAppend(nodes,node);
    DebugN("Adding...."+node+" "+num);
    for(int i=1;i<=num;++i)  {
      sprintf(item,"%s:%02d",node,i);
      dynAppend(slots,item);
    }
    res = SliceEditor_Save(sys,dp1,nodes,dp2,slots);
  }
  if ( res != 0 ) ctrlUtils_checkErrors(res);
  SliceEditor_init(sys);
}

//=============================================================================
int SliceEditor_Save(string sys, 
                     string dp_nodes, dyn_string nodes, 
                     string dp_slots, dyn_string slots)  
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
    SliceEditor_init(sys);
    return res;
  }
  return 0;
}
//=============================================================================
void SliceEditor_Close(string stream)  {
  SliceEditor_init(stream);
  PanelOff();
}
