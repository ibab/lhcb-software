
{
  gROOT->ProcessLine(".L GlobalPID.C+");
  const double GeV(1000);

  // make a new pid object
  GlobalPID * pid = new GlobalPID(dataFile);

  // K-pi RICH
  pid->configReset();
  pid->config.title     = "RICH Kaon ID";
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minCut    = -10;
  pid->config.maxCut    = 50;
  pid->config.idType    = GlobalPID::Kaon;
  pid->config.misidType = GlobalPID::Proton;
  pid->config.var1      = GlobalPID::richDLLk;
  pid->config.var2      = GlobalPID::richDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = false;
  pid->config.color = kRed;
  // create the plot
  pid->makeCurve(nTracks);

  // K-pi RICH
  pid->configReset();
  pid->config.title     = "RICH Kaon ID";
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minCut    = -10;
  pid->config.maxCut    = 50;
  pid->config.idType    = GlobalPID::Kaon;
  pid->config.misidType = GlobalPID::Proton;
  pid->config.var1      = GlobalPID::richDLLk;
  pid->config.var2      = GlobalPID::richDLLpi;
  pid->config.secvar1   = GlobalPID::richDLLk;
  pid->config.secvar2   = GlobalPID::richDLLp;
  pid->config.secondaryCut = -10;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = true;
  pid->config.color = 49;
  // create the plot
  pid->makeCurve(nTracks);

  // K-pi RICH
  pid->configReset();
  pid->config.title     = "RICH Kaon ID";
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minCut    = -10;
  pid->config.maxCut    = 50;
  pid->config.idType    = GlobalPID::Kaon;
  pid->config.misidType = GlobalPID::Proton;
  pid->config.var1      = GlobalPID::richDLLk;
  pid->config.var2      = GlobalPID::richDLLpi;
  pid->config.secvar1   = GlobalPID::richDLLk;
  pid->config.secvar2   = GlobalPID::richDLLp;
  pid->config.secondaryCut = -5;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = true;
  pid->config.color = kBlue;
  // create the plot
  pid->makeCurve(nTracks);

  // K-pi RICH
  pid->configReset();
  pid->config.title     = "RICH Kaon ID";
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minCut    = -10;
  pid->config.maxCut    = 50;
  pid->config.idType    = GlobalPID::Kaon;
  pid->config.misidType = GlobalPID::Proton;
  pid->config.var1      = GlobalPID::richDLLk;
  pid->config.var2      = GlobalPID::richDLLpi;
  pid->config.secvar1   = GlobalPID::richDLLk;
  pid->config.secvar2   = GlobalPID::richDLLp;
  pid->config.secondaryCut = 0;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = true;
  pid->config.color = kBlack;
  // create the plot
  pid->makeCurve(nTracks);

  // save the figures
  pid->saveFigures();

  // cleanup
  delete pid;

}
