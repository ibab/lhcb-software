
int currFillNo = 0;
string currFillStart;
string currFillRamped;
string currFillStable;
string currFillEnded;

dyn_string fill_history;

string getTimeStamp() {
  return formatTime("%d-%m-%Y %H:%M:%S",getCurrentTime());
}
float getIntensityBeam1() {
  float val;
  int result = dpGet("LHC:LHCCOM/LHC.LHC.Beam.Intensity.Beam1.totalIntensity",val);
  return val;
}
float getIntensityBeam2() {
  float val;
  int result = dpGet("LHC:LHCCOM/LHC.LHC.Beam.Intensity.Beam2.totalIntensity",val);
  return val;
}

string getValues() {
  string r = t+"/";
  float v1, v2, v3, v4, v5;
  int fillNo;
  int result = dpGet("LHC:LHCCOM/LHC.LHC.Beam.Intensity.Beam1.totalIntensity",v1,
		     "LHC:LHCCOM/LHC.LHC.Beam.Intensity.Beam2.totalIntensity",v2,
		     "LHC:LHCCOM/LHC.LHC.Beam.Energy",v3,
		     "LHC:LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_GP",v4,
		     "LHC:LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntDel_GP",v5,
		     "LHC:LHCCOM/LHC.LHC.RunControl.FillNumber",fillNo
		     );
  if ( result == 0 ) {
    float hv_effi, velo_effi, daq_effi, dead_effi, lumi;
    result = dpGet("ECS:LHCbEfficiency.ResultsLumi.HV",hv_effi,
		   "ECS:LHCbEfficiency.ResultsLumi.VELO",velo_effi,
		   "ECS:LHCbEfficiency.ResultsLumi.DAQ",daq_effi,
		   "ECS:LHCbEfficiency.ResultsLumi.DAQLiveTime",dead_effi,
		   "ECS:LHCbEfficiency.Sources.FillNumber", fillNo,
		   "ECS:LHCbEfficiency.LumiTotal", lumi);
    if ( result == 0 ) {
      r = r + fillNo+"/"+v1+"/"+v2+"/"+v3+"/"+hv_effi+"/"+velo_effi+"/"+daq_effi+"/"+dead_effi+"/"+lumi;
      DebugN("[ONLINE]--->",r);
      return r;
    }
  }
  DebugN("[ONLINE]---> Error reading data!");
  return "";
}

string getValuesArchive(time t) {
  string r = getTimeStamp()+"/";
  float v1, v2, v3, v4, v5;
  int fillNo;
  int result = dpGetAsynch(t,
			   "LHC:LHCCOM/LHC.LHC.Beam.Intensity.Beam1.totalIntensity",v1,
			   "LHC:LHCCOM/LHC.LHC.Beam.Intensity.Beam2.totalIntensity",v2,
			   "LHC:LHCCOM/LHC.LHC.Beam.Energy",v3,
			   "LHC:LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntRec_GP",v4,
			   "LHC:LHCCOM/LHC.LHCb.Internal.Luminosity.LumiIntDel_GP",v5,
			   "LHC:LHCCOM/LHC.LHC.RunControl.FillNumber",fillNo
			   );
  if ( result == 0 ) {
    float hv_effi, velo_effi, daq_effi, dead_effi, lumi;
    result = dpGetAsynch(t,
			 "ECS:LHCbEfficiency.ResultsLumi.HV",hv_effi,
			 "ECS:LHCbEfficiency.ResultsLumi.VELO",velo_effi,
			 "ECS:LHCbEfficiency.ResultsLumi.DAQ",daq_effi,
			 "ECS:LHCbEfficiency.ResultsLumi.DAQLiveTime",dead_effi,
			 "ECS:LHCbEfficiency.Sources.FillNumber", fillNo,
			 "ECS:LHCbEfficiency.LumiTotal", lumi);
    if ( result == 0 ) {
      r = r + fillNo+"/"+v1+"/"+v2+"/"+v3+"/"+v4+"/"+v5+"/"+hv_effi+"/"+velo_effi+"/"+daq_effi+"/"+dead_effi+"/"+lumi;
      DebugN("[ARCHIVE]--->",r);
      return r;
    }
  }
  DebugN("[ARCHIVE]---> Error reading data!");
  return "";
}

