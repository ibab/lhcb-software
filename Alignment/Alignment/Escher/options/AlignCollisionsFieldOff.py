##############################################################################
# File for running Velo alignment on FEST data
#
# Syntax is:
#
#   gaudirun.py $ESCHEROPTS/AlignVeloSensors.py $ESCHEROPTS/2008-MC-Files.py
#   gaudirun.py $ESCHEROPTS/AlignVeloSensors.py $ESCHEROPTS/DC06-Files.py
#
#   $ESCHEROPTS/gaudiiter.py -n 10 -e -1 $ESCHEROPTS/AlignVeloHalves_ok2.py $ESCHEROPTS/2008-MC-Files.py
#
##############################################################################

from Configurables import ( Escher, TrackSys, RecSysConf )
from Gaudi.Configuration import *
from GaudiConf.Configuration import *

from GaudiKernel.ProcessJobOptions import importOptions
importOptions('$STDOPTS/PreloadUnits.opts')

#Escher().DataType  = "2008"
#Escher().DDDBtag   = "default"
#Escher().CondDBtag = "default"
Escher().Detectors = ["Velo"]
Escher().Kalman = True
Escher().Millepede = False
Escher().TrackContainer = "Rec/Track/Best" # Velo, VeloR, Long, Upstream, Downstream, Ttrack, Muon or Calo
Escher().EvtMax = 100
Escher().UseOracle = False
Escher().SpecialData = [ "fieldOff" ]
#TrackSys().ExpertTracking += [ "noMaterialCorrections"]
TrackSys().ExpertTracking += [ "simplifiedGeometry"]
#TrackSys().ExpertTracking += ["noDrifttimes" ]
TrackSys().TrackExtraInfoAlgorithms = ['']
#TrackSys().TrackPatRecAlgorithms = ["Velo","Forward","PatSeed","PatMatch","Downstream","VeloTT"]
TrackSys().TrackPatRecAlgorithms = ["Velo","Forward","PatSeed","PatMatch"]
TrackSys().TrackPatRecAlgorithms = ["Velo","PatSeed","PatMatch"]
#TrackSys().TrackPatRecAlgorithms = ["Velo","TsaSeed","PatMatch"]
TrackSys().TrackPatRecAlgorithms = ["Velo","TsaSeed","Match"]
#RecSysConf().RecoSequence = ["VELO","TT","IT","OT","Tr","Vertex"]
#RecSysConf().RecoSequence = ["VELO","TT","IT","OT","Tr"]
RecSysConf().RecoSequence = ["CALO","VELO","TT","IT","OT","Tr","Vertex"]
Escher().MoniSequence = ["VELO","Tr", "OT","ST"]


from Configurables import TAlignment
from TAlignment.Alignables import *
from TAlignment.SurveyConstraints import SurveyConstraints

elements = Alignables()
constraints = []
surveyconstraints = SurveyConstraints()

elements.Velo("Rx")
elements.VeloRight("Ry")
#elements.VeloLeft("RxRy")
elements.TT("TxTy")
#elements.TTBoxes("Tx")
elements.TTHalfLayers("Tx")
#elements.TTHalfModules("Tx")
#elements.ITLayers("Tx")
#elements.IT("TxTy")
elements.ITBoxes("TxTyRz")
elements.OTCFrames("TxTyRz")
#elements.OTModules("Tx")

#surveyconstraints.ITBoxes()
surveyconstraints.OT()
surveyconstraints.TT()
surveyconstraints.IT()
surveyconstraints.Velo()

#elements.OTCFrames("TxTy")
#surveyconstraints.append("/.*?/IT/.*?Layer.{1,2} : Tx : 0.02")
#surveyconstraints.append("/.*?/IT/.*?Layer.{1,2} : Rz : 0.001")
#surveyconstraints.append("Velo : 0 0 0 

# make sure the average layer does not move
constraints.append("TTHalfLayerAverage : TT..Layer.Side : Tx : total") ;
constraints.append("ITGlobal : T/IT : Tx Ty : total")

