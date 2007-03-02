
{
  gROOT->ProcessLine(".L GlobalPID.C+");
  const double GeV(1000);

  // make a new pid object
  GlobalPID * pid = new GlobalPID();

  pid->configReset();
  pid->config.title     = "Kaon ID";
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minCut    = -15;
  pid->config.maxCut    = 50;
  pid->config.minMisIDeff = 1;
  pid->config.idType    = GlobalPID::Kaon;
  pid->config.misidType = GlobalPID::Electron;
  pid->config.var1      = GlobalPID::combDLLk;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose     = false;
  pid->config.color = kBlue;
  // create the plot
  pid->makeCurve(nTracks);

  pid->configReset();
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minCut    = -15;
  pid->config.maxCut    = 50;
  pid->config.idType    = GlobalPID::Kaon;
  pid->config.misidType = GlobalPID::Electron;
  pid->config.var1      = GlobalPID::combDLLk;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.secvar1   = GlobalPID::combDLLk;
  pid->config.secvar2   = GlobalPID::combDLLe;
  pid->config.secondaryCut = -10;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = true;
  pid->config.color = kRed;
  // create the plot
  pid->makeCurve(nTracks);

  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minCut    = -15;
  pid->config.maxCut    = 50;
  pid->config.idType    = GlobalPID::Kaon;
  pid->config.misidType = GlobalPID::Electron;
  pid->config.var1      = GlobalPID::combDLLk;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.secvar1   = GlobalPID::combDLLk;
  pid->config.secvar2   = GlobalPID::combDLLe;
  pid->config.secondaryCut = 0;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = true;
  pid->config.color = 49;
  // create the plot
  pid->makeCurve(nTracks);

  pid->configReset();
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minCut    = -15;
  pid->config.maxCut    = 50;
  pid->config.idType    = GlobalPID::Kaon;
  pid->config.misidType = GlobalPID::Electron;
  pid->config.var1      = GlobalPID::combDLLk;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.secvar1   = GlobalPID::combDLLk;
  pid->config.secvar2   = GlobalPID::combDLLe;
  pid->config.secondaryCut = 10;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = true;
  pid->config.color = kBlack;
  // create the plot
  pid->makeCurve(nTracks);

  // save the figures
  pid->saveFigures();

}
