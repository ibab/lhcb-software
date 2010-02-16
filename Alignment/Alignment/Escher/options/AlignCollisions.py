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
Escher().SpecialData = [ "earlyData" ]
#TrackSys().ExpertTracking += [ "noMaterialCorrections"]
TrackSys().ExpertTracking += [ "simplifiedGeometry"]
#TrackSys().ExpertTracking += ["noDrifttimes" ]
TrackSys().TrackExtraInfoAlgorithms = ['']
#RecSysConf().RecoSequence = ["VELO","TT","IT","OT","Tr","Vertex"]
#RecSysConf().RecoSequence = ["VELO","TT","IT","OT","Tr"]
RecSysConf().RecoSequence = ["CALO","VELO","TT","IT","OT","Tr","Vertex"]
Escher().MoniSequence = ["VELO","Tr", "OT","ST"]
TrackSys().TrackPatRecAlgorithms = ["Velo","TsaSeed","Match","Downstream"]

#TrackSys().TrackPatRecAlgorithms=["Velo","VeloTT","Forward"]
#TrackSys().TrackPatRecAlgorithms=["Velo","VeloTT","Forward","ITGeneric"]
#TrackSys().TrackPatRecAlgorithms=["Velo","VeloTT"]
#TrackSys().TrackPatRecAlgorithms = [ "Velo","Forward","PatSeed","PatMatch"]

from Configurables import TAlignment
from TAlignment.Alignables import *
from TAlignment.SurveyConstraints import *

#elements = Alignables()
constraints = []
surveyconstraints = SurveyConstraints()

#elements.Velo("Rx")
#elements.VeloRight("Ry")
##elements.Tracker("Rz")
#elements.TT("TxTy")
#elements.TTHalfLayers("Tx")
#elements.TTHalfModules("TxRz")
##elements.ITLayers("Tx")
##elements.IT("TxTy")
#elements.ITBoxes("TxTyRz")
#elements.ITLayers("Tx")
#elements.ITLadders("TxRz")
#elements.OTCFrames("Tx")
#elements.OTCFrameLayers("Ty")
#elements.OTModules("Tx")

#surveyconstraints.ITBoxes()
#surveyconstraints.OTCFrames()
surveyconstraints.All()

#elements.OTCFrames("TxTy")
#surveyconstraints.append("/.*?/IT/.*?Layer.{1,2} : Tx : 0.02")
#surveyconstraints.append("/.*?/IT/.*?Layer.{1,2} : Rz : 0.001")

#constraints.append("OTCSide : OT/T3VX2CSide : Tx")
#constraints.append("OTASide : OT/T3VX2ASide : Tx")

# make sure to constrain both q/P and a twist. does this work?

#constraints.append("ITGlobal : T/IT : Tx Ty : total")

elements = Alignables()
constraints = []
#elements += ['/dd/Structure/LHCb/AfterMagnetRegion/T/IT/Station(1|2|3)/(CSide|ASide|Top|Bottom)Box:TxTyTzRz']
#elements += ['/dd/Structure/LHCb/AfterMagnetRegion/T/IT/Station(1|2|3)/(CSide|ASide|Top|Bottom)Box/Layer.{1,2}:TxTz']
elements.Velo("None")
elements.VeloRight("None")
elements.ITBoxes("TxTyTzRzRyRx")
elements.ITLayers("Tx")
elements.ITLadders("TxRz")
elements.OTCFrames("Tx")
elements.OTCFrameLayers("Tz")
elements.OTModules("TxRz")
elements.TT("TxTyTz")
#elements.TTBoxes("Tx")
#elements.TTHalfLayers("Tx")
elements.TTHalfModules("TxRz")
#constraints.append("TTHalfLayerAverage : TT..Layer.Side : Tx : total") 
#constraints.append("OT3Frames : OT/T3VX2.Side : Tx")
#constraints.append("OT3       : OT/T3VX2      : Tx Ty Tz")
constraints.append("IT3Top    : IT/Station3/TopBox : Tx Ty Tz Rx Ry Rz")
constraints.append("IT3Bottom : IT/Station3/BottomBox : Tx Ty Tz Rx Ry Rz")
constraints.append("IT3A      : IT/Station3/ASideBox : Ty Tz Rx Ry Rz")
constraints.append("IT3C      : IT/Station3/CSideBox : Ty Tz Rx Ry Rz")

