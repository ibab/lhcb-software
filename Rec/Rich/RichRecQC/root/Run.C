
{

  // load the macros needed
  gROOT->ProcessLine(".L Utilities.C+");
  gROOT->ProcessLine(".L RichCKResolution.C+");
  gROOT->ProcessLine(".L RichTrackResolutions.C+");
  gROOT->ProcessLine(".L RichPIDPerformance.C+");
  gROOT->ProcessLine(".L RichNCKPhotons.C+");
  gROOT->ProcessLine(".L RichMakeAllPlots.C+");

  // make the plots

  //Rich::performance( "/usera/jonesc/NFS/RootFiles/DC09/NominalGasP/Brunel-histos.root", "NominalGas", " | DC09 - Nominal Gas" );

  //Rich::performance( "/usera/jonesc/NFS/RootFiles/DC09/GasP+10mbar/Brunel-histos.root", "p10mbarGasP", " | DC09 - +10mbar Expected Gas Pressure" );

  //Rich::performance( "/usera/jonesc/NFS/RootFiles/DC09/GasP+20mbar/Brunel-histos.root", "p20mbarGasP", " | DC09 - +20mbar Expected Gas Pressure" );

  //Rich::performance( "/usera/jonesc/NFS/RootFiles/DC09/GasP-10mbar/Brunel-histos.root", "m10mbarGasP", " | DC09 - -10mbar Expected Gas Pressure" );

  //Rich::performance( "/usera/jonesc/NFS/RootFiles/DC09/GasP-20mbar/Brunel-histos.root", "m20mbarGasP", " | DC09 - -20mbar Expected Gas Pressure" );

  //Rich::performance( "/usera/jonesc/NFS/RootFiles/DC09/GasP+1mbar/Brunel-histos.root", "p1mbarGasP", " | DC09 - +1mbar Expected Gas Pressure" );
  //Rich::performance( "/usera/jonesc/NFS/RootFiles/DC09/GasP-1mbar/Brunel-histos.root", "m1mbarGasP", " | DC09 - -1mbar Expected Gas Pressure" );

  //Rich::performance( "/usera/jonesc/NFS/RootFiles/DC09/GasP+2mbar/Brunel-histos.root", "p2mbarGasP", " | DC09 - +2mbar Expected Gas Pressure" );
  //Rich::performance( "/usera/jonesc/NFS/RootFiles/DC09/GasP-2mbar/Brunel-histos.root", "m2mbarGasP", " | DC09 - -2mbar Expected Gas Pressure" );




  //Rich::performance( "/usera/jonesc/NFS/RootFiles/DC09/AllHPDs/Brunel-histos.root", "noDisabledHPDs", " | DC09 | All HPDs" );

  // Rich::performance( "/usera/jonesc/NFS/RootFiles/DC09/MissingHPDsTopBotRICH2/Brunel-histos.root", "missingHPDsTopBotRICH2", " | DC09 | RICH2 Top/Bot 1 HPD Rows missing" );

  Rich::performance( "/usera/jonesc/NFS/RootFiles/DC09/MissingHPDsTop2Bot2RICH2/Brunel-histos.root", "missingHPDsTop2Bot2RICH2", " | DC09 | RICH2 Top/Bot 2 HPD Rows missing" );

}
