
{

  // load the macros needed
  gROOT->ProcessLine(".L Utilities.C+");
  gROOT->ProcessLine(".L RichCKResolution.C+");
  gROOT->ProcessLine(".L RichTrackResolutions.C+");
  gROOT->ProcessLine(".L RichPIDPerformance.C+");
  gROOT->ProcessLine(".L RichNCKPhotons.C+");
  gROOT->ProcessLine(".L RichMakeAllPlots.C+");

  // make the plots

  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/DC06-Brv30r14.histos.root", "DC06-Brv30r14", " | DC06 Brunel v30r14" );
  
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-AllRads-AllTracks_CloneKill.histos.root", "Offline-AllRads-AllTracks-CloneKill", " | Offline | All Rads | Clone Kill" );

  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-AllRads-AllTracks.histos.root", "Offline-AllRads-AllTracks", " | Offline | All Rads" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-AllRads-AllTracks_2.histos.root", "Offline-AllRads-AllTracks", " | Offline | All Rads" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-GasRads-AllTracks.histos.root", "Offline-GasRads-AllTracks", " | Offline | Gas Rads" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-AerogelOnly-AllTracks.histos.root", "Offline-AerogelOnly-AllTracks", " | Offline | Aerogel Only" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-Rich1GasOnly-AllTracks.histos.root", "Offline-Rich1GasOnly-AllTracks", " | Offline | Rich1Gas Only" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-Rich2GasOnly-AllTracks.histos.root", "Offline-Rich2GasOnly-AllTracks", " | Offline | Rich2Gas Only" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-Rich1Only-AllTracks.histos.root", "Offline-Rich1Only-AllTracks", " | Offline | Rich1 Only" );

  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-AllRads-LongTracks.histos.root", "Offline-AllRads-LongTracks", " | Offline | All Rads | Long Tracks Only" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-GasRads-LongTracks.histos.root", "Offline-GasRads-LongTracks", " | Offline | Gas Rads | Long Tks Only" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-AllRads-SeedTracks.histos.root", "Offline-AllRads-SeedTracks", " | Offline | All Rads | Seed Tracks Only" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-AllRads-VeloTTTracks.histos.root", "Offline-AllRads-VeloTTTks", " | Offline | All Rads | VeloTT Tracks Only" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-AllRads-KsTracks.histos.root", "Offline-AllRads-KsTracks", " | Offline | All Rads | Ks Tracks Only" );

  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-AllRads-NoSeedTracks.histos.root", "Offline-AllRads-NoSeedTracks", " | Offline | All Rads | No Seed Tracks" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-AllRads-NoKsTracks.histos.root", "Offline-AllRads-NoKsTracks", " | Offline | All Rads | No Ks Tracks" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-AllRads-NoVeloTTTracks.histos.root", "Offline-AllRads-NoVeloTTTracks", " | Offline | All Rads | No VeloTT Tracks" );

  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Fast-AllRads-LongTracks.histos.root", "Fast-AllRads-LongTracks-2its", " | Fast Offline | All Rads" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Fast-GasRads-LongTracks.histos.root", "Fast-GasRads-LongTracks-2its", " | Fast Offline | Gas Rads" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Fast-AerogelOnly-LongTracks.histos.root", "Fast-AerogelOnly-LongTracks-2its", " | Fast Offline | Aerogel Only" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Fast-Rich1GasOnly-LongTracks.histos.root", "Fast-Rich1GasOnly-LongTracks-2its", " | Fast Offline | Rich1Gas Only" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Fast-Rich2GasOnly-LongTracks.histos.root", "Fast-Rich2GasOnly-LongTracks-2its", " | Fast Offline | Rich2Gas Only" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Fast-Rich1Only-LongTracks.histos.root", "Fast-Rich1Only-LongTracks-2its", " | Fast Offline | Rich1 Only" );

  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/RichHLT-AllRads.histos.root", "HLT-AllRads-2its", " | HLT | All Rads" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/RichHLT-GasRads.histos.root", "HLT-GasRads-2its", " | HLT | Gas Rads" );

  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-AllRads-LongTracks-90pcInEff.histos.root", "Offline-AllRads-LongTracks_90pcInEff", " | Offline | Long Tracks Only | 90% Tk InEff | All Rads" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-AllRads-LongTracks-75pcInEff.histos.root", "Offline-AllRads-LongTracks_75pcInEff", " | Offline | Long Tracks Only | 75% Tk InEff | All Rads" );
  //RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-AllRads-LongTracks-50pcInEff.histos.root", "Offline-AllRads-LongTracks_50pcInEff", " | Offline | Long Tracks Only | 50% Tk InEff | All Rads" );

  RICH::performance( "/var/work/pcgs/jonesc/RootFiles/Offline-AllRads-AllTracks-Brv31r9Check.histos.root", "Offline-AllRads-AllTracks-Brv31r9Check", " | Offline | All Rads | Brunel v31r9" );

}
