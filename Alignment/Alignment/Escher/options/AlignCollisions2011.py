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

Escher().Kalman = True
Escher().Millepede = False
Escher().EvtMax = 1000
Escher().UseOracle = False
Escher().DatasetName = "Collisions2011"
TrackSys().ExpertTracking += [ "simplifiedGeometry"]
TrackSys().TrackExtraInfoAlgorithms = ['']
RecSysConf().RecoSequence = ["Decoding","VELO","TT","IT","OT","Tr","Vertex","CALO","MUON"]
Escher().MoniSequence = ["Tr","OT","ST"]

from Configurables import MagneticFieldSvc
MagneticFieldSvc().OutputLevel = 1

from Configurables import TAlignment
from TAlignment.Alignables import *
from TAlignment.SurveyConstraints import *

elements = Alignables()

elements.Velo("None")
elements.VeloRight("None")
elements.VeloLeft("TxTy")
#elements.VeloModules("TxTyTzRz")
#elements.VeloPhiSensors("TxTy")
#elements.VeloRSensors("None")

elements.IT("None")
elements.ITBoxes("TxTyTzRxRyRz")
elements.ITLayers("TxTyTzRxRyRz")
#elements.ITLadders("TxRz")
elements.OT("None")
elements.OTCFrames("Tx")
elements.OTCFrameLayers("Tz")
elements.OTModules("TxRz")
elements.TT("None")
elements.TTLayers("None")
#elements.TTBoxes("Ry")
#elements.TTHalfLayers("Tx")
#elements.TTHalfModules("TxRz")
elements.TTModules("TxRz")

constraints = []

# we fix the OT x, until we have seen magnet-off data
# fix the X-position of OT in T3 two C-frames:

surveyconstraints = SurveyConstraints()
surveyconstraints.All()
surveyconstraints.XmlUncertainties += ["OT/T3X1U.Side : 0.00001 0.5 0.5 0.0001 0.0001 0.0001" ]

# fix two C-frame layers in Z
surveyconstraints.XmlUncertainties += ["OT/T1X1U : 0.00001 0.5 0.00001 0.0001 0.0001 0.0001" ]
surveyconstraints.XmlUncertainties += ["OT/T3X1U : 0.00001 0.5 0.00001 0.0001 0.0001 0.0001" ]

# move the IT boxes back to their survey position in Y, Rx and Ry
surveyconstraints.XmlUncertainties += [ "ITT.*?Box : 0.5 0.0001 0.5 0.000001 0.000001 0.001" ]

# move the IT layers  back to their survey position in Y, Z, Rx, Ry, and Rz
surveyconstraints.XmlUncertainties += [ "ITT.*?Layer.{1,2} : 0.5 0.0001 0.0001 0.000001 0.000001 0.000001" ]

# need to increase the TT module survey because of temperature expansion
surveyconstraints.XmlUncertainties += [ "TT..LayerR.Module.*? : 1.0 0.1 0.1 0.0005 0.0005 0.0005"]

fixSurveyZ = False
if fixSurveyZ:
    # fix Tz, Rx and Ry all to survey. how do we know this really worked?
    survey.XmlUncertainties += [ "ITT.*?Box : 0.5 0.0001 0.0001 0.000001 0.0000001 0.001",
                                 "ITT.*?Layer.{1,2} : 0.05 0.05 0.0001 0.0000001 0.000001 0.0001",
                                 #"TTSystem : 0.5 0.5 0.0001 0.000001 0.0000001 0.001",
                                 #"TT..Layer.Side      : 0.1 0.0001 0.0001 0.0000001 0.0000001 0.0005",
                                 "OT/T.(X1U|VX2)      : 0.5 0.5 0.0001 0.0000001 0.0000001 0.0001",
                                 "OT/T.(X1U|VX2).Side : 0.5 0.5 0.0001 0.0000001 0.0000001 0.0001" ]
    

print "aligning elements ", elements

