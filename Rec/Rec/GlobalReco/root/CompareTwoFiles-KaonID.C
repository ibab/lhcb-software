
{
  gROOT->ProcessLine(".L GlobalPID.C+");
  const double GeV(1000);

  // make a new pid object
  GlobalPID * pid = new GlobalPID();

  pid->loadTTree("/var/work/pcgs/jonesc/fastreco/Offline-AllRads-AllTracks.tuples.root");
  //pid->loadTTree("/var/work/pcgs/jonesc/fastreco/Offline-GasRads-AllTracks.tuples.root");

  // K-pi RICH
  pid->configReset();
  pid->config.title     = "RICH Kaon ID : All Radiators";
  pid->config.subtitle  = "Full Offline";
  pid->config.minP      = 3   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.0 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minCut    = -30;
  pid->config.maxCut    = 35;
  pid->config.minMisIDeff = 1.0;
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

  // K-pi RICH
  pid->configReset();
  pid->config.subtitle  = "Full Offline";
  pid->config.minP      = 3    * GeV;
  pid->config.maxP      = 100  * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100  * GeV;
  pid->config.minCut    = -30;
  pid->config.maxCut    = 35;
  pid->config.idType    = GlobalPID::Kaon;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::richDLLk;
  pid->config.var2      = GlobalPID::richDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = true;
  pid->config.color = kBlue;
  // create the plot
  pid->makeCurve(nTracks);

  pid->loadTTree("/var/work/pcgs/jonesc/fastreco/Fast-AllRads-LongTracks.tuples.root");
  //pid->loadTTree("/var/work/pcgs/jonesc/fastreco/Fast-GasRads-LongTracks.tuples.root");

  // K-pi RICH
  pid->configReset();
  pid->config.subtitle  = "Fast Offline";
  pid->config.minP      = 3    * GeV;
  pid->config.maxP      = 100  * GeV;
  pid->config.minPt     = 0.0 * GeV;
  pid->config.maxPt     = 100  * GeV;
  pid->config.minCut    = -30;
  pid->config.maxCut    = 35;
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

  // K-pi RICH
  pid->configReset();
  pid->config.subtitle  = "Fast Offline";
  pid->config.minP      = 3    * GeV;
  pid->config.maxP      = 100  * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100  * GeV;
  pid->config.minCut    = -30;
  pid->config.maxCut    = 35;
  pid->config.idType    = GlobalPID::Kaon;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::richDLLk;
  pid->config.var2      = GlobalPID::richDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = true;
  pid->config.color = 49;
  // create the plot
  pid->makeCurve(nTracks);

  // save the figures
  pid->saveFigures();

  // cleanup
  delete pid;

}
