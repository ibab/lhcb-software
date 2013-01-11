/*
=================  Algorithm execution list:
*/
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","DAQ/ODIN","DAQ/RawEvent","Other/RawEvent","Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Header","/Event/Rec/Status",0};
    AlgConfig cfg("BrunelInit",manager,inputs,outputs,1.240e+03,8.162e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "InitBrunelSeq";
    cfg.order   = 4;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("BrunelEventCount",manager,inputs,outputs,7.500e+00,1.432e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "InitBrunelSeq";
    cfg.order   = 5;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FilterNoBeam",manager,inputs,outputs,6.680e+01,3.278e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiNoBeamSeq";
    cfg.order   = 9;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiDecodeNoBeam",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiNoBeamSeq";
    cfg.order   = 10;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiFilterNoBeam",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiNoBeamSeq";
    cfg.order   = 11;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiCountNoBeam",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiNoBeamSeq";
    cfg.order   = 12;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FilterBeamCrossing",manager,inputs,outputs,5.760e+01,3.099e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeamCrossingSeq";
    cfg.order   = 14;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent","Hlt/LumiSummary",0};
    const char* outputs[] = {"/Event/Hlt/LumiSummary",0};
    AlgConfig cfg("LumiDecodeBeamCrossing",manager,inputs,outputs,3.307e+02,2.770e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeamCrossingSeq";
    cfg.order   = 15;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Hlt/LumiSummary","Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiFilterBeamCrossing",manager,inputs,outputs,1.273e+02,1.009e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeamCrossingSeq";
    cfg.order   = 16;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","/Event/Hlt/LumiSummary","DAQ/ODIN","DAQ/RawEvent","Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiCountBeamCrossing",manager,inputs,outputs,4.190e+02,2.809e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeamCrossingSeq";
    cfg.order   = 17;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FilterBeam1",manager,inputs,outputs,5.560e+01,2.467e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam1Seq";
    cfg.order   = 19;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/LumiSummary","/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiDecodeBeam1",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam1Seq";
    cfg.order   = 20;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/LumiSummary","/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiFilterBeam1",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam1Seq";
    cfg.order   = 21;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/LumiSummary","/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FillingBeam1",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam1Seq";
    cfg.order   = 22;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/LumiSummary","/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiCountBeam1",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam1Seq";
    cfg.order   = 23;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FilterBeam2",manager,inputs,outputs,5.210e+01,2.499e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam2Seq";
    cfg.order   = 25;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/LumiSummary","/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiDecodeBeam2",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam2Seq";
    cfg.order   = 26;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/LumiSummary","/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiFilterBeam2",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam2Seq";
    cfg.order   = 27;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/LumiSummary","/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FillingBeam2",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam2Seq";
    cfg.order   = 28;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/LumiSummary","/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiCountBeam2",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam2Seq";
    cfg.order   = 29;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TimeSpanFilter",manager,inputs,outputs,5.460e+01,2.254e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "TimeSpanSeq";
    cfg.order   = 31;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","DAQ/ODIN","DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TimeSpanAccounting",manager,inputs,outputs,2.417e+02,1.898e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "TimeSpanSeq";
    cfg.order   = 32;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/LumiSummary","/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EventAccount",manager,inputs,outputs,6.900e+00,1.221e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiCounters";
    cfg.order   = 33;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/LumiSummary","/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiFilter",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiSeq";
    cfg.order   = 34;
    cfg.level   = 3;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent","Trigger/RawEvent","pRec/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PhysFilter",manager,inputs,outputs,2.108e+02,1.012e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiSeq";
    cfg.order   = 35;
    cfg.level   = 3;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"","/Event/DAQ/RawEvent","DAQ/RawEvent","Trigger/RawEvent","pRec/RawEvent",0};
    const char* outputs[] = {"/Event/Hlt/DecReports",0};
    AlgConfig cfg("HltDecReportsDecoder",manager,inputs,outputs,1.116e+06,2.731e+06);
    cfg.setDaughters(daughters);
    cfg.parent  = "HltFilterSeq";
    cfg.order   = 39;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Hlt/DecReports","Hlt/DecReports",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HltErrorFilter",manager,inputs,outputs,9.414e+01,5.126e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "HltFilterSeq";
    cfg.order   = 40;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HltErrorProc",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "HltErrorSeq";
    cfg.order   = 41;
    cfg.level   = 3;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","Calo/RawEvent","DAQ/RawEvent","Transient/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Prs/Digits","/Event/Transient/DAQ/Status",0};
    AlgConfig cfg("PrsFromRaw",manager,inputs,outputs,1.135e+03,5.179e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PrsBanksHandler";
    cfg.order   = 45;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","/Event/Transient/DAQ/Status","Rec/Status","Transient/DAQ/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PrsProcStatus",manager,inputs,outputs,1.146e+02,4.795e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "PrsBanksHandler";
    cfg.order   = 46;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Transient/DAQ/Status","Calo/RawEvent","DAQ/RawEvent","Transient/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Ecal/Digits",0};
    AlgConfig cfg("EcalZSup",manager,inputs,outputs,1.346e+03,1.885e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "EcalBanksHandler";
    cfg.order   = 48;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","/Event/Transient/DAQ/Status","Rec/Status","Transient/DAQ/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalProcStatus",manager,inputs,outputs,9.229e+01,5.180e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "EcalBanksHandler";
    cfg.order   = 49;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Transient/DAQ/Status","Calo/RawEvent","DAQ/RawEvent","Transient/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Hcal/Digits",0};
    AlgConfig cfg("HcalZSup",manager,inputs,outputs,4.801e+02,8.111e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "HcalBanksHandler";
    cfg.order   = 51;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","/Event/Transient/DAQ/Status","Rec/Status","Transient/DAQ/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HcalProcStatus",manager,inputs,outputs,9.514e+01,4.239e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "HcalBanksHandler";
    cfg.order   = 52;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent","Trigger/RawEvent",0};
    const char* outputs[] = {"/Event/Trig/L0/L0DUReport",0};
    AlgConfig cfg("L0DUFromRaw",manager,inputs,outputs,1.731e+03,2.726e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "L0TriggerSeq";
    cfg.order   = 55;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent","Other/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters",0};
    AlgConfig cfg("DecodeVeloClusters",manager,inputs,outputs,2.429e+03,7.280e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoDecodingSeq";
    cfg.order   = 58;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","DAQ/ODIN","DAQ/RawEvent","Other/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/TT/Clusters","/Event/Rec/TT/Summary",0};
    AlgConfig cfg("CreateTTClusters",manager,inputs,outputs,1.774e+03,6.817e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoDecodingSeq";
    cfg.order   = 59;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","DAQ/ODIN","DAQ/RawEvent","Other/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/TT/LiteClusters",0};
    AlgConfig cfg("CreateTTLiteClusters",manager,inputs,outputs,5.153e+02,2.158e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoDecodingSeq";
    cfg.order   = 60;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","DAQ/ODIN","DAQ/RawEvent","Other/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/IT/Clusters","/Event/Rec/IT/Summary",0};
    AlgConfig cfg("CreateITClusters",manager,inputs,outputs,1.192e+03,3.742e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoDecodingSeq";
    cfg.order   = 61;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","DAQ/ODIN","DAQ/RawEvent","Other/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/IT/LiteClusters",0};
    AlgConfig cfg("CreateITLiteClusters",manager,inputs,outputs,4.090e+02,8.316e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoDecodingSeq";
    cfg.order   = 62;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Velo/LiteClusters","Raw/Velo/LiteClusters","Rec/Track/Velo",0};
    const char* outputs[] = {"/Event/Rec/Track/Velo",0};
    AlgConfig cfg("FastVeloTracking",manager,inputs,outputs,5.763e+03,3.396e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoVELOSeq";
    cfg.order   = 64;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/IT/LiteClusters","/Event/Raw/TT/LiteClusters","/Event/Rec/Track/Velo","Raw/IT/LiteClusters","Raw/TT/LiteClusters","Rec/Track/Forward","Rec/Track/Velo",0};
    const char* outputs[] = {"/Event/Rec/Track/Forward",0};
    AlgConfig cfg("PatForward",manager,inputs,outputs,1.248e+05,9.828e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackForwardPatSeq";
    cfg.order   = 71;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Track/Seed",0};
    AlgConfig cfg("PatSeeding",manager,inputs,outputs,1.095e+05,1.089e+05);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackSeedPatSeq";
    cfg.order   = 73;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","Rec/Track/Seed","Rec/Track/Velo",0};
    const char* outputs[] = {"/Event/Rec/Track/Match",0};
    AlgConfig cfg("PatMatch",manager,inputs,outputs,2.876e+03,2.338e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackMatchPatSeq";
    cfg.order   = 75;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Match","/Event/Rec/Track/Seed","Rec/Track/Match","Rec/Track/Seed",0};
    const char* outputs[] = {"/Event/Rec/Track/Downstream",0};
    AlgConfig cfg("PatDownstream",manager,inputs,outputs,3.577e+04,6.005e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackDownstreamPatSeq";
    cfg.order   = 77;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/TT/Clusters","/Event/Raw/Velo/Clusters","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Velo","Raw/TT/Clusters","Raw/Velo/Clusters","Rec/Track/Forward","Rec/Track/Match","Rec/Track/Velo",0};
    const char* outputs[] = {"/Event/Rec/Track/VeloTT",0};
    AlgConfig cfg("PatVeloTT",manager,inputs,outputs,1.201e+05,2.284e+05);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackVeloTTPatSeq";
    cfg.order   = 79;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","Raw/IT/Clusters","Raw/IT/LiteClusters","Raw/TT/Clusters","Raw/TT/LiteClusters","Raw/Velo/Clusters","Rec/Track/Downstream","Rec/Track/Forward","Rec/Track/Match","Rec/Track/Seed","Rec/Track/Velo","Rec/Track/VeloTT",0};
    const char* outputs[] = {"/Event/Rec/Track/Best",0};
    AlgConfig cfg("TrackBestTrackCreator",manager,inputs,outputs,8.415e+05,1.003e+06);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackFitSeq";
    cfg.order   = 81;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Link/Rec/Track/BestClones","Link/Rec/Track/BestClones",0};
    AlgConfig cfg("FindTrackClones",manager,inputs,outputs,2.205e+04,1.898e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackClonesSeq";
    cfg.order   = 84;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Link/Rec/Track/BestClones","/Event/Rec/Track/Best","Link/Rec/Track/BestClones","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FlagTrackClones",manager,inputs,outputs,6.083e+02,5.454e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackClonesSeq";
    cfg.order   = 85;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackAddLikelihood",manager,inputs,outputs,3.842e+04,1.733e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackAddExtraInfoSeq";
    cfg.order   = 86;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/IT/Clusters","/Event/Raw/TT/Clusters","/Event/Raw/Velo/Clusters","/Event/Rec/Track/Best","Raw/IT/Clusters","Raw/TT/Clusters","Raw/Velo/Clusters","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackAddNNGhostId",manager,inputs,outputs,1.023e+05,5.302e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackAddExtraInfoSeq";
    cfg.order   = 87;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackEraseExtraInfo",manager,inputs,outputs,1.426e+02,5.544e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackEraseExtraInformationSeq";
    cfg.order   = 89;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Vertex/Primary",0};
    AlgConfig cfg("PatPVOffline",manager,inputs,outputs,3.442e+03,2.543e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoVertexSeq";
    cfg.order   = 91;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","/Event/Rec/Vertex/Primary","Rec/Track/Best","Rec/Vertex/Primary",0};
    const char* outputs[] = {"/Event/Rec/Vertex/V0",0};
    AlgConfig cfg("TrackV0Finder",manager,inputs,outputs,6.755e+03,4.940e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoVertexSeq";
    cfg.order   = 92;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Rich/RecoEvent/Offline/Status","Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Rich/RecoEvent/Offline/Status",0};
    AlgConfig cfg("CheckProcOfflineStatus",manager,inputs,outputs,2.487e+02,1.526e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecInitOfflineSeq";
    cfg.order   = 95;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent","Rich/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("DecodeRawRichOffline",manager,inputs,outputs,1.596e+03,1.393e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflinePixelsSeq";
    cfg.order   = 97;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Rich/RecoEvent/Offline/Pixels","Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Rich/RecoEvent/Offline/Pixels",0};
    AlgConfig cfg("CreateOfflinePixels",manager,inputs,outputs,1.272e+04,4.588e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflinePixelsSeq";
    cfg.order   = 98;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","/Event/Rec/Track/Best","Rec/Rich/RecoEvent/Offline/SegmentHypoRings","Rec/Rich/RecoEvent/Offline/Segments","Rec/Rich/RecoEvent/Offline/Tracks","Rec/Status","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Tracks",0};
    AlgConfig cfg("CreateOfflineTracks",manager,inputs,outputs,6.484e+04,3.270e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineTracksSeq";
    cfg.order   = 100;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Rich/RecoEvent/Offline/Photons","Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Rich/RecoEvent/Offline/Photons",0};
    AlgConfig cfg("CreateOfflinePhotons",manager,inputs,outputs,2.240e+05,1.413e+05);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflinePhotonsSeq";
    cfg.order   = 102;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll",0};
    AlgConfig cfg("OfflineENNFinderR1GasTop",manager,inputs,outputs,6.001e+02,3.796e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 104;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll","Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OfflineENNFinderR1GasBottom",manager,inputs,outputs,3.151e+02,1.507e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 105;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll","Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OfflineENNFinderR2GasLeft",manager,inputs,outputs,3.660e+02,2.175e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 106;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll","Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OfflineENNFinderR2GasRight",manager,inputs,outputs,3.807e+02,1.417e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 107;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll","Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OfflineENNRingsSegAssoc",manager,inputs,outputs,9.886e+01,1.091e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 108;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll","Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsBest",0};
    AlgConfig cfg("OfflineBestENNRings",manager,inputs,outputs,1.913e+02,1.423e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 109;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsBest","Rec/Rich/ENN/Offline/RingsBest",0};
    const char* outputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsIsolated",0};
    AlgConfig cfg("OfflineIsolatedENNRings",manager,inputs,outputs,1.231e+02,5.784e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 110;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Prs/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDInit",manager,inputs,outputs,7.286e+00,1.829e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDInitSeq";
    cfg.order   = 114;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDDigSel",manager,inputs,outputs,7.686e+01,4.880e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDInitSeq";
    cfg.order   = 115;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Rich/GlobalPID/Offline/PIDs","Rec/Rich/GlobalPID/Offline/Tracks","Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks",0};
    AlgConfig cfg("RichOfflineGPIDTrSel",manager,inputs,outputs,6.794e+02,2.938e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDInitSeq";
    cfg.order   = 116;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Prs/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineBckEstIt0",manager,inputs,outputs,1.111e+03,4.402e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDLLSeq";
    cfg.order   = 118;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Status","Rec/Rich/GlobalPID/Offline/Tracks","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDLLIt0",manager,inputs,outputs,1.657e+05,1.137e+05);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDLLSeq";
    cfg.order   = 119;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Prs/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineBckEstIt1",manager,inputs,outputs,1.588e+03,5.931e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDLLSeq";
    cfg.order   = 120;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Status","Rec/Rich/GlobalPID/Offline/Tracks","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDLLIt1",manager,inputs,outputs,1.749e+05,1.483e+05);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDLLSeq";
    cfg.order   = 121;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/GlobalPID/Offline/Tracks","Rec/Rich/GlobalPID/Offline/Tracks",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDFin",manager,inputs,outputs,1.444e+02,1.014e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDSeq";
    cfg.order   = 122;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Status","Rec/Rich/GlobalPID/Offline/PIDs","Rec/Rich/PIDs","Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Rich/PIDs",0};
    AlgConfig cfg("MergeOfflineRichPIDs",manager,inputs,outputs,3.103e+02,1.446e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflinePIDSeq";
    cfg.order   = 123;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Transient/DAQ/Status","Calo/RawEvent","DAQ/RawEvent","Transient/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Spd/Digits",0};
    AlgConfig cfg("SpdFromRaw",manager,inputs,outputs,5.281e+02,1.001e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigits";
    cfg.order   = 127;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","Raw/Ecal/Digits","Raw/Hcal/Digits","Raw/Prs/Digits","Raw/Spd/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloDigitFilter",manager,inputs,outputs,4.055e+03,2.313e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ClusterReco";
    cfg.order   = 129;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Ecal/Digits","Raw/Ecal/Digits",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalClusters",0};
    AlgConfig cfg("EcalClust",manager,inputs,outputs,2.651e+03,1.181e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ClusterReco";
    cfg.order   = 130;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","Rec/Calo/EcalClusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalShare",manager,inputs,outputs,1.623e+03,1.078e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ClusterReco";
    cfg.order   = 131;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","Rec/Calo/EcalClusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalCovar",manager,inputs,outputs,5.416e+02,1.952e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ClusterReco";
    cfg.order   = 132;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/InAccEcal",0};
    AlgConfig cfg("InECAL",manager,inputs,outputs,1.372e+03,1.113e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloTrackMatch";
    cfg.order   = 135;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/InAccEcal","/Event/Rec/Track/Best","Rec/Calo/EcalClusters","Rec/Calo/InAccEcal","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/ClusterMatch",0};
    AlgConfig cfg("ClusterMatch",manager,inputs,outputs,4.274e+03,3.647e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloTrackMatch";
    cfg.order   = 136;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalClusters","Raw/Prs/Digits","Raw/Spd/Digits","Rec/Calo/ClusterMatch","Rec/Calo/EcalClusters",0};
    const char* outputs[] = {"/Event/Rec/Calo/Photons",0};
    AlgConfig cfg("SinglePhotonRec",manager,inputs,outputs,1.526e+04,3.467e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "PhotonReco";
    cfg.order   = 137;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/EcalClusters","Raw/Prs/Digits","Raw/Spd/Digits","Rec/Calo/EcalClusters",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/SplitPhotons",0};
    AlgConfig cfg("MergedPi0Rec",manager,inputs,outputs,1.120e+03,9.593e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MergedPi0Reco";
    cfg.order   = 139;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalClusters","Raw/Prs/Digits","Raw/Spd/Digits","Rec/Calo/ClusterMatch","Rec/Calo/EcalClusters",0};
    const char* outputs[] = {"/Event/Rec/Calo/Electrons",0};
    AlgConfig cfg("SingleElectronRec",manager,inputs,outputs,1.749e+04,7.958e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ElectronReco";
    cfg.order   = 141;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/InAccSpd",0};
    AlgConfig cfg("InSPD",manager,inputs,outputs,8.777e+02,4.798e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "InCaloAcceptance";
    cfg.order   = 146;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/InAccPrs",0};
    AlgConfig cfg("InPRS",manager,inputs,outputs,5.621e+02,2.820e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "InCaloAcceptance";
    cfg.order   = 147;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/InAccHcal",0};
    AlgConfig cfg("InHCAL",manager,inputs,outputs,7.616e+02,4.313e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "InCaloAcceptance";
    cfg.order   = 148;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/InAccBrem",0};
    AlgConfig cfg("InBREM",manager,inputs,outputs,3.417e+02,1.620e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "InCaloAcceptance";
    cfg.order   = 149;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/Electrons","/Event/Rec/Calo/InAccEcal","/Event/Rec/Track/Best","Rec/Calo/Electrons","Rec/Calo/InAccEcal","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/ElectronMatch",0};
    AlgConfig cfg("ElectronMatch",manager,inputs,outputs,4.917e+03,4.498e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloMatch";
    cfg.order   = 151;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/Photons","/Event/Rec/Track/Best","Rec/Calo/InAccBrem","Rec/Calo/Photons","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/BremMatch",0};
    AlgConfig cfg("BremMatch",manager,inputs,outputs,2.575e+03,1.652e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloMatch";
    cfg.order   = 152;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Spd/Digits","/Event/Rec/Calo/InAccSpd","/Event/Rec/Track/Best","Raw/Spd/Digits","Rec/Calo/InAccSpd","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/SpdE",0};
    AlgConfig cfg("SpdE",manager,inputs,outputs,6.974e+02,7.003e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloEnergy";
    cfg.order   = 154;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Rec/Calo/InAccPrs","/Event/Rec/Track/Best","Raw/Prs/Digits","Rec/Calo/InAccPrs","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/PrsE",0};
    AlgConfig cfg("PrsE",manager,inputs,outputs,4.969e+02,2.704e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloEnergy";
    cfg.order   = 155;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Ecal/Digits","/Event/Rec/Calo/InAccEcal","/Event/Rec/Track/Best","Raw/Ecal/Digits","Rec/Calo/InAccEcal","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalE",0};
    AlgConfig cfg("EcalE",manager,inputs,outputs,8.769e+02,4.084e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloEnergy";
    cfg.order   = 156;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Rec/Calo/InAccHcal","/Event/Rec/Track/Best","Raw/Hcal/Digits","Rec/Calo/InAccHcal","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/HcalE",0};
    AlgConfig cfg("HcalE",manager,inputs,outputs,8.776e+02,4.188e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloEnergy";
    cfg.order   = 157;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/ElectronMatch","/Event/Rec/Track/Best","Rec/Calo/ElectronMatch","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalChi2",0};
    AlgConfig cfg("EcalChi22ID",manager,inputs,outputs,6.907e+02,8.968e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloChi2";
    cfg.order   = 159;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/BremMatch","/Event/Rec/Track/Best","Rec/Calo/BremMatch","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/BremChi2",0};
    AlgConfig cfg("BremChi22ID",manager,inputs,outputs,2.704e+02,1.501e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloChi2";
    cfg.order   = 160;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/ClusterMatch","/Event/Rec/Track/Best","Rec/Calo/ClusterMatch","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/ClusChi2",0};
    AlgConfig cfg("ClusChi22ID",manager,inputs,outputs,1.899e+03,3.755e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloChi2";
    cfg.order   = 161;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/PrsE","Rec/Calo/PrsE",0};
    const char* outputs[] = {"/Event/Rec/Calo/PrsPIDe",0};
    AlgConfig cfg("PrsPIDe",manager,inputs,outputs,2.256e+02,1.306e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLe";
    cfg.order   = 163;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalChi2","Rec/Calo/EcalChi2",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalPIDe",0};
    AlgConfig cfg("EcalPIDe",manager,inputs,outputs,2.211e+02,1.085e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLe";
    cfg.order   = 164;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/BremChi2","Rec/Calo/BremChi2",0};
    const char* outputs[] = {"/Event/Rec/Calo/BremPIDe",0};
    AlgConfig cfg("BremPIDe",manager,inputs,outputs,2.016e+02,8.715e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLe";
    cfg.order   = 165;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/HcalE","Rec/Calo/HcalE",0};
    const char* outputs[] = {"/Event/Rec/Calo/HcalPIDe",0};
    AlgConfig cfg("HcalPIDe",manager,inputs,outputs,1.933e+02,9.443e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLe";
    cfg.order   = 166;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalE","Rec/Calo/EcalE",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalPIDmu",0};
    AlgConfig cfg("EcalPIDmu",manager,inputs,outputs,2.019e+02,9.741e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLmu";
    cfg.order   = 168;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/HcalE","Rec/Calo/HcalE",0};
    const char* outputs[] = {"/Event/Rec/Calo/HcalPIDmu",0};
    AlgConfig cfg("HcalPIDmu",manager,inputs,outputs,1.923e+02,9.972e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLmu";
    cfg.order   = 169;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Photons","Raw/Hcal/Digits","Raw/Prs/Digits","Raw/Spd/Digits","Rec/Calo/BremMatch","Rec/Calo/ClusterMatch","Rec/Calo/ElectronMatch","Rec/Calo/Photons",0};
    const char* outputs[] = {"/Event/Rec/Calo/PhotonID",0};
    AlgConfig cfg("PhotonID",manager,inputs,outputs,2.600e+04,9.954e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "NeutralPIDs";
    cfg.order   = 171;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/MergedPi0s","Raw/Hcal/Digits","Raw/Prs/Digits","Raw/Spd/Digits","Rec/Calo/BremMatch","Rec/Calo/ClusterMatch","Rec/Calo/ElectronMatch","Rec/Calo/MergedPi0s",0};
    const char* outputs[] = {"/Event/Rec/Calo/MergedID",0};
    AlgConfig cfg("MergedID",manager,inputs,outputs,1.044e+03,1.074e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "NeutralPIDs";
    cfg.order   = 172;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/SplitPhotons","Raw/Hcal/Digits","Raw/Prs/Digits","Raw/Spd/Digits","Rec/Calo/BremMatch","Rec/Calo/ClusterMatch","Rec/Calo/ElectronMatch","Rec/Calo/SplitPhotons",0};
    const char* outputs[] = {"/Event/Rec/Calo/PhotonFromMergedID",0};
    AlgConfig cfg("PhotonFromMergedID",manager,inputs,outputs,9.856e+02,9.859e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "NeutralPIDs";
    cfg.order   = 173;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Transient/DAQ/Status","DAQ/RawEvent","Muon/RawEvent","Transient/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Muon/Coords",0};
    AlgConfig cfg("MuonRec",manager,inputs,outputs,1.024e+03,7.653e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoMUONSeq";
    cfg.order   = 175;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","/Event/Transient/DAQ/Status","Rec/Status","Transient/DAQ/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonProcStatus",manager,inputs,outputs,7.486e+01,1.192e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoMUONSeq";
    cfg.order   = 176;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Transient/DAQ/Status","Transient/DAQ/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonROFilter",manager,inputs,outputs,3.857e+01,7.743e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoMUONSeq";
    cfg.order   = 177;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Muon/Coords","/Event/Rec/Track/Best","Raw/Muon/Coords","Rec/Muon/MuonPID","Rec/Track/Best","Rec/Track/Muon",0};
    const char* outputs[] = {"/Event/Rec/Muon/MuonPID","/Event/Rec/Track/Muon",0};
    AlgConfig cfg("MuonIDAlg",manager,inputs,outputs,2.365e+04,3.291e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "MuonIDSeq";
    cfg.order   = 179;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/ProtoP/Charged","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/ProtoP/Charged",0};
    AlgConfig cfg("ChargedProtoPMaker",manager,inputs,outputs,6.491e+02,8.347e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 182;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/ProtoP/Charged","Raw/Hcal/Digits","Raw/Prs/Digits","Raw/Spd/Digits","Rec/Calo/BremMatch","Rec/Calo/ClusChi2","Rec/Calo/ClusterMatch","Rec/Calo/EcalChi2","Rec/Calo/EcalE","Rec/Calo/EcalPIDe","Rec/Calo/EcalPIDmu","Rec/Calo/ElectronMatch","Rec/Calo/InAccEcal","Rec/Calo/MergedID","Rec/Calo/PhotonFromMergedID","Rec/Calo/PhotonID","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddEcal",manager,inputs,outputs,5.566e+04,3.838e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 183;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/ProtoP/Charged","Raw/Hcal/Digits","Raw/Prs/Digits","Raw/Spd/Digits","Rec/Calo/BremChi2","Rec/Calo/BremMatch","Rec/Calo/BremPIDe","Rec/Calo/ClusterMatch","Rec/Calo/ElectronMatch","Rec/Calo/InAccBrem","Rec/Calo/MergedID","Rec/Calo/PhotonFromMergedID","Rec/Calo/PhotonID","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddBrem",manager,inputs,outputs,5.864e+04,2.072e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 184;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccHcal","/Event/Rec/ProtoP/Charged","Rec/Calo/HcalE","Rec/Calo/HcalPIDe","Rec/Calo/HcalPIDmu","Rec/Calo/InAccHcal","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddHcal",manager,inputs,outputs,3.246e+02,1.274e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 185;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/ProtoP/Charged","Rec/Calo/InAccPrs","Rec/Calo/PrsE","Rec/Calo/PrsPIDe","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddPrs",manager,inputs,outputs,7.610e+02,1.251e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 186;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/SpdE","/Event/Rec/ProtoP/Charged","Rec/Calo/InAccSpd","Rec/Calo/SpdE","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddSpd",manager,inputs,outputs,1.819e+02,6.098e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 187;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddVeloDEDX",manager,inputs,outputs,6.014e+02,3.073e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 188;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","/Event/Rec/Rich/PIDs","Rec/ProtoP/Charged","Rec/Rich/PIDs",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddRich",manager,inputs,outputs,5.494e+02,5.999e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 189;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","Rec/Muon/MuonPID","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddMuon",manager,inputs,outputs,7.740e+02,1.326e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 190;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPCombDLLs",manager,inputs,outputs,3.247e+02,2.229e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 191;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongElectron",manager,inputs,outputs,9.099e+03,4.536e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 193;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongMuon",manager,inputs,outputs,7.340e+02,7.198e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 194;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongPion",manager,inputs,outputs,8.281e+03,3.862e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 195;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongKaon",manager,inputs,outputs,8.439e+03,3.943e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 196;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongProton",manager,inputs,outputs,9.072e+03,4.241e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 197;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongGhost",manager,inputs,outputs,8.205e+03,3.824e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 198;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamElectron",manager,inputs,outputs,3.600e+03,2.076e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 199;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamMuon",manager,inputs,outputs,3.237e+02,2.511e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 200;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamPion",manager,inputs,outputs,3.642e+03,2.109e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 201;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamKaon",manager,inputs,outputs,3.747e+03,2.173e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 202;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamProton",manager,inputs,outputs,3.696e+03,2.151e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 203;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamGhost",manager,inputs,outputs,3.743e+03,2.161e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 204;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamElectron",manager,inputs,outputs,1.033e+03,4.997e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 205;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamMuon",manager,inputs,outputs,9.997e+02,4.847e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 206;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamPion",manager,inputs,outputs,1.017e+03,4.870e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 207;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamKaon",manager,inputs,outputs,1.069e+03,5.171e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 208;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamProton",manager,inputs,outputs,1.066e+03,5.061e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 209;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamGhost",manager,inputs,outputs,1.067e+03,5.131e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 210;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/SplitPhotons","Raw/Hcal/Digits","Raw/Prs/Digits","Raw/Spd/Digits","Rec/Calo/BremMatch","Rec/Calo/ClusterMatch","Rec/Calo/ElectronMatch","Rec/Calo/MergedID","Rec/Calo/MergedPi0s","Rec/Calo/PhotonFromMergedID","Rec/Calo/PhotonID","Rec/Calo/Photons","Rec/Calo/SplitPhotons","Rec/ProtoP/Neutrals",0};
    const char* outputs[] = {"/Event/Rec/ProtoP/Neutrals",0};
    AlgConfig cfg("NeutralProtoPMaker",manager,inputs,outputs,9.863e+04,4.787e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "NeutralProtoParticles";
    cfg.order   = 212;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/IT/Clusters","/Event/Raw/Muon/Coords","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/Velo/Clusters","/Event/Rec/Track/Best","/Event/Rec/Track/Muon","/Event/Rec/Vertex/Primary","Raw/IT/Clusters","Raw/Muon/Coords","Raw/Spd/Digits","Raw/TT/Clusters","Raw/Velo/Clusters","Rec/Track/Best","Rec/Track/Muon","Rec/Vertex/Primary",0};
    const char* outputs[] = {"/Event/Rec/Summary",0};
    AlgConfig cfg("RecSummary",manager,inputs,outputs,8.854e+02,6.185e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoSUMMARYSeq";
    cfg.order   = 214;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OverallEventProcTime",manager,inputs,outputs,1.943e+01,5.876e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 217;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackEventProcTime",manager,inputs,outputs,2.186e+01,1.552e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 218;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("VertexEventProcTime",manager,inputs,outputs,1.600e+01,3.854e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 219;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichEventProcTime",manager,inputs,outputs,1.171e+01,3.411e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 220;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloEventProcTime",manager,inputs,outputs,1.100e+01,3.423e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 221;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonEventProcTime",manager,inputs,outputs,1.086e+01,2.997e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 222;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ProtoEventProcTime",manager,inputs,outputs,1.000e+01,3.295e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 223;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ProcStatAbortMoni",manager,inputs,outputs,6.157e+01,3.865e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 224;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Ecal/Digits","Raw/Ecal/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalDigitMon",manager,inputs,outputs,3.826e+03,1.296e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigitsMoni";
    cfg.order   = 227;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","Raw/Hcal/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HcalDigitMon",manager,inputs,outputs,9.616e+02,2.767e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigitsMoni";
    cfg.order   = 228;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits","Raw/Prs/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PrsDigitMon",manager,inputs,outputs,2.723e+03,4.651e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigitsMoni";
    cfg.order   = 229;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Spd/Digits","Raw/Spd/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("SpdDigitMon",manager,inputs,outputs,9.799e+02,3.138e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigitsMoni";
    cfg.order   = 230;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Spd/Digits","Raw/Spd/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("SpdMon",manager,inputs,outputs,3.879e+03,1.351e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigitsMoni";
    cfg.order   = 231;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Ecal/Digits","Raw/Ecal/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalEFlowMon",manager,inputs,outputs,8.581e+02,2.752e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloEFlowMoni";
    cfg.order   = 233;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","Rec/Calo/EcalClusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalClusterMon",manager,inputs,outputs,6.109e+02,1.799e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloClustersMoni";
    cfg.order   = 235;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/Electrons","Rec/Calo/EcalClusters","Rec/Calo/Electrons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ElectronMon",manager,inputs,outputs,4.800e+02,2.112e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloHyposMoni";
    cfg.order   = 237;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/Photons","Rec/Calo/EcalClusters","Rec/Calo/Photons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PhotonMon",manager,inputs,outputs,6.027e+02,1.562e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloHyposMoni";
    cfg.order   = 238;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/SplitPhotons","Rec/Calo/EcalClusters","Rec/Calo/SplitPhotons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("SplitPhotonMon",manager,inputs,outputs,1.014e+02,4.661e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloHyposMoni";
    cfg.order   = 239;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/MergedPi0s","Rec/Calo/EcalClusters","Rec/Calo/MergedPi0s",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MergedPi0Mon",manager,inputs,outputs,1.059e+02,5.486e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloHyposMoni";
    cfg.order   = 240;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/Photons","Raw/Prs/Digits","Raw/Spd/Digits","Rec/Calo/Photons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ResolvedPi0Mon",manager,inputs,outputs,9.303e+02,3.746e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "DiPhotonMoni";
    cfg.order   = 242;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/Photons","Raw/Prs/Digits","Raw/Spd/Digits","Rec/Calo/Photons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EtaMon",manager,inputs,outputs,1.540e+03,1.833e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "DiPhotonMoni";
    cfg.order   = 243;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ProtoElectronMon",manager,inputs,outputs,1.048e+03,5.922e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniCALOSeq";
    cfg.order   = 244;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalSplitClusters","Rec/Calo/ClusterMatch","Rec/Calo/EcalClusters","Rec/Calo/EcalSplitClusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PhotonMatchMon",manager,inputs,outputs,1.318e+03,8.875e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 246;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","Rec/Calo/ElectronMatch","Rec/Calo/Electrons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ElectronMatchMon",manager,inputs,outputs,2.859e+02,1.423e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 247;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/Photons","Rec/Calo/BremMatch","Rec/Calo/Photons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("BremMatchMon",manager,inputs,outputs,5.477e+02,2.378e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 248;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloEMuPIDMonUncut",manager,inputs,outputs,3.953e+02,1.750e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 249;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloEMuPIDMonSoft",manager,inputs,outputs,1.171e+02,3.379e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 250;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloEMuPIDMonHard",manager,inputs,outputs,9.671e+01,1.568e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 251;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent","Rich/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRawDataSize",manager,inputs,outputs,1.193e+03,1.031e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawMoni";
    cfg.order   = 254;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Trig/L0/L0DUReport","Trig/L0/L0DUReport",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichL0Filter",manager,inputs,outputs,6.857e+01,4.090e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawDataSizeL0Seq";
    cfg.order   = 256;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Hlt/DecReports","Hlt/DecReports",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichHlt1Filter",manager,inputs,outputs,5.929e+01,1.994e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawDataSizeL0Seq";
    cfg.order   = 257;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRawDataSizeL0",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawDataSizeL0Seq";
    cfg.order   = 258;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRawDataDBCheck",manager,inputs,outputs,9.556e+02,2.419e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawMoni";
    cfg.order   = 259;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichHotPixels",manager,inputs,outputs,1.503e+03,5.071e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawMoni";
    cfg.order   = 260;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichDecodingErrors",manager,inputs,outputs,7.369e+02,8.938e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawMoni";
    cfg.order   = 261;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRecPixelQC",manager,inputs,outputs,1.613e+03,4.588e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPixelMoni";
    cfg.order   = 263;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRecPixelClusters",manager,inputs,outputs,2.085e+03,6.997e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPixelMoni";
    cfg.order   = 264;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiLongTrkEff",manager,inputs,outputs,5.786e+02,2.936e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 266;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiForwardTrkEff",manager,inputs,outputs,3.953e+02,1.502e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 267;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiMatchTrkEff",manager,inputs,outputs,1.639e+02,5.235e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 268;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiKsTrackTrkEff",manager,inputs,outputs,3.011e+02,1.446e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 269;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiVeloTTTrkEff",manager,inputs,outputs,1.850e+02,4.802e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 270;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiSeedTrkEff",manager,inputs,outputs,4.790e+02,3.204e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 271;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResLong",manager,inputs,outputs,2.287e+04,1.438e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 273;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResForward",manager,inputs,outputs,4.726e+03,2.876e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 274;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResMatch",manager,inputs,outputs,9.110e+02,5.427e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 275;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResKsTrack",manager,inputs,outputs,1.383e+03,7.983e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 276;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResVeloTT",manager,inputs,outputs,2.540e+02,1.848e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 277;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResSeed",manager,inputs,outputs,2.597e+03,2.123e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 278;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResLongTight",manager,inputs,outputs,1.349e+04,6.085e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 280;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResForwardTight",manager,inputs,outputs,2.872e+03,1.150e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 281;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResMatchTight",manager,inputs,outputs,5.667e+02,3.059e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 282;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResKsTrackTight",manager,inputs,outputs,5.736e+02,4.318e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 283;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResVeloTTTight",manager,inputs,outputs,1.696e+02,1.342e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 284;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResSeedTight",manager,inputs,outputs,1.806e+03,1.306e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 285;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/PIDs","/Event/Rec/Track/Best","Rec/Rich/PIDs",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiPIDMonLong2To100",manager,inputs,outputs,6.533e+02,1.022e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPIDMoni";
    cfg.order   = 287;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/PIDs","/Event/Rec/Track/Best","Rec/Rich/PIDs",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiPIDMonLong2To10",manager,inputs,outputs,2.733e+02,2.294e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPIDMoni";
    cfg.order   = 288;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/PIDs","/Event/Rec/Track/Best","Rec/Rich/PIDs",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiPIDMonLong10To70",manager,inputs,outputs,2.517e+02,2.616e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPIDMoni";
    cfg.order   = 289;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/PIDs","/Event/Rec/Track/Best","Rec/Rich/PIDs",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiPIDMonLong70To100",manager,inputs,outputs,1.846e+02,1.275e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPIDMoni";
    cfg.order   = 290;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll","Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ENNRingMoniAll",manager,inputs,outputs,1.049e+02,5.322e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichENNTracklessRingsMoni";
    cfg.order   = 292;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsBest","Rec/Rich/ENN/Offline/RingsBest",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ENNRingMoniBest",manager,inputs,outputs,8.729e+01,1.665e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichENNTracklessRingsMoni";
    cfg.order   = 293;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsIsolated","Rec/Rich/ENN/Offline/RingsIsolated",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ENNRingMoniIsolated",manager,inputs,outputs,8.000e+01,1.369e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichENNTracklessRingsMoni";
    cfg.order   = 294;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichAlignMoniR1Gas",manager,inputs,outputs,8.179e+03,5.812e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichMirrAlignMoni";
    cfg.order   = 296;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichAlignMoniR2Gas",manager,inputs,outputs,5.713e+03,4.221e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichMirrAlignMoni";
    cfg.order   = 297;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichHPDIFBMoni",manager,inputs,outputs,3.689e+03,9.127e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichHPDIonFeedback";
    cfg.order   = 299;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Muon/MuonPID","/Event/Rec/Track/Best","/Event/Rec/Track/Muon","Rec/Muon/MuonPID","Rec/Track/Best","Rec/Track/Muon",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonPIDMonitorDown",manager,inputs,outputs,9.000e+02,6.820e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniMUONSeq";
    cfg.order   = 301;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Muon/MuonPID","/Event/Rec/Track/Best","/Event/Rec/Track/Muon","Rec/Muon/MuonPID","Rec/Track/Best","Rec/Track/Muon",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonPIDMonitorLong",manager,inputs,outputs,1.291e+03,6.242e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniMUONSeq";
    cfg.order   = 302;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Muon","Rec/Track/Muon",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonTrackFitter",manager,inputs,outputs,1.313e+05,2.905e+05);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniMUONSeq";
    cfg.order   = 303;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Muon/MuonPID","Rec/Muon/MuonPID",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonTrackAligMonitor",manager,inputs,outputs,6.236e+02,8.836e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniMUONSeq";
    cfg.order   = 304;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Velo/Clusters","Raw/Velo/Clusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("VeloClusterMonitor",manager,inputs,outputs,3.086e+03,9.895e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniVELOSeq";
    cfg.order   = 306;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Velo/Clusters","/Event/Rec/Track/Best","Raw/Velo/Clusters","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("VeloTrackMonitor",manager,inputs,outputs,1.991e+04,9.245e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniVELOSeq";
    cfg.order   = 307;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","/Event/Rec/Vertex/Primary","Rec/Track/Best","Rec/Vertex/Primary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("VeloIPResolutionMonitor",manager,inputs,outputs,1.573e+03,1.885e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniVELOSeq";
    cfg.order   = 308;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackMonitor",manager,inputs,outputs,4.224e+04,2.280e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 310;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Muon","Rec/Track/Muon",0};
    const char* outputs[] = {"/Event/Rec/Vertex/DiMuon",0};
    AlgConfig cfg("TrackDiMuonMonitor",manager,inputs,outputs,4.321e+02,5.408e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 311;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","/Event/Rec/Vertex/Primary","Rec/Track/Best","Rec/Vertex/Primary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackVertexMonitor",manager,inputs,outputs,2.560e+03,1.382e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 312;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","Rec/Vertex/Primary","Rec/Vertex/V0",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackV0Monitor",manager,inputs,outputs,3.781e+02,2.890e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 313;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackFitMatchMonitor",manager,inputs,outputs,3.598e+03,2.434e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 314;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackAlignMonitor",manager,inputs,outputs,2.894e+03,1.522e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 315;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackITOverlapMonitor",manager,inputs,outputs,1.024e+04,8.294e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 316;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackVeloOverlapMonitor",manager,inputs,outputs,2.059e+03,1.649e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 317;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/TT/Clusters","/Event/Rec/Track/Best","Raw/TT/Clusters","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TTTrackMonitor",manager,inputs,outputs,4.708e+03,2.550e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 318;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/IT/Clusters","/Event/Rec/Track/Best","Raw/IT/Clusters","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ITTrackMonitor",manager,inputs,outputs,6.916e+03,4.392e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 319;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Rec/Track/Best","DAQ/ODIN","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackPV2HalfAlignMonitor",manager,inputs,outputs,2.854e+03,1.541e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 320;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Track/Best","Rec/Calo/EcalClusters","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackEcalMatchMonitor",manager,inputs,outputs,2.468e+03,1.075e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 321;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Spd/Digits","/Event/Rec/Track/Best","Raw/Spd/Digits","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackSpdMatchMonitor",manager,inputs,outputs,2.806e+03,1.638e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 322;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Rec/Track/Best","Raw/Prs/Digits","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackPrsMatchMonitor",manager,inputs,outputs,3.660e+03,1.914e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 323;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Muon/Coords","/Event/Rec/Track/Best","Raw/Muon/Coords","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackMuonMatchMonitor",manager,inputs,outputs,1.611e+03,8.189e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 324;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/DiMuon","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OTTimeMonitor",manager,inputs,outputs,9.210e+02,3.109e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniOTSeq";
    cfg.order   = 326;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OTTrackMonitor",manager,inputs,outputs,1.055e+04,5.442e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniOTSeq";
    cfg.order   = 327;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OTHitEfficiencyMonitor",manager,inputs,outputs,4.504e+03,2.415e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniOTSeq";
    cfg.order   = 328;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Raw/TT/Clusters","DAQ/ODIN","Raw/TT/Clusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TTClusterMonitor",manager,inputs,outputs,1.669e+03,7.154e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniSTSeq";
    cfg.order   = 330;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Raw/IT/Clusters","DAQ/ODIN","Raw/IT/Clusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ITClusterMonitor",manager,inputs,outputs,1.012e+03,3.908e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniSTSeq";
    cfg.order   = 331;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","/Event/Rec/Track/Best","Rec/ProtoP/Charged","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPMoni",manager,inputs,outputs,1.880e+03,1.297e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniPROTOSeq";
    cfg.order   = 333;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoANNPIDMoni",manager,inputs,outputs,6.966e+02,6.079e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniPROTOSeq";
    cfg.order   = 334;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Hlt/DecReports","DAQ/ODIN","Hlt/DecReports",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HltRateMonitor",manager,inputs,outputs,1.664e+02,1.862e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniHltSeq";
    cfg.order   = 336;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Hlt/DecReports","DAQ/ODIN","Hlt/DecReports",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HltRateMonitorMB",manager,inputs,outputs,9.743e+01,7.392e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniHltSeq";
    cfg.order   = 337;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Hlt/DecReports","Hlt/DecReports",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HltCompositionMonitor",manager,inputs,outputs,2.554e+02,2.177e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniHltSeq";
    cfg.order   = 338;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Hlt/DecReports","/Event/Rec/Track/Velo","DAQ/ODIN","Hlt/DecReports","Rec/Track/Velo",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuMonitor",manager,inputs,outputs,1.366e+02,3.662e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniHltSeq";
    cfg.order   = 339;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EventTimeMonitor",manager,inputs,outputs,5.029e+01,9.376e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniHltSeq";
    cfg.order   = 340;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent","Trigger/RawEvent",0};
    const char* outputs[] = {"/Event/Trigger/RawEvent",0};
    AlgConfig cfg("TriggerRawBank",manager,inputs,outputs,1.911e+02,7.606e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "OutputDSTSeq";
    cfg.order   = 343;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","Calo/RawEvent","DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Calo/RawEvent",0};
    AlgConfig cfg("CaloRawBank",manager,inputs,outputs,1.439e+02,1.102e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "OutputDSTSeq";
    cfg.order   = 344;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent","Muon/RawEvent",0};
    const char* outputs[] = {"/Event/Muon/RawEvent",0};
    AlgConfig cfg("MuonRawBank",manager,inputs,outputs,1.120e+02,1.912e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "OutputDSTSeq";
    cfg.order   = 345;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent","Rich/RawEvent",0};
    const char* outputs[] = {"/Event/Rich/RawEvent",0};
    AlgConfig cfg("RichRawBank",manager,inputs,outputs,1.377e+02,2.232e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "OutputDSTSeq";
    cfg.order   = 346;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent","Other/RawEvent",0};
    const char* outputs[] = {"/Event/Other/RawEvent",0};
    AlgConfig cfg("OtherRawEvent",manager,inputs,outputs,2.061e+02,2.216e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "OutputDSTSeq";
    cfg.order   = 347;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Calo/RawEvent","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Muon/RawEvent","/Event/Other/RawEvent","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/DiMuon","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rich/RawEvent","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","/Event/Trigger/RawEvent","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("DAQKiller",manager,inputs,outputs,5.486e+01,3.728e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "OutputDSTSeq";
    cfg.order   = 348;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FilterBestTrackStates",manager,inputs,outputs,6.147e+02,2.743e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "OutputDSTSeq";
    cfg.order   = 349;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Muon",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FilterMuonTrackStates",manager,inputs,outputs,5.014e+01,1.216e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "OutputDSTSeq";
    cfg.order   = 350;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/pRec/Track/Best",0};
    AlgConfig cfg("PackTracks",manager,inputs,outputs,6.744e+02,3.569e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 352;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/PIDs","Rec/Rich/PIDs","pRec/Rich/PIDs",0};
    const char* outputs[] = {"/Event/pRec/Rich/PIDs",0};
    AlgConfig cfg("PackRichPIDs",manager,inputs,outputs,2.129e+02,1.116e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 353;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Muon/MuonPID","Rec/Muon/MuonPID","pRec/Muon/MuonPID",0};
    const char* outputs[] = {"/Event/pRec/Muon/MuonPID",0};
    AlgConfig cfg("PackMuonPIDs",manager,inputs,outputs,2.126e+02,1.084e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 354;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/Photons","Rec/Calo/Photons",0};
    const char* outputs[] = {"/Event/pRec/Calo/Photons",0};
    AlgConfig cfg("PackPhotons",manager,inputs,outputs,2.101e+02,1.230e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPacking";
    cfg.order   = 356;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/Electrons","Rec/Calo/Electrons",0};
    const char* outputs[] = {"/Event/pRec/Calo/Electrons",0};
    AlgConfig cfg("PackElectrons",manager,inputs,outputs,1.364e+02,2.330e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPacking";
    cfg.order   = 357;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/MergedPi0s","Rec/Calo/MergedPi0s",0};
    const char* outputs[] = {"/Event/pRec/Calo/MergedPi0s",0};
    AlgConfig cfg("PackMergedPi0s",manager,inputs,outputs,1.024e+02,1.363e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPacking";
    cfg.order   = 358;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/SplitPhotons","Rec/Calo/SplitPhotons",0};
    const char* outputs[] = {"/Event/pRec/Calo/SplitPhotons",0};
    AlgConfig cfg("PackSplitPhotons",manager,inputs,outputs,1.009e+02,2.103e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPacking";
    cfg.order   = 359;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ProtoParticlePIDClean",manager,inputs,outputs,2.487e+02,1.305e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 360;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {"/Event/pRec/ProtoP/Charged",0};
    AlgConfig cfg("PackChargedProtos",manager,inputs,outputs,2.330e+02,1.382e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 361;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Neutrals",0};
    const char* outputs[] = {"/Event/pRec/ProtoP/Neutrals",0};
    AlgConfig cfg("PackNeutralProtos",manager,inputs,outputs,1.150e+02,1.108e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 362;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Vertex/Primary","Rec/Vertex/Primary",0};
    const char* outputs[] = {"/Event/pRec/Vertex/Primary",0};
    AlgConfig cfg("PackRecVertex",manager,inputs,outputs,1.514e+02,1.005e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 363;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Vertex/V0","Rec/Vertex/V0",0};
    const char* outputs[] = {"/Event/pRec/Vertex/V0",0};
    AlgConfig cfg("PackTwoProngVertex",manager,inputs,outputs,1.396e+02,7.802e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 364;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Muon",0};
    const char* outputs[] = {"/Event/pRec/Track/Muon",0};
    AlgConfig cfg("PackMuonTracks",manager,inputs,outputs,1.174e+02,2.217e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 365;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Calo","/Event/Calo/RawEvent","/Event/Muon","/Event/Muon/RawEvent","/Event/Other","/Event/Other/RawEvent","/Event/Rec","/Event/Rec/Header","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rich","/Event/Rich/RawEvent","/Event/Trigger","/Event/Trigger/RawEvent","/Event/pRec","/Event/pRec/Calo","/Event/pRec/Calo/Electrons","/Event/pRec/Calo/MergedPi0s","/Event/pRec/Calo/Photons","/Event/pRec/Calo/SplitPhotons","/Event/pRec/Muon","/Event/pRec/Muon/MuonPID","/Event/pRec/ProtoP","/Event/pRec/ProtoP/Charged","/Event/pRec/ProtoP/Neutrals","/Event/pRec/Rich","/Event/pRec/Rich/PIDs","/Event/pRec/Track","/Event/pRec/Track/Best","/Event/pRec/Track/Muon","/Event/pRec/Vertex","/Event/pRec/Vertex/Primary","/Event/pRec/Vertex/V0",0};
    const char* outputs[] = {0};
    AlgConfig cfg("DstWriter",manager,inputs,outputs,2.625e+05,6.166e+05);
    cfg.setDaughters(daughters);
    cfg.parent  = "Framework";
    cfg.order   = 366;
    cfg.level   = 1;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Calo/RawEvent","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Muon/RawEvent","/Event/Other/RawEvent","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/DiMuon","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rich/RawEvent","/Event/Transient/DAQ/Status","/Event/Trig/L0/L0DUReport","/Event/Trigger/RawEvent","/Event/pRec/Calo/Electrons","/Event/pRec/Calo/MergedPi0s","/Event/pRec/Calo/Photons","/Event/pRec/Calo/SplitPhotons","/Event/pRec/Muon/MuonPID","/Event/pRec/ProtoP/Charged","/Event/pRec/ProtoP/Neutrals","/Event/pRec/Rich/PIDs","/Event/pRec/Track/Best","/Event/pRec/Track/Muon","/Event/pRec/Vertex/Primary","/Event/pRec/Vertex/V0","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FSROutputStreamDstWriter",manager,inputs,outputs,9.300e+00,3.164e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "Framework";
    cfg.order   = 367;
    cfg.level   = 1;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/ODIN","DAQ/RawEvent","Other/RawEvent","Trigger/RawEvent",0};
    const char* outputs[] = {"/Event/DAQ/ODIN",0};
    AlgConfig cfg("Framework",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "Framework";
    cfg.order   = 368;
    cfg.level   = 0;
    cfg.flags   = 0x2000000;
    cfg.type    = "Framework";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
