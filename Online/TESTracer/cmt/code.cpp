/*
=================  Algorithm execution list:
*/
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","DAQ/ODIN","DAQ/RawEvent","Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Header","/Event/Rec/Status",0};
    AlgConfig cfg("BrunelInit",manager,inputs,outputs,1.308e+03,1.653e+03);
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
    // Input data estimated 
    const char* inputs[] = {"/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("BrunelEventCount",manager,inputs,outputs,1.480e+01,2.227e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "InitBrunelSeq";
    cfg.order   = 6;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("BrunelBankKiller",manager,inputs,outputs,7.990e+01,2.937e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "InitBrunelSeq";
    cfg.order   = 7;
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
    AlgConfig cfg("FilterNoBeam",manager,inputs,outputs,1.353e+02,1.433e+02);
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
    AlgConfig cfg("LumiDecodeNoBeam",manager,inputs,outputs,0.000e+00,0.000e+00);
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
    // Input data estimated 
    const char* inputs[] = {"/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiFilterNoBeam",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiNoBeamSeq";
    cfg.order   = 13;
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
    cfg.order   = 14;
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
    AlgConfig cfg("FilterBeamCrossing",manager,inputs,outputs,8.160e+01,1.722e+01);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent","Hlt/LumiSummary",0};
    const char* outputs[] = {"/Event/Hlt/LumiSummary",0};
    AlgConfig cfg("LumiDecodeBeamCrossing",manager,inputs,outputs,3.643e+02,2.372e+02);
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
    const char* inputs[] = {"/Event/Hlt/LumiSummary","Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiFilterBeamCrossing",manager,inputs,outputs,1.867e+02,1.424e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeamCrossingSeq";
    cfg.order   = 18;
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
    AlgConfig cfg("LumiCountBeamCrossing",manager,inputs,outputs,3.998e+03,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeamCrossingSeq";
    cfg.order   = 19;
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
    AlgConfig cfg("FilterBeam1",manager,inputs,outputs,8.420e+01,1.398e+01);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent","Hlt/LumiSummary",0};
    const char* outputs[] = {"/Event/Hlt/LumiSummary",0};
    AlgConfig cfg("LumiDecodeBeam1",manager,inputs,outputs,4.360e+02,0.000e+00);
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
    const char* inputs[] = {"/Event/Hlt/LumiSummary","Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiFilterBeam1",manager,inputs,outputs,2.060e+02,0.000e+00);
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
    AlgConfig cfg("FillingBeam1",manager,inputs,outputs,2.420e+02,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam1Seq";
    cfg.order   = 24;
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
    AlgConfig cfg("LumiCountBeam1",manager,inputs,outputs,5.050e+02,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam1Seq";
    cfg.order   = 25;
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
    AlgConfig cfg("FilterBeam2",manager,inputs,outputs,8.600e+01,1.977e+01);
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
    AlgConfig cfg("LumiDecodeBeam2",manager,inputs,outputs,0.000e+00,0.000e+00);
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
    AlgConfig cfg("LumiFilterBeam2",manager,inputs,outputs,0.000e+00,0.000e+00);
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
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/LumiSummary","/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FillingBeam2",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam2Seq";
    cfg.order   = 30;
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
    cfg.order   = 31;
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
    AlgConfig cfg("TimeSpanFilter",manager,inputs,outputs,8.350e+01,2.193e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "TimeSpanSeq";
    cfg.order   = 33;
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
    AlgConfig cfg("TimeSpanAccounting",manager,inputs,outputs,2.409e+02,1.011e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "TimeSpanSeq";
    cfg.order   = 34;
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
    AlgConfig cfg("EventAccount",manager,inputs,outputs,1.750e+01,8.370e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiCounters";
    cfg.order   = 35;
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
    cfg.order   = 36;
    cfg.level   = 3;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"","/Event/DAQ/RawEvent","DAQ/RawEvent","pRec/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PhysFilter",manager,inputs,outputs,2.327e+02,7.867e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiSeq";
    cfg.order   = 37;
    cfg.level   = 3;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"","/Event/DAQ/RawEvent","DAQ/RawEvent","pRec/RawEvent",0};
    const char* outputs[] = {"/Event/Hlt/DecReports",0};
    AlgConfig cfg("HltDecReportsDecoder",manager,inputs,outputs,2.735e+05,7.192e+05);
    cfg.setDaughters(daughters);
    cfg.parent  = "HltfilterSeq";
    cfg.order   = 41;
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
    AlgConfig cfg("HltErrorFilter",manager,inputs,outputs,2.440e+02,2.740e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "HltfilterSeq";
    cfg.order   = 42;
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
    cfg.order   = 43;
    cfg.level   = 3;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PrsCheckBanks",manager,inputs,outputs,1.431e+02,4.478e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "PrsBanksHandler";
    cfg.order   = 47;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent","DAQ/Status",0};
    const char* outputs[] = {"/Event/DAQ/Status","/Event/Raw/Prs/Adcs",0};
    AlgConfig cfg("PrsADCs",manager,inputs,outputs,2.281e+03,1.333e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "PrsBanksHandler";
    cfg.order   = 48;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status","DAQ/RawEvent","DAQ/Status",0};
    const char* outputs[] = {"/Event/Trig/L0/PrsRaw",0};
    AlgConfig cfg("PrsL0Bit",manager,inputs,outputs,1.532e+03,2.586e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PrsBanksHandler";
    cfg.order   = 49;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status","DAQ/RawEvent","DAQ/Status",0};
    const char* outputs[] = {"/Event/Trig/L0/SpdRaw",0};
    AlgConfig cfg("SpdL0Bit",manager,inputs,outputs,1.441e+03,2.414e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PrsBanksHandler";
    cfg.order   = 50;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Raw/Prs/Adcs","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","DAQ/RawEvent","Raw/Prs/Adcs","Trig/L0/PrsRaw","Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PrsSpdFillRawBuffer",manager,inputs,outputs,9.358e+02,5.661e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PrsBanksHandler";
    cfg.order   = 51;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RemovePrsPackedBanks",manager,inputs,outputs,3.315e+02,3.152e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PrsBanksHandler";
    cfg.order   = 52;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalCheckBanks",manager,inputs,outputs,1.206e+02,1.895e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "EcalBanksHandler";
    cfg.order   = 54;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status","DAQ/RawEvent","DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Ecal/Adcs",0};
    AlgConfig cfg("EcalADCs",manager,inputs,outputs,4.502e+03,9.462e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "EcalBanksHandler";
    cfg.order   = 55;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status","DAQ/RawEvent","DAQ/Status",0};
    const char* outputs[] = {"/Event/Trig/L0/EcalRaw",0};
    AlgConfig cfg("EcalTrigFromRaw",manager,inputs,outputs,1.790e+03,4.274e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "EcalBanksHandler";
    cfg.order   = 56;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Raw/Ecal/Adcs","/Event/Trig/L0/EcalRaw","DAQ/RawEvent","Raw/Ecal/Adcs","Trig/L0/EcalRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalFillRawBuffer",manager,inputs,outputs,2.735e+02,2.562e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "EcalBanksHandler";
    cfg.order   = 57;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RemoveEcalPackedBanks",manager,inputs,outputs,3.550e+02,6.757e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "EcalBanksHandler";
    cfg.order   = 58;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HcalCheckBanks",manager,inputs,outputs,1.196e+02,3.173e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "HcalBanksHandler";
    cfg.order   = 60;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status","DAQ/RawEvent","DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Hcal/Adcs",0};
    AlgConfig cfg("HcalADCs",manager,inputs,outputs,1.293e+03,2.910e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "HcalBanksHandler";
    cfg.order   = 61;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status","DAQ/RawEvent","DAQ/Status",0};
    const char* outputs[] = {"/Event/Trig/L0/HcalRaw",0};
    AlgConfig cfg("HcalTrigFromRaw",manager,inputs,outputs,6.478e+02,1.154e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "HcalBanksHandler";
    cfg.order   = 62;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Raw/Hcal/Adcs","/Event/Trig/L0/HcalRaw","DAQ/RawEvent","Raw/Hcal/Adcs","Trig/L0/HcalRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HcalFillRawBuffer",manager,inputs,outputs,1.770e+02,3.110e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "HcalBanksHandler";
    cfg.order   = 63;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RemoveHcalPackedBanks",manager,inputs,outputs,1.591e+02,3.455e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "HcalBanksHandler";
    cfg.order   = 64;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters",0};
    AlgConfig cfg("DecodeVeloClusters",manager,inputs,outputs,1.460e+03,1.313e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoDecodingSeq";
    cfg.order   = 67;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","DAQ/ODIN","DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/TT/Clusters","/Event/Rec/TT/Summary",0};
    AlgConfig cfg("CreateTTClusters",manager,inputs,outputs,1.598e+03,1.817e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoDecodingSeq";
    cfg.order   = 68;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","DAQ/ODIN","DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/TT/LiteClusters",0};
    AlgConfig cfg("CreateTTLiteClusters",manager,inputs,outputs,7.022e+02,3.760e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoDecodingSeq";
    cfg.order   = 69;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","DAQ/ODIN","DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/IT/Clusters","/Event/Rec/IT/Summary",0};
    AlgConfig cfg("CreateITClusters",manager,inputs,outputs,1.299e+03,1.693e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoDecodingSeq";
    cfg.order   = 70;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","DAQ/ODIN","DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/IT/LiteClusters",0};
    AlgConfig cfg("CreateITLiteClusters",manager,inputs,outputs,6.155e+02,3.544e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoDecodingSeq";
    cfg.order   = 71;
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
    AlgConfig cfg("FastVeloTracking",manager,inputs,outputs,2.113e+03,3.547e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoVELOSeq";
    cfg.order   = 73;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Raw/IT/LiteClusters","/Event/Raw/TT/LiteClusters","/Event/Rec/Track/Velo","DAQ/RawEvent","Raw/IT/LiteClusters","Raw/TT/LiteClusters","Rec/Track/Forward","Rec/Track/Velo",0};
    const char* outputs[] = {"/Event/Rec/Track/Forward",0};
    AlgConfig cfg("PatForward",manager,inputs,outputs,5.274e+03,7.122e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackForwardPatSeq";
    cfg.order   = 80;
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
    AlgConfig cfg("PatSeeding",manager,inputs,outputs,2.355e+03,3.299e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackSeedPatSeq";
    cfg.order   = 82;
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
    AlgConfig cfg("PatMatch",manager,inputs,outputs,5.619e+02,7.169e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackMatchPatSeq";
    cfg.order   = 84;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/TT/LiteClusters","/Event/Rec/Track/Match","/Event/Rec/Track/Seed","Raw/TT/LiteClusters","Rec/Track/Match","Rec/Track/Seed",0};
    const char* outputs[] = {"/Event/Rec/Track/Downstream",0};
    AlgConfig cfg("PatDownstream",manager,inputs,outputs,1.325e+03,1.388e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackDownstreamPatSeq";
    cfg.order   = 86;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Velo","Rec/Track/Forward","Rec/Track/Match","Rec/Track/Velo",0};
    const char* outputs[] = {"/Event/Rec/Track/VeloTT",0};
    AlgConfig cfg("PatVeloTT",manager,inputs,outputs,1.534e+03,3.346e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackVeloTTPatSeq";
    cfg.order   = 88;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Seed","/Event/Rec/Track/VeloTT","Rec/Track/Downstream","Rec/Track/Forward","Rec/Track/Match","Rec/Track/Seed","Rec/Track/VeloTT",0};
    const char* outputs[] = {"/Event/Rec/Track/AllBest",0};
    AlgConfig cfg("TrackEventCloneKiller",manager,inputs,outputs,4.789e+02,3.571e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackFitSeq";
    cfg.order   = 90;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/IT/LiteClusters","/Event/Rec/Track/AllBest","Raw/IT/LiteClusters","Rec/Track/AllBest",0};
    const char* outputs[] = {0};
    AlgConfig cfg("InitBestFit",manager,inputs,outputs,1.464e+03,3.664e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackFitSeq";
    cfg.order   = 91;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/TT/Clusters","/Event/Raw/Velo/Clusters","/Event/Rec/Track/AllBest","Raw/TT/Clusters","Raw/Velo/Clusters","Rec/Track/AllBest",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FitBest",manager,inputs,outputs,3.195e+05,8.452e+05);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackFitSeq";
    cfg.order   = 92;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/AllBest","Rec/Track/AllBest","Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Track/Best",0};
    AlgConfig cfg("CopyBest",manager,inputs,outputs,8.260e+02,1.261e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackFitSeq";
    cfg.order   = 93;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","/Event/Rec/Track/Velo","Rec/Track/Best","Rec/Track/Velo",0};
    const char* outputs[] = {"/Event/Rec/Track/PreparedVelo",0};
    AlgConfig cfg("TrackPrepareVelo",manager,inputs,outputs,2.451e+02,2.051e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackVeloFitSeq";
    cfg.order   = 95;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Velo/Clusters","/Event/Rec/Track/PreparedVelo","Raw/Velo/Clusters","Rec/Track/PreparedVelo",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FitVelo",manager,inputs,outputs,6.484e+03,1.697e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackVeloFitSeq";
    cfg.order   = 96;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","/Event/Rec/Track/PreparedVelo","Rec/Track/Best","Rec/Track/PreparedVelo",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CopyVelo",manager,inputs,outputs,1.619e+02,1.457e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackVeloFitSeq";
    cfg.order   = 97;
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
    AlgConfig cfg("FindTrackClones",manager,inputs,outputs,1.448e+03,2.452e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackClonesSeq";
    cfg.order   = 100;
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
    AlgConfig cfg("FlagTrackClones",manager,inputs,outputs,7.494e+02,1.588e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackClonesSeq";
    cfg.order   = 101;
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
    AlgConfig cfg("TrackAddLikelihood",manager,inputs,outputs,2.846e+03,7.325e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackAddExtraInfoSeq";
    cfg.order   = 102;
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
    AlgConfig cfg("TrackAddNNGhostId",manager,inputs,outputs,5.489e+02,1.193e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackAddExtraInfoSeq";
    cfg.order   = 103;
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
    AlgConfig cfg("TrackEraseExtraInfo",manager,inputs,outputs,9.025e+01,8.483e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackEraseExtraInformationSeq";
    cfg.order   = 105;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","Rec/Track/Best","Rec/Vertex/Primary",0};
    const char* outputs[] = {"/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/Weights",0};
    AlgConfig cfg("PatPVOffline",manager,inputs,outputs,3.744e+03,7.943e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoVertexSeq";
    cfg.order   = 107;
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
    AlgConfig cfg("TrackV0Finder",manager,inputs,outputs,6.599e+02,1.151e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoVertexSeq";
    cfg.order   = 108;
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
    AlgConfig cfg("CheckProcOfflineStatus",manager,inputs,outputs,3.862e+02,4.832e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecInitOfflineSeq";
    cfg.order   = 111;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("DecodeRawRichOffline",manager,inputs,outputs,4.872e+03,9.385e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflinePixelsSeq";
    cfg.order   = 113;
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
    AlgConfig cfg("CreateOfflinePixels",manager,inputs,outputs,5.654e+03,7.672e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflinePixelsSeq";
    cfg.order   = 114;
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
    AlgConfig cfg("CreateOfflineTracks",manager,inputs,outputs,1.159e+04,2.937e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineTracksSeq";
    cfg.order   = 116;
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
    AlgConfig cfg("CreateOfflinePhotons",manager,inputs,outputs,3.635e+03,9.258e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflinePhotonsSeq";
    cfg.order   = 118;
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
    AlgConfig cfg("OfflineENNFinderR1GasTop",manager,inputs,outputs,1.595e+03,2.924e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 120;
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
    AlgConfig cfg("OfflineENNFinderR1GasBottom",manager,inputs,outputs,3.268e+02,3.402e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 121;
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
    AlgConfig cfg("OfflineENNFinderR2GasLeft",manager,inputs,outputs,1.625e+02,1.195e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 122;
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
    AlgConfig cfg("OfflineENNFinderR2GasRight",manager,inputs,outputs,1.537e+03,3.573e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 123;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll","Rec/Rich/ENN/Offline/RingsAll","Rec/Rich/RecoEvent/Offline/Segments",0};
    const char* outputs[] = {"/Event/Rec/Rich/RecoEvent/Offline/Segments",0};
    AlgConfig cfg("OfflineENNRingsSegAssoc",manager,inputs,outputs,1.097e+03,1.929e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 124;
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
    AlgConfig cfg("OfflineBestENNRings",manager,inputs,outputs,9.559e+02,2.039e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 125;
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
    AlgConfig cfg("OfflineIsolatedENNRings",manager,inputs,outputs,3.320e+02,4.289e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 126;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Hcal/Adcs","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Prs/Adcs","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDInit",manager,inputs,outputs,2.425e+01,3.584e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDInitSeq";
    cfg.order   = 130;
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
    AlgConfig cfg("RichOfflineGPIDDigSel",manager,inputs,outputs,1.304e+02,1.791e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDInitSeq";
    cfg.order   = 131;
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
    AlgConfig cfg("RichOfflineGPIDTrSel",manager,inputs,outputs,9.895e+02,1.412e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDInitSeq";
    cfg.order   = 132;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Hcal/Adcs","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Prs/Adcs","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineBckEstIt0",manager,inputs,outputs,5.320e+02,1.380e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDLLSeq";
    cfg.order   = 134;
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
    AlgConfig cfg("RichOfflineGPIDLLIt0",manager,inputs,outputs,3.712e+03,9.795e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDLLSeq";
    cfg.order   = 135;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Hcal/Adcs","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Prs/Adcs","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineBckEstIt1",manager,inputs,outputs,1.701e+02,4.252e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDLLSeq";
    cfg.order   = 136;
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
    AlgConfig cfg("RichOfflineGPIDLLIt1",manager,inputs,outputs,8.800e+01,2.094e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDLLSeq";
    cfg.order   = 137;
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
    AlgConfig cfg("RichOfflineGPIDFin",manager,inputs,outputs,1.198e+02,2.652e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDSeq";
    cfg.order   = 138;
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
    AlgConfig cfg("MergeOfflineRichPIDs",manager,inputs,outputs,4.964e+02,4.988e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflinePIDSeq";
    cfg.order   = 139;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status","DAQ/RawEvent","DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Spd/Digits",0};
    AlgConfig cfg("SpdFromRaw",manager,inputs,outputs,4.784e+02,5.171e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigits";
    cfg.order   = 143;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status","DAQ/RawEvent","DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Prs/Digits",0};
    AlgConfig cfg("PrsFromRaw",manager,inputs,outputs,4.184e+02,3.058e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigits";
    cfg.order   = 144;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status","DAQ/RawEvent","DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Ecal/Digits",0};
    AlgConfig cfg("EcalZSup",manager,inputs,outputs,2.922e+02,7.338e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigits";
    cfg.order   = 145;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status","DAQ/RawEvent","DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Hcal/Digits",0};
    AlgConfig cfg("HcalZSup",manager,inputs,outputs,2.939e+02,4.330e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigits";
    cfg.order   = 146;
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
    AlgConfig cfg("CaloDigitFilter",manager,inputs,outputs,1.194e+04,5.927e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ClusterReco";
    cfg.order   = 148;
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
    AlgConfig cfg("EcalClust",manager,inputs,outputs,2.618e+03,4.011e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ClusterReco";
    cfg.order   = 149;
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
    AlgConfig cfg("EcalShare",manager,inputs,outputs,9.656e+02,1.503e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ClusterReco";
    cfg.order   = 150;
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
    AlgConfig cfg("EcalCovar",manager,inputs,outputs,3.992e+02,7.357e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ClusterReco";
    cfg.order   = 151;
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
    AlgConfig cfg("InECAL",manager,inputs,outputs,1.513e+03,2.356e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloTrackMatch";
    cfg.order   = 154;
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
    AlgConfig cfg("ClusterMatch",manager,inputs,outputs,2.324e+03,3.720e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloTrackMatch";
    cfg.order   = 155;
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
    AlgConfig cfg("SinglePhotonRec",manager,inputs,outputs,5.198e+03,9.096e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "PhotonReco";
    cfg.order   = 156;
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
    AlgConfig cfg("MergedPi0Rec",manager,inputs,outputs,3.879e+02,1.165e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MergedPi0Reco";
    cfg.order   = 158;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/EcalClusters","Raw/Prs/Digits","Raw/Spd/Digits","Rec/Calo/EcalClusters",0};
    const char* outputs[] = {"/Event/Rec/Calo/Electrons",0};
    AlgConfig cfg("SingleElectronRec",manager,inputs,outputs,2.866e+02,2.409e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ElectronReco";
    cfg.order   = 160;
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
    AlgConfig cfg("InSPD",manager,inputs,outputs,3.090e+02,2.298e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "InCaloAcceptance";
    cfg.order   = 165;
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
    AlgConfig cfg("InPRS",manager,inputs,outputs,2.501e+02,1.199e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "InCaloAcceptance";
    cfg.order   = 166;
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
    AlgConfig cfg("InHCAL",manager,inputs,outputs,2.239e+02,8.197e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "InCaloAcceptance";
    cfg.order   = 167;
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
    AlgConfig cfg("InBREM",manager,inputs,outputs,2.459e+02,1.625e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "InCaloAcceptance";
    cfg.order   = 168;
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
    AlgConfig cfg("ElectronMatch",manager,inputs,outputs,1.536e+03,3.136e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloMatch";
    cfg.order   = 170;
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
    AlgConfig cfg("BremMatch",manager,inputs,outputs,6.610e+02,6.918e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloMatch";
    cfg.order   = 171;
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
    AlgConfig cfg("SpdE",manager,inputs,outputs,1.304e+03,1.982e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloEnergy";
    cfg.order   = 173;
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
    AlgConfig cfg("PrsE",manager,inputs,outputs,2.479e+02,8.569e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloEnergy";
    cfg.order   = 174;
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
    AlgConfig cfg("EcalE",manager,inputs,outputs,2.925e+02,2.031e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloEnergy";
    cfg.order   = 175;
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
    AlgConfig cfg("HcalE",manager,inputs,outputs,2.839e+02,1.389e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloEnergy";
    cfg.order   = 176;
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
    AlgConfig cfg("EcalChi22ID",manager,inputs,outputs,4.386e+02,3.062e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloChi2";
    cfg.order   = 178;
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
    AlgConfig cfg("BremChi22ID",manager,inputs,outputs,2.948e+02,9.841e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloChi2";
    cfg.order   = 179;
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
    AlgConfig cfg("ClusChi22ID",manager,inputs,outputs,4.549e+02,3.637e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloChi2";
    cfg.order   = 180;
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
    AlgConfig cfg("PrsPIDe",manager,inputs,outputs,3.209e+02,2.646e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLe";
    cfg.order   = 182;
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
    AlgConfig cfg("EcalPIDe",manager,inputs,outputs,1.669e+02,2.121e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLe";
    cfg.order   = 183;
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
    AlgConfig cfg("BremPIDe",manager,inputs,outputs,1.654e+02,2.071e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLe";
    cfg.order   = 184;
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
    AlgConfig cfg("HcalPIDe",manager,inputs,outputs,1.841e+02,3.669e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLe";
    cfg.order   = 185;
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
    AlgConfig cfg("EcalPIDmu",manager,inputs,outputs,1.730e+02,2.900e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLmu";
    cfg.order   = 187;
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
    AlgConfig cfg("HcalPIDmu",manager,inputs,outputs,1.721e+02,2.065e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLmu";
    cfg.order   = 188;
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
    AlgConfig cfg("PhotonID",manager,inputs,outputs,2.235e+03,4.020e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "NeutralPIDs";
    cfg.order   = 190;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/MergedPi0s","Rec/Calo/MergedPi0s",0};
    const char* outputs[] = {"/Event/Rec/Calo/MergedID",0};
    AlgConfig cfg("MergedID",manager,inputs,outputs,1.925e+02,4.758e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "NeutralPIDs";
    cfg.order   = 191;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/SplitPhotons","Rec/Calo/SplitPhotons",0};
    const char* outputs[] = {"/Event/Rec/Calo/PhotonFromMergedID",0};
    AlgConfig cfg("PhotonFromMergedID",manager,inputs,outputs,1.801e+02,2.352e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "NeutralPIDs";
    cfg.order   = 192;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status","DAQ/RawEvent","DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Muon/Coords",0};
    AlgConfig cfg("MuonRec",manager,inputs,outputs,1.744e+03,1.702e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoMUONSeq";
    cfg.order   = 194;
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
    AlgConfig cfg("MuonIDAlg",manager,inputs,outputs,1.152e+04,2.606e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "MuonIDSeq";
    cfg.order   = 196;
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
    AlgConfig cfg("ChargedProtoPMaker",manager,inputs,outputs,9.265e+02,1.480e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 199;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/InAccEcal","/Event/Rec/ProtoP/Charged","Rec/Calo/ClusChi2","Rec/Calo/ClusterMatch","Rec/Calo/EcalChi2","Rec/Calo/EcalE","Rec/Calo/EcalPIDe","Rec/Calo/EcalPIDmu","Rec/Calo/ElectronMatch","Rec/Calo/InAccEcal","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddEcal",manager,inputs,outputs,1.440e+03,1.304e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 200;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/InAccBrem","/Event/Rec/ProtoP/Charged","Rec/Calo/BremChi2","Rec/Calo/BremMatch","Rec/Calo/BremPIDe","Rec/Calo/InAccBrem","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddBrem",manager,inputs,outputs,4.192e+02,5.991e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 201;
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
    AlgConfig cfg("ChargedProtoPAddHcal",manager,inputs,outputs,4.346e+02,7.066e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 202;
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
    AlgConfig cfg("ChargedProtoPAddPrs",manager,inputs,outputs,3.425e+02,5.507e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 203;
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
    AlgConfig cfg("ChargedProtoPAddSpd",manager,inputs,outputs,2.662e+02,5.026e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 204;
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
    AlgConfig cfg("ChargedProtoPAddVeloDEDX",manager,inputs,outputs,4.252e+02,8.297e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 205;
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
    AlgConfig cfg("ChargedProtoPAddRich",manager,inputs,outputs,8.441e+02,1.079e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 206;
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
    AlgConfig cfg("ChargedProtoPAddMuon",manager,inputs,outputs,7.422e+02,1.001e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 207;
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
    AlgConfig cfg("ChargedProtoPCombDLLs",manager,inputs,outputs,1.528e+02,1.491e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 208;
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
    AlgConfig cfg("ANNGPIDLongElectron",manager,inputs,outputs,4.728e+02,1.026e+03);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongMuon",manager,inputs,outputs,1.344e+02,1.555e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 211;
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
    AlgConfig cfg("ANNGPIDLongPion",manager,inputs,outputs,1.018e+02,1.109e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 212;
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
    AlgConfig cfg("ANNGPIDLongKaon",manager,inputs,outputs,1.028e+02,1.200e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 213;
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
    AlgConfig cfg("ANNGPIDLongProton",manager,inputs,outputs,1.005e+02,1.096e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 214;
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
    AlgConfig cfg("ANNGPIDLongGhost",manager,inputs,outputs,1.061e+02,1.077e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 215;
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
    AlgConfig cfg("ANNGPIDDownstreamElectron",manager,inputs,outputs,6.350e+01,2.263e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 216;
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
    AlgConfig cfg("ANNGPIDDownstreamMuon",manager,inputs,outputs,6.125e+01,1.845e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 217;
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
    AlgConfig cfg("ANNGPIDDownstreamPion",manager,inputs,outputs,6.975e+01,3.758e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 218;
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
    AlgConfig cfg("ANNGPIDDownstreamKaon",manager,inputs,outputs,6.788e+01,2.800e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 219;
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
    AlgConfig cfg("ANNGPIDDownstreamProton",manager,inputs,outputs,6.112e+01,1.882e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 220;
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
    AlgConfig cfg("ANNGPIDDownstreamGhost",manager,inputs,outputs,6.588e+01,2.449e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 221;
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
    AlgConfig cfg("ANNGPIDUpstreamElectron",manager,inputs,outputs,6.000e+01,1.720e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 222;
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
    AlgConfig cfg("ANNGPIDUpstreamMuon",manager,inputs,outputs,6.312e+01,1.975e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 223;
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
    AlgConfig cfg("ANNGPIDUpstreamPion",manager,inputs,outputs,6.262e+01,1.817e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 224;
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
    AlgConfig cfg("ANNGPIDUpstreamKaon",manager,inputs,outputs,6.688e+01,1.957e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 225;
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
    AlgConfig cfg("ANNGPIDUpstreamProton",manager,inputs,outputs,6.312e+01,2.695e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 226;
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
    AlgConfig cfg("ANNGPIDUpstreamGhost",manager,inputs,outputs,5.950e+01,1.777e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 227;
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
    AlgConfig cfg("NeutralProtoPMaker",manager,inputs,outputs,1.650e+03,2.686e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "NeutralProtoParticles";
    cfg.order   = 229;
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
    AlgConfig cfg("RecSummary",manager,inputs,outputs,1.671e+03,1.504e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoSUMMARYSeq";
    cfg.order   = 231;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OverallEventProcTime",manager,inputs,outputs,3.738e+01,3.691e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 234;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackEventProcTime",manager,inputs,outputs,2.888e+01,2.841e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 235;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("VertexEventProcTime",manager,inputs,outputs,1.962e+01,1.564e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 236;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichEventProcTime",manager,inputs,outputs,3.150e+01,3.351e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 237;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloEventProcTime",manager,inputs,outputs,1.900e+01,1.513e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 238;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonEventProcTime",manager,inputs,outputs,2.250e+01,2.041e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 239;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ProtoEventProcTime",manager,inputs,outputs,1.988e+01,1.604e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 240;
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
    AlgConfig cfg("ProcStatAbortMoni",manager,inputs,outputs,8.350e+01,4.866e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 241;
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
    AlgConfig cfg("EcalDigitMon",manager,inputs,outputs,1.990e+02,1.852e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigitsMoni";
    cfg.order   = 244;
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
    AlgConfig cfg("HcalDigitMon",manager,inputs,outputs,4.229e+02,3.557e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigitsMoni";
    cfg.order   = 245;
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
    AlgConfig cfg("PrsDigitMon",manager,inputs,outputs,2.188e+02,4.102e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigitsMoni";
    cfg.order   = 246;
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
    AlgConfig cfg("SpdDigitMon",manager,inputs,outputs,1.249e+02,3.447e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigitsMoni";
    cfg.order   = 247;
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
    AlgConfig cfg("SpdMon",manager,inputs,outputs,2.336e+02,2.884e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigitsMoni";
    cfg.order   = 248;
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
    AlgConfig cfg("EcalEFlowMon",manager,inputs,outputs,1.355e+02,8.111e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloEFlowMoni";
    cfg.order   = 250;
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
    AlgConfig cfg("EcalClusterMon",manager,inputs,outputs,2.599e+02,2.813e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloClustersMoni";
    cfg.order   = 252;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/Electrons","Rec/Calo/Electrons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ElectronMon",manager,inputs,outputs,1.759e+02,2.306e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloHyposMoni";
    cfg.order   = 254;
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
    AlgConfig cfg("PhotonMon",manager,inputs,outputs,2.790e+02,4.261e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloHyposMoni";
    cfg.order   = 255;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/SplitPhotons","Rec/Calo/SplitPhotons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("SplitPhotonMon",manager,inputs,outputs,9.662e+01,1.838e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloHyposMoni";
    cfg.order   = 256;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/MergedPi0s","Rec/Calo/MergedPi0s",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MergedPi0Mon",manager,inputs,outputs,9.762e+01,2.078e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloHyposMoni";
    cfg.order   = 257;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Spd/Digits","/Event/Rec/Calo/Photons","Raw/Spd/Digits","Rec/Calo/Photons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ResolvedPi0Mon",manager,inputs,outputs,6.420e+02,8.795e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniCALOSeq";
    cfg.order   = 258;
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
    AlgConfig cfg("ProtoElectronMon",manager,inputs,outputs,2.970e+02,3.304e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniCALOSeq";
    cfg.order   = 259;
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
    AlgConfig cfg("PhotonMatchMon",manager,inputs,outputs,3.631e+02,3.755e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 261;
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
    AlgConfig cfg("ElectronMatchMon",manager,inputs,outputs,3.804e+02,5.153e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 262;
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
    AlgConfig cfg("BremMatchMon",manager,inputs,outputs,1.714e+02,3.250e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 263;
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
    AlgConfig cfg("CaloEMuPIDMonUncut",manager,inputs,outputs,1.891e+02,1.482e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 264;
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
    AlgConfig cfg("CaloEMuPIDMonSoft",manager,inputs,outputs,1.135e+02,2.653e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 265;
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
    AlgConfig cfg("CaloEMuPIDMonHard",manager,inputs,outputs,1.019e+02,2.419e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 266;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRawDataSize",manager,inputs,outputs,3.534e+03,3.197e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawMoni";
    cfg.order   = 269;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Hlt/DecReports","Hlt/DecReports",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichDataSizeL0Filter",manager,inputs,outputs,1.149e+02,6.945e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawDataSizeL0Seq";
    cfg.order   = 271;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRawDataSizeL0",manager,inputs,outputs,2.031e+03,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawDataSizeL0Seq";
    cfg.order   = 272;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRawDataDBCheck",manager,inputs,outputs,1.287e+03,2.536e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawMoni";
    cfg.order   = 273;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichHotPixels",manager,inputs,outputs,4.402e+02,8.244e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawMoni";
    cfg.order   = 274;
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
    AlgConfig cfg("RichDecodingErrors",manager,inputs,outputs,3.837e+03,1.906e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawMoni";
    cfg.order   = 275;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRecPixelQC",manager,inputs,outputs,3.979e+02,8.874e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPixelMoni";
    cfg.order   = 277;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRecPixelClusters",manager,inputs,outputs,1.649e+02,3.469e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPixelMoni";
    cfg.order   = 278;
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
    AlgConfig cfg("RiLongTrkEff",manager,inputs,outputs,2.355e+02,5.647e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 280;
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
    AlgConfig cfg("RiForwardTrkEff",manager,inputs,outputs,5.062e+01,8.278e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 281;
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
    AlgConfig cfg("RiMatchTrkEff",manager,inputs,outputs,4.312e+01,6.016e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 282;
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
    AlgConfig cfg("RiKsTrackTrkEff",manager,inputs,outputs,4.125e+01,5.889e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 283;
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
    AlgConfig cfg("RiVeloTTTrkEff",manager,inputs,outputs,4.238e+01,6.284e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 284;
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
    AlgConfig cfg("RiSeedTrkEff",manager,inputs,outputs,4.062e+01,5.810e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 285;
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
    AlgConfig cfg("RiCKResLong",manager,inputs,outputs,1.963e+03,5.139e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 287;
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
    AlgConfig cfg("RiCKResForward",manager,inputs,outputs,2.082e+02,4.985e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 288;
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
    AlgConfig cfg("RiCKResMatch",manager,inputs,outputs,6.912e+01,1.312e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 289;
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
    AlgConfig cfg("RiCKResKsTrack",manager,inputs,outputs,6.738e+01,1.265e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 290;
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
    AlgConfig cfg("RiCKResVeloTT",manager,inputs,outputs,6.712e+01,1.252e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 291;
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
    AlgConfig cfg("RiCKResSeed",manager,inputs,outputs,6.888e+01,1.312e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 292;
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
    AlgConfig cfg("RiCKResLongTight",manager,inputs,outputs,6.800e+01,1.259e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 294;
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
    AlgConfig cfg("RiCKResForwardTight",manager,inputs,outputs,7.250e+01,1.320e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 295;
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
    AlgConfig cfg("RiCKResMatchTight",manager,inputs,outputs,6.662e+01,1.244e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 296;
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
    AlgConfig cfg("RiCKResKsTrackTight",manager,inputs,outputs,6.850e+01,1.273e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 297;
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
    AlgConfig cfg("RiCKResVeloTTTight",manager,inputs,outputs,6.688e+01,1.245e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 298;
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
    AlgConfig cfg("RiCKResSeedTight",manager,inputs,outputs,6.650e+01,1.239e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 299;
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
    AlgConfig cfg("RiPIDMonLong2To100",manager,inputs,outputs,1.491e+03,3.490e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPIDMoni";
    cfg.order   = 301;
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
    AlgConfig cfg("RiPIDMonLong2To10",manager,inputs,outputs,1.746e+02,2.705e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPIDMoni";
    cfg.order   = 302;
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
    AlgConfig cfg("RiPIDMonLong10To70",manager,inputs,outputs,1.609e+02,2.436e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPIDMoni";
    cfg.order   = 303;
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
    AlgConfig cfg("RiPIDMonLong70To100",manager,inputs,outputs,1.121e+02,1.200e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPIDMoni";
    cfg.order   = 304;
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
    AlgConfig cfg("ENNRingMoniAll",manager,inputs,outputs,2.244e+02,5.329e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichENNTracklessRingsMoni";
    cfg.order   = 306;
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
    AlgConfig cfg("ENNRingMoniBest",manager,inputs,outputs,5.550e+01,9.529e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichENNTracklessRingsMoni";
    cfg.order   = 307;
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
    AlgConfig cfg("ENNRingMoniIsolated",manager,inputs,outputs,6.588e+01,1.193e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichENNTracklessRingsMoni";
    cfg.order   = 308;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichAlignMoniR1Gas",manager,inputs,outputs,4.151e+02,8.970e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichMirrAlignMoni";
    cfg.order   = 310;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichAlignMoniR2Gas",manager,inputs,outputs,2.366e+02,5.713e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichMirrAlignMoni";
    cfg.order   = 311;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichHPDIFBMoni",manager,inputs,outputs,1.033e+03,8.997e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichHPDIonFeedback";
    cfg.order   = 313;
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
    AlgConfig cfg("MuonPIDMonitorDown",manager,inputs,outputs,2.828e+02,1.606e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniMUONSeq";
    cfg.order   = 315;
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
    AlgConfig cfg("MuonPIDMonitorLong",manager,inputs,outputs,5.536e+02,9.878e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniMUONSeq";
    cfg.order   = 316;
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
    AlgConfig cfg("MuonTrackFitter",manager,inputs,outputs,1.315e+05,3.477e+05);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniMUONSeq";
    cfg.order   = 317;
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
    AlgConfig cfg("MuonTrackAligMonitor",manager,inputs,outputs,1.561e+03,3.671e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniMUONSeq";
    cfg.order   = 318;
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
    AlgConfig cfg("VeloClusterMonitor",manager,inputs,outputs,3.835e+02,3.443e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniVELOSeq";
    cfg.order   = 320;
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
    AlgConfig cfg("VeloTrackMonitor",manager,inputs,outputs,1.104e+03,2.187e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniVELOSeq";
    cfg.order   = 321;
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
    AlgConfig cfg("VeloIPResolutionMonitor",manager,inputs,outputs,2.128e+02,2.068e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniVELOSeq";
    cfg.order   = 322;
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
    AlgConfig cfg("TrackMonitor",manager,inputs,outputs,4.059e+03,9.928e+03);
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
    const char* inputs[] = {"/Event/Rec/Track/Muon","Rec/Track/Muon",0};
    const char* outputs[] = {"/Event/Rec/Vertex/DiMuon",0};
    AlgConfig cfg("TrackDiMuonMonitor",manager,inputs,outputs,6.748e+02,7.566e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 325;
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
    AlgConfig cfg("TrackVertexMonitor",manager,inputs,outputs,3.306e+03,7.636e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 326;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Vertex/V0","Rec/Vertex/V0",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackV0Monitor",manager,inputs,outputs,3.011e+02,4.349e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
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
    AlgConfig cfg("TrackFitMatchMonitor",manager,inputs,outputs,9.898e+02,2.345e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 328;
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
    AlgConfig cfg("TrackAlignMonitor",manager,inputs,outputs,1.292e+03,3.208e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 329;
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
    AlgConfig cfg("TrackITOverlapMonitor",manager,inputs,outputs,6.774e+02,1.474e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 330;
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
    AlgConfig cfg("TrackVeloOverlapMonitor",manager,inputs,outputs,8.198e+02,1.593e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 331;
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
    AlgConfig cfg("TTTrackMonitor",manager,inputs,outputs,1.411e+03,3.117e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 332;
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
    AlgConfig cfg("ITTrackMonitor",manager,inputs,outputs,6.564e+02,1.263e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 333;
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
    AlgConfig cfg("TrackPV2HalfAlignMonitor",manager,inputs,outputs,4.634e+02,6.971e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 334;
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
    AlgConfig cfg("TrackEcalMatchMonitor",manager,inputs,outputs,4.516e+02,4.928e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 335;
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
    AlgConfig cfg("TrackSpdMatchMonitor",manager,inputs,outputs,4.749e+02,7.791e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 336;
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
    AlgConfig cfg("TrackPrsMatchMonitor",manager,inputs,outputs,2.144e+02,9.321e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 337;
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
    AlgConfig cfg("TrackMuonMatchMonitor",manager,inputs,outputs,3.270e+02,6.519e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 338;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/DiMuon","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OTTimeMonitor",manager,inputs,outputs,7.325e+02,1.099e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniOTSeq";
    cfg.order   = 340;
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
    AlgConfig cfg("OTTrackMonitor",manager,inputs,outputs,1.308e+03,1.695e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniOTSeq";
    cfg.order   = 341;
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
    AlgConfig cfg("OTHitEfficiencyMonitor",manager,inputs,outputs,2.312e+03,3.388e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniOTSeq";
    cfg.order   = 342;
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
    AlgConfig cfg("TTClusterMonitor",manager,inputs,outputs,4.959e+02,3.264e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniSTSeq";
    cfg.order   = 344;
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
    AlgConfig cfg("ITClusterMonitor",manager,inputs,outputs,8.405e+02,1.564e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniSTSeq";
    cfg.order   = 345;
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
    AlgConfig cfg("ChargedProtoPMoni",manager,inputs,outputs,6.041e+02,1.196e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniPROTOSeq";
    cfg.order   = 347;
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
    AlgConfig cfg("HltRateMonitor",manager,inputs,outputs,7.574e+02,1.702e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniHltSeq";
    cfg.order   = 349;
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
    AlgConfig cfg("HltCompositionMonitor",manager,inputs,outputs,5.050e+02,1.064e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniHltSeq";
    cfg.order   = 350;
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
    AlgConfig cfg("MuMonitor",manager,inputs,outputs,2.918e+02,1.227e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniHltSeq";
    cfg.order   = 351;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Trigger/RawEvent",0};
    AlgConfig cfg("TriggerRawBank",manager,inputs,outputs,3.112e+02,1.443e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "OutputDSTSeq";
    cfg.order   = 354;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Muon/RawEvent",0};
    AlgConfig cfg("MuonRawBank",manager,inputs,outputs,2.054e+02,4.576e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "OutputDSTSeq";
    cfg.order   = 355;
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
    AlgConfig cfg("TrackToDST",manager,inputs,outputs,1.594e+02,2.217e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "OutputDSTSeq";
    cfg.order   = 356;
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
    AlgConfig cfg("PackTracks",manager,inputs,outputs,8.746e+02,1.237e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 358;
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
    AlgConfig cfg("PackRichPIDs",manager,inputs,outputs,7.366e+02,8.432e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 359;
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
    AlgConfig cfg("PackMuonPIDs",manager,inputs,outputs,6.990e+02,7.590e+02);
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
    const char* inputs[] = {"/Event/Rec/Calo/Photons","Rec/Calo/Photons",0};
    const char* outputs[] = {"/Event/pRec/Calo/Photons",0};
    AlgConfig cfg("PackPhotons",manager,inputs,outputs,7.300e+02,9.570e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPacking";
    cfg.order   = 362;
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
    AlgConfig cfg("PackElectrons",manager,inputs,outputs,1.595e+02,2.569e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPacking";
    cfg.order   = 363;
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
    AlgConfig cfg("PackMergedPi0s",manager,inputs,outputs,1.564e+02,3.103e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPacking";
    cfg.order   = 364;
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
    AlgConfig cfg("PackSplitPhotons",manager,inputs,outputs,1.740e+02,6.617e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPacking";
    cfg.order   = 365;
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
    AlgConfig cfg("ProtoParticlePIDClean",manager,inputs,outputs,1.349e+02,3.591e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 366;
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
    AlgConfig cfg("PackChargedProtos",manager,inputs,outputs,4.310e+02,6.671e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 367;
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
    AlgConfig cfg("PackNeutralProtos",manager,inputs,outputs,3.174e+02,3.986e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 368;
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
    AlgConfig cfg("PackRecVertex",manager,inputs,outputs,5.668e+02,6.697e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 369;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Vertex/Weights","Rec/Vertex/Weights","pRec/Vertex/Weights",0};
    const char* outputs[] = {"/Event/pRec/Vertex/Weights",0};
    AlgConfig cfg("PackPVWeights",manager,inputs,outputs,9.984e+02,1.269e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 370;
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
    AlgConfig cfg("PackTwoProngVertex",manager,inputs,outputs,3.565e+02,5.392e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 371;
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
    AlgConfig cfg("PackMuonTracks",manager,inputs,outputs,2.085e+02,7.617e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 372;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Muon","/Event/Muon/RawEvent","/Event/Rec","/Event/Rec/Header","/Event/Rec/Status","/Event/Rec/Summary","/Event/Trigger","/Event/Trigger/RawEvent","/Event/pRec","/Event/pRec/Calo","/Event/pRec/Calo/Electrons","/Event/pRec/Calo/MergedPi0s","/Event/pRec/Calo/Photons","/Event/pRec/Calo/SplitPhotons","/Event/pRec/Muon","/Event/pRec/Muon/MuonPID","/Event/pRec/ProtoP","/Event/pRec/ProtoP/Charged","/Event/pRec/ProtoP/Neutrals","/Event/pRec/Rich","/Event/pRec/Rich/PIDs","/Event/pRec/Track","/Event/pRec/Track/Best","/Event/pRec/Track/Muon","/Event/pRec/Vertex","/Event/pRec/Vertex/Primary","/Event/pRec/Vertex/V0","/Event/pRec/Vertex/Weights",0};
    const char* outputs[] = {0};
    AlgConfig cfg("DstWriter",manager,inputs,outputs,5.234e+05,1.556e+06);
    cfg.setDaughters(daughters);
    cfg.parent  = "Framework";
    cfg.order   = 373;
    cfg.level   = 1;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Muon/RawEvent","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/DiMuon","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","/Event/Trigger/RawEvent","/Event/pRec/Calo/Electrons","/Event/pRec/Calo/MergedPi0s","/Event/pRec/Calo/Photons","/Event/pRec/Calo/SplitPhotons","/Event/pRec/Muon/MuonPID","/Event/pRec/ProtoP/Charged","/Event/pRec/ProtoP/Neutrals","/Event/pRec/Rich/PIDs","/Event/pRec/Track/Best","/Event/pRec/Track/Muon","/Event/pRec/Vertex/Primary","/Event/pRec/Vertex/V0","/Event/pRec/Vertex/Weights","Link/Rec/Track/BestClones",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FSROutputStreamDstWriter",manager,inputs,outputs,1.380e+01,5.997e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "Framework";
    cfg.order   = 374;
    cfg.level   = 1;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","DAQ/ODIN","DAQ/RawEvent","pRec/RawEvent",0};
    const char* outputs[] = {"/Event/DAQ/ODIN",0};
    AlgConfig cfg("Framework",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "Framework";
    cfg.order   = 375;
    cfg.level   = 0;
    cfg.flags   = 0x2000000;
    cfg.type    = "Framework";
    AlgEntry* a = new AlgEntry(getMultiplicity(multiplicity,cfg),cfg);
    a->initialize();
  }
