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
Escher().TrackContainer = "Rec/Track/Best" # Velo, VeloR, Long, Upstream, Downstream, Ttrack, Muon or Calo
Escher().EvtMax = 1000
Escher().UseOracle = False
Escher().SpecialData = [ "earlyData" ]
Escher().DatasetName = "Collisions2010"
TrackSys().ExpertTracking += [ "simplifiedGeometry"]
TrackSys().TrackExtraInfoAlgorithms = ['']
RecSysConf().RecoSequence = ["Decoding","VELO","TT","IT","OT","Tr","Vertex"]
Escher().MoniSequence = ["Tr","OT","ST"]

from Configurables import MagneticFieldSvc
MagneticFieldSvc().OutputLevel = 1

from Configurables import TAlignment
from TAlignment.Alignables import *
from TAlignment.SurveyConstraints import *

constraints = []
surveyconstraints = SurveyConstraints()
surveyconstraints.All()
elements = Alignables()

elements.Velo("None")
elements.VeloRight("TxTyTzRxRyRz")
elements.VeloLeft("TxTyTzRxRyRz")
elements.VeloModules("TxTyTzRz")
elements.VeloPhiSensors("TxTy")
elements.VeloRSensors("None")

elements.ITBoxes("TxTyTzRzRyRx")
elements.ITLayers("TxTzRx")
elements.ITLadders("TxRz")
elements.OTCFrames("TxTy")
elements.OTCFrameLayers("Tz")
elements.OTModules("Tx")
elements.TT("TyTz")
#elements.TTLayers("Tz")
#elements.TTBoxes("Ry")
elements.TTHalfLayers("TxTzRy")
#elements.TTHalfModules("TxRz")
elements.TTModules("TxRz")

#constraints.append("TTHalfLayerAverage : TT..Layer.Side : Tx : total") 

# now constrain the total shearing in each Velo half to get the z-axis nominal
constraints.append("VeloLeftFirst : Velo/VeloLeft/Module00 : Tx Ty : total")
constraints.append("VeloRightFirst : Velo/VeloRight/Module01 : Tx Ty : total")
constraints.append("VeloLeftLast : Velo/VeloLeft/Module40 : Tx Ty : total")
constraints.append("VeloRightLast : Velo/VeloRight/Module41 : Tx Ty : total")

# we also fix the average position and rotation of the velo halves. we
# may later replace the translation by the PV constraint.
#constraints += ["PVx","PVy","PVz"]
constraints.append("VeloHalfAverage : Velo/Velo(Left|Right) : Rx Ry Rz Tx Ty Tz : total")

# we fix the OT x, until we have seen magnet-off data
constraints.append("OT3Frames : OT/T3VX2.Side : Tx")
#constraints.append("OT3       : OT/T3VX2      : Tx Ty Tz")
constraints.append("IT3Top    : IT/Station3/TopBox : Rx Ry Rz")
constraints.append("IT3Bottom : IT/Station3/BottomBox : Rx Ry Rz")
constraints.append("IT3A      : IT/Station3/ASideBox : Rx Ry Rz")
constraints.append("IT3C      : IT/Station3/CSideBox : Rx Ry Rz")

from Configurables import Al__AlignChisqConstraintTool as AlignChisqConstraintTool
# fix the z-position of OT in T3
surveyconstraints.append("OT/T3X1U : -1.52 -1.25  0.0  0.0 0.0 0.0 : 0.5 0.5 0.0001 0.0001 0.0001 0.0001")

# fix the Tz and Rz of the last and first velo modules
AlignChisqConstraintTool().XmlUncertainties += ["Module(00|01|40|41) : 0.02 0.02 0.00001 0.0002 0.0002 0.0000001"]

print "aligning elements ", elements

TAlignment().ElementsToAlign      = list(elements)
TAlignment().TrackLocation        = "Rec/Track/AlignTracks"
# comment following line if one does not want vertex
TAlignment().VertexLocation       = "Rec/Vertex/AlignPrimaryVertices"
TAlignment().Constraints          = constraints
TAlignment().SurveyConstraints    = list(surveyconstraints)
TAlignment().WriteCondSubDetList  = ['Velo','TT','IT','OT']
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
                   MaxDistTT = 0.5),
    ConfiguredEventFitter("TrackRefitter",
                          TracksInContainer = "Rec/Track/Best" )
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


trackFilterSeq.Members += [ alignSelectorA, alignSelectorB, alignSelectorC, alignSelectorD ]

trackFilterSeq.Members.append( TrackListMerger("AlignTrackMerger",
                                               inputLocations= [ "Rec/Track/AlignTracksA", "Rec/Track/AlignTracksB",
                                                                 "Rec/Track/AlignTracksC", "Rec/Track/AlignTracksD" ],
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
    TweakFitter('TrackRefitter')
    
    TrackEventFitter('TrackRefitter').Fitter.UpdateTransport = True

    # add a filter on Velo tracks
    from Configurables import TrackListFilter
    GaudiSequencer("RecoVELOSeq").Members += [TrackListFilter("VeloFilter",
                                                              inputLocation = "Rec/Track/Velo")]
    
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