# I want to define the Box by the average of the two double layers
#elements.ITDoubleLayers("Tx")
#constraints.append("IT1A : IT/Station1/ASideBox/Layer.*? : Tx : total")
#constraints.append("IT1C : IT/Station1/CSideBox/Layer.*? : Tx : total")
#constraints.append("IT1T : IT/Station1/TopBox/Layer.*? : Tx : total")
#constraints.append("IT1B : IT/Station1/BottomBox/Layer.*? : Tx : total")
#constraints.append("IT2A : IT/Station2/ASideBox/Layer.*? : Tx : total")
#constraints.append("IT2C : IT/Station2/CSideBox/Layer.*? : Tx : total")
#constraints.append("IT2T : IT/Station2/TopBox/Layer.*? : Tx : total")
#constraints.append("IT2B : IT/Station2/BottomBox/Layer.*? : Tx : total")
#constraints.append("IT3A : IT/Station3/ASideBox/Layer.*? : Tx : total")
#constraints.append("IT3C : IT/Station3/CSideBox/Layer.*? : Tx : total")
#constraints.append("IT3T : IT/Station3/TopBox/Layer.*? : Tx : total")
#constraints.append("IT3B : IT/Station3/BottomBox/Layer.*? : Tx : total")

# for now, alsofix the TT global in y
#constraints.append("TTGlobal : TT : Ty : total") ;

# don't rotate T3
#constraints.append("IT3Top : T/IT/Station3/TopBox : Rz")
#constraints.append("IT3Bot : T/IT/Station3/BottomBox : Rz")
#constraints.append("IT3A   : T/IT/Station3/ASideBox : Rz")
#constraints.append("IT3C   : T/IT/Station3/CSideBox : Rz")

#constraints.append("OT/T3X2A : OT/T3VX2ASide : Tx Ty Tz Rz")
#constraints.append("OT/T3X2C : OT/T3VX2CSide : Tx Ty Tz Rz")
#constraints.append("IT1Bot  : T/IT/Station1/BottomBox : Tx Ty Tz Rz")
#constraints.append("IT2Bot  : T/IT/Station2/BottomBox : Tx Ty Tz Rz")
#constraints.append("IT3Bot  : T/IT/Station3/BottomBox : Tx Ty Tz Rz")
#constraints.append("IT1A  : T/IT/Station1/ASideBox : Tx Ty Tz Rz")
#constraints.append("IT2A  : T/IT/Station2/ASideBox : Tx Ty Tz Rz")
#constraints.append("IT3A  : T/IT/Station3/ASideBox : Tx Ty Tz Rz")
    
#constraints.append("OT/T3X2 : OT/T3X1U.Side : Tx Ty Tz Rz")
#constraints.append("OT/T3X2A : OT/T3X2VASide : Tx Ty Tz Rz")
        
print "aligning elements ", elements

TAlignment().ElementsToAlign      = list(elements)
TAlignment().TrackLocation        = "Rec/Track/AlignTracks"
# comment following line if one does not want vertex
#TAlignment().VertexLocation       = "Rec/Vertex/Primary"
TAlignment().Constraints          = constraints
TAlignment().SurveyConstraints    = list(surveyconstraints)
TAlignment().WriteCondSubDetList  = ['Velo','TT','IT','OT']
TAlignment().EigenValueThreshold  = 1
TAlignment().MinNumberOfHits = 5

# still set up a track selection
from Configurables import (GaudiSequencer,TrackContainerCopy,TrackSelector,
                           ITTrackSelector,TrackMonitor,TrackListRefiner,TrackListMerger) 
trackFilterSeq = GaudiSequencer("TrackFilterSeq")

alignSelectorA  = TrackListRefiner("AlignSelectorA",
                                   inputLocation = "Rec/Track/Best",
                                   outputLocation = "Rec/Track/AlignTracksA",
                                   Selector = TrackSelector())
alignSelectorA.Selector.MaxChi2Cut = 10
alignSelectorA.Selector.MaxChi2PerDoFMatch = 10
alignSelectorA.Selector.MaxChi2PerDoFVelo = 3
alignSelectorA.Selector.MaxChi2PerDoFDownstream = 5
#alignSelectorA.Selector.MinNVeloRHits = 4
#alignSelectorA.Selector.MinNVeloPhiHits = 4
#alignSelectorA.Selector.MaxNVeloHoles = 2
#alignSelectorA.Selector.,MinNTTHits = 3
#alignSelectorA.Selector.TrackTypes = ["Long","Ttrack","Downstream","Upstream"]
alignSelectorA.Selector.TrackTypes = ["Long"]

