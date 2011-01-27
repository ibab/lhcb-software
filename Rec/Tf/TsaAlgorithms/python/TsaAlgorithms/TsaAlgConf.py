from TrackSys.Configuration import *

from Configurables import ( Tf__Tsa__Seed, Tf__Tsa__SeedTrackCnv,
                            Tf__Tsa__ITXSearch, Tf__Tsa__OTXSearch,
                            Tf__Tsa__ITStereoSearch, Tf__Tsa__OTStereoSearch,
                            Tf__Tsa__TStationHitManager, Tf__Tsa__SeedTrackCnvTool,
                            Tf__Tsa__StubFind, Tf__Tsa__StubExtender,
                            Tf__Tsa__SeedAddHits, Tf__Tsa__Likelihood,Tf__Tsa__StubLinker, Tf__Tsa__StubExtender, Tf__Tsa__StubFind
                           )

class TsaSeedConf(object):
    '''Apply configuration to a TsaSeed algorithm'''
    OnlyUnusedHits = False
    def configureAlg(self, tsaSeed = Tf__Tsa__Seed("TsaSeed"), tsaConv=Tf__Tsa__SeedTrackCnv( "TsaSeedTrackCnv"), tsaDataMan=Tf__Tsa__TStationHitManager("ToolSvc.TsaDataManager")):
        
        tsaConv.outputLocation = "Rec/Track/Seed"
        ## Hit cleaning options
        #This is a public tool so doesn't need to be added to anything
        #tsaSeed.addTool( Tf__Tsa__TStationHitManager,"TsaDataManager" )
        #tsaSeed.TsaDataManager.CleanITHits = True
        #tsaSeed.TsaDataManager.CleanOTHits = True
        tsaDataMan.CleanITHits = True
        tsaDataMan.CleanOTHits = True
        
        tsaSeed.addTool(Tf__Tsa__ITXSearch, name="xSearchS0")
        tsaSeed.addTool(Tf__Tsa__ITXSearch, name="xSearchS1")
        tsaSeed.addTool(Tf__Tsa__ITXSearch, name="xSearchS2")
        tsaSeed.addTool(Tf__Tsa__OTXSearch, name="xSearchS3")
        tsaSeed.addTool(Tf__Tsa__OTXSearch, name="xSearchS4")
        
        tsaSeed.addTool(Tf__Tsa__ITStereoSearch, name="stereoS0")
        tsaSeed.addTool(Tf__Tsa__ITStereoSearch, name="stereoS1")
        tsaSeed.addTool(Tf__Tsa__ITStereoSearch, name="stereoS2")
        tsaSeed.addTool(Tf__Tsa__OTStereoSearch, name="stereoS3")
        tsaSeed.addTool(Tf__Tsa__OTStereoSearch, name="stereoS4")       
        
        self.configureTools([tsaSeed.xSearchS0,tsaSeed.xSearchS1,
                             tsaSeed.xSearchS2,tsaSeed.xSearchS3,tsaSeed.xSearchS4],
                            [tsaSeed.stereoS0, tsaSeed.stereoS1, tsaSeed.stereoS2,
                             tsaSeed.stereoS3, tsaSeed.stereoS4])
        
        tsaSeed.addTool(Tf__Tsa__SeedAddHits, name="SeedAddHits")
        tsaSeed.SeedAddHits.OnlyUnusedHits = self.OnlyUnusedHits
        tsaSeed.addTool(Tf__Tsa__StubLinker, name = "stubLinker")
        tsaSeed.addTool(Tf__Tsa__StubExtender, "stubExtender")
        tsaSeed.addTool(Tf__Tsa__StubFind, "stubFinder")
        
        if TrackSys().fieldOff():
            tsaConv.addTool( Tf__Tsa__SeedTrackCnvTool, name="SeedTrackCnvTool")
            tsaConv.SeedTrackCnvTool.pFromCurvature = True
            
            tsaSeed.addTool(Tf__Tsa__StubFind, "stubFind")
            tsaSeed.addTool(Tf__Tsa__StubExtender, "stubExtender")
            tsaSeed.stubFind.sxCut = 0.1;
            tsaSeed.stubFind.dAngle = 0.08;
            tsaSeed.stubExtender.y0Cut1 = 100.;
            tsaSeed.stubExtender.txCut = 0.1;
            tsaSeed.stubExtender.tyCut = 0.1;
            
        if TrackSys().noDrifttimes():
            tsaSeed.calcLikelihood = False
            
        
        
    def configureTools(self, xTools, stereoTools):
        
        if len(xTools)<5 or len(stereoTools)<5:
            raise ValueError, "TsaAlgTools, you must provide 5 xTools and 5 stereoTools to configure"
        
        
        for xtool in xTools:
            xtool.sector = xTools.index(xtool)
            xtool.OnlyUnusedHits = self.OnlyUnusedHits
                
        for stool in stereoTools:
            stool.sector = stereoTools.index(stool)
            stool.OnlyUnusedHits = self.OnlyUnusedHits
        
        
        
        
        if TrackSys().fieldOff():
            
            xTools[3].sxCut = 0.40;
            xTools[4].sxCut = 0.40;
            xTools[3].collectPolicy = "Linear";
            xTools[4].collectPolicy = "Linear";
            xTools[3].sx2Cut = 0.330;
            xTools[4].sx2Cut = 0.330;
            
            for xtool in xTools:
                xtool.dthCut = 0.08;
                xtool.x0Cut = 400.;
                xtool.sxCut = 0.10;
            
            
        if TrackSys().noDrifttimes():
                
            xTools[3].outlierCut = 21
            xTools[4].outlierCut = 21
            stereoTools[3].outlierCut = 21
            stereoTools[4].outlierCut = 21
            xTools[3].deltaX = 2.7
            xTools[4].deltaX = 2.7
            xTools[3].x0Cut = 800
            xTools[4].x0Cut = 800
            stereoTools[3].syCut = 0.08 
            stereoTools[4].syCut = 0.08
            stereoTools[3].yTol = 25
            stereoTools[4].yTol = 25
            stereoTools[3].yTol2 = 20
            stereoTools[4].yTol2 = 20
            

        xTools[0].outlierCut = 6.0
        xTools[1].outlierCut = 6.0
        xTools[2].outlierCut = 6.0
        xTools[3].outlierCut = 6.0
        xTools[4].outlierCut = 6.0
        stereoTools[0].outlierCut = 6.0
        stereoTools[1].outlierCut = 6.0
        stereoTools[2].outlierCut = 6.0
        stereoTools[3].outlierCut = 6.0
        stereoTools[4].outlierCut = 6.0
        xTools[0].deltaX = 0.7
        xTools[1].deltaX = 0.7
        xTools[2].deltaX = 0.7
        #            xTools[0].x0Cut = 2000.
        #            xTools[1].x0Cut = 2000.
        #            xTools[2].x0Cut = 2000.
        xTools[3].deltaX = 1.2
        xTools[4].deltaX = 1.2
        #            xTools[0].dthCut = 0.45
        #            xTools[1].dthCut = 0.45
        #            xTools[2].dthCut = 0.45
        #            xTools[3].dthCut = 0.75
        #            xTools[4].dthCut = 0.75
        stereoTools[0].syCut = 0.02
        stereoTools[1].syCut = 0.02
        stereoTools[2].syCut = 0.02
        stereoTools[0].yTol = 10.0
        stereoTools[1].yTol = 10.0
        stereoTools[2].yTol = 10.0
        stereoTools[3].nHit = 14
        stereoTools[4].nHit = 14
        stereoTools[3].syCut = 0.08
        stereoTools[4].syCut = 0.08
        stereoTools[3].yTol2 = 10.0
        stereoTools[4].yTol2 = 10.0
        stereoTools[3].maxDriftRadius = 3.
        xTools[3].maxDriftRadius = 3.
        stereoTools[4].maxDriftRadius = 3.
        xTools[4].maxDriftRadius = 3.
        
        