void call_lhc_state_change(string dp, string value) {
  string t;
  float v1, v2;
  
  if ( value == "PHYSICS" ) {
    t = getValues();
    if ( strlen(t) ) dpSet("STORAGE:LHCbPerformance.CurrentFillStable",t);
  }
  else if ( value == "PHYS_ADJUST" ) {
    t = getValues();
    if ( strlen(t) ) dpSet("STORAGE:LHCbPerformance.CurrentFillRamped",t);
  }
  else if ( value == "DUMP" || value == "EOF" || value == "MD" || value == "NO_BEAM") {
    if ( !strlen(currFillEnded) ) {
      currFillEnded = getValues();
      if ( strlen(currFillEnded) ) {
	dpSet("STORAGE:LHCbPerformance.CurrentFillEnded",currFillEnded);
	dynAppend(fill_history,r);
	if ( dynlen(fill_history)>10 ) {
	  for(int i=1, len=dynlen(fill_history); i<len-10; ++i)
	    dynRemove(i);
	}
	dpSet("STORAGE:LHCbPerformance.History",fill_history);
      }
    }
  }
  else if ( value == "INJECTION" ) {
    t = getValues();
    if ( strlen(t) ) 
      dpSet("STORAGE:LHCbPerformance.CurrentFillStart",t);
    else
      dpSet("STORAGE:LHCbPerformance.CurrentFillStart","");
    dpSet("STORAGE:LHCbPerformance.CurrentFillStable","");
    dpSet("STORAGE:LHCbPerformance.CurrentFillRamped","");
    dpSet("STORAGE:LHCbPerformance.CurrentFillEnded","");
  }
}

void currentFill(string dp, int value) {
  currFillNo = value;
  currFillStart = getTimeStamp();
  currFillRamped = "";
  currFillStable = "";
}

void main() {
  dyn_string lhcState;
  dyn_time   lhcTime, hist_time;
  time t = getCurrentTime();
  int result = dpGetPeriod(makeTime(year(t),month(t),day(t)-10),
			   t,
			   -1,
			   "LHC:LHC_STATE/LHC.State.Text", //Datenpunktelement
			   lhcState, lhcTime);

  if ((result==1) ){//|| (dynlen(lhcState) == 0))     { 
    DebugN("dpGetPeriod verursachte einen Fehler oder es existieren keine lhcState"); 
  }
  else  { 
    int fillNo, started=0, ramped=0, stable=0, ended=0;
    string t, st, prev;
    for(int i=1, len = dynlen(lhcState); i<=len; i++) {
      st = lhcState[i];
      if ( st == "PHYSICS" ) stable = i;
      else if ( st == "INJECTION" ) started = i;
      else if ( st == "PHYS_ADJUST" ) ramped = i;
      else if ( st == "DUMP" || st == "EOF" || st == "MD" || st == "NO_BEAM") ended = i;

      if ( i>1 ) {
	prev = lhcState[i-1];
	float hv_effi, velo_effi, daq_effi, dead_effi, lumi;
	if ( prev=="PHYSICS" ) {
	  dynAppend(hist_time,lhcTime[i]);
	}
	else {
	  DebugN("--->",result,lhcTime[i],prev+" (now:"+lhcState[i]+") ");
	}
      }
    }
    if ( dynlen(hist_time)>10 ) {
      for(int i=1, len=dynlen(fill_history); i<len-10; ++i)
	dynRemove(hist_time,i);
    }
    for(int i=1, len = dynlen(hist_time); i<=len; i++) {
      string r = getValuesArchive(hist_time[i]);
      if ( strlen(r) ) dynAppend(fill_history,r);
    }
    dpSet("STORAGE:LHCbPerformance.History",fill_history);
    if ( started > 0 ) {
      string t = getValuesArchive(lhcTime[started]);
      dpSet("STORAGE:LHCbPerformance.CurrentFillStart",t);
      if ( started>ramped ) {
	dpSet("STORAGE:LHCbPerformance.CurrentFillStable","");
	dpSet("STORAGE:LHCbPerformance.CurrentFillRamped","");
	dpSet("STORAGE:LHCbPerformance.CurrentFillEnded","");
      }
      else if ( ramped > 0 ) {
	string t = getValuesArchive(lhcTime[ramped]);
	dpSet("STORAGE:LHCbPerformance.CurrentFillRamped",t);
	if ( ramped>stable ) {
	  dpSet("STORAGE:LHCbPerformance.CurrentFillStable","");
	  dpSet("STORAGE:LHCbPerformance.CurrentFillEnded","");
	}
	else if ( stable > 0 ) {
	  t = getValuesArchive(lhcTime[stable]);
	  dpSet("STORAGE:LHCbPerformance.CurrentFillStable",t);
	  if ( stable>ended ) {
	    dpSet("STORAGE:LHCbPerformance.CurrentFillEnded","");
	  }
	  else {
	    t = getValuesArchive(lhcTime[ended]);
	    dpSet("STORAGE:LHCbPerformance.CurrentFillStable",t);
	  }
	}
      }
    }
  }

  result = dpConnect("call_lhc_state_change","LHC:LHC_STATE/LHC.State.Text");
  if ( result != 0 ) {
    DebugN("Failed to connect to FillNumber datapoint.");
    exit(0);
  }
  exit(0);
}
