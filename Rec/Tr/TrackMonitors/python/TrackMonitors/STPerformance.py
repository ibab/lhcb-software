def STPerformanceStudy(Name_Prefix = "STPerf_", Efficiency = True, Residuals = True, ActivateFullClusters = True, TrackContainter = False):
    #Define tracks for ST Performance
    if not TrackContainter:
        from Configurables import TrackSelector,TrackContainerCopy
        goodtracksselector  = TrackContainerCopy(name = Name_Prefix+"Selection",
                                                inputLocation  =   '/Event/Rec/Track/Best',
                                                outputLocation =   '/Event/Rec/Track/PerformanceTracks')  
        goodtracksselector.addTool( TrackSelector, name = "Selector")
        goodtracksselector.Selector.MinPCut    = 10000.
        goodtracksselector.Selector.MaxChi2Cut = 2.
        goodtracksselector.Selector.MaxChi2PerDoFMatch = 2.
        goodtracksselector.Selector.MaxChi2PerDoFDownstream = 2.
        goodtracksselector.Selector.MaxChi2PerDoFVelo = 2.
        goodtracksselector.Selector.TrackTypes = [ "Long" ]
        TrackLocation = '/Event/Rec/Track/PerformanceTracks'
    else:
        TrackLocation = TrackContainter

    #Define clusters used for ST Efficiency studies
    from Configurables import STClusterCollector
    # Search window (residual) for STEfficiency and STClusterCollector
    window = 0.4
    # Tolerance for X an Y in the first estimate of the track crossing the silicion position
    # and central position of the cluster
    xTol = 1.
    yTol = 0.

    # Collecting the ST clusters
    itClusterCollector = STClusterCollector( "ToolSvc.ITClusterCollector" )
    itClusterCollector.DetType           = "IT"
    itClusterCollector.ignoreHitsOnTrack = False
    itClusterCollector.xTol              = xTol
    itClusterCollector.yTol              = yTol
    itClusterCollector.window            = window
    itClusterCollector.MagFieldOn        = True
    itClusterCollector.dataLocation      = "/Event/Raw/IT/Clusters"

    ttClusterCollector = STClusterCollector( "ToolSvc.TTClusterCollector" )
    ttClusterCollector.DetType           = "TT"
    ttClusterCollector.ignoreHitsOnTrack = False
    ttClusterCollector.xTol              = xTol
    ttClusterCollector.yTol              = yTol
    ttClusterCollector.window            = window
    ttClusterCollector.MagFieldOn        = True
    ttClusterCollector.dataLocation      = "/Event/Raw/TT/Clusters"


    #Configuring ST Efficiency algorithms
    from Configurables import STEfficiency
    ttEff = STEfficiency( Name_Prefix+"TTHitEfficiency" )
    ttEff.TracksInContainer  = TrackLocation
    ttEff.DetType            = "TT"
    ttEff.Cuts               = [window]
    ttEff.XLayerCut          = window #0.4
    ttEff.StereoLayerCut     = window #0.4
    ttEff.MinExpectedSectors = 2 #2
    ttEff.MaxNbResSectors    = 10 #10
    ttEff.MinStationPassed   = 1 #1
    # Edge size excluded of the computation
    ttEff.MinDistToEdgeX     = 2 #2
    ttEff.MinDistToEdgeY     = 2 #2
    ttEff.ResidualsPlot      = False
    ttEff.FullDetail         = False
    ttEff.TH1DSummaryHist    = True
    ttEff.EfficiencyPlot     = False
    ttEff.SingleHitPerSector = True #True
    ttEff.TakeEveryHit       = True #True
    ttEff.OutputLevel        = 3

    itEff = STEfficiency( Name_Prefix+"ITHitEfficiency" )
    itEff.TracksInContainer  = TrackLocation
    itEff.DetType            = "IT"
    itEff.Cuts               = [window]
    itEff.XLayerCut          = window #0.4
    itEff.StereoLayerCut     = window #0.4
    itEff.MinExpectedSectors =  6 #6
    itEff.MaxNbResSectors    = 10
    itEff.MinStationPassed   =  3 #3
    # Edge size excluded of the computation
    itEff.MinDistToEdgeX     = 2 #2
    itEff.MinDistToEdgeY     = 2 #2
    itEff.ResidualsPlot      = False
    itEff.FullDetail         = False
    itEff.TH1DSummaryHist    = True
    ttEff.EfficiencyPlot     = False
    itEff.SingleHitPerSector = True
    itEff.TakeEveryHit       = True
    itEff.OutputLevel        = 3


    #Configuring Track Monitors to work in performance mode 
    from Configurables import ITTrackMonitor, TTTrackMonitor
    itMon = ITTrackMonitor(Name_Prefix+'ITTrackMonitor')
    itMon.TracksInContainer = TrackLocation
    itMon.FullDetail = False
    itMon.plotsByLayer = False
    itMon.plotsBySector = False
    itMon.ProfileSummaryHist = False
    itMon.TH2DSummaryHist = True
    itMon.minNumITHits = 6

    ttMon = TTTrackMonitor(Name_Prefix+'TTTrackMonitor')
    ttMon.TracksInContainer = TrackLocation
    ttMon.FullDetail = False
    ttMon.plotsBySector = False
    ttMon.ProfileSummaryHist = False
    ttMon.TH2DSummaryHist = True
    ttMon.minNumTTHits = 2
    

    #These lines activates Full clusters. For .raw data only!
    #Access to the full clusters
    if ActivateFullClusters:
        from Configurables import TrackSys
        TrackSys().ExpertTracking = [ "fullClustersHLT1", "fullClustersHLT2" ]
    

    # Post configuration actions
    def PostConfigurationActions():
        from Configurables import GaudiSequencer
        if not TrackContainter:
            GaudiSequencer('MoniTrSeq').Members += [goodtracksselector]
        if Efficiency:
            GaudiSequencer('MoniTrSeq').Members += [itEff,ttEff]
        if Residuals:
            GaudiSequencer('MoniTrSeq').Members += [ttMon, itMon]


    from GaudiKernel.Configurable import appendPostConfigAction
    appendPostConfigAction(PostConfigurationActions)

    return True