trackFilterSeq.Members.append( alignSelectorA )

alignSelectorB  = TrackListRefiner("AlignSelectorB",
                                   inputLocation = "Rec/Track/Best",
                                   outputLocation = "Rec/Track/AlignTracksB",
                                   Selector = ITTrackSelector())
alignSelectorB.Selector.RequireOverlap = True
alignSelectorB.Selector.MaxChi2Cut = 20
alignSelectorB.Selector.MaxChi2PerDoFVelo = 3
trackFilterSeq.Members.append( alignSelectorB )
trackFilterSeq.Members.append( TrackListMerger("AlignTrackMerger",
                                               inputLocations= [ "Rec/Track/AlignTracksA", "Rec/Track/AlignTracksB"],
                                               #inputLocations= [ "Rec/Track/AlignTracksA" ],
                                               outputLocation = "Rec/Track/AlignTracks") )

#refine track selection for a few more iterations
alignSelectorB.Selector.MaxChi2Cut = 10
alignSelectorB.Selector.MaxChi2PerDoFDownstream = 10
alignSelectorB.Selector.MaxChi2PerDoFMatch = 10

from Configurables import TrackCaloMatchMonitor,TrackFieldOffMatchMonitor
GaudiSequencer("AlignMonitorSeq").Members += [TrackCaloMatchMonitor('TrackEcalMatchMonitor'),
                                              TrackCaloMatchMonitor('TrackSpdMatchMonitor'),
                                              TrackCaloMatchMonitor('TrackPrsMatchMonitor'),
                                              TrackMonitor("ITOverlapTrackMonitor",
                                                           TracksInContainer = "Rec/Track/AlignTracksB"),
                                              TrackFieldOffMatchMonitor("AlignMatchMonitor",
                                                                        MatchTrackLocation = "Rec/Track/AlignTracks",
                                                                        VeloTrackLocation = 'Rec/Track/SelectedVeloOnly',
                                                                        SeedTrackLocation = 'Rec/Track/SelectedSeed')
                                              ]

#from Configurables import OTCalibrationIO
#GaudiSequencer("RecoOTSeq").Members.append(OTCalibrationIO())

#from Configurables import VeloTrackMonitor
#VeloTrackMonitor().OutputLevel = 6

from Configurables import TrackV0Finder

#GaudiSequencer("AlignMonitorSeq").Members.append(TrackMonitorNT(TracksInContainer = "Rec/Track/Best"))
#GaudiSequencer("AlignmentAlgSeq").Members.append(TrackListPrinter(inputLocation = "Rec/Track/Best"))
from Escher.UsefulSequences import SplitByITSequence
SplitByITSequence("AlignMonitorSeq",'Rec/Track/AlignTracks')

def TweakFitter( eventfitter ) :
    from Configurables import MeasurementProvider
    eventfitter.Fitter.MomentumForScattering  = 10000
    eventfitter.Fitter.MaxNumberOutliers = 0
    eventfitter.Fitter.addTool( MeasurementProvider(), name = "MeasProvider")
    #eventfitter.Fitter.MeasProvider.IgnoreTT = True

