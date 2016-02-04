
#include <tuple>
#include <vector>

#include "GlobalPID.C"

void RichKaonIDCompareFiles()
{

  // make a new pid object
  GlobalPID * pid = new GlobalPID();

  // Default Config Object
  GlobalPID::Configuration defaultConfig;

  const Long64_t nTracks = 1e8;

  const double GeV(1000);

  // Histo range
  defaultConfig.useFixedGraphRange = true;
  //defaultConfig.minGraphX = 93;
  //defaultConfig.maxGraphX = 97;
  //defaultConfig.minGraphY = 3;
  //defaultConfig.maxGraphY = 8;
  defaultConfig.minGraphX = 80;
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
  defaultConfig.title = "RICH Kaon ID";
  defaultConfig.idType    = GlobalPID::Kaon;
  defaultConfig.misidType = GlobalPID::Pion;
  defaultConfig.var1      = GlobalPID::richDLLk;
  defaultConfig.var2      = GlobalPID::richDLLpi;

  //const std::string dir = "/Users/chris/LHCb/RootFiles/Run2";
  const std::string dir = "/usera/jonesc/LHCbCMake/lhcb-head/BrunelDevNightly/output/Run2";

  //const std::vector<Color_t> colors = { kBlack, kRed+1, kBlue+1, kGreen+2, 
  //                                      kYellow+3, kMagenta+1, kCyan+1 };
  //const std::vector< std::pair<std::string,std::string> > plots =
  //  {
  //   { "Quartic", "Quartic" },
  //   { "Quartic-NoAmbig", "Quartic - No Ambiguous Photon Test" },
  //    { "Esti-100points", "Estimation - All Hypos - 100 points" }
  //  };

  typedef std::vector< std::tuple<std::string,std::string,Color_t> > PlotData;
  const PlotData plotdata = 
    {
      std::make_tuple ( "Quartic",         "Quartic",                             kBlack   ),
      //std::make_tuple ( "Quartic-WithDeuteron",  "Quartic | With Deuteron",        kRed-6   )
      std::make_tuple ( "Quartic-NoAmbig", "Quartic - No Ambiguous Photon Test",  kRed-6 ),
      std::make_tuple ( "Esti-50points",   "Estimation | 50 points",  kBlue+1  ),
      std::make_tuple ( "Esti-100points",  "Estimation | 100 points", kGreen+2  ),
      std::make_tuple ( "Esti-200points",  "Estimation | 200 points", kYellow+3 ),
      std::make_tuple ( "Esti-1000points", "Estimation | 1000 points", kRed+1 ),
      //std::make_tuple ( "Adaptive-100points-0to90pc",  "Adaptive | 100 points | 0 to 90%", kYellow+3 ),
      //std::make_tuple ( "Adaptive-100points-0to925pc", "Adaptive | 100 points | 0 to 92.5%", kMagenta+2 ),
      std::make_tuple ( "Adaptive-100points-0to95pc", "Adaptive | 100 points | 0 to 95%", kCyan+2 ),
      std::make_tuple ( "Adaptive-75to100points-0to95pc", "Adaptive | 75 to 100 points | 0 to 95%", kMagenta+2 )
      //std::make_tuple ( "Adaptive-100points-0to98pc", "Adaptive | 100 points | 0 to 98%", kRed+1 )
    };

  unsigned int iPlot = 0;
  for ( const auto& pd : plotdata )
  {
    pid->loadTTree((dir+"/"+std::get<0>(pd)+"/50000events/protoparticles.tuples.root").c_str());
    pid->config = defaultConfig;
    pid->config.subtitle = std::get<1>(pd);
    pid->config.superImpose = (iPlot != 0);
    pid->config.color = std::get<2>(pd);
    // create the plot
    pid->makeCurve(nTracks);
    ++iPlot;
  }
  
  // save the figures
  pid->saveFigures();
  
  // cleanup
  delete pid;

}
