
#include "TROOT.h"

#include "GlobalPID.h"

void RichKaonIDCompareFiles()
{
  
  gROOT->ProcessLine(".L GlobalPID.C+");

  // make a new pid object
  GlobalPID * pid = new GlobalPID();

  // Default Config Object
  GlobalPID::Configuration defaultConfig;

  const Long64_t nTracks = 1e7;

  const double GeV(1000);

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
  defaultConfig.minPt     = 0.0 * GeV;
  defaultConfig.maxPt     = 100 * GeV;
  // track selection
  //defaultConfig.trackType = GlobalPID::Long;
  //defaultConfig.trackType = GlobalPID::Upstream;
  defaultConfig.trackType = GlobalPID::Downstream;
  // detector selection
  defaultConfig.mustHaveAnyRICH = true;
  // Plot Type
  defaultConfig.title = "RICH Kaon ID";
  defaultConfig.idType    = GlobalPID::Kaon;
  defaultConfig.misidType = GlobalPID::Pion;
  defaultConfig.var1      = GlobalPID::richDLLk;
  defaultConfig.var2      = GlobalPID::richDLLpi;

  pid->loadTTree("/usera/jonesc/LHCbCMake/BrunelDevNightly/Run2/AllTracksAtOnce/protoparticles.tuples.root");
  pid->config = defaultConfig;
  pid->config.subtitle = "Tracks Combined";
  pid->config.superImpose = false;
  pid->config.color = kRed+1;;
  // create the plot
  pid->makeCurve(nTracks);

  pid->loadTTree("/usera/jonesc/LHCbCMake/BrunelDevNightly/Run2/TracksSeperately/protoparticles.tuples.root");
  pid->config = defaultConfig;
  pid->config.subtitle = "Tracks Seperately";
  pid->config.superImpose = true;
  pid->config.color = kBlue+1;
  // create the plot
  pid->makeCurve(nTracks);

  // save the figures
  pid->saveFigures();

  // cleanup
  delete pid;

}