TAlignment().ElementsToAlign      = list(elements)
TAlignment().TrackLocation        = "Rec/Track/AlignTracks"
# comment following line if one does not want vertex
TAlignment().VertexLocation       = "Rec/Vertex/AlignPrimaryVertices"
#TAlignment().VertexLocation       = "Rec/Vertex/Primary"
TAlignment().Constraints          = constraints
TAlignment().WriteCondSubDetList  = ['Velo','TT','IT','OT','MUON']
TAlignment().MinNumberOfHits = 6
#TAlignment().SolvTool = "gslSVDsolver"

# still set up a track selection
from Configurables import (GaudiSequencer,TrackListRefiner,TrackListMerger,
                           TrackSelector,TrackMonitor,ITTrackSelector,
                           TrackHitAdder,TrackDoubleHitPruner ) 
from TrackFitter.ConfiguredFitters import *

GaudiSequencer("TrackAddExtraInfoSeq").Members += [
    TrackHitAdder( "TrackHitAdder",
                   TrackLocation = "Rec/Track/Best",
                   MaxDistTT = 0.5,
                   MaxDistIT = 2),
    ConfiguredEventFitter("TrackRefitter",
                          TracksInContainer = "Rec/Track/Best")
    #,TrackDoubleHitPruner("TrackDoubleHitPruner",TrackLocation = "Rec/Track/Best")
    ]

trackFilterSeq = GaudiSequencer("TrackFilterSeq")
alignSelectorA = TrackListRefiner("AlignSelectorA",
                                  inputLocation = "Rec/Track/Best",
                                  outputLocation = "Rec/Track/AlignTracksA",
                                  Selector = TrackSelector())
alignSelectorA.Selector.MaxChi2Cut = 10
alignSelectorA.Selector.MaxChi2PerDoFMatch = 10
alignSelectorA.Selector.MaxChi2PerDoFVelo = 5
alignSelectorA.Selector.MaxChi2PerDoFDownstream = 5
alignSelectorA.Selector.MinPCut = 5000
alignSelectorA.Selector.TrackTypes = ["Long","Downstream","Upstream"]
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

# I'll also add high momentum T tracks
alignSelectorT = TrackListRefiner("AlignSelectorT",
                                  inputLocation = "Rec/Track/Best",
                                  outputLocation = "Rec/Track/AlignTracksT",
                                  Selector = TrackSelector())
alignSelectorT.Selector.MinPCut = 20000
alignSelectorT.Selector.MaxChi2Cut = 5
alignSelectorT.Selector.TrackTypes = ["Ttrack"]

#select VeloBackward tracks. these don't have a lot of momentum
alignSelectorC = TrackListRefiner("AlignSelectorC",
                                  inputLocation = "Rec/Track/Best",
                                  outputLocation = "Rec/Track/AlignTracksC",
                                  Selector = TrackSelector())
alignSelectorC.Selector.MaxChi2Cut = 5
alignSelectorC.Selector.TrackTypes = ["Backward"]
alignSelectorC.Selector.MinNVeloRHits = 4
alignSelectorC.Selector.MinNVeloPhiHits = 4
alignSelectorC.Selector.MaxNVeloHoles = 0

#select Velo overlap tracks.
from Configurables import VeloTrackSelector
alignSelectorD = TrackListRefiner("AlignSelectorD",
                                  inputLocation = "Rec/Track/Best",
                                  outputLocation = "Rec/Track/AlignTracksD",
                                  Selector = VeloTrackSelector())
alignSelectorD.Selector.MaxChi2PerDoFVelo = 10
alignSelectorD.Selector.MaxChi2Cut = 10
alignSelectorD.Selector.MinHitsASide = 1
alignSelectorD.Selector.MinHitsCSide = 1
alignSelectorD.Selector.MinNVeloRHits = 4
alignSelectorD.Selector.MinNVeloPhiHits = 4
alignSelectorD.Selector.MaxNVeloHoles = 0


trackFilterSeq.Members += [ alignSelectorA, alignSelectorB, alignSelectorC, alignSelectorD, alignSelectorT ]

trackFilterSeq.Members.append( TrackListMerger("AlignTrackMerger",
                                               inputLocations= [ "Rec/Track/AlignTracksA", "Rec/Track/AlignTracksB",
                                                                 "Rec/Track/AlignTracksC", "Rec/Track/AlignTracksD",
                                                                 "Rec/Track/AlignTracksT"],
                                               outputLocation = "Rec/Track/AlignTracks") )

