
{

  const Long64_t nTracks = 1000000;

  gROOT->ProcessLine(".L GlobalPID.C+");
  const double GeV(1000);

  // make a new pid object
  GlobalPID * pid = new GlobalPID();

  // Default Config Object
  GlobalPID::Configuration defaultConfig;

  // Histo range
  defaultConfig.useFixedGraphRange = true;
  defaultConfig.minGraphX = 40;
  defaultConfig.maxGraphX = 100;
  defaultConfig.minGraphY = 2;
  defaultConfig.maxGraphY = 70;
  // Stepping options
  defaultConfig.maxCut      = 35;
  defaultConfig.nSteps      = 100;
  defaultConfig.minMisIDeff = 1.0;
  // Momentum range
  defaultConfig.minP      = 3   * GeV;
  defaultConfig.maxP      = 100 * GeV;
  defaultConfig.minPt     = 0.5 * GeV;
  defaultConfig.maxPt     = 100 * GeV;
  // track selection
  defaultConfig.trackType = GlobalPID::Long;
  // detector selection
  defaultConfig.mustHaveAnyRICH = true;
  // Plot Type
  defaultConfig.title = "RICH Proton ID";
  defaultConfig.idType    = GlobalPID::Proton;
  defaultConfig.misidType = GlobalPID::Kaon;
  defaultConfig.var1      = GlobalPID::richDLLp;
  defaultConfig.var2      = GlobalPID::richDLLk;

  // DC06 v30
  pid->loadTTree("/Users/chris/LHCb/RootFiles/DC06/DC06-v30.protoparticles.tuples.root");
  pid->config = defaultConfig;
  pid->config.subtitle = "DC06 (v30)";
  pid->config.superImpose = false;
  pid->config.color = kBlack;
  // create the plot
  pid->makeCurve(nTracks);

  // DC06 v31
  pid->loadTTree("/Users/chris/LHCb/RootFiles/DC06/DC06-v31.protoparticles.tuples.root");
  pid->config = defaultConfig;
  pid->config.subtitle = "DC06 (v31)";
  pid->config.superImpose = true;
  pid->config.color = kBlue+1;
  // create the plot
  pid->makeCurve(nTracks);

  // DC09
  pid->loadTTree("/Users/chris/LHCb/RootFiles/DC09/DC09-Test3.NormalPixClean.protoparticles.tuples.root");
  pid->config = defaultConfig;
  pid->config.subtitle = "DC09 (Test3)";
  pid->config.superImpose = true;
  pid->config.color = kRed+1;
  // create the plot
  pid->makeCurve(nTracks);

  // DC09
  pid->loadTTree("/Users/chris/LHCb/RootFiles/DC09/DC09-Test3.NoPixClean.protoparticles.tuples.root");
  pid->config = defaultConfig;
  pid->config.subtitle = "DC09 (Test3) | No Pixel Cleaning";
  pid->config.superImpose = true;
  pid->config.color = kGreen+2;
  // create the plot
  pid->makeCurve(nTracks);

  // save the figures
  pid->saveFigures();

  // cleanup
  delete pid;

}
