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
Escher().PrintFreq = 100
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
#elements.VeloRight("TxTyTzRxRy")
#elements.VeloLeft("TxTyTzRxRy")
elements.VeloLeft("None")
elements.VeloRight("None")
#elements.VeloModules("TxTyTzRz")
#elements.VeloPhiSensors("TxTy")
#elements.VeloRSensors("None")
#elements.VeloRSensors("None")
#elements.VeloPhiSensors("None")
elements.VeloModules("None")

elements.IT("None")
elements.ITBoxes("TxTyTzRxRyRz")
elements.ITLayers("TxTz")
elements.ITLadders("TxRz")
#elements.ITBoxes("Tx")
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

elements.Tracker("None")
# let's try this:
# elements.Tracker("TxTzRxRz") This doesn't solve anything ... we need to look somewhere else

# to get Fred's y alignment from survey
elements.OTStations("Ty")
elements.TT("Ty")
elements.ITBoxes("Ty")
elements.ITLayers("Ty")

surveyconstraints = SurveyConstraints()
surveyconstraints.All()
constraints = []

# make sure that the velo stays where it was
constraints.append("VeloHalfAverage : Velo/Velo(Left|Right) : Tx Ty Tz Rx Ry : total")

# we fix the OT x, until we have seen magnet-off data
# fix the X-position of OT in T3 two C-frames:
# release the constraint now that we have J/psi
# constraints.append("OT3X : OT/T3X1U.Side : Tx")
# now fix OT2 because this was the only one that wasn't moved
constraints.append("OT2X : OT/T2X1U.Side : Tx")

# fix two C-frame layers in Z
#surveyconstraints.XmlUncertainties += ["OT/T1X1U : 0.5 0.5 0.00001 0.0001 0.0001 0.0001" ]
surveyconstraints.XmlUncertainties += ["OT/T3X1U : 0.5 0.5 0.00001 0.0001 0.0001 0.0001" ]

# move the IT boxes back to their survey position in Y, Rx and Ry
surveyconstraints.XmlUncertainties += [ "ITT.*?Box : 0.5 0.0001 0.5 0.000001 0.000001 0.001" ]

# move the IT layers  back to their survey position in Y, Z, Rx, Ry, and Rz
surveyconstraints.XmlUncertainties += [ "ITT.*?Layer.{1,2} : 0.5 0.0001 0.0001 0.000001 0.000001 0.000001" ]

# need to increase the TT module survey because of temperature expansion
# move TT back to survey in Z, Rx, Ry, Y
#surveyconstraints.XmlUncertainties += [ "TT..LayerR.Module.*? : 1.0 0.1 0.1 0.0005 0.0005 0.0005"]
surveyconstraints.XmlUncertainties += [ "TTSystem : 0.5 0.00001 0.0001 0.000001 0.0000001 0.001"]
surveyconstraints.XmlUncertainties += [ "TT..LayerR.Module.*? : 1.0 0.1 0.00001 0.0000001 0.0000001 0.0005"]

print "aligning elements ", elements

TAlignment().ElementsToAlign      = list(elements)
TAlignment().TrackLocation        = "Rec/Track/AlignTracks"
# comment following line if one does not want vertex
#TAlignment().VertexLocation       = "Rec/Vertex/AlignPrimaryVertices"
#TAlignment().VertexLocation       = "Rec/Vertex/Primary"
TAlignment().Constraints          = constraints
TAlignment().WriteCondSubDetList  = ['Velo','TT','IT','OT','MUON']
TAlignment().MinNumberOfHits = 6
#TAlignment().SolvTool = "gslSVDsolver"
TAlignment().DimuonLocation = "Rec/Vertex/DiMuon"

# still set up a track selection
from Configurables import (GaudiSequencer,TrackListRefiner,TrackListMerger,
                           TrackSelector,TrackMonitor,ITTrackSelector,ITTrackMonitor,
                           TrackHitAdder,TrackDoubleHitPruner,
                           TrackOTClusterPruner) 
from TrackFitter.ConfiguredFitters import *

GaudiSequencer("TrackAddExtraInfoSeq").Members += [
    TrackHitAdder( "TrackHitAdder",
                   TrackLocation = "Rec/Track/Best",
                   MaxDistVelo = -1,
                   MaxDistOT = -1,
                   MaxDistTT = 0.3,
                   MaxDistIT = 0.3),
    #TrackOTClusterPruner(),
    ConfiguredEventFitter("TrackRefitter",
                          TracksInContainer = "Rec/Track/Best")
    #,TrackDoubleHitPruner("TrackDoubleHitPruner",TrackLocation = "Rec/Track/Best")
    ]

alignSelectorAlgs = []
alignSelectorA = TrackListRefiner("AlignSelectorA",
                                  inputLocation = "Rec/Track/Best",
                                  outputLocation = "Rec/Track/AlignTracksA",
                                  Selector = TrackSelector())
