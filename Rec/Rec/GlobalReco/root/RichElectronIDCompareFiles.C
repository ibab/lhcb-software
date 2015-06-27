
#include "TROOT.h"

#include "GlobalPID.h"

void RichElectronIDCompareFiles()
{

  const Long64_t nTracks = 10e5;

  gROOT->ProcessLine(".L GlobalPID.C");

  const double GeV(1000);

  // make a new pid object
  GlobalPID * pid = new GlobalPID();

  // Default Config Object
  GlobalPID::Configuration defaultConfig;

  // Histo range
  defaultConfig.useFixedGraphRange = true;
  defaultConfig.minGraphX = 60;
  defaultConfig.maxGraphX = 100;
  defaultConfig.minGraphY = 1;
  defaultConfig.maxGraphY = 20;
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
  defaultConfig.title = "RICH Electron ID";
  defaultConfig.idType    = GlobalPID::Electron;
  defaultConfig.misidType = GlobalPID::Pion;
  defaultConfig.var1      = GlobalPID::richDLLe;
  defaultConfig.var2      = GlobalPID::richDLLpi;

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
