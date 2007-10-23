
{
  gROOT->ProcessLine(".L GlobalPID.C+");
  const double GeV(1000);

  // make a new pid object
  GlobalPID * pid = new GlobalPID(dataFile);

  // Ecal Only PID
  pid->configReset();
  pid->config.title     = "Muon ID";
  pid->config.useFixedGraphRange = true;
  pid->config.minGraphX = 60;
  pid->config.maxGraphX = 100;
  pid->config.minGraphY = 0.3;
  pid->config.maxGraphY = 40;
  pid->config.minCut    = -4;
  pid->config.maxCut    = 4;
  pid->config.nSteps    = 41;
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minMisIDeff  = 0.05;
  pid->config.idType    = GlobalPID::Muon;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::caloPIDmu;
  pid->config.var2      = GlobalPID::NoVar;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveECAL = true;
  pid->config.mustHaveHCAL = true;
  pid->config.logY  = true;
  pid->config.color = kBlue;
  pid->config.superImpose = false;
  // create the plot
  pid->makeCurve(nTracks);

  // RICH Only PID
  pid->configReset();
  pid->config.minCut    = -20;
  pid->config.maxCut    = 20;
  pid->config.nSteps    = 41;
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Muon;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::richDLLmu;
  pid->config.var2      = GlobalPID::richDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.logY  = true;
  pid->config.color = 6;
  pid->config.superImpose = true;
  // create the plot
  pid->makeCurve(nTracks);

  // Muon Only PID
  pid->configReset();
  pid->config.minCut    = -8;
  pid->config.maxCut    = 4;
  //pid->config.minIDeff  = 50;
  pid->config.nSteps    = 41;
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Muon;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::muonMuLL;
  pid->config.var2      = GlobalPID::muonBkgLL;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveMUON = true;
  pid->config.logY  = true;
  pid->config.color = 49;
  pid->config.superImpose = true;
  // create the plot
  pid->makeCurve(nTracks);

  // Combined DLL PID
  pid->configReset();
  pid->config.minCut    = -8;
  pid->config.maxCut    = 4;
  //pid->config.minIDeff  = 50;
  pid->config.nSteps    = 21;
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Muon;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::combDLLmu;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveMUON = true;
  pid->config.logY  = true;
  pid->config.color = kBlack;
  pid->config.superImpose = true;
  // create the plot
  pid->makeCurve(nTracks);

  //---------------------------------------------------------------------

  // Combined DLL PID
  pid->configReset();
  pid->config.minCut    = -8;
  pid->config.maxCut    = 4;
  //pid->config.minIDeff  = 50;
  pid->config.nSteps    = 21;
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Muon;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::combDLLmu;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveMUON = true;
  pid->config.logY  = true;
  pid->config.color = kBlack;
  pid->config.superImpose = false;
  // create the plot
  //pid->makeCurve(nTracks);

  // Combined DLL PID
  pid->configReset();
  //pid->config.recreateCombinedDLLs = true;
  pid->config.minCut    = -8;
  pid->config.maxCut    = 4;
  //pid->config.minIDeff  = 50;
  pid->config.nSteps    = 21;
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Muon;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::combDLLmu;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveMUON = true;
  pid->config.logY  = true;
  pid->config.color = kRed;
  pid->config.superImpose = true;
  // create the plot
  // pid->makeCurve(nTracks);

  // Combined DLL PID
  pid->configReset();
  //pid->config.recreateCombinedDLLs = true;
  pid->config.minCut    = -8;
  pid->config.maxCut    = 4;
  //pid->config.minIDeff  = 50;
  pid->config.nSteps    = 21;
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Muon;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::combDLLmu;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.secvar1   = GlobalPID::muonNShared;
  pid->config.secondaryCut = 1;
  pid->config.secCutType = "<";
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveMUON = true;
  pid->config.logY  = true;
  pid->config.color = kBlue;
  pid->config.superImpose = true;
  // create the plot
  //pid->makeCurve(nTracks);

  // save the figures
  pid->saveFigures();

  // cleanup
  delete pid;

}