#from Configurables import TrackListPrinter
#trackFilterSeq.Members.append(TrackListPrinter("BackwardPrinter",InputLocation="Rec/Track/AlignTracksC"))

# now select the primary vertices
from Configurables import VertexListRefiner
pvSelector = VertexListRefiner("AlignPVSelector",
                               InputLocation = "Rec/Vertex/Primary",
                               OutputLocation = "Rec/Vertex/AlignPrimaryVertices",
                               MaxChi2PerDoF = 10,
                               MinNumTracks = 5,
                               MinNumLongTracks = 2 )
trackFilterSeq.Members.append( pvSelector )


from Configurables import TrackListRefiner, TrackITOverlapMonitor, TrackAlignMonitor

longTrackSelector = TrackListRefiner("LongTrackSelector",
                                     inputLocation = "Rec/Track/Best",
                                     outputLocation = "Rec/Track/Long",
                                     Selector = TrackSelector())
longTrackSelector.Selector.TrackTypes = ["Long","Downstream"]

def TweakFitter( eventfittername ) :
    from Configurables import TrackEventFitter
    eventfitter = TrackEventFitter(eventfittername)
    eventfitter.Fitter.MaxNumberOutliers = 2
    eventfitter.Fitter.MaxDeltaChiSqConverged = 0.01
    eventfitter.Fitter.NumberFitIterations = 10
    eventfitter.Fitter.Chi2Outliers = 25
    #eventfitter.Fitter.MeasProvider.IgnoreIT = True
    #eventfitter.Fitter.MaxMomentumForScattering = 50000
    #eventfitter.Fitter.TransverseMomentumForScattering = 200
    #eventfitter.Fitter.FitUpstream = False
    
def doMyAlignChanges():
    from Configurables import TESCheck
    TESCheck().Stop = False
    TESCheck().Enable = False

    TweakFitter('FitDownstream')
    TweakFitter('FitMatch')
    TweakFitter('FitSeed')
    TweakFitter('RefitSeed')
    TweakFitter('FitForward')
    TweakFitter('FitVelo')
    TweakFitter('FitVeloTT')
    TweakFitter('FitBest')    
    TweakFitter('TrackRefitter')
    
    TrackEventFitter('TrackRefitter').Fitter.UpdateTransport = True

    # add a filter on Velo tracks
    from Configurables import TrackListFilter
    GaudiSequencer("RecoVELOSeq").Members += [TrackListFilter("VeloFilter",
                                                              inputLocation = "Rec/Track/Velo")]

    from Configurables import TrackMultiplicityFilter
    GaudiSequencer("RecoDecodingSeq").Members += [TrackMultiplicityFilter("MultFilter",
                                                                      MaxNumITHits = 200)]

    from Configurables import PatSeeding, PatSeedingTool
    ps = PatSeeding('PatSeeding')
    ps.PatSeedingTool.CurveTol = 10
    ps.PatSeedingTool.TolCollectIT = 5
    ps.PatSeedingTool.QualityWeights = [0,-0.2]
    ps.PatSeedingTool.MaxRangeIT = 100
    ps.PatSeedingTool.MaxChi2HitIT = 60
    ps.PatSeedingTool.MinXPlanes = 4
    ps.PatSeedingTool.MinTotalPlanes = 8
    ps.PatSeedingTool.MaxMisses = 2
    ps.PatSeedingTool.MaxHoles = 2
    ps.PatSeedingTool.MaxTrackChi2 = 30
    ps.PatSeedingTool.MaxFinalChi2 = 40
    ps.PatSeedingTool.MaxFinalTrackChi2 = 30

    from Configurables import PatForward, PatForwardTool
    pf = PatForward('PatForward')
    pf.PatForwardTool.MinXPlanes = 4
    pf.PatForwardTool.MinPlanes = 7
    pf.PatForwardTool.MaxSpreadX = 2
    pf.PatForwardTool.MinHits = 8
    
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

    from Configurables import ST__STClusterMonitor
    ST__STClusterMonitor('ITClusterMonitor').HitMaps = True

appendPostConfigAction(doMyAlignChanges)
