/*
=================  Algorithm execution list:
*/
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Rec/Header","/Event/Rec/Status",0};
    AlgConfig cfg("BrunelInit",manager,inputs,outputs,9.809e+02,1.315e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "InitBrunelSeq";
    cfg.order   = 5;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("BrunelEventCount",manager,inputs,outputs,1.537e+01,2.938e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "InitBrunelSeq";
    cfg.order   = 6;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("BrunelBankKiller",manager,inputs,outputs,7.203e+01,2.820e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "InitBrunelSeq";
    cfg.order   = 7;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FilterNoBeam",manager,inputs,outputs,1.054e+02,1.297e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiNoBeamSeq";
    cfg.order   = 11;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
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
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
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
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
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
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FilterBeamCrossing",manager,inputs,outputs,6.997e+01,2.687e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeamCrossingSeq";
    cfg.order   = 16;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Hlt/LumiSummary",0};
    AlgConfig cfg("LumiDecodeBeamCrossing",manager,inputs,outputs,2.539e+02,2.001e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeamCrossingSeq";
    cfg.order   = 17;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiFilterBeamCrossing",manager,inputs,outputs,1.434e+02,1.118e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeamCrossingSeq";
    cfg.order   = 18;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","/Event/Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiCountBeamCrossing",manager,inputs,outputs,1.459e+03,1.686e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeamCrossingSeq";
    cfg.order   = 19;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FilterBeam1",manager,inputs,outputs,6.743e+01,2.167e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam1Seq";
    cfg.order   = 21;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Hlt/LumiSummary",0};
    AlgConfig cfg("LumiDecodeBeam1",manager,inputs,outputs,3.330e+02,5.400e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam1Seq";
    cfg.order   = 22;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiFilterBeam1",manager,inputs,outputs,1.675e+02,1.750e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam1Seq";
    cfg.order   = 23;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent","/Event/Hlt/LumiSummary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("LumiCountBeam1",manager,inputs,outputs,5.345e+02,2.450e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam1Seq";
    cfg.order   = 24;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FilterBeam2",manager,inputs,outputs,6.450e+01,2.038e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiBeam2Seq";
    cfg.order   = 26;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
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
    cfg.order   = 27;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
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
    cfg.order   = 28;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
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
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TimeSpanFilter",manager,inputs,outputs,6.160e+01,2.036e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "TimeSpanSeq";
    cfg.order   = 31;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TimeSpanAccounting",manager,inputs,outputs,1.871e+02,9.779e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "TimeSpanSeq";
    cfg.order   = 32;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/Hlt/LumiSummary","/Event/Rec/Header","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EventAccount",manager,inputs,outputs,1.747e+01,6.339e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiCounters";
    cfg.order   = 33;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
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
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PhysFilter",manager,inputs,outputs,1.382e+02,4.878e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "LumiSeq";
    cfg.order   = 35;
    cfg.level   = 3;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Hlt/DecReports",0};
    AlgConfig cfg("HltDecReportsDecoder",manager,inputs,outputs,9.084e+04,4.375e+05);
    cfg.setDaughters(daughters);
    cfg.parent  = "HltfilterSeq";
    cfg.order   = 39;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Hlt/DecReports",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HltErrorFilter",manager,inputs,outputs,1.678e+02,2.235e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "HltfilterSeq";
    cfg.order   = 40;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
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
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PrsCheckBanks",manager,inputs,outputs,9.924e+01,4.663e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "PrsBanksHandler";
    cfg.order   = 45;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/DAQ/Status","/Event/Raw/Prs/Adcs",0};
    AlgConfig cfg("PrsADCs",manager,inputs,outputs,1.713e+03,1.300e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "PrsBanksHandler";
    cfg.order   = 46;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Trig/L0/PrsRaw",0};
    AlgConfig cfg("PrsL0Bit",manager,inputs,outputs,1.214e+03,2.356e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PrsBanksHandler";
    cfg.order   = 47;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Trig/L0/SpdRaw",0};
    AlgConfig cfg("SpdL0Bit",manager,inputs,outputs,1.203e+03,1.795e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PrsBanksHandler";
    cfg.order   = 48;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Raw/Prs/Adcs","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PrsSpdFillRawBuffer",manager,inputs,outputs,6.891e+02,5.323e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PrsBanksHandler";
    cfg.order   = 49;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RemovePrsPackedBanks",manager,inputs,outputs,2.521e+02,3.317e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PrsBanksHandler";
    cfg.order   = 50;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalCheckBanks",manager,inputs,outputs,9.100e+01,3.913e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "EcalBanksHandler";
    cfg.order   = 52;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Ecal/Adcs",0};
    AlgConfig cfg("EcalADCs",manager,inputs,outputs,3.847e+03,3.547e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "EcalBanksHandler";
    cfg.order   = 53;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Trig/L0/EcalRaw",0};
    AlgConfig cfg("EcalTrigFromRaw",manager,inputs,outputs,1.556e+03,3.023e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "EcalBanksHandler";
    cfg.order   = 54;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Raw/Ecal/Adcs","/Event/Trig/L0/EcalRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalFillRawBuffer",manager,inputs,outputs,2.009e+02,2.254e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "EcalBanksHandler";
    cfg.order   = 55;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RemoveEcalPackedBanks",manager,inputs,outputs,3.513e+02,3.749e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "EcalBanksHandler";
    cfg.order   = 56;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HcalCheckBanks",manager,inputs,outputs,8.724e+01,3.297e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "HcalBanksHandler";
    cfg.order   = 58;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Hcal/Adcs",0};
    AlgConfig cfg("HcalADCs",manager,inputs,outputs,1.191e+03,1.234e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "HcalBanksHandler";
    cfg.order   = 59;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Trig/L0/HcalRaw",0};
    AlgConfig cfg("HcalTrigFromRaw",manager,inputs,outputs,5.519e+02,8.423e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "HcalBanksHandler";
    cfg.order   = 60;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Raw/Hcal/Adcs","/Event/Trig/L0/HcalRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HcalFillRawBuffer",manager,inputs,outputs,1.541e+02,9.349e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "HcalBanksHandler";
    cfg.order   = 61;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RemoveHcalPackedBanks",manager,inputs,outputs,1.433e+02,2.324e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "HcalBanksHandler";
    cfg.order   = 62;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters",0};
    AlgConfig cfg("DecodeVeloClusters",manager,inputs,outputs,1.079e+03,1.369e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoDecodingSeq";
    cfg.order   = 65;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/TT/Clusters","/Event/Rec/TT/Summary",0};
    AlgConfig cfg("CreateTTClusters",manager,inputs,outputs,1.292e+03,1.721e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoDecodingSeq";
    cfg.order   = 66;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/TT/LiteClusters",0};
    AlgConfig cfg("CreateTTLiteClusters",manager,inputs,outputs,6.230e+02,3.776e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoDecodingSeq";
    cfg.order   = 67;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/IT/Clusters","/Event/Rec/IT/Summary",0};
    AlgConfig cfg("CreateITClusters",manager,inputs,outputs,1.092e+03,1.632e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoDecodingSeq";
    cfg.order   = 68;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/Raw/IT/LiteClusters",0};
    AlgConfig cfg("CreateITLiteClusters",manager,inputs,outputs,5.585e+02,3.585e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoDecodingSeq";
    cfg.order   = 69;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Velo/LiteClusters",0};
    const char* outputs[] = {"/Event/Rec/Track/Velo",0};
    AlgConfig cfg("FastVeloTracking",manager,inputs,outputs,1.556e+03,3.353e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoVELOSeq";
    cfg.order   = 71;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/Raw/IT/LiteClusters","/Event/Raw/TT/LiteClusters","/Event/Rec/Track/Velo",0};
    const char* outputs[] = {"/Event/Rec/Track/Forward",0};
    AlgConfig cfg("PatForward",manager,inputs,outputs,6.233e+03,1.169e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackForwardPatSeq";
    cfg.order   = 78;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Track/Seed",0};
    AlgConfig cfg("PatSeeding",manager,inputs,outputs,3.314e+03,6.407e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackSeedPatSeq";
    cfg.order   = 80;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Seed","/Event/Rec/Track/Velo",0};
    const char* outputs[] = {"/Event/Rec/Track/Match",0};
    AlgConfig cfg("PatMatch",manager,inputs,outputs,5.034e+02,8.538e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackMatchPatSeq";
    cfg.order   = 82;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/TT/LiteClusters","/Event/Rec/Track/Match","/Event/Rec/Track/Seed",0};
    const char* outputs[] = {"/Event/Rec/Track/Downstream",0};
    AlgConfig cfg("PatDownstream",manager,inputs,outputs,1.589e+03,2.748e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackDownstreamPatSeq";
    cfg.order   = 84;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/TT/Clusters","/Event/Raw/Velo/Clusters","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Velo",0};
    const char* outputs[] = {"/Event/Rec/Track/VeloTT",0};
    AlgConfig cfg("PatVeloTT",manager,inputs,outputs,8.719e+03,2.808e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackVeloTTPatSeq";
    cfg.order   = 86;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Seed","/Event/Rec/Track/VeloTT",0};
    const char* outputs[] = {"/Event/Rec/Track/AllBest",0};
    AlgConfig cfg("TrackEventCloneKiller",manager,inputs,outputs,5.704e+02,7.621e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackFitSeq";
    cfg.order   = 88;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/IT/LiteClusters","/Event/Raw/TT/LiteClusters","/Event/Rec/Track/AllBest",0};
    const char* outputs[] = {0};
    AlgConfig cfg("InitBestFit",manager,inputs,outputs,2.411e+03,6.362e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackFitSeq";
    cfg.order   = 89;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/IT/Clusters","/Event/Raw/TT/Clusters","/Event/Raw/Velo/Clusters","/Event/Rec/Track/AllBest",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FitBest",manager,inputs,outputs,2.189e+05,6.762e+05);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackFitSeq";
    cfg.order   = 90;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/AllBest",0};
    const char* outputs[] = {"/Event/Rec/Track/Best",0};
    AlgConfig cfg("CopyBest",manager,inputs,outputs,9.451e+02,1.829e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackFitSeq";
    cfg.order   = 91;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","/Event/Rec/Track/Velo",0};
    const char* outputs[] = {"/Event/Rec/Track/PreparedVelo",0};
    AlgConfig cfg("TrackPrepareVelo",manager,inputs,outputs,2.124e+02,2.225e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackVeloFitSeq";
    cfg.order   = 93;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Velo/Clusters","/Event/Rec/Track/PreparedVelo",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FitVelo",manager,inputs,outputs,1.504e+04,4.632e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackVeloFitSeq";
    cfg.order   = 94;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","/Event/Rec/Track/PreparedVelo",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CopyVelo",manager,inputs,outputs,2.390e+02,4.757e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackVeloFitSeq";
    cfg.order   = 95;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Link/Rec/Track/BestClones",0};
    AlgConfig cfg("FindTrackClones",manager,inputs,outputs,2.998e+03,7.902e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackClonesSeq";
    cfg.order   = 98;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Link/Rec/Track/BestClones","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FlagTrackClones",manager,inputs,outputs,8.492e+02,2.423e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackClonesSeq";
    cfg.order   = 99;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackAddLikelihood",manager,inputs,outputs,1.051e+04,3.085e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackAddExtraInfoSeq";
    cfg.order   = 100;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/TT/Clusters","/Event/Raw/Velo/Clusters","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackAddNNGhostId",manager,inputs,outputs,2.035e+03,7.107e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackAddExtraInfoSeq";
    cfg.order   = 101;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackEraseExtraInfo",manager,inputs,outputs,9.944e+01,1.214e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "TrackEraseExtraInformationSeq";
    cfg.order   = 103;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/Weights",0};
    AlgConfig cfg("PatPVOffline",manager,inputs,outputs,2.644e+03,6.931e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoVertexSeq";
    cfg.order   = 105;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","/Event/Rec/Vertex/Primary",0};
    const char* outputs[] = {"/Event/Rec/Vertex/V0",0};
    AlgConfig cfg("TrackV0Finder",manager,inputs,outputs,1.255e+03,4.317e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoVertexSeq";
    cfg.order   = 106;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Rich/RecoEvent/Offline/Status",0};
    AlgConfig cfg("CheckProcOfflineStatus",manager,inputs,outputs,2.443e+02,3.395e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecInitOfflineSeq";
    cfg.order   = 109;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("DecodeRawRichOffline",manager,inputs,outputs,3.479e+03,7.834e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflinePixelsSeq";
    cfg.order   = 111;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Rich/RecoEvent/Offline/Pixels",0};
    AlgConfig cfg("CreateOfflinePixels",manager,inputs,outputs,6.158e+03,1.207e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflinePixelsSeq";
    cfg.order   = 112;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Tracks",0};
    AlgConfig cfg("CreateOfflineTracks",manager,inputs,outputs,2.221e+04,5.934e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineTracksSeq";
    cfg.order   = 114;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Rich/RecoEvent/Offline/Photons",0};
    AlgConfig cfg("CreateOfflinePhotons",manager,inputs,outputs,2.618e+04,8.394e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflinePhotonsSeq";
    cfg.order   = 116;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Hcal/Adcs","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Prs/Adcs","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll",0};
    AlgConfig cfg("OfflineENNFinderR1GasTop",manager,inputs,outputs,1.216e+03,2.649e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 118;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OfflineENNFinderR1GasBottom",manager,inputs,outputs,3.791e+02,8.711e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 119;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OfflineENNFinderR2GasLeft",manager,inputs,outputs,1.035e+03,3.176e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 120;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OfflineENNFinderR2GasRight",manager,inputs,outputs,8.479e+02,3.069e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 121;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {"/Event/Rec/Rich/RecoEvent/Offline/Segments",0};
    AlgConfig cfg("OfflineENNRingsSegAssoc",manager,inputs,outputs,6.314e+02,1.758e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 122;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsBest",0};
    AlgConfig cfg("OfflineBestENNRings",manager,inputs,outputs,5.424e+02,1.856e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 123;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsBest",0};
    const char* outputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsIsolated",0};
    AlgConfig cfg("OfflineIsolatedENNRings",manager,inputs,outputs,7.350e+02,2.338e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineENNRingFinderSeq";
    cfg.order   = 124;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Hcal/Adcs","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Prs/Adcs","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDInit",manager,inputs,outputs,2.112e+01,2.964e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDInitSeq";
    cfg.order   = 128;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDDigSel",manager,inputs,outputs,8.752e+01,1.495e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDInitSeq";
    cfg.order   = 129;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks",0};
    AlgConfig cfg("RichOfflineGPIDTrSel",manager,inputs,outputs,6.029e+02,1.205e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDInitSeq";
    cfg.order   = 130;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Hcal/Adcs","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Prs/Adcs","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineBckEstIt0",manager,inputs,outputs,6.074e+02,1.718e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDLLSeq";
    cfg.order   = 132;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDLLIt0",manager,inputs,outputs,1.914e+04,5.880e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDLLSeq";
    cfg.order   = 133;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Hcal/Adcs","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Prs/Adcs","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineBckEstIt1",manager,inputs,outputs,4.147e+02,1.249e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDLLSeq";
    cfg.order   = 134;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDLLIt1",manager,inputs,outputs,1.477e+03,4.882e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDLLSeq";
    cfg.order   = 135;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/GlobalPID/Offline/Tracks",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichOfflineGPIDFin",manager,inputs,outputs,8.592e+01,2.676e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflineGPIDSeq";
    cfg.order   = 136;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Status",0};
    const char* outputs[] = {"/Event/Rec/Rich/PIDs",0};
    AlgConfig cfg("MergeOfflineRichPIDs",manager,inputs,outputs,3.016e+02,4.375e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichOfflinePIDSeq";
    cfg.order   = 137;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Spd/Digits",0};
    AlgConfig cfg("SpdFromRaw",manager,inputs,outputs,3.545e+02,4.454e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigits";
    cfg.order   = 141;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Prs/Digits",0};
    AlgConfig cfg("PrsFromRaw",manager,inputs,outputs,3.159e+02,2.821e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigits";
    cfg.order   = 142;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Ecal/Digits",0};
    AlgConfig cfg("EcalZSup",manager,inputs,outputs,2.776e+02,1.723e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigits";
    cfg.order   = 143;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Hcal/Digits",0};
    AlgConfig cfg("HcalZSup",manager,inputs,outputs,2.297e+02,5.198e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigits";
    cfg.order   = 144;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloDigitFilter",manager,inputs,outputs,1.417e+04,6.719e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ClusterReco";
    cfg.order   = 146;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Ecal/Digits",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalClusters",0};
    AlgConfig cfg("EcalClust",manager,inputs,outputs,1.792e+03,3.730e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ClusterReco";
    cfg.order   = 147;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalShare",manager,inputs,outputs,8.154e+02,1.761e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ClusterReco";
    cfg.order   = 148;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalCovar",manager,inputs,outputs,3.356e+02,7.154e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ClusterReco";
    cfg.order   = 149;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/InAccEcal",0};
    AlgConfig cfg("InECAL",manager,inputs,outputs,9.818e+02,2.149e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloTrackMatch";
    cfg.order   = 152;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/InAccEcal","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/ClusterMatch",0};
    AlgConfig cfg("ClusterMatch",manager,inputs,outputs,1.736e+03,3.551e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloTrackMatch";
    cfg.order   = 153;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalClusters",0};
    const char* outputs[] = {"/Event/Rec/Calo/Photons",0};
    AlgConfig cfg("SinglePhotonRec",manager,inputs,outputs,5.821e+03,1.073e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "PhotonReco";
    cfg.order   = 154;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/EcalClusters",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/SplitPhotons",0};
    AlgConfig cfg("MergedPi0Rec",manager,inputs,outputs,2.795e+02,9.938e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MergedPi0Reco";
    cfg.order   = 156;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalClusters",0};
    const char* outputs[] = {"/Event/Rec/Calo/Electrons",0};
    AlgConfig cfg("SingleElectronRec",manager,inputs,outputs,8.906e+02,2.332e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ElectronReco";
    cfg.order   = 158;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/InAccSpd",0};
    AlgConfig cfg("InSPD",manager,inputs,outputs,3.024e+02,4.282e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "InCaloAcceptance";
    cfg.order   = 163;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/InAccPrs",0};
    AlgConfig cfg("InPRS",manager,inputs,outputs,2.603e+02,3.646e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "InCaloAcceptance";
    cfg.order   = 164;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/InAccHcal",0};
    AlgConfig cfg("InHCAL",manager,inputs,outputs,2.479e+02,3.519e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "InCaloAcceptance";
    cfg.order   = 165;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/InAccBrem",0};
    AlgConfig cfg("InBREM",manager,inputs,outputs,1.965e+02,1.872e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "InCaloAcceptance";
    cfg.order   = 166;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/Electrons","/Event/Rec/Calo/InAccEcal","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/ElectronMatch",0};
    AlgConfig cfg("ElectronMatch",manager,inputs,outputs,1.310e+03,3.437e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloMatch";
    cfg.order   = 168;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/Photons","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/BremMatch",0};
    AlgConfig cfg("BremMatch",manager,inputs,outputs,9.300e+02,1.951e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloMatch";
    cfg.order   = 169;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Spd/Digits","/Event/Rec/Calo/InAccSpd","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/SpdE",0};
    AlgConfig cfg("SpdE",manager,inputs,outputs,8.072e+02,1.815e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloEnergy";
    cfg.order   = 171;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Rec/Calo/InAccPrs","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/PrsE",0};
    AlgConfig cfg("PrsE",manager,inputs,outputs,2.499e+02,2.263e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloEnergy";
    cfg.order   = 172;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Ecal/Digits","/Event/Rec/Calo/InAccEcal","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalE",0};
    AlgConfig cfg("EcalE",manager,inputs,outputs,3.434e+02,4.759e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloEnergy";
    cfg.order   = 173;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Rec/Calo/InAccHcal","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/HcalE",0};
    AlgConfig cfg("HcalE",manager,inputs,outputs,3.449e+02,5.192e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloEnergy";
    cfg.order   = 174;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/ElectronMatch","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalChi2",0};
    AlgConfig cfg("EcalChi22ID",manager,inputs,outputs,3.936e+02,4.857e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloChi2";
    cfg.order   = 176;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/BremMatch","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/BremChi2",0};
    AlgConfig cfg("BremChi22ID",manager,inputs,outputs,2.015e+02,4.456e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloChi2";
    cfg.order   = 177;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/ClusterMatch","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Calo/ClusChi2",0};
    AlgConfig cfg("ClusChi22ID",manager,inputs,outputs,3.367e+02,3.874e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloChi2";
    cfg.order   = 178;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/PrsE",0};
    const char* outputs[] = {"/Event/Rec/Calo/PrsPIDe",0};
    AlgConfig cfg("PrsPIDe",manager,inputs,outputs,2.113e+02,2.342e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLe";
    cfg.order   = 180;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalChi2",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalPIDe",0};
    AlgConfig cfg("EcalPIDe",manager,inputs,outputs,1.435e+02,3.139e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLe";
    cfg.order   = 181;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/BremChi2",0};
    const char* outputs[] = {"/Event/Rec/Calo/BremPIDe",0};
    AlgConfig cfg("BremPIDe",manager,inputs,outputs,1.358e+02,2.686e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLe";
    cfg.order   = 182;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/HcalE",0};
    const char* outputs[] = {"/Event/Rec/Calo/HcalPIDe",0};
    AlgConfig cfg("HcalPIDe",manager,inputs,outputs,1.354e+02,2.438e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLe";
    cfg.order   = 183;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalE",0};
    const char* outputs[] = {"/Event/Rec/Calo/EcalPIDmu",0};
    AlgConfig cfg("EcalPIDmu",manager,inputs,outputs,1.347e+02,2.160e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLmu";
    cfg.order   = 185;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/HcalE",0};
    const char* outputs[] = {"/Event/Rec/Calo/HcalPIDmu",0};
    AlgConfig cfg("HcalPIDmu",manager,inputs,outputs,1.391e+02,2.599e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDLLmu";
    cfg.order   = 186;
    cfg.level   = 9;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Photons",0};
    const char* outputs[] = {"/Event/Rec/Calo/PhotonID",0};
    AlgConfig cfg("PhotonID",manager,inputs,outputs,2.242e+03,4.614e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "NeutralPIDs";
    cfg.order   = 188;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/MergedPi0s",0};
    const char* outputs[] = {"/Event/Rec/Calo/MergedID",0};
    AlgConfig cfg("MergedID",manager,inputs,outputs,1.415e+02,3.708e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "NeutralPIDs";
    cfg.order   = 189;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/SplitPhotons",0};
    const char* outputs[] = {"/Event/Rec/Calo/PhotonFromMergedID",0};
    AlgConfig cfg("PhotonFromMergedID",manager,inputs,outputs,1.357e+02,1.910e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "NeutralPIDs";
    cfg.order   = 190;
    cfg.level   = 8;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent","/Event/DAQ/Status",0};
    const char* outputs[] = {"/Event/Raw/Muon/Coords",0};
    AlgConfig cfg("MuonRec",manager,inputs,outputs,1.143e+03,1.580e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoMUONSeq";
    cfg.order   = 192;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Muon/Coords","/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/Muon/MuonPID","/Event/Rec/Track/Muon",0};
    AlgConfig cfg("MuonIDAlg",manager,inputs,outputs,6.832e+03,2.078e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "MuonIDSeq";
    cfg.order   = 194;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/Rec/ProtoP/Charged",0};
    AlgConfig cfg("ChargedProtoPMaker",manager,inputs,outputs,6.710e+02,1.443e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 197;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddEcal",manager,inputs,outputs,1.629e+03,2.742e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 198;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddBrem",manager,inputs,outputs,1.708e+03,4.206e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 199;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccHcal","/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddHcal",manager,inputs,outputs,9.738e+02,3.026e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 200;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddPrs",manager,inputs,outputs,2.934e+02,8.477e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 201;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/SpdE","/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddSpd",manager,inputs,outputs,2.303e+02,7.247e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 202;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddVeloDEDX",manager,inputs,outputs,2.759e+02,7.438e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 203;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","/Event/Rec/Rich/PIDs",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddRich",manager,inputs,outputs,5.336e+02,1.069e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 204;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPAddMuon",manager,inputs,outputs,4.607e+02,9.384e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 205;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPCombDLLs",manager,inputs,outputs,1.411e+02,1.753e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ChargedProtoParticles";
    cfg.order   = 206;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongElectron",manager,inputs,outputs,4.347e+02,1.083e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 208;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongMuon",manager,inputs,outputs,1.001e+02,1.428e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 209;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongPion",manager,inputs,outputs,2.596e+02,6.632e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 210;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongKaon",manager,inputs,outputs,2.568e+02,6.512e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 211;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongProton",manager,inputs,outputs,2.669e+02,6.820e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 212;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDLongGhost",manager,inputs,outputs,2.461e+02,6.234e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 213;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamElectron",manager,inputs,outputs,1.152e+02,2.198e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 214;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamMuon",manager,inputs,outputs,5.648e+01,2.702e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 215;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamPion",manager,inputs,outputs,1.154e+02,2.229e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 216;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamKaon",manager,inputs,outputs,1.174e+02,2.372e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 217;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamProton",manager,inputs,outputs,1.148e+02,2.309e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 218;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDDownstreamGhost",manager,inputs,outputs,1.142e+02,2.274e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 219;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamElectron",manager,inputs,outputs,9.380e+01,1.512e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 220;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamMuon",manager,inputs,outputs,8.836e+01,1.362e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 221;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamPion",manager,inputs,outputs,8.544e+01,1.254e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 222;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamKaon",manager,inputs,outputs,9.684e+01,1.674e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 223;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamProton",manager,inputs,outputs,8.600e+01,1.298e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 224;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ANNGPIDUpstreamGhost",manager,inputs,outputs,8.828e+01,1.363e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "ANNGPIDSeq";
    cfg.order   = 225;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Hcal/Digits","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/SplitPhotons",0};
    const char* outputs[] = {"/Event/Rec/ProtoP/Neutrals",0};
    AlgConfig cfg("NeutralProtoPMaker",manager,inputs,outputs,2.428e+03,5.082e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "NeutralProtoParticles";
    cfg.order   = 227;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/IT/Clusters","/Event/Raw/Muon/Coords","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/Velo/Clusters","/Event/Rec/Track/Best","/Event/Rec/Track/Muon","/Event/Rec/Vertex/Primary",0};
    const char* outputs[] = {"/Event/Rec/Summary",0};
    AlgConfig cfg("RecSummary",manager,inputs,outputs,1.090e+03,1.315e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RecoSUMMARYSeq";
    cfg.order   = 229;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OverallEventProcTime",manager,inputs,outputs,4.056e+01,4.331e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 232;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackEventProcTime",manager,inputs,outputs,3.108e+01,1.762e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 233;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("VertexEventProcTime",manager,inputs,outputs,2.336e+01,8.795e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 234;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichEventProcTime",manager,inputs,outputs,2.432e+01,9.005e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 235;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloEventProcTime",manager,inputs,outputs,2.156e+01,8.400e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 236;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonEventProcTime",manager,inputs,outputs,2.200e+01,9.968e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 237;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ProtoEventProcTime",manager,inputs,outputs,2.268e+01,8.479e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 238;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ProcStatAbortMoni",manager,inputs,outputs,6.168e+01,3.564e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniGENERALSeq";
    cfg.order   = 239;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Ecal/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalDigitMon",manager,inputs,outputs,6.062e+02,1.446e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigitsMoni";
    cfg.order   = 242;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Hcal/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HcalDigitMon",manager,inputs,outputs,6.350e+02,6.629e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigitsMoni";
    cfg.order   = 243;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PrsDigitMon",manager,inputs,outputs,4.098e+02,6.637e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigitsMoni";
    cfg.order   = 244;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Spd/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("SpdDigitMon",manager,inputs,outputs,2.187e+02,4.957e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigitsMoni";
    cfg.order   = 245;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Spd/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("SpdMon",manager,inputs,outputs,6.993e+02,1.798e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloDigitsMoni";
    cfg.order   = 246;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Ecal/Digits",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalEFlowMon",manager,inputs,outputs,1.581e+02,2.200e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloEFlowMoni";
    cfg.order   = 248;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("EcalClusterMon",manager,inputs,outputs,2.298e+02,2.908e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloClustersMoni";
    cfg.order   = 250;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/Electrons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ElectronMon",manager,inputs,outputs,1.414e+02,2.034e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloHyposMoni";
    cfg.order   = 252;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/Photons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PhotonMon",manager,inputs,outputs,2.514e+02,4.606e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloHyposMoni";
    cfg.order   = 253;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/SplitPhotons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("SplitPhotonMon",manager,inputs,outputs,7.512e+01,1.378e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloHyposMoni";
    cfg.order   = 254;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/MergedPi0s",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MergedPi0Mon",manager,inputs,outputs,7.636e+01,2.079e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloHyposMoni";
    cfg.order   = 255;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Rec/Calo/Photons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ResolvedPi0Mon",manager,inputs,outputs,5.751e+02,1.024e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniCALOSeq";
    cfg.order   = 256;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ProtoElectronMon",manager,inputs,outputs,2.490e+02,3.675e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniCALOSeq";
    cfg.order   = 257;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalSplitClusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("PhotonMatchMon",manager,inputs,outputs,3.091e+02,3.799e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 259;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ElectronMatchMon",manager,inputs,outputs,2.728e+02,4.834e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 260;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/Photons",0};
    const char* outputs[] = {0};
    AlgConfig cfg("BremMatchMon",manager,inputs,outputs,1.456e+02,6.426e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 261;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloEMuPIDMonUncut",manager,inputs,outputs,1.499e+02,1.734e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 262;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloEMuPIDMonSoft",manager,inputs,outputs,8.312e+01,2.483e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 263;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("CaloEMuPIDMonHard",manager,inputs,outputs,7.680e+01,1.776e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPIDsMoni";
    cfg.order   = 264;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRawDataSize",manager,inputs,outputs,3.014e+03,2.898e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawMoni";
    cfg.order   = 267;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Hlt/DecReports",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichDataSizeL0Filter",manager,inputs,outputs,9.056e+01,6.955e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawDataSizeL0Seq";
    cfg.order   = 269;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRawDataSizeL0",manager,inputs,outputs,2.208e+03,1.791e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawDataSizeL0Seq";
    cfg.order   = 270;
    cfg.level   = 7;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRawDataDBCheck",manager,inputs,outputs,9.620e+02,2.333e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawMoni";
    cfg.order   = 271;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichHotPixels",manager,inputs,outputs,4.198e+02,8.934e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawMoni";
    cfg.order   = 272;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichDecodingErrors",manager,inputs,outputs,2.203e+03,1.114e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRawMoni";
    cfg.order   = 273;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRecPixelQC",manager,inputs,outputs,4.034e+02,9.988e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPixelMoni";
    cfg.order   = 275;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichRecPixelClusters",manager,inputs,outputs,3.394e+02,9.012e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPixelMoni";
    cfg.order   = 276;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiLongTrkEff",manager,inputs,outputs,1.806e+02,5.069e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 278;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiForwardTrkEff",manager,inputs,outputs,7.704e+01,1.599e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 279;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiMatchTrkEff",manager,inputs,outputs,3.384e+01,4.311e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 280;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiKsTrackTrkEff",manager,inputs,outputs,4.396e+01,7.167e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 281;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiVeloTTTrkEff",manager,inputs,outputs,4.336e+01,6.991e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 282;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiSeedTrkEff",manager,inputs,outputs,4.904e+01,8.138e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichTrackMoni";
    cfg.order   = 283;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResLong",manager,inputs,outputs,1.534e+03,4.960e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 285;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResForward",manager,inputs,outputs,5.508e+02,1.653e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 286;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResMatch",manager,inputs,outputs,4.584e+01,8.715e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 287;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResKsTrack",manager,inputs,outputs,1.566e+02,4.354e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 288;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResVeloTT",manager,inputs,outputs,4.816e+01,8.645e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 289;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResSeed",manager,inputs,outputs,2.832e+02,8.471e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoni";
    cfg.order   = 290;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResLongTight",manager,inputs,outputs,4.810e+02,1.599e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 292;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResForwardTight",manager,inputs,outputs,4.387e+02,1.428e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 293;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResMatchTight",manager,inputs,outputs,4.388e+01,8.310e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 294;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResKsTrackTight",manager,inputs,outputs,1.150e+02,2.710e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 295;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResVeloTTTight",manager,inputs,outputs,4.460e+01,8.383e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 296;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Status",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiCKResSeedTight",manager,inputs,outputs,2.732e+02,8.382e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichRecoMoniTight";
    cfg.order   = 297;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/PIDs","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiPIDMonLong2To100",manager,inputs,outputs,8.738e+02,3.119e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPIDMoni";
    cfg.order   = 299;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/PIDs","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiPIDMonLong2To10",manager,inputs,outputs,1.179e+02,2.117e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPIDMoni";
    cfg.order   = 300;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/PIDs","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiPIDMonLong10To70",manager,inputs,outputs,1.103e+02,1.770e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPIDMoni";
    cfg.order   = 301;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/PIDs","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RiPIDMonLong70To100",manager,inputs,outputs,6.864e+01,7.244e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichPIDMoni";
    cfg.order   = 302;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsAll",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ENNRingMoniAll",manager,inputs,outputs,1.392e+02,4.358e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichENNTracklessRingsMoni";
    cfg.order   = 304;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsBest",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ENNRingMoniBest",manager,inputs,outputs,4.600e+01,7.446e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichENNTracklessRingsMoni";
    cfg.order   = 305;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/ENN/Offline/RingsIsolated",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ENNRingMoniIsolated",manager,inputs,outputs,3.956e+01,7.192e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichENNTracklessRingsMoni";
    cfg.order   = 306;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichAlignMoniR1Gas",manager,inputs,outputs,6.288e+02,1.589e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichMirrAlignMoni";
    cfg.order   = 308;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichAlignMoniR2Gas",manager,inputs,outputs,3.044e+02,8.558e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichMirrAlignMoni";
    cfg.order   = 309;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("RichHPDIFBMoni",manager,inputs,outputs,1.465e+03,2.610e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "RichHPDIonFeedback";
    cfg.order   = 311;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Muon/MuonPID","/Event/Rec/Track/Best","/Event/Rec/Track/Muon",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonPIDMonitorDown",manager,inputs,outputs,2.211e+02,2.009e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniMUONSeq";
    cfg.order   = 313;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Muon/MuonPID","/Event/Rec/Track/Best","/Event/Rec/Track/Muon",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonPIDMonitorLong",manager,inputs,outputs,3.913e+02,9.156e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniMUONSeq";
    cfg.order   = 314;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Muon",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonTrackFitter",manager,inputs,outputs,6.071e+04,2.661e+05);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniMUONSeq";
    cfg.order   = 315;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Muon/MuonPID",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuonTrackAligMonitor",manager,inputs,outputs,1.505e+03,4.451e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniMUONSeq";
    cfg.order   = 316;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Raw/Muon/Coords","/Event/Rec/ProtoP/Charged","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuEffMonitor",manager,inputs,outputs,1.571e+03,2.867e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniMUONSeq";
    cfg.order   = 317;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Velo/Clusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("VeloClusterMonitor",manager,inputs,outputs,3.984e+02,6.725e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniVELOSeq";
    cfg.order   = 319;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Velo/Clusters","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("VeloTrackMonitor",manager,inputs,outputs,2.273e+03,6.137e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniVELOSeq";
    cfg.order   = 320;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","/Event/Rec/Vertex/Primary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("VeloIPResolutionMonitor",manager,inputs,outputs,4.675e+03,1.660e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniVELOSeq";
    cfg.order   = 321;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackMonitor",manager,inputs,outputs,4.468e+03,1.117e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 323;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Muon",0};
    const char* outputs[] = {"/Event/Rec/Vertex/DiMuon",0};
    AlgConfig cfg("TrackDiMuonMonitor",manager,inputs,outputs,4.164e+02,8.051e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 324;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best","/Event/Rec/Vertex/Primary",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackVertexMonitor",manager,inputs,outputs,3.516e+03,9.346e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 325;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Vertex/V0",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackV0Monitor",manager,inputs,outputs,2.362e+02,4.668e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 326;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackFitMatchMonitor",manager,inputs,outputs,7.279e+02,1.902e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 327;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackAlignMonitor",manager,inputs,outputs,1.971e+03,5.225e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 328;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/IT/LiteClusters","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackITOverlapMonitor",manager,inputs,outputs,4.083e+03,1.142e+04);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 329;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackVeloOverlapMonitor",manager,inputs,outputs,8.582e+02,2.077e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 330;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/TT/Clusters","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TTTrackMonitor",manager,inputs,outputs,1.115e+03,2.873e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 331;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/IT/Clusters","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ITTrackMonitor",manager,inputs,outputs,1.070e+03,2.842e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 332;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackPV2HalfAlignMonitor",manager,inputs,outputs,1.557e+03,4.637e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 333;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/EcalClusters","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackEcalMatchMonitor",manager,inputs,outputs,7.638e+02,1.669e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 334;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Spd/Digits","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackSpdMatchMonitor",manager,inputs,outputs,7.220e+02,1.702e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 335;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Prs/Digits","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackPrsMatchMonitor",manager,inputs,outputs,6.617e+02,1.752e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 336;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Raw/Muon/Coords","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackMuonMatchMonitor",manager,inputs,outputs,4.842e+02,1.124e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniTrSeq";
    cfg.order   = 337;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/DiMuon","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OTTimeMonitor",manager,inputs,outputs,5.722e+02,1.036e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniOTSeq";
    cfg.order   = 339;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OTTrackMonitor",manager,inputs,outputs,2.702e+03,6.036e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniOTSeq";
    cfg.order   = 340;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("OTHitEfficiencyMonitor",manager,inputs,outputs,3.267e+03,7.323e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniOTSeq";
    cfg.order   = 341;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Raw/TT/Clusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TTClusterMonitor",manager,inputs,outputs,4.610e+02,5.827e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniSTSeq";
    cfg.order   = 343;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Raw/IT/Clusters",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ITClusterMonitor",manager,inputs,outputs,5.731e+02,1.383e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniSTSeq";
    cfg.order   = 344;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged","/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ChargedProtoPMoni",manager,inputs,outputs,5.442e+02,1.270e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniPROTOSeq";
    cfg.order   = 346;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Hlt/DecReports",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HltRateMonitor",manager,inputs,outputs,4.299e+02,1.663e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniHltSeq";
    cfg.order   = 348;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Hlt/DecReports",0};
    const char* outputs[] = {0};
    AlgConfig cfg("HltCompositionMonitor",manager,inputs,outputs,2.952e+02,1.000e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniHltSeq";
    cfg.order   = 349;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/ODIN","/Event/Hlt/DecReports","/Event/Rec/Track/Velo",0};
    const char* outputs[] = {0};
    AlgConfig cfg("MuMonitor",manager,inputs,outputs,2.305e+02,1.449e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "MoniHltSeq";
    cfg.order   = 350;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {0};
    AlgConfig cfg("TrackToDST",manager,inputs,outputs,1.270e+02,2.277e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "OutputDSTSeq";
    cfg.order   = 353;
    cfg.level   = 4;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Best",0};
    const char* outputs[] = {"/Event/pRec/Track/Best",0};
    AlgConfig cfg("PackTracks",manager,inputs,outputs,5.537e+02,1.183e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 355;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Rich/PIDs",0};
    const char* outputs[] = {"/Event/pRec/Rich/PIDs",0};
    AlgConfig cfg("PackRichPIDs",manager,inputs,outputs,4.257e+02,8.134e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 356;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Muon/MuonPID",0};
    const char* outputs[] = {"/Event/pRec/Muon/MuonPID",0};
    AlgConfig cfg("PackMuonPIDs",manager,inputs,outputs,3.811e+02,7.560e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 357;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/Photons",0};
    const char* outputs[] = {"/Event/pRec/Calo/Photons",0};
    AlgConfig cfg("PackPhotons",manager,inputs,outputs,3.937e+02,8.714e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPacking";
    cfg.order   = 359;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/Electrons",0};
    const char* outputs[] = {"/Event/pRec/Calo/Electrons",0};
    AlgConfig cfg("PackElectrons",manager,inputs,outputs,1.088e+02,3.695e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPacking";
    cfg.order   = 360;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/MergedPi0s",0};
    const char* outputs[] = {"/Event/pRec/Calo/MergedPi0s",0};
    AlgConfig cfg("PackMergedPi0s",manager,inputs,outputs,9.980e+01,2.595e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPacking";
    cfg.order   = 361;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Calo/SplitPhotons",0};
    const char* outputs[] = {"/Event/pRec/Calo/SplitPhotons",0};
    AlgConfig cfg("PackSplitPhotons",manager,inputs,outputs,9.844e+01,2.919e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "CaloPacking";
    cfg.order   = 362;
    cfg.level   = 6;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {0};
    AlgConfig cfg("ProtoParticlePIDClean",manager,inputs,outputs,1.329e+02,1.543e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 363;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Charged",0};
    const char* outputs[] = {"/Event/pRec/ProtoP/Charged",0};
    AlgConfig cfg("PackChargedProtos",manager,inputs,outputs,2.487e+02,6.134e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 364;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/ProtoP/Neutrals",0};
    const char* outputs[] = {"/Event/pRec/ProtoP/Neutrals",0};
    AlgConfig cfg("PackNeutralProtos",manager,inputs,outputs,1.770e+02,3.327e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 365;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Vertex/Primary",0};
    const char* outputs[] = {"/Event/pRec/Vertex/Primary",0};
    AlgConfig cfg("PackRecVertex",manager,inputs,outputs,2.953e+02,6.560e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 366;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Vertex/Weights",0};
    const char* outputs[] = {"/Event/pRec/Vertex/Weights",0};
    AlgConfig cfg("PackPVWeights",manager,inputs,outputs,5.262e+02,1.242e+03);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 367;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Vertex/V0",0};
    const char* outputs[] = {"/Event/pRec/Vertex/V0",0};
    AlgConfig cfg("PackTwoProngVertex",manager,inputs,outputs,2.096e+02,5.045e+02);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 368;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Track/Muon",0};
    const char* outputs[] = {"/Event/pRec/Track/Muon",0};
    AlgConfig cfg("PackMuonTracks",manager,inputs,outputs,1.220e+02,5.745e+01);
    cfg.setDaughters(daughters);
    cfg.parent  = "PackDST";
    cfg.order   = 369;
    cfg.level   = 5;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/Rec/Header","/Event/Rec/Status","/Event/Rec/Summary","/Event/pRec/Calo/Electrons","/Event/pRec/Calo/MergedPi0s","/Event/pRec/Calo/Photons","/Event/pRec/Calo/SplitPhotons","/Event/pRec/Muon/MuonPID","/Event/pRec/ProtoP/Charged","/Event/pRec/ProtoP/Neutrals","/Event/pRec/Rich/PIDs","/Event/pRec/Track/Best","/Event/pRec/Track/Muon","/Event/pRec/Vertex/Primary","/Event/pRec/Vertex/V0","/Event/pRec/Vertex/Weights",0};
    const char* outputs[] = {0};
    AlgConfig cfg("DstWriter",manager,inputs,outputs,4.526e+04,2.228e+05);
    cfg.setDaughters(daughters);
    cfg.parent  = "Framework";
    cfg.order   = 370;
    cfg.level   = 1;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    // Input data estimated 
    const char* inputs[] = {"/Event/DAQ/Status","/Event/Hlt/DecReports","/Event/Hlt/LumiSummary","/Event/Link/Rec/Track/BestClones","/Event/Raw/Ecal/Adcs","/Event/Raw/Ecal/Digits","/Event/Raw/Hcal/Adcs","/Event/Raw/Hcal/Digits","/Event/Raw/IT/Clusters","/Event/Raw/IT/LiteClusters","/Event/Raw/Muon/Coords","/Event/Raw/Prs/Adcs","/Event/Raw/Prs/Digits","/Event/Raw/Spd/Digits","/Event/Raw/TT/Clusters","/Event/Raw/TT/LiteClusters","/Event/Raw/Velo/Clusters","/Event/Raw/Velo/LiteClusters","/Event/Rec/Calo/BremChi2","/Event/Rec/Calo/BremMatch","/Event/Rec/Calo/BremPIDe","/Event/Rec/Calo/ClusChi2","/Event/Rec/Calo/ClusterMatch","/Event/Rec/Calo/EcalChi2","/Event/Rec/Calo/EcalClusters","/Event/Rec/Calo/EcalE","/Event/Rec/Calo/EcalPIDe","/Event/Rec/Calo/EcalPIDmu","/Event/Rec/Calo/EcalSplitClusters","/Event/Rec/Calo/ElectronMatch","/Event/Rec/Calo/Electrons","/Event/Rec/Calo/HcalE","/Event/Rec/Calo/HcalPIDe","/Event/Rec/Calo/HcalPIDmu","/Event/Rec/Calo/InAccBrem","/Event/Rec/Calo/InAccEcal","/Event/Rec/Calo/InAccHcal","/Event/Rec/Calo/InAccPrs","/Event/Rec/Calo/InAccSpd","/Event/Rec/Calo/MergedID","/Event/Rec/Calo/MergedPi0s","/Event/Rec/Calo/PhotonFromMergedID","/Event/Rec/Calo/PhotonID","/Event/Rec/Calo/Photons","/Event/Rec/Calo/PrsE","/Event/Rec/Calo/PrsPIDe","/Event/Rec/Calo/SpdE","/Event/Rec/Calo/SplitPhotons","/Event/Rec/Header","/Event/Rec/IT/Summary","/Event/Rec/Muon/MuonPID","/Event/Rec/ProtoP/Charged","/Event/Rec/ProtoP/Neutrals","/Event/Rec/Rich/ENN/Offline/RingsAll","/Event/Rec/Rich/ENN/Offline/RingsBest","/Event/Rec/Rich/ENN/Offline/RingsIsolated","/Event/Rec/Rich/GlobalPID/Offline/PIDs","/Event/Rec/Rich/GlobalPID/Offline/Tracks","/Event/Rec/Rich/PIDs","/Event/Rec/Rich/RecoEvent/Offline/Photons","/Event/Rec/Rich/RecoEvent/Offline/Pixels","/Event/Rec/Rich/RecoEvent/Offline/SegmentHypoRings","/Event/Rec/Rich/RecoEvent/Offline/Segments","/Event/Rec/Rich/RecoEvent/Offline/Status","/Event/Rec/Rich/RecoEvent/Offline/Tracks","/Event/Rec/Status","/Event/Rec/Summary","/Event/Rec/TT/Summary","/Event/Rec/Track/AllBest","/Event/Rec/Track/Best","/Event/Rec/Track/Downstream","/Event/Rec/Track/Forward","/Event/Rec/Track/Match","/Event/Rec/Track/Muon","/Event/Rec/Track/PreparedVelo","/Event/Rec/Track/Seed","/Event/Rec/Track/Velo","/Event/Rec/Track/VeloTT","/Event/Rec/Vertex/DiMuon","/Event/Rec/Vertex/Primary","/Event/Rec/Vertex/V0","/Event/Rec/Vertex/Weights","/Event/Trig/L0/EcalRaw","/Event/Trig/L0/HcalRaw","/Event/Trig/L0/PrsRaw","/Event/Trig/L0/SpdRaw","/Event/pRec/Calo/Electrons","/Event/pRec/Calo/MergedPi0s","/Event/pRec/Calo/Photons","/Event/pRec/Calo/SplitPhotons","/Event/pRec/Muon/MuonPID","/Event/pRec/ProtoP/Charged","/Event/pRec/ProtoP/Neutrals","/Event/pRec/Rich/PIDs","/Event/pRec/Track/Best","/Event/pRec/Track/Muon","/Event/pRec/Vertex/Primary","/Event/pRec/Vertex/V0","/Event/pRec/Vertex/Weights",0};
    const char* outputs[] = {0};
    AlgConfig cfg("FSROutputStreamDstWriter",manager,inputs,outputs,1.583e+01,3.387e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "Framework";
    cfg.order   = 371;
    cfg.level   = 1;
    cfg.flags   = 0x0;
    cfg.type    = "Algorithm";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
  {
    const char* daughters[] = {0};
    const char* inputs[] = {"/Event/DAQ/RawEvent",0};
    const char* outputs[] = {"/Event/DAQ/ODIN",0};
    AlgConfig cfg("Framework",manager,inputs,outputs,0.000e+00,0.000e+00);
    cfg.setDaughters(daughters);
    cfg.parent  = "Framework";
    cfg.order   = 372;
    cfg.level   = 0;
    cfg.flags   = 0x2000000;
    cfg.type    = "Framework";
    AlgEntry* a = new AlgEntry(multiplicity,cfg);
    a->initialize();
  }
