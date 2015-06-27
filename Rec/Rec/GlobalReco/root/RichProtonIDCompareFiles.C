
#include "TROOT.h"

#include "GlobalPID.h"
gROOT->ProcessLine(".L GlobalPID.C");

void RichProtonIDCompareFiles()
{

  const Long64_t nTracks = 10e5;

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

  pid->loadTTree("/usera/jonesc/LHCbCMake/BrunelDevNightly/Run2/AllTracks/protoparticles.tuples.root");
  pid->config = defaultConfig;
  pid->config.subtitle = "All Tracks in Likelihood";
  pid->config.superImpose = false;
  pid->config.color = kRed+1;;
  // create the plot
  pid->makeCurve(nTracks);

  pid->loadTTree("/usera/jonesc/LHCbCMake/BrunelDevNightly/Run2/LongTracks/protoparticles.tuples.root");
  pid->config = defaultConfig;
  pid->config.subtitle = "Long Tracks only in Likelihood";
  pid->config.superImpose = true;
  pid->config.color = kBlue+1;
  // create the plot
  pid->makeCurve(nTracks);

  // save the figures
  pid->saveFigures();

  // cleanup
  delete pid;

}