#constraints.append("TTAAverage : TT..LayerASide : Tx : total")
#constraints.append("TTCAverage : TT..LayerCSide : Tx : total")

print "aligning elements ", elements

TAlignment().ElementsToAlign      = list(elements)
TAlignment().TrackLocation        = "Rec/Track/AlignTracks"
# comment following line if one does not want vertex
TAlignment().VertexLocation       = "Rec/Vertex/Primary"
TAlignment().Constraints          = constraints
TAlignment().SurveyConstraints    = list(surveyconstraints)
TAlignment().WriteCondSubDetList  = ['Velo','TT','IT','OT']
#TAlignment().EigenValueThreshold   = 100
#TAlignment().MinEigenModeChisquare = 100
TAlignment().MinNumberOfHits = 1

# still set up a track selection
from Configurables import (GaudiSequencer,TrackListRefiner,TrackListMerger,
                           TrackSelector,TrackMonitor,ITTrackSelector,
                           TrackHitAdder) 
from TrackFitter.ConfiguredFitters import *

GaudiSequencer("TrackAddExtraInfoSeq").Members += [
    TrackHitAdder( "TrackHitAdder",
                   TrackLocation = "Rec/Track/Best" ),
    ConfiguredEventFitter("TrackRefitter",
                          TracksInContainer = "Rec/Track/Best" ) ]
TrackEventFitter("TrackRefitter").Fitter.UpdateTransport = True

trackFilterSeq = GaudiSequencer("TrackFilterSeq")
alignSelectorA = TrackListRefiner("AlignSelectorA",
                                  inputLocation = "Rec/Track/Best",
                                  outputLocation = "Rec/Track/AlignTracksA",
                                  Selector = TrackSelector())
alignSelectorA.Selector.MaxChi2Cut = 10
alignSelectorA.Selector.MaxChi2PerDoFMatch = 10
alignSelectorA.Selector.MaxChi2PerDoFVelo = 5
alignSelectorA.Selector.MaxChi2PerDoFDownstream = 5
alignSelectorA.Selector.MinPCut = 10000
alignSelectorA.Selector.TrackTypes = ["Long","Downstream"]

#this one selects specifically overlap tracks. we only take T-tracks with overlaps
alignSelectorB = TrackListRefiner("AlignSelectorB",
                                  inputLocation = "Rec/Track/Best",
                                  outputLocation = "Rec/Track/AlignTracksB",
                                  Selector = ITTrackSelector())
alignSelectorB.Selector.RequireOverlap = True
alignSelectorB.Selector.MinPCut = 5000
alignSelectorB.Selector.MaxChi2Cut = 10
alignSelectorB.Selector.MaxChi2PerDoFMatch = 10
alignSelectorB.Selector.MaxChi2PerDoFVelo = 5
alignSelectorB.Selector.MaxChi2PerDoFDownstream = 10
alignSelectorB.Selector.TrackTypes = ["Long","Downstream","Ttrack"]

#alignSelectorC = TrackListRefiner("AlignSelectorC",
#                                  inputLocation = "Rec/Track/Best",
#                                  outputLocation = "Rec/Track/AlignTracksC",
#                                  Selector = ITTrackSelector())
#alignSelectorC.Selector.RequireOverlap = True
#alignSelectorC.Selector.MinPCut = 10000
#alignSelectorC.Selector.MaxChi2Cut = 5
#alignSelectorC.Selector.TrackTypes = ["Ttrack"]
trackFilterSeq.Members += [ alignSelectorA, alignSelectorB ]

trackFilterSeq.Members.append( TrackListMerger("AlignTrackMerger",
                                               inputLocations= [ "Rec/Track/AlignTracksA", "Rec/Track/AlignTracksB"],
                                               outputLocation = "Rec/Track/AlignTracks") )

from Configurables import TrackCaloMatchMonitor, TrackListRefiner, TrackITOverlapMonitor, TrackAlignMonitor

longTrackSelector = TrackListRefiner("LongTrackSelector",
                                     inputLocation = "Rec/Track/Best",
                                     outputLocation = "Rec/Track/Long",
                                     Selector = TrackSelector())
longTrackSelector.Selector.TrackTypes = ["Long","Downstream"]