def doMyAlignChanges():
    from Configurables import TrackEventFitter, VeloTrackSelector
    from TrackFitter.ConfiguredFitters import ConfiguredStraightLineFitter
    TweakFitter( TrackEventFitter('FitMatch') )
    TweakFitter( TrackEventFitter('FitForward') )
    TweakFitter( TrackEventFitter('FitSeed') )
    TweakFitter( TrackEventFitter('FitVelo') )
    TweakFitter( TrackEventFitter('FitVeloTT') )
    TweakFitter( TrackEventFitter('RefitSeed') )
    
    # add a filter on Velo tracks
    from Configurables import TrackListFilter
    GaudiSequencer("RecoVELOSeq").Members += [TrackListFilter("VeloFilter",
                                                              inputLocation = "Rec/Track/Velo")]

    GaudiSequencer("RecoVELOSeq").Members += [
        TrackContainerCopy('VeloOnlyContainerCopy',
                           inputLocation ="Rec/Track/Velo",
                           outputLocation = "Rec/Track/VeloOnly",
                           copyFailures = True,
                           Selector = None ),
        ConfiguredStraightLineFitter( 'VeloOnlyFitter',
                                      TracksInContainer = "Rec/Track/VeloOnly")
        ]
    
    veloOnlyRefiner = TrackListRefiner("VeloOnlySelector",
                                       inputLocation = "Rec/Track/VeloOnly",
                                       outputLocation = "Rec/Track/SelectedVeloOnly",
                                       Selector = TrackSelector() )
    veloOnlyRefiner.Selector.MaxChi2Cut = 5
    #veloOnlyRefiner.OutputLevel = 1
    
    GaudiSequencer("RecoVELOSeq").Members.append(veloOnlyRefiner)
    
    GaudiSequencer("RecoVELOSeq").Members += [
        TrackListRefiner('VeloASideSelector',
                         inputLocation ="Rec/Track/SelectedVeloOnly",
                         outputLocation = "Rec/Track/VeloASide",
                         Selector = VeloTrackSelector()),
        TrackCaloMatchMonitor("VeloAEcalMatchMonitor",
                              TrackLocation = 'Rec/Track/VeloASide',
                              CaloSystem='Ecal',
                              RequireTHits = False)
        ]
    TrackListRefiner('VeloASideSelector').Selector.MinHitsASide = 6
    
    GaudiSequencer("RecoVELOSeq").Members += [
        TrackListRefiner('VeloCSideSelector',
                         inputLocation ="Rec/Track/SelectedVeloOnly",
                         outputLocation = "Rec/Track/VeloCSide",
                         Selector = VeloTrackSelector()),
        TrackCaloMatchMonitor("VeloCEcalMatchMonitor",
                              TrackLocation = 'Rec/Track/VeloCSide',
                              CaloSystem='Ecal',
                              RequireTHits = False)
        
        ]
    TrackListRefiner('VeloCSideSelector').Selector.MinHitsCSide = 6
    
    from Configurables import AlRobustAlignAlg
    GaudiSequencer("AlignMonitorSeq").Members += [
         AlRobustAlignAlg('VeloARobustAlign',TrackLocation = "Rec/Track/VeloASide"),
         AlRobustAlignAlg('VeloCRobustAlign',TrackLocation = "Rec/Track/VeloCSide") ]
    GaudiSequencer("AlignPostMonitorSeq").Members += [
         AlRobustAlignAlg('VeloARobustAlignPost',TrackLocation = "Rec/Track/VeloASide"),
         AlRobustAlignAlg('VeloCRobustAlignPost',TrackLocation = "Rec/Track/VeloCSide") ]

    from Configurables import Tf__Tsa__Seed, Tf__Tsa__SeedAddHits
    tsaSeed = Tf__Tsa__Seed("TsaSeed")
    tsaSeed.addTool(Tf__Tsa__SeedAddHits(), name="SeedAddHits")
    # tolerance if inside box mm
    tsaSeed.SeedAddHits.tol = 3
    # tolerance window make bigger mmm
    tsaSeed.SeedAddHits.dCut = 1
    # chi-sq cut
    tsaSeed.SeedAddHits.outlierCutParabola = 5;
    
    from Configurables import TrackFieldOffMatchMonitor

    seedRefiner = TrackListRefiner("SeedSelector",
                                   inputLocation = "Rec/Track/Seed",
                                   outputLocation = "Rec/Track/SelectedSeed",
                                   Selector = TrackSelector() )
    seedRefiner.Selector.MaxChi2Cut = 5
    
    GaudiSequencer('MoniTrSeq').Members += [
        seedRefiner,
        TrackFieldOffMatchMonitor( VeloTrackLocation = 'Rec/Track/SelectedVeloOnly',
                                   SeedTrackLocation = 'Rec/Track/SelectedSeed') ]


    from Configurables import TrackMatchVeloSeed
    TrackMatchVeloSeed("TrackMatch").Chi2MatchingCut = 500

    #from Configurables import TrackVeloTMatch
    #GaudiSequencer("TrackMatchPatSeq").Members = [ TrackVeloTMatch() ]


    
appendPostConfigAction(doMyAlignChanges)

