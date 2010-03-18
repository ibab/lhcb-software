from TrackSys.Configuration import *

from Configurables import ( Tf__Tsa__Seed, Tf__Tsa__SeedTrackCnv,
                            Tf__Tsa__ITXSearch, Tf__Tsa__OTXSearch,
                            Tf__Tsa__ITStereoSearch, Tf__Tsa__OTStereoSearch,
                            Tf__Tsa__TStationHitManager, Tf__Tsa__SeedTrackCnvTool,
                            Tf__Tsa__StubFind, Tf__Tsa__StubExtender,
                            Tf__Tsa__SeedAddHits, Tf__Tsa__Likelihood,Tf__Tsa__StubLinker, Tf__Tsa__StubExtender
                           )

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



# Running over unused hits
OnlyUnusedHits = False

tsaSeed.xSearchS0.OnlyUnusedHits = OnlyUnusedHits
tsaSeed.xSearchS1.OnlyUnusedHits = OnlyUnusedHits
tsaSeed.xSearchS2.OnlyUnusedHits = OnlyUnusedHits
tsaSeed.xSearchS3.OnlyUnusedHits = OnlyUnusedHits
tsaSeed.xSearchS4.OnlyUnusedHits = OnlyUnusedHits

tsaSeed.stereoS0.OnlyUnusedHits = OnlyUnusedHits
tsaSeed.stereoS1.OnlyUnusedHits = OnlyUnusedHits
tsaSeed.stereoS2.OnlyUnusedHits = OnlyUnusedHits
tsaSeed.stereoS3.OnlyUnusedHits = OnlyUnusedHits
tsaSeed.stereoS4.OnlyUnusedHits = OnlyUnusedHits

tsaSeed.addTool(Tf__Tsa__SeedAddHits, name="SeedAddHits")
tsaSeed.SeedAddHits.OnlyUnusedHits = OnlyUnusedHits
tsaSeed.addTool(Tf__Tsa__StubLinker, name = "stubLinker")
tsaSeed.addTool(Tf__Tsa__StubExtender, "stubExtender")

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


if TrackSys().noDrifttimes():
    
    tsaSeed.xSearchS3.outlierCut = 21
    tsaSeed.xSearchS4.outlierCut = 21
    tsaSeed.stereoS3.outlierCut = 21
    tsaSeed.stereoS4.outlierCut = 21
    tsaSeed.xSearchS3.deltaX = 2.7
    tsaSeed.xSearchS4.deltaX = 2.7
    tsaSeed.xSearchS3.x0Cut = 800
    tsaSeed.xSearchS4.x0Cut = 800
    tsaSeed.stereoS3.syCut = 0.08 
    tsaSeed.stereoS4.syCut = 0.08
    tsaSeed.stereoS3.yTol = 25
    tsaSeed.stereoS4.yTol = 25
    tsaSeed.stereoS3.yTol2 = 20
    tsaSeed.stereoS4.yTol2 = 20
    tsaSeed.calcLikelihood = False

        

if  TrackSys().earlyData():
    tsaSeed.addTool(Tf__Tsa__Likelihood(), name = "likelihood")
    tsaSeed.likelihood.LikCut = -40
    tsaSeed.likelihood.outlierCut = 4.0
    tsaSeed.likelihood.OTEff = 0.88
    Tf__Tsa__SeedTrackCnv("TsaSeedTrackCnv").LikCut = -40
    tsaSeed.xSearchS0.outlierCut = 4.0
    tsaSeed.xSearchS1.outlierCut = 4.0
    tsaSeed.xSearchS2.outlierCut = 4.0
    tsaSeed.xSearchS3.outlierCut = 4.0
    tsaSeed.xSearchS4.outlierCut = 4.0
    tsaSeed.stereoS0.outlierCut = 4.0
    tsaSeed.stereoS1.outlierCut = 4.0
    tsaSeed.stereoS2.outlierCut = 4.0
    tsaSeed.stereoS3.outlierCut = 4.0
    tsaSeed.stereoS4.outlierCut = 4.0
    tsaSeed.xSearchS0.deltaX = 0.5
    tsaSeed.xSearchS1.deltaX = 0.5
    tsaSeed.xSearchS2.deltaX = 0.5
    tsaSeed.xSearchS3.deltaX = 1.0
    tsaSeed.xSearchS4.deltaX = 1.0
    tsaSeed.stereoS0.syCut = 0.009
    tsaSeed.stereoS1.syCut = 0.009
    tsaSeed.stereoS2.syCut = 0.009
    tsaSeed.stereoS3.syCut = 0.09
    tsaSeed.stereoS4.syCut = 0.09
    tsaSeed.stereoS3.yTol2 = 8.9
    tsaSeed.stereoS4.yTol2 = 8.9
    tsaSeed.stereoS3.maxDriftRadius = 3.
    tsaSeed.xSearchS3.maxDriftRadius = 3.
    tsaSeed.stereoS4.maxDriftRadius = 3.
    tsaSeed.xSearchS4.maxDriftRadius = 3.
    tsaSeed.SeedAddHits.dCut = 0.5
    tsaSeed.SeedAddHits.outlierCutParabola = 4.0
    tsaSeed.SeedAddHits.outlierCutLine = 4.0
    tsaSeed.stubLinker.outlierCutParabola = 4.0
    tsaSeed.stubLinker.outlierCutLine = 4.0
    tsaSeed.stubExtender.outlierCutParabola = 4.0
    tsaSeed.stubExtender.outlierCutLine = 4.0
    tsaSeed.stubExtender.dxCut = 1.0
    tsaSeed.stubExtender.dyCut = 40.
    
    
    


    

    
    