alignSelectorA.Selector.MaxChi2Cut = 5
alignSelectorA.Selector.MaxChi2PerDoFMatch = 5
alignSelectorA.Selector.MaxChi2PerDoFVelo = 5
alignSelectorA.Selector.MaxChi2PerDoFDownstream = 5
alignSelectorA.Selector.MinNTTHits = 3
alignSelectorA.Selector.MinPCut  =   5000
alignSelectorA.Selector.MaxPCut  = 200000
alignSelectorA.Selector.MinPtCut = 2000
alignSelectorA.Selector.TrackTypes = ["Long","Downstream","Upstream"]
alignSelectorA.Selector.TrackTypes = ["Long","Downstream"]
alignSelectorA.Selector.TrackTypes = ["Long"]
alignSelectorAlgs.append( alignSelectorA )

#this one selects specifically overlap tracks. we only take T-tracks with overlaps
alignSelectorB = TrackListRefiner("AlignSelectorB",
                                  inputLocation = "Rec/Track/Best",
                                  outputLocation = "Rec/Track/AlignTracksB",
                                  Selector = ITTrackSelector())
alignSelectorB.Selector.RequireOverlap = True
alignSelectorB.Selector.MinPCut =  10000
alignSelectorB.Selector.MaxPCut = 200000
alignSelectorB.Selector.MinPtCut = 500
alignSelectorB.Selector.MaxChi2Cut = 5
alignSelectorB.Selector.MaxChi2PerDoFMatch = 5
alignSelectorB.Selector.MaxChi2PerDoFVelo = 5
alignSelectorB.Selector.MaxChi2PerDoFDownstream = 5
alignSelectorB.Selector.MinNTTHits = 3
alignSelectorB.Selector.TrackTypes = ["Long","Downstream","Ttrack"]
alignSelectorB.Selector.TrackTypes = ["Long"]
alignSelectorAlgs.append( alignSelectorB )

#this one selects specifically overlap tracks. we only take T-tracks with overlaps
alignSelectorITMod = TrackListRefiner("AlignSelectorITMod",
                                  inputLocation = "Rec/Track/Best",
                                  outputLocation = "Rec/Track/AlignTracksITMod",
                                  Selector = ITTrackSelector())
alignSelectorITMod.Selector.RequireModuleOverlap = True
alignSelectorITMod.Selector.MinPCut =  10000
alignSelectorITMod.Selector.MaxPCut = 500000
alignSelectorITMod.Selector.MinPtCut = 100
alignSelectorITMod.Selector.MaxChi2Cut = 5
alignSelectorITMod.Selector.MaxChi2PerDoFMatch = 5
alignSelectorITMod.Selector.MaxChi2PerDoFVelo = 5
alignSelectorITMod.Selector.MaxChi2PerDoFDownstream = 5
alignSelectorITMod.Selector.MinNTTHits = 3
alignSelectorITMod.Selector.TrackTypes = ["Long"]
alignSelectorAlgs.append( alignSelectorITMod )

# I'll also add high momentum T tracks
alignSelectorT = TrackListRefiner("AlignSelectorT",
                                  inputLocation = "Rec/Track/Best",
                                  outputLocation = "Rec/Track/AlignTracksT",
                                  Selector = TrackSelector())
alignSelectorT.Selector.MinPCut = 50000
alignSelectorT.Selector.MaxChi2Cut = 5
alignSelectorT.Selector.TrackTypes = ["Ttrack"]
#alignSelectorAlgs.append( alignSelectorT )


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
#alignSelectorAlgs.append( alignSelectorC )

#select Velo overlap tracks.
from Configurables import VeloTrackSelector
alignSelectorD = TrackListRefiner("AlignSelectorD",
                                  inputLocation = "Rec/Track/Best",
                                  outputLocation = "Rec/Track/AlignTracksD",
                                  Selector = VeloTrackSelector())
alignSelectorD.Selector.MaxChi2PerDoFVelo = 10
alignSelectorD.Selector.MaxChi2Cut = 5
alignSelectorD.Selector.MinHitsASide = 1
alignSelectorD.Selector.MinHitsCSide = 1
alignSelectorD.Selector.MinNVeloRHits = 4
alignSelectorD.Selector.MinNVeloPhiHits = 4
alignSelectorD.Selector.MaxNVeloHoles = 0
#alignSelectorAlgs.append( alignSelectorD )

# select muons
alignSelectorDiMuons = TrackListRefiner("AlignSelectorDiMuons",
                                        inputLocation = "Rec/Track/DiMuonDaughters",
                                        outputLocation = "Rec/Track/AlignTracksDiMuons",
                                        Selector = TrackSelector())
alignSelectorDiMuons.Selector.MaxChi2PerDoFVelo = 5
alignSelectorDiMuons.Selector.MaxChi2Cut = 5
alignSelectorDiMuons.Selector.MinPCut = 5000
alignSelectorAlgs.append( alignSelectorDiMuons )

trackFilterSeq = GaudiSequencer("TrackFilterSeq")
trackFilterSeq.Members += alignSelectorAlgs