#
#def OldSTPerformance():
#    from Configurables import TrackSelector,TrackContainerCopy
#    goodtracksselector  = TrackContainerCopy(name = "PerformanceTracks",
#                                            inputLocation  =   '/Event/Rec/Track/Best',
#                                            outputLocation =   '/Event/Rec/Track/PerformanceTracks')  
#    goodtracksselector.addTool( TrackSelector, name = "Selector")
#    goodtracksselector.Selector.MinPCut    = 10000.
#    goodtracksselector.Selector.MaxChi2Cut = 2.
#    goodtracksselector.Selector.MaxChi2PerDoFMatch = 2.
#    goodtracksselector.Selector.MaxChi2PerDoFDownstream = 2.
#    goodtracksselector.Selector.MaxChi2PerDoFVelo = 2.
#    goodtracksselector.Selector.TrackTypes = [ "Long" ]
    #
#
#    from Configurables import STClusterCollector, STSelectChannelIDByElement
#    # Search window (residual) for STEfficiency
#    itwindow = 0.4
#    # Search window (residual) for STClusterCollector
#    itcollectorwindow = itwindow
#    # Tolerance for X an Y in the first estimate of the track crossing the silicion position
#    # and central position of the cluster
#    itxTol = 1.
#    ityTol = 0.
#
#    # Search window (residual) for STEfficiency
#    ttwindow = 0.4
#    # Search window (residual) for STClusterCollector
#    ttcollectorwindow = ttwindow
#    # Tolerance for X an Y in the first estimate of the track crossing the silicion position
#    # and central position of the cluster
#    ttxTol = 1.
#    ttyTol = 0.
#
#    # Collecting the ST clusters
#    itClusterCollector = STClusterCollector( "ToolSvc.ITClusterCollector" )
#    itClusterCollector.DetType           = "IT"
#    itClusterCollector.ignoreHitsOnTrack = False
#    itClusterCollector.xTol              = itxTol
#    itClusterCollector.yTol              = ityTol
#    itClusterCollector.window            = itcollectorwindow
#    itClusterCollector.MagFieldOn        = True
#    itClusterCollector.dataLocation      = "/Event/Raw/IT/Clusters"
#
#    ttClusterCollector = STClusterCollector( "ToolSvc.TTClusterCollector" )
#    ttClusterCollector.DetType           = "TT"
#    ttClusterCollector.ignoreHitsOnTrack = False
#    ttClusterCollector.xTol              = ttxTol
#    ttClusterCollector.yTol              = ttyTol
#    ttClusterCollector.window            = ttcollectorwindow
#    ttClusterCollector.MagFieldOn        = True
#    ttClusterCollector.dataLocation      = "/Event/Raw/TT/Clusters"
#
#    from Configurables import STEfficiency
#
#    ttEff = STEfficiency( "TTHitEfficiency" )
#    ttEff.TracksInContainer  = "/Event/Rec/Track/PerformanceTracks"
#    ttEff.DetType            = "TT"
#    ttEff.Cuts               = [4.e-1]
#    ttEff.XLayerCut          = 0.4 #0.4
#    ttEff.StereoLayerCut     = 0.4 #0.4
#    ttEff.MinExpectedSectors = 2 #2
#    ttEff.MaxNbResSectors    = 10 #10
#    ttEff.MinStationPassed   = 1 #1
#    # Edge size excluded of the computation
#    ttEff.MinDistToEdgeX     = 2 #2
#    ttEff.MinDistToEdgeY     = 2 #2
#    ttEff.ResidualsPlot      = False
#    ttEff.FullDetail         = False
#    ttEff.TH1DSummaryHist    = True
#    ttEff.EfficiencyPlot     = False
#    ttEff.SingleHitPerSector = True #True
#    ttEff.TakeEveryHit       = True #True
#    ttEff.OutputLevel        = 3
    #
    #
