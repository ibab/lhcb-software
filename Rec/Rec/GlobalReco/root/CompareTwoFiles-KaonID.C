
{
  gROOT->ProcessLine(".L GlobalPID.C+");
  const double GeV(1000);

  // make a new pid object
  GlobalPID * pid = new GlobalPID();

  pid->loadTTree("/usera/jonesc/cmtuser/Brunel_v34r0/Rec/Brunel/cmt/DC06/protoparticles.tuples.root");

  // K-pi RICH
  pid->configReset();
  pid->config.useFixedGraphRange = true;
  pid->config.minGraphX = 40;
  pid->config.maxGraphX = 100;
  pid->config.minGraphY = 1;
  pid->config.maxGraphY = 20;
  pid->config.title     = "RICH Kaon ID";
  pid->config.subtitle  = "2006 MC";
  pid->config.minP      = 3   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.0 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minCut    = -30;
  pid->config.maxCut    = 35;
  pid->config.nSteps    = 100;
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
  pid->config.useFixedGraphRange = true;
  pid->config.subtitle  = "2006 MC";
  pid->config.minP      = 3    * GeV;
  pid->config.maxP      = 100  * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100  * GeV;
  pid->config.minCut    = -30;
  pid->config.maxCut    = 35;
  pid->config.nSteps    = 100;
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

  pid->loadTTree("/usera/jonesc/cmtuser/Brunel_v34r0/Rec/Brunel/cmt/2008MC/protoparticles.tuples.root");

  // K-pi RICH
  pid->configReset();
  pid->config.useFixedGraphRange = true;
  pid->config.subtitle  = "2008 MC";
  pid->config.minP      = 3    * GeV;
  pid->config.maxP      = 100  * GeV;
  pid->config.minPt     = 0.0 * GeV;
  pid->config.maxPt     = 100  * GeV;
  pid->config.minCut    = -30;
  pid->config.maxCut    = 35;
  pid->config.nSteps    = 100;
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
  pid->config.useFixedGraphRange = true;
  pid->config.subtitle  = "2008 MC";
  pid->config.minP      = 3    * GeV;
  pid->config.maxP      = 100  * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100  * GeV;
  pid->config.minCut    = -30;
  pid->config.maxCut    = 35;
  pid->config.nSteps    = 100;
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
