void declare(std::string lib){
  gSystem->Load(lib.c_str());
  gSystem->AddLinkedLibs(lib.c_str());
};

void libLoader(){ 
  // load libraries
	gSystem->AddIncludePath(" -I${ROOLIB}");
	gSystem->AddIncludePath(" -I${ROOLIB}/libUtils");
	gSystem->AddIncludePath(" -I${ROOLIB}/libPDF");
	gSystem->AddIncludePath(" -I${ROOLIB}/libPID");
  declare("${ROOLIB}/libPDF/RooFlatte_cxx.so");
  declare("${ROOLIB}/libPDF/RooFlatte_cxx.so"); 
  declare("${ROOLIB}/libPDF/ErfThresholdPDF_cxx.so"); 
  declare("${ROOLIB}/libPDF/BifurcatedCB_cxx.so");
  declare("${ROOLIB}/libPDF/SlidingBifurcatedCB_cxx.so");
  declare("${ROOLIB}/libPDF/SlidingGaussian_cxx.so");
  declare("${ROOLIB}/libPDF/RooApollonios_cxx.so");
  declare("${ROOLIB}/libPDF/RooProperTime_cxx.so");
  declare("${ROOLIB}/libPDF/RooBsProperTime_cxx.so");
  declare("${ROOLIB}/libPDF/RooBsPsiPhiProperTime_cxx.so");
  declare("${ROOLIB}/libPDF/RooRelBreitWigner_cxx.so");
  declare("${ROOLIB}/libPDF/myExpo_cxx.so");
  declare("${ROOLIB}/libUtils/rooLib_C.so");
  declare("${ROOLIB}/libUtils/TChainer_C.so");
  declare("${ROOLIB}/libUtils/TWeighter1D_C.so");
  declare("${ROOLIB}/libUtils/TWeighter2D_C.so");
  declare("${ROOLIB}/libPID/wPID_C.so");
  // -----
  using namespace RooFit;
  gSystem->Load("libMinuit") ; 
  // ---
  gROOT->ProcessLine(".L ${ROOLIB}/libUtils/bibFunc.C");
  gROOT->ProcessLine(".x ${ROOLIB}/libUtils/lhcbstyle.C");
  gROOT->ProcessLine(".x ${ROOLIB}/setup.C");
};


