
//#include "TROOT.h"

//#include "GlobalPID.h"
#include "GlobalPID.C"

void RichProtonIDCompareFiles()
{

  const std::string dir = "/Users/chris/LHCb/RootFiles/Run2";
  //const std::string dir = "/usera/jonesc/LHCbCMake/lhcb-head/BrunelDevNightly/output/Run2";

  {

    // make a new pid object
    GlobalPID * pid = new GlobalPID();

    // Default Config Object
    GlobalPID::Configuration defaultConfig;

    const Long64_t nTracks = 1e8;

    const double GeV(1000);

    // Histo range
    defaultConfig.useFixedGraphRange = true;
    defaultConfig.minGraphX = 76;
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
    //defaultConfig.trackType = GlobalPID::Upstream;
    //defaultConfig.trackType = GlobalPID::Downstream;
    // detector selection
    defaultConfig.mustHaveAnyRICH = true;
    // Plot Type
    defaultConfig.title = "RICH Proton ID";
    defaultConfig.idType    = GlobalPID::Proton;
    defaultConfig.misidType = GlobalPID::Pion;
    defaultConfig.var1      = GlobalPID::richDLLp;
    defaultConfig.var2      = GlobalPID::richDLLpi;

    pid->loadTTree((dir+"/Quartic/50000events/protoparticles.tuples.root").c_str());
    pid->config = defaultConfig;
    pid->config.subtitle = "Quartic";
    pid->config.superImpose = false;
    pid->config.color = kBlack;
    // create the plot
    pid->makeCurve(nTracks);

    pid->loadTTree((dir+"/Quartic-NoAmbig/50000events/protoparticles.tuples.root").c_str());
    pid->config = defaultConfig;
    pid->config.subtitle = "Quartic - No Ambiguous Photon Test";
    pid->config.superImpose = true;
    pid->config.color = kRed+1;
    // create the plot
    pid->makeCurve(nTracks);

    pid->loadTTree((dir+"/Esti-LightestOnly/50000events/protoparticles.tuples.root").c_str());
    pid->config = defaultConfig;
    pid->config.subtitle = "Estimation - Lightest Hypo Only";
    pid->config.superImpose = true;
    pid->config.color = kBlue+1;
    // create the plot
    pid->makeCurve(nTracks);

    pid->loadTTree((dir+"/Esti-All/50000events/protoparticles.tuples.root").c_str());
    pid->config = defaultConfig;
    pid->config.subtitle = "Estimation - All Hypos";
    pid->config.superImpose = true;
    pid->config.color = kGreen+2;
    // create the plot
    pid->makeCurve(nTracks);

    // save the figures
    pid->saveFigures();

    // cleanup
    delete pid;

  }

  // {

  //   // make a new pid object
  //   GlobalPID * pid = new GlobalPID();

  //   // Default Config Object
  //   GlobalPID::Configuration defaultConfig;

  //   const Long64_t nTracks = 1e8;

  //   const double GeV(1000);

  //   // Histo range
  //   defaultConfig.useFixedGraphRange = true;
  //   defaultConfig.minGraphX = 40;
  //   defaultConfig.maxGraphX = 100;
  //   defaultConfig.minGraphY = 2;
  //   defaultConfig.maxGraphY = 70;
  //   // Stepping options
  //   defaultConfig.maxCut      = 35;
  //   defaultConfig.nSteps      = 100;
  //   defaultConfig.minMisIDeff = 1.0;
  //   // Momentum range
  //   defaultConfig.minP      = 3   * GeV;
  //   defaultConfig.maxP      = 100 * GeV;
  //   defaultConfig.minPt     = 0.5 * GeV;
  //   defaultConfig.maxPt     = 100 * GeV;
  //   // track selection
  //   defaultConfig.trackType = GlobalPID::Long;
  //   //defaultConfig.trackType = GlobalPID::Upstream;
  //   //defaultConfig.trackType = GlobalPID::Downstream;
  //   // detector selection
  //   defaultConfig.mustHaveAnyRICH = true;
  //   // Plot Type
  //   defaultConfig.title = "RICH Proton ID";
  //   defaultConfig.idType    = GlobalPID::Proton;
  //   defaultConfig.misidType = GlobalPID::Kaon;
  //   defaultConfig.var1      = GlobalPID::richDLLp;
  //   defaultConfig.var2      = GlobalPID::richDLLk;

  //   pid->loadTTree((dir+"/Quartic/50000events/protoparticles.tuples.root").c_str());
  //   pid->config = defaultConfig;
  //   pid->config.subtitle = "Quartic";
  //   pid->config.superImpose = false;
  //   pid->config.color = kBlack;
  //   // create the plot
  //   pid->makeCurve(nTracks);

  //   pid->loadTTree((dir+"/Quartic-NoAmbig/50000events/protoparticles.tuples.root").c_str());
  //   pid->config = defaultConfig;
  //   pid->config.subtitle = "Quartic - No Ambiguous Photon Test";
  //   pid->config.superImpose = true;
  //   pid->config.color = kRed+1;
  //   // create the plot
  //   pid->makeCurve(nTracks);

  //   pid->loadTTree((dir+"/Esti-LightestOnly/50000events/protoparticles.tuples.root").c_str());
  //   pid->config = defaultConfig;
  //   pid->config.subtitle = "Estimation - Lightest Hypo Only";
  //   pid->config.superImpose = true;
  //   pid->config.color = kBlue+1;
  //   // create the plot
  //   pid->makeCurve(nTracks);

  //   pid->loadTTree((dir+"/Esti-All/50000events/protoparticles.tuples.root").c_str());
  //   pid->config = defaultConfig;
  //   pid->config.subtitle = "Estimation - All Hypos";
  //   pid->config.superImpose = true;
  //   pid->config.color = kGreen+2;
  //   // create the plot
  //   pid->makeCurve(nTracks);

  //   // save the figures
  //   pid->saveFigures();

  //   // cleanup
  //   delete pid;

  // }

}
