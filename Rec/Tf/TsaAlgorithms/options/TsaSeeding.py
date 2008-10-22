from TrackSys.Configuration import *

from Configurables import ( Tf__Tsa__Seed, Tf__Tsa__SeedTrackCnv,
                            Tf__Tsa__ITXSearch, Tf__Tsa__OTXSearch,
                            Tf__Tsa__ITStereoSearch, Tf__Tsa__OTStereoSearch,
                            Tf__Tsa__TStationHitManager, Tf__Tsa__SeedTrackCnvTool,
                            Tf__Tsa__StubFind, Tf__Tsa__StubExtender )

Tf__Tsa__SeedTrackCnv( "TsaSeedTrackCnv").outputLocation = "Rec/Track/Seed"
## Hit cleaning options
Tf__Tsa__Seed("TsaSeed").addTool( Tf__Tsa__TStationHitManager("TsaDataManager") )
Tf__Tsa__TStationHitManager("TsaDataManager").CleanITHits = True;
Tf__Tsa__TStationHitManager("TsaDataManager").CleanOTHits = True;

tsaSeed = Tf__Tsa__Seed("TsaSeed")
  
tsaSeed.addTool(Tf__Tsa__ITXSearch(), name="xSearchS0")
tsaSeed.addTool(Tf__Tsa__ITXSearch(), name="xSearchS1")
tsaSeed.addTool(Tf__Tsa__ITXSearch(), name="xSearchS2")
tsaSeed.addTool(Tf__Tsa__OTXSearch(), name="xSearchS3")
tsaSeed.addTool(Tf__Tsa__OTXSearch(), name="xSearchS4")

tsaSeed.addTool(Tf__Tsa__ITStereoSearch(), name="stereoS0")
tsaSeed.addTool(Tf__Tsa__ITStereoSearch(), name="stereoS1")
tsaSeed.addTool(Tf__Tsa__ITStereoSearch(), name="stereoS2")
tsaSeed.addTool(Tf__Tsa__OTStereoSearch(), name="stereoS3")
tsaSeed.addTool(Tf__Tsa__OTStereoSearch(), name="stereoS4")       

tsaSeed.xSearchS0.sector = 0
tsaSeed.xSearchS1.sector = 1
tsaSeed.xSearchS2.sector = 2
tsaSeed.xSearchS3.sector = 3
tsaSeed.xSearchS4.sector = 4

tsaSeed.stereoS0.sector = 0
tsaSeed.stereoS1.sector = 1
tsaSeed.stereoS2.sector = 2
tsaSeed.stereoS3.sector = 3
tsaSeed.stereoS4.sector = 4

if TrackSys().fieldOff():
    Tf__Tsa__SeedTrackCnv( "TsaSeedTrackCnv").addTool( Tf__Tsa__SeedTrackCnvTool("SeedTrackCnvTool"))
    Tf__Tsa__SeedTrackCnvTool("SeedTrackCnvTool").pFromCurvature = True
    
    tsaSeed.xSearchS3.sxCut = 0.40;
    tsaSeed.xSearchS4.sxCut = 0.40;
    tsaSeed.xSearchS3.collectPolicy = "Linear";
    tsaSeed.xSearchS4.collectPolicy = "Linear";
    tsaSeed.xSearchS3.sx2Cut = 0.330;
    tsaSeed.xSearchS4.sx2Cut = 0.330;
    
    tsaSeed.xSearchS0.dthCut = 0.08;
    tsaSeed.xSearchS1.dthCut = 0.08;
    tsaSeed.xSearchS2.dthCut = 0.08;
    tsaSeed.xSearchS3.dthCut = 0.08;
    tsaSeed.xSearchS4.dthCut = 0.08;
    
    tsaSeed.xSearchS0.x0Cut = 400.;
    tsaSeed.xSearchS1.x0Cut = 400.;
    tsaSeed.xSearchS2.x0Cut = 400.;
    tsaSeed.xSearchS3.x0Cut = 400.;
    tsaSeed.xSearchS4.x0Cut = 400.;
    
    tsaSeed.xSearchS0.sxCut = 0.10;
    tsaSeed.xSearchS1.sxCut = 0.10;
    tsaSeed.xSearchS2.sxCut = 0.10;
    tsaSeed.xSearchS0.sx2Cut = 0.08; 
    tsaSeed.xSearchS1.sx2Cut = 0.08;
    tsaSeed.xSearchS2.sx2Cut = 0.08;
    
    tsaSeed.addTool(Tf__Tsa__StubFind, "stubFind")
    tsaSeed.addTool(Tf__Tsa__StubExtender, "stubExtender")
    tsaSeed.stubFind.sxCut = 0.1;
    tsaSeed.stubFind.dAngle = 0.08;
    tsaSeed.stubExtender.y0Cut1 = 100.;
    tsaSeed.stubExtender.txCut = 0.1;
    tsaSeed.stubExtender.tyCut = 0.1;
      
  
