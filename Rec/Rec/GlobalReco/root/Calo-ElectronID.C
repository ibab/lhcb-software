
{

  unsigned int nTracks = 1000000;

  //std::string dataFile = "/usera/jonesc/NFS/RootFiles/DC09/MC09v2-JPsiee-nu1.protoparticles.root";
  std::string dataFile = "/usera/jonesc/NFS/RootFiles/DC06/DC06-v31.protoparticles.tuples.root";

  gROOT->ProcessLine(".L GlobalPID.C+");
  const double GeV(1000);

  // make a new pid object
  GlobalPID * pid = new GlobalPID(dataFile);

  // First make a CALO only plot
  //----------------------------------------------------------------------

  // Ecal only
  pid->configReset();
  pid->config.title     = "CALO Electron ID";
  pid->config.minCut    = -10;
  pid->config.maxCut    = 10;
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Electron;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::ecalPIDe;
  pid->config.var2      = GlobalPID::NoVar;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveECAL = true;
  pid->config.logY  = true;
  pid->config.color = kRed;
  pid->config.superImpose = false;
  // create the plot
  pid->makeCurve(nTracks);

  // Hcal only
  pid->configReset();
  pid->config.minCut    = -3;
  pid->config.maxCut    = 4;
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Electron;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::hcalPIDe;
  pid->config.var2      = GlobalPID::NoVar;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveHCAL = true;
  pid->config.logY  = true;
  pid->config.color = kBlue;
  pid->config.superImpose = true;
  // create the plot
  pid->makeCurve(nTracks);

  // PRS only
  pid->configReset();
  pid->config.minCut    = -6;
  pid->config.maxCut    = 6;
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Electron;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::prsPIDe;
  pid->config.var2      = GlobalPID::NoVar;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHavePRS = true;
  pid->config.logY  = true;
  pid->config.color = 37;
  pid->config.superImpose = true;
  // create the plot
  pid->makeCurve(nTracks);

  // Brem only
  pid->configReset();
  pid->config.minCut    = -1;
  pid->config.maxCut    = 1;
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Electron;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::bremPIDe;
  pid->config.var2      = GlobalPID::NoVar;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveBREM = true;
  pid->config.logY  = true;
  pid->config.color = 49;
  pid->config.superImpose = true;
  // create the plot
  pid->makeCurve(nTracks);

  // Calo Combined
  pid->configReset();
  pid->config.minCut    = -10;
  pid->config.maxCut    = 10;
  pid->config.minP      = 2   * GeV;
  pid->config.maxP      = 100 * GeV;
  pid->config.minPt     = 0   * GeV;
  pid->config.maxPt     = 100 * GeV;
  pid->config.idType    = GlobalPID::Electron;
  pid->config.misidType = GlobalPID::Pion;
  pid->config.var1      = GlobalPID::caloPIDe;
  pid->config.var2      = GlobalPID::NoVar;
  pid->config.trackType = GlobalPID::Long;
  pid->config.mustHaveECALorPRS = true;
  //pid->config.mustHavePRS  = true;
  pid->config.logY  = true;
  pid->config.color = kBlack;
  pid->config.superImpose = true;
  // create the plot
  pid->makeCurve(nTracks);

  // save the figures
  pid->saveFigures();

  // cleanup
  delete pid;

}
