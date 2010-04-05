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
#Escher().Detectors = ["Velo"]
Escher().Kalman = True
Escher().Millepede = False
Escher().TrackContainer = "Rec/Track/Best" # Velo, VeloR, Long, Upstream, Downstream, Ttrack, Muon or Calo
Escher().EvtMax = 1000
Escher().UseOracle = False
Escher().SpecialData = [ "earlyData" ]
TrackSys().ExpertTracking += [ "simplifiedGeometry"]
TrackSys().TrackExtraInfoAlgorithms = ['']
RecSysConf().RecoSequence = ["Decoding","CALO","VELO","TT","IT","OT","Tr","Vertex"]
Escher().MoniSequence = ["Tr", "OT","ST"]

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
elements.VeloPhiSensors("None")
elements.VeloRSensors("None")
elements.ITBoxes("None")
elements.OTCFrames("None")
elements.ITBoxes("TxTyTzRzRyRx")
elements.ITLayers("TxTzRx")
elements.ITLadders("TxRz")
elements.OTCFrames("TxTy")
elements.OTCFrameLayers("Tz")
elements.OTModules("TxRz")
elements.TT("TyTz")
#elements.TTLayers("Tz")
#elements.TTBoxes("Ry")
elements.TTHalfLayers("TxTzRy")
#elements.TTHalfModules("TxRz")
elements.TTModules("TxRz")


#constraints.append("TTHalfLayerAverage : TT..Layer.Side : Tx : total") 

# constrain the global position of the 2 halves by fixing the PV
constraints += ["PVx","PVy","PVz"]
# now constrain the total shearing in each Velo half to get the z-axis nominal
#constraints.append("VeloLeftShearing : Velo/VeloLeft/Module.. : Szx Szy : total")
#constraints.append("VeloRightShearing : Velo/VeloRight/Module.. : Szx Szy : total")
constraints.append("VeloLeftFirst : Velo/VeloLeft/Module00 : Tx Ty : total")
constraints.append("VeloRightFirst : Velo/VeloRight/Module01 : Tx Ty : total")
constraints.append("VeloLeftLast : Velo/VeloLeft/Module40 : Tx Ty : total")
constraints.append("VeloRightLast : Velo/VeloRight/Module41 : Tx Ty : total")


constraints.append("OT3Frames : OT/T3VX2.Side : Tx Tz")
#constraints.append("OT3       : OT/T3VX2      : Tx Ty Tz")
constraints.append("IT3Top    : IT/Station3/TopBox : Rx Ry Rz")
constraints.append("IT3Bottom : IT/Station3/BottomBox : Rx Ry Rz")
constraints.append("IT3A      : IT/Station3/ASideBox : Rx Ry Rz")
constraints.append("IT3C      : IT/Station3/CSideBox : Rx Ry Rz")


from Configurables import Al__AlignChisqConstraintTool as AlignChisqConstraintTool
# fix the z-position of OT in T3
surveyconstraints.append("OT/T3X1U : -1.52 -1.25  0.0  0.0 0.0 0.0 : 0.5 0.5 0.0001 0.0001 0.0001 0.0001")

# fix the z-position of the last and first velo modules
AlignChisqConstraintTool().XmlUncertainties += ["Velo/VeloLeft/Module(00|40) : 0.02 0.02 0.00001 0.0002 0.0002 0.0002",
                                                "Velo/VeloRight/Module(01|41) : 0.02 0.02 0.00001 0.0002 0.0002 0.0002"]

print "aligning elements ", elements

TAlignment().ElementsToAlign      = list(elements)
TAlignment().TrackLocation        = "Rec/Track/AlignTracks"
# comment following line if one does not want vertex
TAlignment().VertexLocation       = "Rec/Vertex/Primary"
TAlignment().Constraints          = constraints
TAlignment().SurveyConstraints    = list(surveyconstraints)
TAlignment().WriteCondSubDetList  = ['Velo','TT','IT','OT']
TAlignment().MinNumberOfHits = 6

# still set up a track selection
from Configurables import (GaudiSequencer,TrackListRefiner,TrackListMerger,
                           TrackSelector,TrackMonitor,ITTrackSelector,
                           TrackHitAdder) 
from TrackFitter.ConfiguredFitters import *

GaudiSequencer("TrackAddExtraInfoSeq").Members += [
    TrackHitAdder( "TrackHitAdder",
                   TrackLocation = "Rec/Track/Best",
                   MaxDistTT = 0.5),
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
alignSelectorA.Selector.MinPCut = 5000
alignSelectorA.Selector.TrackTypes = ["Long","Downstream","Upstream"]

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

trackFilterSeq.Members += [ alignSelectorA, alignSelectorB, alignSelectorC ]

trackFilterSeq.Members.append( TrackListMerger("AlignTrackMerger",
                                               inputLocations= [ "Rec/Track/AlignTracksA", "Rec/Track/AlignTracksB",
                                                                 "Rec/Track/AlignTracksC" ],
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

def TweakFitter( eventfittername ) :
    from Configurables import TrackEventFitter
    eventfitter = TrackEventFitter(eventfittername)
    eventfitter.Fitter.MaxNumberOutliers = 3
    eventfitter.Fitter.MaxDeltaChiSqConverged = 0.01
    eventfitter.Fitter.NumberFitIterations = 10
    eventfitter.Fitter.Chi2Outliers = 25
        
def doMyAlignChanges():
    from Configurables import TESCheck
    TESCheck().Stop = False
    TESCheck().Enable = False

    from Configurables import VeloTrackSelector
    TweakFitter('FitDownstream')
    TweakFitter('FitMatch')
    TweakFitter('FitSeed')
    TweakFitter('RefitSeed')
    TweakFitter('FitForward')
    TweakFitter('FitVelo')
    TweakFitter('FitVeloTT')
    TweakFitter('TrackRefitter')

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
