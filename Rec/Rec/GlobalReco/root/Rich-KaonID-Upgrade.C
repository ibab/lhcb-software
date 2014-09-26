
{

  const Long64_t nTracks = 10000000;

  gROOT->ProcessLine(".L GlobalPID.C+");
  const double GeV(1000);

  std::string LumiVal="  Lumi4-S ";
  //    std::string OpticsConfig=" RICH-V7G-Optics-Reco-DataSetR";
  //    std::string OpticsConfig=" RICH-V7G-Optics-Reco-DataSetV";
     // std::string OpticsConfig=" RICH-V7G-Optics-Reco-DataSetW";

    std::string OpticsConfig=" Lumi4-v7H-Optics-LHCbUpgrade-Comparison";

    //    std::string OpticsConfig=" RICH-V8C-Optics-Reco-DataSetZA";

      //    std::string OpticsConfig=" RICH-V8A-Optics-Reco-DataSetX";

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
  defaultConfig.minCut    = -30;
  defaultConfig.maxCut      = 50;
  defaultConfig.nSteps      = 100;
  // defaultConfig.minMisIDeff = 1.0;
  // Momentum range
  // defaultConfig.minP      = 1.5   * GeV;
  defaultConfig.minP      = 1.5   * GeV;
  //defaultConfig.minP      = 35   * GeV;
  defaultConfig.maxP      = 100 * GeV;
  defaultConfig.minPt     = 0.5 * GeV;
  defaultConfig.maxPt     = 100 * GeV;
  defaultConfig.minAngle  = 0.0;
  // defaultConfig.maxAngle  = 120.0;
  defaultConfig.maxAngle  = 300.0;
  defaultConfig.minMisIDeff = 0.5;

  // track selection
  defaultConfig.trackType = GlobalPID::Long;
  // detector selection
  defaultConfig.mustHaveAnyRICH = true;
  // Plot Type
  defaultConfig.title = "RICH Kaon ID  " +OpticsConfig ;
  defaultConfig.idType    = GlobalPID::Kaon;
  defaultConfig.misidType = GlobalPID::Pion;
  defaultConfig.var1      = GlobalPID::richDLLk;
  defaultConfig.var2      = GlobalPID::richDLLpi;



          


  pid->loadTTree("/afs/cern.ch/work/s/seaso/public/BrunelOutput/URich_v7H/lumi4/SetSTT/NtupFile/merge/protoparticles_URich_v7H_SetSTT_lumi4_Rich2Std_00_199.root");
  

  pid->config = defaultConfig;
  pid->config.subtitle = "Lumi4-S Current LHCb + Upgrade RICH version  v7H ";
  pid->config.superImpose = false;
  pid->config.color = kBlack;
  // create the plot
  pid->makeCurve(nTracks);




  pid->loadTTree("/afs/cern.ch/work/s/seaso/public/BrunelOutput/URich_v7H/LHCbUpgrade/lumi4/SetASA/NtupFile/merge/protoparticles_URich_v7H_LHCbUpgrade_SetASA_lumi4_Rich2Mixed_00_199.root");
  

  pid->config = defaultConfig;
  pid->config.subtitle = "Lumi4-S  Upgrade LHCb + Upgrade RICH version v8D ";
  pid->config.superImpose = true;
  pid->config.color = kBlue+1;
  // create the plot
  pid->makeCurve(nTracks);
 
  // save the figures
  pid->saveFigures();

  // cleanup
  delete pid;

}
