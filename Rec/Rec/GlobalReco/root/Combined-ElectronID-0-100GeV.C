
{
  gROOT->ProcessLine(".L GlobalPID.C+");
  const double GeV(1000);

  // make a new pid object
  GlobalPID * pid = new GlobalPID();

  // Compare CALO combined to LHCb combined ID

  // All CALO
  pid->configReset();
  pid->config.title     = "Electron ID : CALO V RICH V Combined";
  pid->config.minCut    = -10;
  pid->config.maxCut    = 10;
  pid->config.minP      = 0   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minMisIDeff  = 0.1;
  pid->config.idType    = GlobalPID::Electron;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::caloPIDe;
  pid->config.var2      = GlobalPID::NoVar;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveECALorPRS = true;
  pid->config.logY = true;
  pid->config.color = 49;
  pid->config.superImpose = false;
  // create the plot
  pid->makeCurve(nTracks);

  // All RICH
  pid->configReset();
  pid->config.minCut    = -40;
  pid->config.maxCut    = 40;
  pid->config.minP      = 0   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Electron;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::richDLLe;
  pid->config.var2      = GlobalPID::richDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.logY = true;
  pid->config.color = kBlue;
  pid->config.superImpose = true;
  // create the plot
  pid->makeCurve(nTracks);

  // Combined DLL
  pid->configReset();
  pid->config.recreateCombinedDLLs = false;
  pid->config.minCut    = -40;
  pid->config.maxCut    = 40;
  pid->config.minP      = 0   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Electron;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::combDLLe;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveECALorPRS = true;
  pid->config.logY = true;
  pid->config.color = kRed;
  pid->config.superImpose = true;
  // create the plot
  pid->makeCurve(nTracks);

  // save the figures so far
  pid->saveFigures();

  // Combined DLL (retuned)
  pid->configReset();
  pid->config.recreateCombinedDLLs = true;
  pid->config.minCut    = -15;
  pid->config.maxCut    = 15;
  pid->config.minP      = 0   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Electron;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::combDLLe;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveECALorPRS = true;
  pid->config.logY = true;
  pid->config.color = kBlack;
  pid->config.superImpose = true;
  // create the plot
  pid->makeCurve(nTracks);

  // save the figures
  pid->saveFigures();

}
