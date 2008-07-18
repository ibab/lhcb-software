from Brunel.Configuration import *

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
  
Tf__Tsa__Seed("TsaSeed").addTool(Tf__Tsa__ITXSearch(), name="xSearchS0")
Tf__Tsa__Seed("TsaSeed").addTool(Tf__Tsa__ITXSearch(), name="xSearchS1")
Tf__Tsa__Seed("TsaSeed").addTool(Tf__Tsa__ITXSearch(), name="xSearchS2")
Tf__Tsa__Seed("TsaSeed").addTool(Tf__Tsa__OTXSearch(), name="xSearchS3")
Tf__Tsa__Seed("TsaSeed").addTool(Tf__Tsa__OTXSearch(), name="xSearchS4")

Tf__Tsa__Seed("TsaSeed").addTool(Tf__Tsa__ITStereoSearch(), name="stereoS0")
Tf__Tsa__Seed("TsaSeed").addTool(Tf__Tsa__ITStereoSearch(), name="stereoS1")
Tf__Tsa__Seed("TsaSeed").addTool(Tf__Tsa__ITStereoSearch(), name="stereoS2")
Tf__Tsa__Seed("TsaSeed").addTool(Tf__Tsa__OTStereoSearch(), name="stereoS3")
Tf__Tsa__Seed("TsaSeed").addTool(Tf__Tsa__OTStereoSearch(), name="stereoS4")       

Tf__Tsa__Seed("TsaSeed").xSearchS0.sector = 0
Tf__Tsa__Seed("TsaSeed").xSearchS1.sector = 1
Tf__Tsa__Seed("TsaSeed").xSearchS2.sector = 2
Tf__Tsa__Seed("TsaSeed").xSearchS3.sector = 3
Tf__Tsa__Seed("TsaSeed").xSearchS4.sector = 4

Tf__Tsa__Seed("TsaSeed").stereoS0.sector = 0
Tf__Tsa__Seed("TsaSeed").stereoS1.sector = 1
Tf__Tsa__Seed("TsaSeed").stereoS2.sector = 2
Tf__Tsa__Seed("TsaSeed").stereoS3.sector = 3
Tf__Tsa__Seed("TsaSeed").stereoS4.sector = 4

if Brunel().getProp("fieldOff"):
    Tf__Tsa__SeedTrackCnv( "TsaSeedTrackCnv").addTool( Tf__Tsa__SeedTrackCnvTool("SeedTrackCnvTool"))
    Tf__Tsa__SeedTrackCnvTool("SeedTrackCnvTool").pFromCurvature = True
    
    Tf__Tsa__Seed("TsaSeed").xSearchS3.sxCut = 0.40;
    Tf__Tsa__Seed("TsaSeed").xSearchS4.sxCut = 0.40;
    Tf__Tsa__Seed("TsaSeed").xSearchS3.collectPolicy = "Linear";
    Tf__Tsa__Seed("TsaSeed").xSearchS4.collectPolicy = "Linear";
    Tf__Tsa__Seed("TsaSeed").xSearchS3.sx2Cut = 0.330;
    Tf__Tsa__Seed("TsaSeed").xSearchS4.sx2Cut = 0.330;
    
    Tf__Tsa__Seed("TsaSeed").xSearchS0.dthCut = 0.08;
    Tf__Tsa__Seed("TsaSeed").xSearchS1.dthCut = 0.08;
    Tf__Tsa__Seed("TsaSeed").xSearchS2.dthCut = 0.08;
    Tf__Tsa__Seed("TsaSeed").xSearchS3.dthCut = 0.08;
    Tf__Tsa__Seed("TsaSeed").xSearchS4.dthCut = 0.08;
    
    Tf__Tsa__Seed("TsaSeed").xSearchS0.x0Cut = 400.;
    Tf__Tsa__Seed("TsaSeed").xSearchS1.x0Cut = 400.;
    Tf__Tsa__Seed("TsaSeed").xSearchS2.x0Cut = 400.;
    Tf__Tsa__Seed("TsaSeed").xSearchS3.x0Cut = 400.;
    Tf__Tsa__Seed("TsaSeed").xSearchS4.x0Cut = 400.;
    
    Tf__Tsa__Seed("TsaSeed").xSearchS0.sxCut = 0.10;
    Tf__Tsa__Seed("TsaSeed").xSearchS1.sxCut = 0.10;
    Tf__Tsa__Seed("TsaSeed").xSearchS2.sxCut = 0.10;
    Tf__Tsa__Seed("TsaSeed").xSearchS0.sx2Cut = 0.08; 
    Tf__Tsa__Seed("TsaSeed").xSearchS1.sx2Cut = 0.08;
    Tf__Tsa__Seed("TsaSeed").xSearchS2.sx2Cut = 0.08;
    
    Tf__Tsa__Seed("TsaSeed").addTool(Tf__Tsa__StubFind, "stubFind")
    Tf__Tsa__Seed("TsaSeed").addTool(Tf__Tsa__StubExtender, "stubExtender")
    Tf__Tsa__Seed("TsaSeed").stubFind.sxCut = 0.1;
    Tf__Tsa__Seed("TsaSeed").stubFind.dAngle = 0.08;
    Tf__Tsa__Seed("TsaSeed").stubExtender.y0Cut1 = 100.;
    Tf__Tsa__Seed("TsaSeed").stubExtender.txCut = 0.1;
    Tf__Tsa__Seed("TsaSeed").stubExtender.tyCut = 0.1;
      
  
