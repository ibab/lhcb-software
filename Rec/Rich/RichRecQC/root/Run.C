
{

  // load the macros needed
  gROOT->ProcessLine(".L $HOME/richRoot/macros/Utilities.C+");
  gROOT->ProcessLine(".L $HOME/richRoot/macros/RichCKResolution.C+");
  gROOT->ProcessLine(".L $HOME/richRoot/macros/RichTrackResolutions.C+");
  gROOT->ProcessLine(".L $HOME/richRoot/macros/RichPIDPerformance.C+");
  gROOT->ProcessLine(".L $HOME/richRoot/macros/RichNCKPhotons.C+");
  gROOT->ProcessLine(".L $HOME/richRoot/macros/RichMakeAllPlots.C+");

  // make the plots
  RICH::performance( "rootFiles/lhcc-NewQE.root", "NewQE", " : 1.15 * DC06 HPD QE" );

}
