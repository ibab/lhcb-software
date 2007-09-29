
{
  gROOT->ProcessLine(".L GlobalPID.C+");
  const double GeV(1000);

  // make a new pid object
  GlobalPID * pid = new GlobalPID(dataFile);
    
  // K-pi RICH
  pid->configReset();
  pid->config.title     = "Proton ID";
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Proton;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::combDLLp;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = false;
  pid->config.color = kBlack;
  pid->config.logY  = true;
  // create the plot
  pid->makeCurve(nTracks);

  // K-pi RICH
  pid->configReset();
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Proton;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::combDLLp;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.secvar1   = GlobalPID::combDLLp;
  pid->config.secvar2   = GlobalPID::combDLLk;
  pid->config.secondaryCut = -10;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = true;
  pid->config.color = kBlue;
  pid->config.logY  = true;
  // create the plot
  pid->makeCurve(nTracks);

  // K-pi RICH
  pid->configReset();
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Proton;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::combDLLp;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.secvar1   = GlobalPID::combDLLp;
  pid->config.secvar2   = GlobalPID::combDLLk;
  pid->config.secondaryCut = -5;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = true;
  pid->config.color = kRed;
  pid->config.logY  = true;
  // create the plot
  pid->makeCurve(nTracks);

  // K-pi RICH
  pid->configReset();
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Proton;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::combDLLp;
  pid->config.var2      = GlobalPID::combDLLpi;
  pid->config.secvar1   = GlobalPID::combDLLp;
  pid->config.secvar2   = GlobalPID::combDLLk;
  pid->config.secondaryCut = 0;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = true;
  pid->config.color = 49;
  pid->config.logY  = true;
  // create the plot
  pid->makeCurve(nTracks);


  // =====================================================

  // K-pi RICH
  pid->configReset();
  pid->config.title     = "Proton ID";
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.minMisIDeff = 3;
  pid->config.idType    = GlobalPID::Proton;
  pid->config.misidType = GlobalPID::Kaon;
  pid->config.var1      = GlobalPID::combDLLp;
  pid->config.var2      = GlobalPID::combDLLk;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = false;
  pid->config.color = kBlack;
  pid->config.logY  = true;
  // create the plot
  pid->makeCurve(nTracks);

  // K-pi RICH
  pid->configReset();
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Proton;
  pid->config.misidType = GlobalPID::Kaon;
  pid->config.var1      = GlobalPID::combDLLp;
  pid->config.var2      = GlobalPID::combDLLk;
  pid->config.secvar1   = GlobalPID::combDLLp;
  pid->config.secvar2   = GlobalPID::combDLLpi;
  pid->config.secondaryCut = -40;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = true;
  pid->config.color = kBlue;
  pid->config.logY  = true;
  // create the plot
  pid->makeCurve(nTracks);

  // K-pi RICH
  pid->configReset();
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Proton;
  pid->config.misidType = GlobalPID::Kaon;
  pid->config.var1      = GlobalPID::combDLLp;
  pid->config.var2      = GlobalPID::combDLLk;
  pid->config.secvar1   = GlobalPID::combDLLp;
  pid->config.secvar2   = GlobalPID::combDLLpi;
  pid->config.secondaryCut = -10;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = true;
  pid->config.color = kRed;
  pid->config.logY  = true;
  // create the plot
  pid->makeCurve(nTracks);

  // K-pi RICH
  pid->configReset();
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0.5 * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Proton;
  pid->config.misidType = GlobalPID::Kaon;
  pid->config.var1      = GlobalPID::combDLLp;
  pid->config.var2      = GlobalPID::combDLLk;
  pid->config.secvar1   = GlobalPID::combDLLp;
  pid->config.secvar2   = GlobalPID::combDLLpi;
  pid->config.secondaryCut = 0;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveAnyRICH = true;
  pid->config.superImpose = true;
  pid->config.color = 49;
  pid->config.logY  = true;
  // create the plot
  pid->makeCurve(nTracks);

  // save the figures
  pid->saveFigures();

  // cleanup
  delete pid;

}
