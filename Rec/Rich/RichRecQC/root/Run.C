
{

  // load the macros needed
  gROOT->ProcessLine(".L Utilities.C+");
  gROOT->ProcessLine(".L RichCKResolution.C+");
  gROOT->ProcessLine(".L RichTrackResolutions.C+");
  gROOT->ProcessLine(".L RichPIDPerformance.C+");
  gROOT->ProcessLine(".L RichNCKPhotons.C+");
  gROOT->ProcessLine(".L RichMakeAllPlots.C+");

  // make the plots

  //RICH::performance( "/usera/jonesc/RootFiles/NewOffline.histos.root", "Offline", " | Offline | All Rads" );

  //RICH::performance( "/usera/jonesc/RootFiles/NewOfflineRayTrace.histos.root", "Offline-RayTrace", " | Offline RayTraced Photon Reco | All Rads" );

  //RICH::performance( "/usera/jonesc/RootFiles/FullHLT_FastLocal_AllEvents.histos.root", "FullHLT-Local", " | HLT Tracks | HLT Local" );
  //RICH::performance( "/usera/jonesc/RootFiles/FullHLT_FastGlobal_AllEvents.histos.root", "FullHLT-Global", " | HLT Tracks | HLT global" );

  //RICH::performance( "/usera/jonesc/RootFiles/NewHLTGlobal-OfflineTracks.histos.root", "HLTGlobal-OfflineTracks", " | Offline Tracks | HLT Global All Rads" );
  
  // Tk Reject
  //RICH::performance( "/usera/jonesc/RootFiles/NewOffline-10pcTkReject.histos.root", "Offline-10pcTkReject", " | Offline | 10% Track Reject | All Rads" );
  //RICH::performance( "/usera/jonesc/RootFiles/NewOffline-25pcTkReject.histos.root", "Offline-25pcTkReject", " | Offline | 25% Track Reject | All Rads" );
  //RICH::performance( "/usera/jonesc/RootFiles/NewOffline-50pcTkReject.histos.root", "Offline-50pcTkReject", " | Offline | 50% Track Reject | All Rads" );
  //RICH::performance( "/usera/jonesc/RootFiles/NewOffline-75pcTkReject.histos.root", "Offline-75pcTkReject", " | Offline | 75% Track Reject | All Rads" );
  //RICH::performance( "/usera/jonesc/RootFiles/NewOffline-90pcTkReject.histos.root", "Offline-90pcTkReject", " | Offline | 90% Track Reject | All Rads" );
  //RICH::performance( "/usera/jonesc/RootFiles/NewOffline-95pcTkReject.histos.root", "Offline-95pcTkReject", " | Offline | 95% Track Reject | All Rads" );

  // TK Add
  //RICH::performance( "/usera/jonesc/RootFiles/NewOffline-Add_25pcMCTracks.histos.root", "Offline-25pcAddMCTks", " | Offline | +25% Missing Tracks | All Rads" );
  //RICH::performance( "/usera/jonesc/RootFiles/NewOffline-Add_50pcMCTracks.histos.root", "Offline-50pcAddMCTks", " | Offline | +50% Missing Tracks | All Rads" );
  //RICH::performance( "/usera/jonesc/RootFiles/NewOffline-Add_75pcMCTracks.histos.root", "Offline-75pcAddMCTks", " | Offline | +75% Missing Tracks | All Rads" );
  //RICH::performance( "/usera/jonesc/RootFiles/NewOffline-Add_100pcMCTracks.histos.root", "Offline-100pcAddMCTks", " | Offline | +75% Missing Tracks | All Rads" );

  RICH::performance( "/usera/jonesc/NFS/RootFiles/Brunelv32r3-Gaussv30r5.histos.root", "gv30r5-standard", " | Standard Gauss v30r5" );
  //RICH::performance( "/usera/jonesc/NFS/RootFiles/Brunelv32r3-Gaussv30r5-HPDReflBkgs.histos.root", "gv30r5-newsim", " | New RICH Simulation" );
  //RICH::performance( "/usera/jonesc/NFS/RootFiles/Brunelv32r3-Gaussv30r5-HPDRefls-MCRej.histos.root", "gv30r5-newsim-mcBkgPurge", " | New RICH Simulation + MC remove background" );

}
