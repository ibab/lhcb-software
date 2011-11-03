/*
=================  Algorithm execution list:
*/
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Rec/Header","/Event/Rec/Status",0};
    AlgConfig cfg("BrunelInit",manager,inputs,outputs,5.471e+02,7.002e+02);
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
    AlgConfig cfg("BrunelEventCount",manager,inputs,outputs,5.982e+00,3.206e+00);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("BrunelBankKiller",manager,inputs,outputs,1.322e+01,2.246e+01);
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
    const char* inputs[] = {"/Event/DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FilterNoBeam",manager,inputs,outputs,1.657e+01,6.745e+00);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Hlt/LumiSummary",0};
    AlgConfig cfg("LumiDecodeNoBeam",manager,inputs,outputs,3.415e+01,6.484e+00);
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
    const char* inputs[] = {"/Event/Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiFilterNoBeam",manager,inputs,outputs,1.435e+01,2.444e+00);
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
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","/Event/Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiCountNoBeam",manager,inputs,outputs,8.415e+01,1.764e+02);
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
    const char* inputs[] = {"/Event/DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FilterBeamCrossing",manager,inputs,outputs,1.039e+01,5.007e+00);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Hlt/LumiSummary",0};
    AlgConfig cfg("LumiDecodeBeamCrossing",manager,inputs,outputs,3.595e+01,3.113e+01);
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
    const char* inputs[] = {"/Event/Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiFilterBeamCrossing",manager,inputs,outputs,1.606e+01,1.009e+01);
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
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","/Event/Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiCountBeamCrossing",manager,inputs,outputs,5.346e+01,8.284e+01);
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
    const char* inputs[] = {"/Event/DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FilterBeam1",manager,inputs,outputs,1.140e+01,2.713e+01);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Hlt/LumiSummary",0};
    AlgConfig cfg("LumiDecodeBeam1",manager,inputs,outputs,3.483e+01,8.336e+00);
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
    const char* inputs[] = {"/Event/Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiFilterBeam1",manager,inputs,outputs,1.499e+01,3.355e+00);
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
    const char* inputs[] = {"/Event/DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FillingBeam1",manager,inputs,outputs,1.715e+01,4.054e+01);
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
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","/Event/Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiCountBeam1",manager,inputs,outputs,5.355e+01,8.070e+01);
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
    const char* inputs[] = {"/Event/DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FilterBeam2",manager,inputs,outputs,1.034e+01,4.853e+00);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Hlt/LumiSummary",0};
    AlgConfig cfg("LumiDecodeBeam2",manager,inputs,outputs,3.529e+01,9.566e+00);
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
    const char* inputs[] = {"/Event/Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiFilterBeam2",manager,inputs,outputs,1.514e+01,4.134e+00);
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
    const char* inputs[] = {"/Event/DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FillingBeam2",manager,inputs,outputs,1.386e+01,4.880e+00);
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
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","/Event/Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiCountBeam2",manager,inputs,outputs,5.880e+01,1.124e+02);
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
    const char* inputs[] = {"/Event/DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TimeSpanFilter",manager,inputs,outputs,1.035e+01,5.140e+00);
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
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TimeSpanAccounting",manager,inputs,outputs,1.927e+01,6.832e+00);
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
    AlgConfig cfg("EventAccount",manager,inputs,outputs,6.604e+00,3.256e+01);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PhysFilter",manager,inputs,outputs,2.751e+01,2.275e+01);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Hlt/DecReports",0};
    AlgConfig cfg("HltDecReportsDecoder",manager,inputs,outputs,5.552e+02,1.411e+04);
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
    const char* inputs[] = {"/Event/Hlt/DecReports",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HltErrorFilter",manager,inputs,outputs,2.977e+01,9.586e+00);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PrsCheckBanks",manager,inputs,outputs,1.915e+01,1.056e+01);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/DAQ/Status","/Event/Raw/Prs/Adcs",0};
    AlgConfig cfg("PrsADCs",manager,inputs,outputs,2.705e+02,3.186e+02);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Trig/L0/PrsRaw",0};
    AlgConfig cfg("PrsL0Bit",manager,inputs,outputs,1.383e+02,1.081e+02);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Trig/L0/SpdRaw",0};
    AlgConfig cfg("SpdL0Bit",manager,inputs,outputs,1.512e+02,1.135e+02);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Raw/Prs/Adcs","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PrsSpdFillRawBuffer",manager,inputs,outputs,7.050e+01,3.260e+01);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RemovePrsPackedBanks",manager,inputs,outputs,2.441e+01,7.737e+00);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalCheckBanks",manager,inputs,outputs,1.363e+01,1.343e+01);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Ecal/Adcs",0};
    AlgConfig cfg("EcalADCs",manager,inputs,outputs,6.785e+02,1.004e+03);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Trig/L0/EcalRaw",0};
    AlgConfig cfg("EcalTrigFromRaw",manager,inputs,outputs,3.339e+02,2.330e+02);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Raw/Ecal/Adcs","/Event/Trig/L0/EcalRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalFillRawBuffer",manager,inputs,outputs,3.470e+01,1.404e+01);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RemoveEcalPackedBanks",manager,inputs,outputs,2.956e+01,8.752e+00);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HcalCheckBanks",manager,inputs,outputs,1.382e+01,5.345e+00);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Hcal/Adcs",0};
    AlgConfig cfg("HcalADCs",manager,inputs,outputs,2.001e+02,6.272e+01);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Trig/L0/HcalRaw",0};
    AlgConfig cfg("HcalTrigFromRaw",manager,inputs,outputs,1.147e+02,5.049e+02);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Raw/Hcal/Adcs","/Event/Trig/L0/HcalRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HcalFillRawBuffer",manager,inputs,outputs,2.858e+01,1.329e+02);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RemoveHcalPackedBanks",manager,inputs,outputs,1.929e+01,7.072e+00);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Rec/Status",0};
    const char* outputs[] = {"/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters",0};
    AlgConfig cfg("DecodeVeloClusters",manager,inputs,outputs,5.559e+02,5.903e+02);
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
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/TT/Clusters","/Event/Rec/TT/Summary",0};
    AlgConfig cfg("CreateTTClusters",manager,inputs,outputs,3.927e+02,3.878e+02);
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
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/TT/LiteClusters",0};
    AlgConfig cfg("CreateTTLiteClusters",manager,inputs,outputs,1.197e+02,6.383e+01);
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
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/IT/Clusters","/Event/Rec/IT/Summary",0};
    AlgConfig cfg("CreateITClusters",manager,inputs,outputs,3.086e+02,3.224e+02);
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
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/IT/LiteClusters",0};
    AlgConfig cfg("CreateITLiteClusters",manager,inputs,outputs,9.877e+01,5.514e+01);
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
    const char* inputs[] = {"/Event/Raw/Velo/LiteClusters",0};
    const char* outputs[] = {"/Event/Rec/Track/Velo",0};
    AlgConfig cfg("FastVeloTracking",manager,inputs,outputs,1.027e+03,1.533e+03);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Raw/IT/LiteClusters","/Event/Raw/TT/LiteClusters","/Event/Rec/Track/Velo",0};
    const char* outputs[] = {"/Event/Rec/Track/Forward",0};
    AlgConfig cfg("PatForward",manager,inputs,outputs,1.025e+04,1.763e+04);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Track/Seed",0};
    AlgConfig cfg("PatSeeding",manager,inputs,outputs,5.739e+03,1.373e+04);
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
    const char* inputs[] = {"/Event/Raw/TT/LiteClusters","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo",0};
    const char* outputs[] = {"/Event/Rec/Track/Match",0};
    AlgConfig cfg("PatMatch",manager,inputs,outputs,3.613e+02,5.480e+02);
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
    const char* inputs[] = {"/Event/Raw/TT/LiteClusters","/Event/Rec/Track/Match","/Event/Rec/Track/Seed",0};
    const char* outputs[] = {"/Event/Rec/Track/Downstream",0};
    AlgConfig cfg("PatDownstream",manager,inputs,outputs,1.632e+03,3.782e+03);
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
    const char* inputs[] = {"/Event/Raw/TT/Clusters","/Event/Raw/Velo/Clusters","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Velo",0};
    const char* outputs[] = {"/Event/Rec/Track/VeloTT",0};
    AlgConfig cfg("PatVeloTT",manager,inputs,outputs,5.475e+03,7.975e+03);
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
    const char* inputs[] = {"/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Seed","/Event/Rec/Track/VeloTT",0};
    const char* outputs[] = {"/Event/Rec/Track/AllBest",0};
    AlgConfig cfg("TrackEventCloneKiller",manager,inputs,outputs,5.840e+02,8.771e+02);
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
    const char* inputs[] = {"/Event/Raw/IT/LiteClusters","/Event/Raw/TT/LiteClusters","/Event/Rec/Track/AllBest",0};
    const char* outputs[] = {0};
    AlgConfig cfg("InitBestFit",manager,inputs,outputs,1.910e+03,2.545e+03);
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
    const char* inputs[] = {"/Event/Raw/IT/Clusters","/Event/Raw/TT/Clusters","/Event/Raw/Velo/Clusters","/Event/Rec/Track/AllBest",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FitBest",manager,inputs,outputs,5.811e+04,8.128e+04);
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
    const char* inputs[] = {"/Event/Rec/Track/AllBest",0};
    const char* outputs[] = {"/Event/Rec/Track/Best",0};
    AlgConfig cfg("CopyBest",manager,inputs,outputs,2.025e+03,2.707e+03);
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
    const char* inputs[] = {"/Event/Rec/Track/Best","/Event/Rec/Track/Velo",0};
    const char* outputs[] = {"/Event/Rec/Track/PreparedVelo",0};
    AlgConfig cfg("TrackPrepareVelo",manager,inputs,outputs,7.758e+01,6.485e+01);
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
    const char* inputs[] = {"/Event/Raw/Velo/Clusters","/Event/Rec/Track/PreparedVelo",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FitVelo",manager,inputs,outputs,7.611e+03,1.050e+04);
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
    const char* inputs[] = {"/Event/Rec/Track/Best","/Event/Rec/Track/PreparedVelo",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CopyVelo",manager,inputs,outputs,2.865e+02,3.962e+02);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Link/Rec/Track/BestClones",0};
    AlgConfig cfg("FindTrackClones",manager,inputs,outputs,1.604e+03,2.680e+03);
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
    const char* inputs[] = {"/Event/Link/Rec/Track/BestClones","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FlagTrackClones",manager,inputs,outputs,3.605e+01,2.656e+01);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackAddLikelihood",manager,inputs,outputs,1.011e+04,1.339e+04);
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
    const char* inputs[] = {"/Event/Raw/IT/Clusters","/Event/Raw/TT/Clusters","/Event/Raw/Velo/Clusters","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackAddNNGhostId",manager,inputs,outputs,1.554e+03,1.997e+03);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackEraseExtraInfo",manager,inputs,outputs,2.333e+01,1.702e+01);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/Weights",0};
    AlgConfig cfg("PatPVOffline",manager,inputs,outputs,4.895e+02,9.859e+02);
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
    const char* inputs[] = {"/Event/Rec/Track/Best","/Event/Rec/Vertex/Primary",0};
    const char* outputs[] = {"/Event/Rec/Vertex/V0",0};
    AlgConfig cfg("TrackV0Finder",manager,inputs,outputs,8.876e+02,3.768e+03);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Rich/RecoEvent/Offline/Status",0};
    AlgConfig cfg("CheckProcOfflineStatus",manager,inputs,outputs,5.000e+01,1.318e+01);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("DecodeRawRichOffline",manager,inputs,outputs,3.913e+02,2.697e+02);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Rich/RecoEvent/Offline/Pixels",0};
    AlgConfig cfg("CreateOfflinePixels",manager,inputs,outputs,3.818e+03,4.432e+03);
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
    const char* inputs[] = {"/Event/Rec/Status","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Tracks",0};
    AlgConfig cfg("CreateOfflineTracks",manager,inputs,outputs,1.375e+04,1.779e+04);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Rich/RecoEvent/Offline/Photons",0};
    AlgConfig cfg("CreateOfflinePhotons",manager,inputs,outputs,4.036e+04,6.428e+04);
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
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Hcal/Adcs","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Prs/Adcs","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll",0};
    AlgConfig cfg("OfflineENNFinderR1GasTop",manager,inputs,outputs,2.572e+02,3.669e+02);
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
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OfflineENNFinderR1GasBottom",manager,inputs,outputs,2.164e+02,3.561e+02);
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
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OfflineENNFinderR2GasLeft",manager,inputs,outputs,3.757e+02,7.807e+02);
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
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OfflineENNFinderR2GasRight",manager,inputs,outputs,3.310e+02,7.068e+02);
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
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {"/Event/Rec/Rich/RecoEvent/Offline/Segments",0};
    AlgConfig cfg("OfflineENNRingsSegAssoc",manager,inputs,outputs,3.348e+01,1.657e+01);
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
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsBest",0};
    AlgConfig cfg("OfflineBestENNRings",manager,inputs,outputs,3.919e+01,1.611e+01);
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
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsBest",0};
    const char* outputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsIsolated",0};
    AlgConfig cfg("OfflineIsolatedENNRings",manager,inputs,outputs,7.908e+01,9.560e+01);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Hcal/Adcs","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Prs/Adcs","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDInit",manager,inputs,outputs,5.657e+00,3.678e+00);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDDigSel",manager,inputs,outputs,1.472e+01,4.306e+00);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/RecoEvent/Offline/Tracks",0};
    AlgConfig cfg("RichOfflineGPIDTrSel",manager,inputs,outputs,1.298e+02,8.948e+01);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Hcal/Adcs","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Prs/Adcs","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineBckEstIt0",manager,inputs,outputs,3.671e+02,4.147e+02);
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
    const char* inputs[] = {"/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDLLIt0",manager,inputs,outputs,2.847e+04,5.520e+04);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Hcal/Adcs","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Prs/Adcs","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineBckEstIt1",manager,inputs,outputs,5.684e+02,7.240e+02);
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
    const char* inputs[] = {"/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDLLIt1",manager,inputs,outputs,2.881e+04,7.268e+04);
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
    const char* inputs[] = {"/Event/Rec/Rich/GlobalPID/Offline/Tracks",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDFin",manager,inputs,outputs,2.099e+01,1.878e+01);
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
    const char* inputs[] = {"/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Rich/PIDs",0};
    AlgConfig cfg("MergeOfflineRichPIDs",manager,inputs,outputs,6.202e+01,2.329e+01);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Spd/Digits",0};
    AlgConfig cfg("SpdFromRaw",manager,inputs,outputs,8.567e+01,4.847e+01);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Prs/Digits",0};
    AlgConfig cfg("PrsFromRaw",manager,inputs,outputs,1.132e+02,1.305e+02);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Ecal/Digits",0};
    AlgConfig cfg("EcalZSup",manager,inputs,outputs,1.600e+02,1.471e+02);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Hcal/Digits",0};
    AlgConfig cfg("HcalZSup",manager,inputs,outputs,7.267e+01,3.581e+01);
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
    const char* inputs[] = {"/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloDigitFilter",manager,inputs,outputs,2.700e+03,1.591e+03);
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
    const char* inputs[] = {"/Event/Raw/Ecal/Digits",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalClusters",0};
    AlgConfig cfg("EcalClust",manager,inputs,outputs,1.057e+03,6.226e+02);
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
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalShare",manager,inputs,outputs,2.341e+02,3.400e+02);
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
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalCovar",manager,inputs,outputs,1.157e+02,1.152e+02);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/InAccEcal",0};
    AlgConfig cfg("InECAL",manager,inputs,outputs,2.362e+02,2.691e+02);
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
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/InAccEcal","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/ClusterMatch",0};
    AlgConfig cfg("ClusterMatch",manager,inputs,outputs,3.949e+02,6.601e+02);
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
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalClusters",0};
    const char* outputs[] = {"/Event/Rec/Calo/Photons",0};
    AlgConfig cfg("SinglePhotonRec",manager,inputs,outputs,2.451e+03,2.358e+03);
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
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/EcalClusters",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/SplitPhotons",0};
    AlgConfig cfg("MergedPi0Rec",manager,inputs,outputs,3.377e+02,6.131e+02);
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
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalClusters",0};
    const char* outputs[] = {"/Event/Rec/Calo/Electrons",0};
    AlgConfig cfg("SingleElectronRec",manager,inputs,outputs,9.580e+02,1.280e+03);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/InAccSpd",0};
    AlgConfig cfg("InSPD",manager,inputs,outputs,1.578e+02,1.700e+02);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/InAccPrs",0};
    AlgConfig cfg("InPRS",manager,inputs,outputs,1.196e+02,1.177e+02);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/InAccHcal",0};
    AlgConfig cfg("InHCAL",manager,inputs,outputs,1.185e+02,1.147e+02);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/InAccBrem",0};
    AlgConfig cfg("InBREM",manager,inputs,outputs,7.019e+01,4.933e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/Electrons","/Event/Rec/Calo/InAccEcal","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/ElectronMatch",0};
    AlgConfig cfg("ElectronMatch",manager,inputs,outputs,4.287e+02,8.266e+02);
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
    const char* inputs[] = {"/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/Photons","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/BremMatch",0};
    AlgConfig cfg("BremMatch",manager,inputs,outputs,3.213e+02,4.764e+02);
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
    const char* inputs[] = {"/Event/Raw/Spd/Digits","/Event/Rec/Calo/InAccSpd","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/SpdE",0};
    AlgConfig cfg("SpdE",manager,inputs,outputs,9.013e+01,6.744e+01);
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
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Rec/Calo/InAccPrs","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/PrsE",0};
    AlgConfig cfg("PrsE",manager,inputs,outputs,8.106e+01,2.188e+02);
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
    const char* inputs[] = {"/Event/Raw/Ecal/Digits","/Event/Rec/Calo/InAccEcal","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalE",0};
    AlgConfig cfg("EcalE",manager,inputs,outputs,1.509e+02,1.563e+02);
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
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Rec/Calo/InAccHcal","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/HcalE",0};
    AlgConfig cfg("HcalE",manager,inputs,outputs,1.586e+02,1.637e+02);
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
    const char* inputs[] = {"/Event/Rec/Calo/ElectronMatch","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalChi2",0};
    AlgConfig cfg("EcalChi22ID",manager,inputs,outputs,5.154e+01,1.440e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/BremMatch","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/BremChi2",0};
    AlgConfig cfg("BremChi22ID",manager,inputs,outputs,4.556e+01,1.287e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/ClusterMatch","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/ClusChi2",0};
    AlgConfig cfg("ClusChi22ID",manager,inputs,outputs,4.584e+01,1.784e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/PrsE",0};
    const char* outputs[] = {"/Event/Rec/Calo/PrsPIDe",0};
    AlgConfig cfg("PrsPIDe",manager,inputs,outputs,3.973e+01,1.609e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/EcalChi2",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalPIDe",0};
    AlgConfig cfg("EcalPIDe",manager,inputs,outputs,3.664e+01,1.532e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/BremChi2",0};
    const char* outputs[] = {"/Event/Rec/Calo/BremPIDe",0};
    AlgConfig cfg("BremPIDe",manager,inputs,outputs,3.462e+01,1.416e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/HcalE",0};
    const char* outputs[] = {"/Event/Rec/Calo/HcalPIDe",0};
    AlgConfig cfg("HcalPIDe",manager,inputs,outputs,3.307e+01,1.259e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/EcalE",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalPIDmu",0};
    AlgConfig cfg("EcalPIDmu",manager,inputs,outputs,3.381e+01,1.236e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/HcalE",0};
    const char* outputs[] = {"/Event/Rec/Calo/HcalPIDmu",0};
    AlgConfig cfg("HcalPIDmu",manager,inputs,outputs,3.308e+01,1.176e+01);
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
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Photons",0};
    const char* outputs[] = {"/Event/Rec/Calo/PhotonID",0};
    AlgConfig cfg("PhotonID",manager,inputs,outputs,9.109e+02,1.078e+03);
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
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/MergedPi0s",0};
    const char* outputs[] = {"/Event/Rec/Calo/MergedID",0};
    AlgConfig cfg("MergedID",manager,inputs,outputs,6.567e+01,6.825e+01);
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
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/SplitPhotons",0};
    const char* outputs[] = {"/Event/Rec/Calo/PhotonFromMergedID",0};
    AlgConfig cfg("PhotonFromMergedID",manager,inputs,outputs,8.427e+01,4.526e+02);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Muon/Coords",0};
    AlgConfig cfg("MuonRec",manager,inputs,outputs,2.148e+02,1.429e+02);
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
    const char* inputs[] = {"/Event/Raw/Muon/Coords","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Muon/MuonPID","/Event/Rec/Track/Muon",0};
    AlgConfig cfg("MuonIDAlg",manager,inputs,outputs,1.653e+03,2.614e+03);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/ProtoP/Charged",0};
    AlgConfig cfg("ChargedProtoPMaker",manager,inputs,outputs,8.995e+01,5.737e+01);
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
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddEcal",manager,inputs,outputs,1.324e+03,1.972e+03);
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
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddBrem",manager,inputs,outputs,1.032e+03,1.391e+03);
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
    const char* inputs[] = {"/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccHcal","/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddHcal",manager,inputs,outputs,7.185e+01,2.506e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddPrs",manager,inputs,outputs,5.679e+01,3.263e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/SpdE","/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddSpd",manager,inputs,outputs,4.365e+01,1.582e+01);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddVeloDEDX",manager,inputs,outputs,1.351e+02,1.987e+02);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","/Event/Rec/Rich/PIDs",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddRich",manager,inputs,outputs,6.061e+01,3.925e+01);
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
    const char* inputs[] = {"/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddMuon",manager,inputs,outputs,4.063e+01,2.116e+01);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPCombDLLs",manager,inputs,outputs,5.270e+01,4.727e+01);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongElectron",manager,inputs,outputs,1.270e+03,1.669e+03);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongMuon",manager,inputs,outputs,7.343e+01,1.126e+02);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongPion",manager,inputs,outputs,1.293e+03,1.688e+03);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongKaon",manager,inputs,outputs,1.307e+03,1.706e+03);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongProton",manager,inputs,outputs,1.385e+03,1.815e+03);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongGhost",manager,inputs,outputs,1.248e+03,1.631e+03);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamElectron",manager,inputs,outputs,4.751e+02,6.765e+02);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamMuon",manager,inputs,outputs,3.547e+01,5.645e+01);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamPion",manager,inputs,outputs,4.841e+02,6.907e+02);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamKaon",manager,inputs,outputs,4.986e+02,7.219e+02);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamProton",manager,inputs,outputs,4.983e+02,7.212e+02);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamGhost",manager,inputs,outputs,4.933e+02,7.046e+02);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamElectron",manager,inputs,outputs,2.390e+02,3.129e+02);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamMuon",manager,inputs,outputs,2.357e+02,3.099e+02);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamPion",manager,inputs,outputs,2.404e+02,3.297e+02);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamKaon",manager,inputs,outputs,2.560e+02,3.389e+02);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamProton",manager,inputs,outputs,2.542e+02,3.353e+02);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamGhost",manager,inputs,outputs,2.567e+02,3.581e+02);
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
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/SplitPhotons",0};
    const char* outputs[] = {"/Event/Rec/ProtoP/Neutrals",0};
    AlgConfig cfg("NeutralProtoPMaker",manager,inputs,outputs,1.891e+03,2.264e+03);
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
    const char* inputs[] = {"/Event/Raw/IT/Clusters","/Event/Raw/Muon/Coords","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/Velo/Clusters","/Event/Rec/Track/Best","/Event/Rec/Track/Muon","/Event/Rec/Vertex/Primary",0};
    const char* outputs[] = {"/Event/Rec/Summary",0};
    AlgConfig cfg("RecSummary",manager,inputs,outputs,1.658e+02,6.178e+01);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OverallEventProcTime",manager,inputs,outputs,1.416e+01,3.983e+00);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackEventProcTime",manager,inputs,outputs,1.193e+01,4.377e+00);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("VertexEventProcTime",manager,inputs,outputs,8.497e+00,3.335e+00);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichEventProcTime",manager,inputs,outputs,8.127e+00,3.178e+00);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloEventProcTime",manager,inputs,outputs,7.198e+00,3.797e+00);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonEventProcTime",manager,inputs,outputs,7.251e+00,2.526e+00);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ProtoEventProcTime",manager,inputs,outputs,7.531e+00,2.994e+00);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ProcStatAbortMoni",manager,inputs,outputs,1.618e+01,5.134e+00);
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
    const char* inputs[] = {"/Event/Raw/Ecal/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalDigitMon",manager,inputs,outputs,1.053e+03,1.323e+03);
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
    const char* inputs[] = {"/Event/Raw/Hcal/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HcalDigitMon",manager,inputs,outputs,3.707e+02,3.490e+02);
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
    const char* inputs[] = {"/Event/Raw/Prs/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PrsDigitMon",manager,inputs,outputs,5.508e+02,6.781e+02);
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
    const char* inputs[] = {"/Event/Raw/Spd/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("SpdDigitMon",manager,inputs,outputs,2.524e+02,3.015e+02);
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
    const char* inputs[] = {"/Event/Raw/Spd/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("SpdMon",manager,inputs,outputs,1.296e+03,1.629e+03);
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
    const char* inputs[] = {"/Event/Raw/Ecal/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalEFlowMon",manager,inputs,outputs,1.732e+02,2.005e+02);
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
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalClusterMon",manager,inputs,outputs,1.527e+02,1.715e+02);
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
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/Electrons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ElectronMon",manager,inputs,outputs,7.622e+01,8.248e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/Photons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PhotonMon",manager,inputs,outputs,1.344e+02,1.450e+02);
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
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/SplitPhotons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("SplitPhotonMon",manager,inputs,outputs,2.076e+01,1.601e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/MergedPi0s",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MergedPi0Mon",manager,inputs,outputs,1.851e+01,1.140e+01);
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
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/Photons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ResolvedPi0Mon",manager,inputs,outputs,1.982e+02,2.292e+02);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ProtoElectronMon",manager,inputs,outputs,1.558e+02,2.115e+02);
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
    const char* inputs[] = {"/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalSplitClusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PhotonMatchMon",manager,inputs,outputs,1.692e+02,2.569e+02);
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
    const char* inputs[] = {"/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ElectronMatchMon",manager,inputs,outputs,4.608e+01,3.078e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/Photons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("BremMatchMon",manager,inputs,outputs,6.809e+01,6.873e+01);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloEMuPIDMonUncut",manager,inputs,outputs,7.355e+01,8.004e+01);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloEMuPIDMonSoft",manager,inputs,outputs,2.146e+01,1.135e+01);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloEMuPIDMonHard",manager,inputs,outputs,1.748e+01,1.454e+01);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRawDataSize",manager,inputs,outputs,4.352e+02,9.848e+01);
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
    const char* inputs[] = {"/Event/Hlt/DecReports",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichDataSizeL0Filter",manager,inputs,outputs,2.063e+01,6.422e+00);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRawDataSizeL0",manager,inputs,outputs,3.243e+02,4.269e+01);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRawDataDBCheck",manager,inputs,outputs,3.115e+02,2.595e+02);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichHotPixels",manager,inputs,outputs,6.826e+02,8.037e+02);
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
    const char* inputs[] = {"/Event/DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichDecodingErrors",manager,inputs,outputs,3.922e+02,7.664e+01);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRecPixelQC",manager,inputs,outputs,4.634e+02,5.481e+02);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRecPixelClusters",manager,inputs,outputs,5.254e+02,6.541e+02);
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
    AlgConfig cfg("RiLongTrkEff",manager,inputs,outputs,1.073e+02,1.319e+02);
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
    AlgConfig cfg("RiForwardTrkEff",manager,inputs,outputs,7.923e+01,9.825e+01);
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
    AlgConfig cfg("RiMatchTrkEff",manager,inputs,outputs,2.928e+01,2.990e+01);
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
    AlgConfig cfg("RiKsTrackTrkEff",manager,inputs,outputs,5.208e+01,6.445e+01);
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
    AlgConfig cfg("RiVeloTTTrkEff",manager,inputs,outputs,4.076e+01,4.301e+01);
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
    AlgConfig cfg("RiSeedTrkEff",manager,inputs,outputs,5.205e+01,5.645e+01);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResLong",manager,inputs,outputs,1.243e+03,1.930e+03);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResForward",manager,inputs,outputs,7.974e+02,1.223e+03);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResMatch",manager,inputs,outputs,8.501e+01,1.587e+02);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResKsTrack",manager,inputs,outputs,2.116e+02,3.896e+02);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResVeloTT",manager,inputs,outputs,6.132e+01,1.062e+02);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResSeed",manager,inputs,outputs,3.807e+02,6.067e+02);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResLongTight",manager,inputs,outputs,7.778e+02,1.250e+03);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResForwardTight",manager,inputs,outputs,5.334e+02,8.190e+02);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResMatchTight",manager,inputs,outputs,5.872e+01,1.176e+02);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResKsTrackTight",manager,inputs,outputs,9.243e+01,1.794e+02);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResVeloTTTight",manager,inputs,outputs,4.241e+01,7.438e+01);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResSeedTight",manager,inputs,outputs,3.218e+02,5.009e+02);
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
    const char* inputs[] = {"/Event/Rec/Rich/PIDs","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiPIDMonLong2To100",manager,inputs,outputs,5.704e+01,5.191e+01);
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
    const char* inputs[] = {"/Event/Rec/Rich/PIDs","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiPIDMonLong2To10",manager,inputs,outputs,3.288e+01,2.797e+01);
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
    const char* inputs[] = {"/Event/Rec/Rich/PIDs","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiPIDMonLong10To70",manager,inputs,outputs,2.916e+01,2.477e+01);
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
    const char* inputs[] = {"/Event/Rec/Rich/PIDs","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiPIDMonLong70To100",manager,inputs,outputs,2.172e+01,1.579e+01);
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
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ENNRingMoniAll",manager,inputs,outputs,3.398e+01,3.707e+01);
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
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsBest",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ENNRingMoniBest",manager,inputs,outputs,1.864e+01,1.793e+01);
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
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsIsolated",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ENNRingMoniIsolated",manager,inputs,outputs,1.220e+01,8.093e+00);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichAlignMoniR1Gas",manager,inputs,outputs,1.032e+03,1.811e+03);
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
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichAlignMoniR2Gas",manager,inputs,outputs,6.598e+02,1.171e+03);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichHPDIFBMoni",manager,inputs,outputs,8.318e+02,8.783e+02);
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
    const char* inputs[] = {"/Event/Rec/Muon/MuonPID","/Event/Rec/Track/Best","/Event/Rec/Track/Muon",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonPIDMonitorDown",manager,inputs,outputs,1.324e+02,1.384e+02);
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
    const char* inputs[] = {"/Event/Rec/Muon/MuonPID","/Event/Rec/Track/Best","/Event/Rec/Track/Muon",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonPIDMonitorLong",manager,inputs,outputs,2.255e+02,2.779e+02);
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
    const char* inputs[] = {"/Event/Rec/Track/Muon",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonTrackFitter",manager,inputs,outputs,1.460e+03,6.906e+03);
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
    const char* inputs[] = {"/Event/Rec/Muon/MuonPID",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonTrackAligMonitor",manager,inputs,outputs,8.286e+01,1.991e+02);
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
    const char* inputs[] = {"/Event/Raw/Velo/Clusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("VeloClusterMonitor",manager,inputs,outputs,7.486e+02,8.764e+02);
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
    const char* inputs[] = {"/Event/Raw/Velo/Clusters","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("VeloTrackMonitor",manager,inputs,outputs,2.335e+03,2.967e+03);
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
    const char* inputs[] = {"/Event/Rec/Track/Best","/Event/Rec/Vertex/Primary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("VeloIPResolutionMonitor",manager,inputs,outputs,9.706e+02,1.546e+03);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackMonitor",manager,inputs,outputs,6.793e+03,8.728e+03);
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
    const char* inputs[] = {"/Event/Rec/Track/Muon",0};
    const char* outputs[] = {"/Event/Rec/Vertex/DiMuon",0};
    AlgConfig cfg("TrackDiMuonMonitor",manager,inputs,outputs,5.565e+01,3.228e+01);
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
    const char* inputs[] = {"/Event/Rec/Track/Best","/Event/Rec/Vertex/Primary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackVertexMonitor",manager,inputs,outputs,3.429e+02,4.112e+02);
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
    const char* inputs[] = {"/Event/Rec/Vertex/V0",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackV0Monitor",manager,inputs,outputs,2.089e+01,1.126e+01);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackFitMatchMonitor",manager,inputs,outputs,5.215e+02,6.863e+02);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackAlignMonitor",manager,inputs,outputs,6.513e+02,8.341e+02);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackITOverlapMonitor",manager,inputs,outputs,1.026e+03,1.369e+03);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackVeloOverlapMonitor",manager,inputs,outputs,1.279e+02,2.421e+02);
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
    const char* inputs[] = {"/Event/Raw/TT/Clusters","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TTTrackMonitor",manager,inputs,outputs,7.831e+02,1.028e+03);
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
    const char* inputs[] = {"/Event/Raw/IT/Clusters","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ITTrackMonitor",manager,inputs,outputs,1.125e+03,1.480e+03);
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
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackPV2HalfAlignMonitor",manager,inputs,outputs,4.392e+02,7.777e+02);
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
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackEcalMatchMonitor",manager,inputs,outputs,5.683e+02,8.496e+02);
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
    const char* inputs[] = {"/Event/Raw/Spd/Digits","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackSpdMatchMonitor",manager,inputs,outputs,5.935e+02,8.363e+02);
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
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackPrsMatchMonitor",manager,inputs,outputs,6.704e+02,9.536e+02);
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
    const char* inputs[] = {"/Event/Raw/Muon/Coords","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackMuonMatchMonitor",manager,inputs,outputs,3.133e+02,3.847e+02);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/DiMuon","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OTTimeMonitor",manager,inputs,outputs,2.914e+02,2.555e+02);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OTTrackMonitor",manager,inputs,outputs,1.992e+03,2.547e+03);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OTHitEfficiencyMonitor",manager,inputs,outputs,1.125e+03,1.259e+03);
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
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Raw/TT/Clusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TTClusterMonitor",manager,inputs,outputs,3.690e+02,5.147e+02);
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
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Raw/IT/Clusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ITClusterMonitor",manager,inputs,outputs,2.577e+02,2.908e+02);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPMoni",manager,inputs,outputs,2.601e+02,3.027e+02);
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
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Hlt/DecReports",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HltRateMonitor",manager,inputs,outputs,2.032e+01,8.113e+00);
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
    const char* inputs[] = {"/Event/Hlt/DecReports",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HltCompositionMonitor",manager,inputs,outputs,1.794e+01,5.663e+00);
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
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Hlt/DecReports","/Event/Rec/Track/Velo",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuMonitor",manager,inputs,outputs,4.007e+01,8.418e+00);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Trigger/RawEvent",0};
    AlgConfig cfg("TriggerRawBank",manager,inputs,outputs,5.098e+01,1.429e+01);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Muon/RawEvent",0};
    AlgConfig cfg("MuonRawBank",manager,inputs,outputs,2.725e+01,9.618e+00);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackToDST",manager,inputs,outputs,1.144e+02,1.318e+02);
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
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/pRec/Track/Best",0};
    AlgConfig cfg("PackTracks",manager,inputs,outputs,1.409e+02,1.359e+02);
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
    const char* inputs[] = {"/Event/Rec/Rich/PIDs",0};
    const char* outputs[] = {"/Event/pRec/Rich/PIDs",0};
    AlgConfig cfg("PackRichPIDs",manager,inputs,outputs,5.225e+01,1.668e+01);
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
    const char* inputs[] = {"/Event/Rec/Muon/MuonPID",0};
    const char* outputs[] = {"/Event/pRec/Muon/MuonPID",0};
    AlgConfig cfg("PackMuonPIDs",manager,inputs,outputs,3.970e+01,1.134e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/Photons",0};
    const char* outputs[] = {"/Event/pRec/Calo/Photons",0};
    AlgConfig cfg("PackPhotons",manager,inputs,outputs,4.453e+01,2.106e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/Electrons",0};
    const char* outputs[] = {"/Event/pRec/Calo/Electrons",0};
    AlgConfig cfg("PackElectrons",manager,inputs,outputs,2.803e+01,1.256e+01);
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
    const char* inputs[] = {"/Event/Rec/Calo/MergedPi0s",0};
    const char* outputs[] = {"/Event/pRec/Calo/MergedPi0s",0};
    AlgConfig cfg("PackMergedPi0s",manager,inputs,outputs,2.006e+01,9.324e+00);
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
    const char* inputs[] = {"/Event/Rec/Calo/SplitPhotons",0};
    const char* outputs[] = {"/Event/pRec/Calo/SplitPhotons",0};
    AlgConfig cfg("PackSplitPhotons",manager,inputs,outputs,2.133e+01,1.205e+02);
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
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ProtoParticlePIDClean",manager,inputs,outputs,4.550e+01,3.906e+01);
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
    AlgConfig cfg("PackChargedProtos",manager,inputs,outputs,3.767e+01,1.817e+01);
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
    AlgConfig cfg("PackNeutralProtos",manager,inputs,outputs,2.869e+01,1.143e+01);
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
    const char* inputs[] = {"/Event/Rec/Vertex/Primary",0};
    const char* outputs[] = {"/Event/pRec/Vertex/Primary",0};
    AlgConfig cfg("PackRecVertex",manager,inputs,outputs,2.626e+01,6.737e+00);
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
    const char* inputs[] = {"/Event/Rec/Vertex/Weights",0};
    const char* outputs[] = {"/Event/pRec/Vertex/Weights",0};
    AlgConfig cfg("PackPVWeights",manager,inputs,outputs,3.651e+01,8.852e+00);
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
    const char* inputs[] = {"/Event/Rec/Vertex/V0",0};
    const char* outputs[] = {"/Event/pRec/Vertex/V0",0};
    AlgConfig cfg("PackTwoProngVertex",manager,inputs,outputs,1.910e+01,5.926e+00);
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
    AlgConfig cfg("PackMuonTracks",manager,inputs,outputs,2.703e+01,8.915e+00);
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
    const char* inputs[] = {"/Event/Muon/RawEvent","/Event/Rec/Header","/Event/Rec/Status","/Event/Rec/Summary","/Event/Trigger/RawEvent","/Event/pRec/Calo/Electrons","/Event/pRec/Calo/MergedPi0s","/Event/pRec/Calo/Photons","/Event/pRec/Calo/SplitPhotons","/Event/pRec/Muon/MuonPID","/Event/pRec/ProtoP/Charged","/Event/pRec/ProtoP/Neutrals","/Event/pRec/Rich/PIDs","/Event/pRec/Track/Best","/Event/pRec/Track/Muon","/Event/pRec/Vertex/Primary","/Event/pRec/Vertex/V0","/Event/pRec/Vertex/Weights",0};
    const char* outputs[] = {0};
    AlgConfig cfg("DstWriter",manager,inputs,outputs,4.314e+03,3.539e+04);
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
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Muon/RawEvent","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/DiMuon","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","/Event/Trigger/RawEvent","/Event/pRec/Calo/Electrons","/Event/pRec/Calo/MergedPi0s","/Event/pRec/Calo/Photons","/Event/pRec/Calo/SplitPhotons","/Event/pRec/Muon/MuonPID","/Event/pRec/ProtoP/Charged","/Event/pRec/ProtoP/Neutrals","/Event/pRec/Rich/PIDs","/Event/pRec/Track/Best","/Event/pRec/Track/Muon","/Event/pRec/Vertex/Primary","/Event/pRec/Vertex/V0","/Event/pRec/Vertex/Weights",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FSROutputStreamDstWriter",manager,inputs,outputs,5.831e+00,2.246e+00);
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
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
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
