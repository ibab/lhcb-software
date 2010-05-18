#uses "fwDIM"
#uses "fwCU"
#uses "fwDU"
#uses "ctrlUtils.cpp"

int s_active = 0;
string s_top_name;
string s_lastState;
dyn_string s_children;
int s_recoveryRetries = 0;
int s_maxRecoveryRetries = 3;

void dexpert_handleTask(string node, string state)    {
  if ( state == "ERROR" ) {
    if ( s_active == 1 ) {
      string dp = s_top_name+"_Dexpert";
      string val = "sys:"+s_top_name+"#node:"+node+"#type:FSM_DimTask#State:"+state;
      dpSet(dp,val);
      ctrlUtils_trace("Set datapoint:"+dp+" to "+val);
    }
    //fwCU_sendCommand(node,"RECOVER");
    //fwCU_sendCommand(s_top_name,"RECOVER");
  }
  else if ( state == "OFFLINE" ) {
  }
}

void dexpert_handleError(string node) {
  string type;
  fwCU_getType(node,type);
}

/**
 *  Connect to chilren's state changes.
 *  @author  M.Frank
 */
void dexpert_connectDevices(string top)   {
  string child;
  dyn_string children = fwCU_getIncludedTreeDevices(top);
  for(int i=1, n=dynlen(children); i<=n; ++i)  {
    child = substr(children[i],strpos(children[i],":")+1);
    fwCU_connectState("dexpert_cb_state",child);
  }
  ctrlUtils_trace("Connected to state of "+dynlen(children)+" children.");
}

/**
 *  Disconnect from chilren's state changes.
 *  @author  M.Frank
 */
void dexpert_disconnectDevices(string top) {
  string child;
  dyn_string children = fwCU_getIncludedTreeDevices(top);
  for(int i=1, n=dynlen(children); i<=n; ++i)  {
    child = substr(children[i],strpos(children[i],":")+1);
    fwCU_disconnectState(child);
  }
  ctrlUtils_trace("Disconnected from state of "+dynlen(children)+" children.");
}

void dexpert_sendCommand(string node, string cmd) {
  ctrlUtils_trace("Sending command '"+cmd+"' to '"+node+"'");
  fwCU_sendCommand(node,cmd);
}

void dexpert_handleTopNodeError(string node, string state) {
  if ( s_recoveryRetries < s_maxRecoveryRetries ) {
    dexpert_sendCommand(s_top_name,"RECOVER");
  }
  else {
    ctrlUtils_trace("Exceeded the maximum number of retries. Need to recover manually!");
    s_active = 0;
  }
}

void dexpert_handleTopNodeRecovery(string node, string state)   {
  if ( s_lastState != state ) {
    switch(state) {
    case "NOT_READY":
      dexpert_sendCommand(s_top_name,"CONFIGURE");
      break;
    case "CONFIGURED":
      dexpert_sendCommand(s_top_name,"CREATE");
      break;
    case "CREATED":
      dexpert_sendCommand(s_top_name,"INITIALIZE");
      break;
    case "READY":
      dexpert_sendCommand(s_top_name,"START");
      break;
    }
    s_lastState = state;
  }
}

/**
 *  Callback executed when the top level node changes it's state
 *  @author  M.Frank
 */
void dexpert_cb_main(string node, string state) {
  ctrlUtils_trace("Node:"+node+" State:"+state);
  if ( s_active < 2 )  {
    switch(state)  {
    case "ERROR":
      s_recoveryRetries = 0;
      dexpert_handleTopNodeError(node,state);
      s_active = 2;
      break;
    default:
      break;
    }
  }

  if ( s_active == 2 )  {
    switch(state)  {
    case "ERROR":
      ++s_recoveryRetries;
      dexpert_handleTopNodeError(node,state);
      break;
    case "RUNNING":
      s_recoveryRetries = 0;
      s_active = 0;
      break;
    default:
      dexpert_handleTopNodeRecovery(node,state);
      break;
    }
  }
}

/**
 *  Callback executed when any of the subscribed children changes it's state
 *  @author  M.Frank
 */
void dexpert_cb_state(string node, string state) {
  string type;
  fwCU_getType(node,type); 
  ctrlUtils_trace("Node:"+node+" Type:"+type+" State:"+state);
  if ( type == "FSM_DimTask" ) {
    dexpert_handleTask(node);
  }
  else if ( state == "ERROR" ) {
    dexpert_handleError(node);
  }
  else if ( state == "OFFLINE" ) {
  }
}

/**
 *  Initialize the dexpert application.
 *  @author  M.Frank
 */
int dexpert_initialize(string name) {
  string dp = name+"_Dexpert";
  s_top_name = name;
  if ( !dpExists(dp) ) {
    int ret = dpCreate(dp,"StreamConfigurator");
    if ( ret != 0 ) {
      ctrlUtils_trace("Failed to create the datapoint:"+dp+" for expert system.");
      return ret;
    }
  }
  fwCU_connectState("dexpert_cb_main",s_top_name);
  return 0;
}

/**
 *  Execute the dexpert application.
 *  @author  M.Frank
 */
void dexpert_run() {
  while(1) {    delay(1);  }
}
