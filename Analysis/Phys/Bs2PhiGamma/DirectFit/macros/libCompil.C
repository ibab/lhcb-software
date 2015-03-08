{
  gROOT->ProcessLine(".x ${ROOLIB}/setup.C");
// PDF's  
gROOT->ProcessLine(".L libPDF/RooBsProperTime.cxx+");
gROOT->ProcessLine(".L libPDF/RooBsPsiPhiProperTime.cxx+");
gROOT->ProcessLine(".L libPDF/BifurcatedCB.cxx+");
gROOT->ProcessLine(".L libPDF/ErfThresholdPDF.cxx+"); 
gROOT->ProcessLine(".L libPDF/myExpo.cxx+");
gROOT->ProcessLine(".L libPDF/RooApollonios.cxx+");
gROOT->ProcessLine(".L libPDF/RooFlatte.cxx+");
gROOT->ProcessLine(".L libPDF/SlidingBifurcatedCB.cxx+");
gROOT->ProcessLine(".L libPDF/SlidingGaussian.cxx+");
gROOT->ProcessLine(".L libPDF/RooProperTime.cxx+");
gROOT->ProcessLine(".L libPDF/RooRelBreitWigner.cxx+");
// Utils
gROOT->ProcessLine(".L libUtils/rooLib.C+");
gROOT->ProcessLine(".L libUtils/TChainer.C+");
gROOT->ProcessLine(".L libUtils/TWeighter1D.C+");
gROOT->ProcessLine(".L libUtils/TWeighter2D.C+");
gROOT->ProcessLine(".L libPID/wPID.C+");

//

 
}