#
#    itEff = STEfficiency( "ITHitEfficiency" )
#    itEff.TracksInContainer  = "/Event/Rec/Track/PerformanceTracks"
#    itEff.DetType            = "IT"
#    itEff.Cuts               = [4.e-1]
#    itEff.XLayerCut          = 0.4 #0.4
#    itEff.StereoLayerCut     = 0.4 #0.4
#    itEff.MinExpectedSectors =  6 #6
#    itEff.MaxNbResSectors    = 10
#    itEff.MinStationPassed   =  3 #3
#    # Edge size excluded of the computation
#    itEff.MinDistToEdgeX     = 2 #2
#    itEff.MinDistToEdgeY     = 2 #2
#    itEff.ResidualsPlot      = False
#    itEff.FullDetail         = False
#    itEff.TH1DSummaryHist    = True
#    ttEff.EfficiencyPlot     = False
#    itEff.SingleHitPerSector = True
#    itEff.TakeEveryHit       = True
#    itEff.OutputLevel        = 3
#
#    from Configurables import ITTrackMonitor, TTTrackMonitor
#
#    itMon = ITTrackMonitor('ITTrackMonitor_performance')
#    itMon.TracksInContainer = "/Event/Rec/Track/PerformanceTracks"
#    # Dump all the plots
#    itMon.FullDetail = False
#    # Dump all the plots per layers
#    itMon.plotsByLayer = False
#    itMon.plotsBySector = False
#    itMon.ProfileSummaryHist = False
#    itMon.TH2DSummaryHist = True
#    # Minimum number of found hits on track
#    itMon.minNumITHits = 6
#
#    ttMon = TTTrackMonitor('TTTrackMonitor_performance')
#    ttMon.TracksInContainer = "/Event/Rec/Track/PerformanceTracks"
#    # Dump all the plots
#    ttMon.FullDetail = False
#    ttMon.plotsBySector = False
#    ttMon.ProfileSummaryHist = False
#    ttMon.TH2DSummaryHist = True
#    # Minimum number of found hits on track
#    ttMon.minNumTTHits = 2
    #
#
#    #These lines activates Full clusters. For .raw data only!
#    #Access to the full clusters
#    from Configurables import TrackSys
#    TrackSys().ExpertTracking = [ "fullClustersHLT1", "fullClustersHLT2" ]
    #
#
#    # Post configuration actions
#    def PostConfigurationActions():
#        from Configurables import GaudiSequencer
#        GaudiSequencer('MoniTrSeq').Members.insert(0, goodtracksselector)
#        GaudiSequencer('MoniTrSeq').Members += [
#                             itEff,ttEff,
#                             ttMon, itMon
#                             ]
#
#    from GaudiKernel.Configurable import appendPostConfigAction
#    appendPostConfigAction(PostConfigurationActions)
#    return True