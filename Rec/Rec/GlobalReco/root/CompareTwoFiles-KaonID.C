
{
  gROOT->ProcessLine(".L GlobalPID.C+");
  const double GeV(1000);

  // make a new pid object
  GlobalPID * pid = new GlobalPID();

  pid->loadTTree("all_limit_m_minSig_10-4_rmapx_fullstat0.rich.tuples.root");

  // K-pi RICH
  pid->configReset();
  pid->config.title     = "RICH Kaon ID";
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0. * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minCut    = -50;
  pid->config.maxCut    = 50;
  pid->config.minMisIDeff = 0.5;
  pid->config.idType    = GlobalPID::Kaon;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::richDLLk;
  pid->config.var2      = GlobalPID::richDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = false;
  pid->config.color = kBlack;
  // create the plot
  pid->makeCurve(nTracks);

  pid->loadTTree("normal_primed0.rich.tuples.root");

  // K-pi RICH
  pid->configReset();
  pid->config.title     = "RICH Kaon ID";
  pid->config.minP      = 2    * GeV;
  pid->config.maxP      = 100  * GeV;
  pid->config.minPt     = 0. * GeV;
  pid->config.maxPt     = 100  * GeV;
  pid->config.minCut    = -50;
  pid->config.maxCut    = 50;
  pid->config.idType    = GlobalPID::Kaon;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::richDLLk;
  pid->config.var2      = GlobalPID::richDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = true;
  pid->config.color = kRed;
  // create the plot
  pid->makeCurve(nTracks);

  // save the figures
  pid->saveFigures();

}
