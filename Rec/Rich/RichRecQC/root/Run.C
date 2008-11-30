
{

  // load the macros needed
  gROOT->ProcessLine(".L Utilities.C+");
  gROOT->ProcessLine(".L RichCKResolution.C+");
  gROOT->ProcessLine(".L RichTrackResolutions.C+");
  gROOT->ProcessLine(".L RichPIDPerformance.C+");
  gROOT->ProcessLine(".L RichNCKPhotons.C+");
  gROOT->ProcessLine(".L RichMakeAllPlots.C+");

  // make the plots

  Rich::performance( "/usera/jonesc/cmtuser/Brunel_v34r0/Rec/Brunel/cmt/2008MC/2008MC-10000ev-expert-histos.root", "2008MC", " | 2008 MC" );

  Rich::performance( "/usera/jonesc/cmtuser/Brunel_v34r0/Rec/Brunel/cmt/DC06/NewHPDQE_25pc-dst-10000ev-expert-histos.root", "DC06", " | DC06" );

}