GaudiSequencer("AlignMonitorSeq").Members += [TrackCaloMatchMonitor('TrackEcalMatchMonitor'),
                                              TrackCaloMatchMonitor('TrackSpdMatchMonitor'),
                                              TrackCaloMatchMonitor('TrackPrsMatchMonitor'),
                                              longTrackSelector,
                                              TrackITOverlapMonitor("LongTrackITOverlapMonitor",
                                                                    TrackLocation = "Rec/Track/Long"),
                                              TrackITOverlapMonitor("AlignITOverlapMonitor",
                                                                    TrackLocation = "Rec/Track/AlignTracks"),
                                              TrackAlignMonitor("AlignTrackAlignMonitor",
                                                                TrackLocation = "Rec/Track/AlignTracks")
                                              ]


#from Configurables import OTCalibrationIO
#GaudiSequencer("RecoOTSeq").Members.append(OTCalibrationIO())

from Configurables import TrackV0Finder, TrackV0Monitor, TrackV0NtupleMonitor, TrackPVFinder

TrackV0Monitor().FullDetail = True
#TrackV0Finder(KsMassCut = 300,AddExtraInfo = True,ExcludePVTracks = False)
TrackV0Finder(AddExtraInfo = True,ExcludePVTracks = False)

def TweakFitter( eventfittername ) :
    from Configurables import TrackEventFitter
    eventfitter = TrackEventFitter(eventfittername)
    eventfitter.Fitter.MaxNumberOutliers = 0
    eventfitter.Fitter.MaxDeltaChiSqConverged = 0.01
    eventfitter.Fitter.NumberFitIterations = 10
    eventfitter.Fitter.Chi2Outliers = 25
        
def doMyAlignChanges():
    from Configurables import TESCheck
    TESCheck().Stop = False
    TESCheck().Enable = False

    # insert a run filter
    from Configurables import LoKi__ODINFilter  as ODINFilter
    odinfilter = ODINFilter("OdinRunFilter",
                            Code = "ODIN_RUN  != ints( [63757,63948,63700,63702,63948] )")
    odinfilter.Preambulo += [ "from LoKiCore.functions import *" ]
    #~ODIN_RUNNUMBER( uints (  63757,63948,63700,63702,63948  ) )" )
    #badruns = [63757,63948,63700,63702,63948]
    #odincode = ""
    #first = True
    #for i in badruns:
    #    if not first: odincode += "&"
    #    first = False
    #    odincode += "(ODIN_RUNNUMBER!=%d)" % i
    #odinfilter = ODINFilter("OdinRunFilter",
    #                        Code = odincode)
    GaudiSequencer('EscherSequencer').Members.insert(1,odinfilter)

    from Configurables import VeloTrackSelector
    TweakFitter('FitDownstream')
    TweakFitter('FitMatch')
    TweakFitter('FitSeed')
    TweakFitter('RefitSeed')
    TweakFitter('FitForward')
    TweakFitter('FitVelo')
    TweakFitter('FitVeloTT')
    TweakFitter('TrackRefitter')

    #GaudiSequencer("RecoVertexSeq").Members = [ TrackPVFinder(),
                                                #tweakedV0Finder,
    #                                            TrackV0Finder() ]

    # add a filter on Velo tracks
    from Configurables import TrackListFilter
    GaudiSequencer("RecoVELOSeq").Members += [TrackListFilter("VeloFilter",
                                                              inputLocation = "Rec/Track/Velo")]
    
    from Configurables import TrackMonitor
    TrackMonitor().FullDetail = True

    from Configurables import Tf__Tsa__Seed, Tf__Tsa__SeedAddHits
    tsaSeed = Tf__Tsa__Seed("TsaSeed")
    tsaSeed.addTool(Tf__Tsa__SeedAddHits(), name="SeedAddHits")
    # tolerance if inside box mm
    tsaSeed.SeedAddHits.tol = 3
    # tolerance window make bigger mmm
    tsaSeed.SeedAddHits.dCut = 2.5
    # chi-sq cut
    tsaSeed.SeedAddHits.outlierCutParabola = 5;

    from Configurables import TrackHitMapMonitor
    GaudiSequencer("MoniTrSeq").Members.append( TrackHitMapMonitor() )

    from Configurables import STOfflinePosition
    itClusterPosition = STOfflinePosition('ToolSvc.ITClusterPosition')
    itClusterPosition.ErrorVec = [0.28, 0.22, 0.35, 0.35]
    itClusterPosition.APE = 0.1

appendPostConfigAction(doMyAlignChanges)