trackFilterSeq.Members.append( TrackListMerger("AlignTrackMerger",
                                               #inputLocations= [ "Rec/Track/AlignTracksA", "Rec/Track/AlignTracksB",
                                               #                  "Rec/Track/AlignTracksC", "Rec/Track/AlignTracksD",
                                                                #                  "Rec/Track/AlignTracksT"],
                                               inputLocations= [ "Rec/Track/AlignTracksA",
                                                                 "Rec/Track/AlignTracksB",
                                                                 "Rec/Track/AlignTracksITMod",
                                                                 "Rec/Track/AlignTracksDiMuons"],
                                               outputLocation = "Rec/Track/AlignTracks") )

for selalg in alignSelectorAlgs:
    trackFilterSeq.Members.append( TrackMonitor( selalg.name() + "Monitor",
                                                 TracksInContainer = selalg.outputLocation ) )
    trackFilterSeq.Members.append( ITTrackMonitor( selalg.name() + "ITMonitor",
                                                 TracksInContainer = selalg.outputLocation ) )
#
#from Configurables import TrackListPrinter
#trackFilterSeq.Members.append(TrackListPrinter("BackwardPrinter",InputLocation="Rec/Track/AlignTracksC"))

# now select the primary vertices
from Configurables import VertexListRefiner
pvSelector = VertexListRefiner("AlignPVSelector",
                               InputLocation = "Rec/Vertex/Primary",
                               OutputLocation = "Rec/Vertex/AlignPrimaryVertices",
                               MaxChi2PerDoF = 5,
                               MinNumTracks = 10,
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
    eventfitter.Fitter.ErrorX = 200
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
    
    ApplicationMgr().ExtSvc += [ "DataOnDemandSvc","ANNDispatchSvc"]
                                 
    # insert a filter get only dimuons
    from Configurables import LoKi__HDRFilter as HDRFilter 
    hltfilter = HDRFilter ( 'HLT2JPsi' ,
                            Code = "HLT_PASS_RE( 'Hlt2.*Muon.*JPsi.*Decision' )"
                            #Code = "HLT_PASS_RE( '.*Decision' )"
                            #Code = "HLT_PASS_RE('Hlt2.*Decision')"
                            #Code ="HLT_PASS('Hlt2ExpressHLT1PhysicsDecision')"
                            )
    hltfilter.Preambulo += [ "from LoKiCore.functions import *" ]
                           
    from Configurables import HltDecReportsDecoder
    DataOnDemandSvc().AlgMap["Hlt/DecReports"] = HltDecReportsDecoder(OutputLevel = 4)
    GaudiSequencer('EscherSequencer').Members.insert(1,hltfilter)
    #from Configurables import DecReportDumper
    #GaudiSequencer('EscherSequencer').Members.insert(2,DecReportDumper())

    #from Configurables import LoKi__VoidFilter as LokiFilter
    #stripfilter = LokiFilter ( 'JPsiMuMuFilter' ,
    #                           Code = "0 < CONTAINS ( '/Event/Dimuon/Phys/FullDSTDiMuonJpsi2MuMuLine/Particles' )"
    #                           #,Preambulo = [ "from LoKiTrigger.decorators import *" ] 
    #                           )
    #GaudiSequencer('EscherSequencer').Members.insert(2,stripfilter)
    
    # add a filter on Velo tracks
    from Configurables import TrackListFilter
    GaudiSequencer("RecoVELOSeq").Members += [TrackListFilter("VeloFilter",
                                                              inputLocation = "Rec/Track/Velo")]

    from Configurables import TrackMultiplicityFilter
    GaudiSequencer("RecoDecodingSeq").Members += [TrackMultiplicityFilter("MultFilter",
                                                                          MaxNumITHits = 600)]

    from Configurables import TrackDiMuonMonitor
    TrackDiMuonMonitor().MaxTwoProngVertexChi2 = 4

    from Configurables import TrackHitMapMonitor
    GaudiSequencer("MoniTrSeq").Members.append( TrackHitMapMonitor() )

    #from Configurables import STOfflinePosition
    #itClusterPosition = STOfflinePosition('ToolSvc.ITClusterPosition')
    #itClusterPosition.ErrorVec = [0.28, 0.22, 0.35, 0.35]
    #itClusterPosition.APE = 0.1

    from Configurables import ST__STClusterMonitor
    ST__STClusterMonitor('ITClusterMonitor').HitMaps = True

    # be a bit more restrictive about clones: basically, reject all overlaps!
    #from Configurables import TrackEventCloneKiller, TrackCloneFinder
    #clonekiller = TrackEventCloneKiller()
    #clonefinder = clonekiller.addTool( TrackCloneFinder, name = "CloneFinderTool" )
    #clonekiller.CloneFinderTool.CompareLDT = True
    #clonekiller.CloneFinderTool.MatchingFractionT = 0.01
    #clonekiller.CloneFinderTool.MatchingFraction  = 0.01
    #clonekiller.SkipSameContainerTracks = False
    
appendPostConfigAction(doMyAlignChanges)
