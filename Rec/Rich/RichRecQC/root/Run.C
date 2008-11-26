
{

  // load the macros needed
  gROOT->ProcessLine(".L Utilities.C+");
  gROOT->ProcessLine(".L RichCKResolution.C+");
  gROOT->ProcessLine(".L RichTrackResolutions.C+");
  gROOT->ProcessLine(".L RichPIDPerformance.C+");
  gROOT->ProcessLine(".L RichNCKPhotons.C+");
  gROOT->ProcessLine(".L RichMakeAllPlots.C+");

  // make the plots

  Rich::performance( "/usera/jonesc/cmtuser/Brunel_v33r3p1/Rec/Brunel/cmt/2008MC-10000ev-histos.root", "RealHPDQE", " | Real HPD QE" );

  //Rich::performance( "/usera/jonesc/cmtuser/Brunel_v33r3p1/Rec/Brunel/cmt/2008MC-FixedHPDQE-10000ev-histos.root", "OldHPDQE", " | DC06 HPD QE" );

}
