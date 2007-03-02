
{
  gROOT->ProcessLine(".L GlobalPID.C+");
  const double GeV(1000);

  // make a new pid object
  GlobalPID * pid = new GlobalPID();
    
  // K-pi RICH
  pid->configReset();
  pid->config.title     = "Kaon ID : RICH V Combined DLL";
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minCut    = -20;
  pid->config.maxCut    = 20;
  pid->config.idType    = GlobalPID::Kaon;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::combDLLk;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = false;
  pid->config.color = kBlue;
  pid->config.logY  = true;
  pid->config.nSteps = 61;
  pid->config.maxLabels = 10;
  // create the plot
  pid->makeCurve(nTracks);

  // K-pi RICH
  pid->configReset();
  pid->config.title     = "Kaon ID : RICH V Combined DLL : Renormalised RICH DLLs";
  pid->config.recreateCombinedDLLs = true;
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minCut    = -4;
  pid->config.maxCut    = 3;
  pid->config.idType    = GlobalPID::Kaon;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::combDLLk;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = false;
  pid->config.color = kRed;
  pid->config.nSteps = 61;
  pid->config.maxLabels = 10;
  pid->config.logY  = true;
  // create the plot
  //pid->makeCurve(nTracks);

  // save the figures
  pid->saveFigures();

}
