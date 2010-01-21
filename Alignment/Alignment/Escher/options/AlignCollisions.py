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

elements = Alignables()
constraints = []
surveyconstraints = SurveyConstraints()

elements.Velo("Rx")
elements.VeloRight("Ry")
#elements.Tracker("Rz")
elements.TT("TxTy")
elements.TTHalfLayers("Tx")
elements.TTHalfModules("TxRz")
#elements.TTHalfModules("Tx")
#elements.ITLayers("Tx")
#elements.IT("TxTy")
elements.ITBoxes("TxTyRz")
elements.ITLayers("Tx")
elements.ITLadders("TxRz")
elements.OTCFrames("TxTyRz")
#elements.OTModules("TxRz")

#surveyconstraints.ITBoxes()
#surveyconstraints.OTCFrames()
surveyconstraints.All()

#elements.OTCFrames("TxTy")
#surveyconstraints.append("/.*?/IT/.*?Layer.{1,2} : Tx : 0.02")
#surveyconstraints.append("/.*?/IT/.*?Layer.{1,2} : Rz : 0.001")

#constraints.append("OTCSide : OT/T3VX2CSide : Tx")
#constraints.append("OTASide : OT/T3VX2ASide : Tx")

# make sure to constrain both q/P and a twist. does this work?
constraints.append("OT3 : OT/T3VX2.Side : Tx Rz")
constraints.append("TTHalfLayerAverage : TT..Layer.Side : Tx : total") ;

#constraints.append("ITGlobal : T/IT : Tx Ty : total")


print "aligning elements ", elements

TAlignment().ElementsToAlign      = list(elements)
TAlignment().TrackLocation        = "Rec/Track/AlignTracks"
# comment following line if one does not want vertex
#TAlignment().VertexLocation       = "Rec/Vertex/Primary"
TAlignment().Constraints          = constraints
TAlignment().SurveyConstraints    = list(surveyconstraints)
TAlignment().WriteCondSubDetList  = ['Velo','TT','IT','OT']
#TAlignment().EigenValueThreshold   = 100
#TAlignment().MinEigenModeChisquare = 100
TAlignment().MinNumberOfHits = 1

# still set up a track selection
from Configurables import (GaudiSequencer,TrackListRefiner,TrackListMerger,
                           TrackSelector,TrackMonitor,ITTrackSelector) 
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

from Configurables import TrackCaloMatchMonitor, TrackListRefiner, TrackITOverlapMonitor

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
                                                                    TrackLocation = "Rec/Track/Long")]
#from Configurables import OTCalibrationIO
#GaudiSequencer("RecoOTSeq").Members.append(OTCalibrationIO())

from Configurables import TrackV0Finder, TrackV0Monitor, TrackV0NtupleMonitor

tweakedV0Finder = TrackV0Finder("TweakedV0Finder",
                                V0Container = 'Rec/Vertex/TweakedV0',
                                KsMassCut = 300,
                                MaxChi2PVConstraint = 1000,
                                MinDecayLengthSignificance = 5,
                                AddExtraInfo = True,
                                ExcludePVTracks = False,
                                #MinDeltaZ = 20,
                                #MaxDistanceLong = 10,
                                #MaxDistanceUpstream = 10,
                                RejectUpstreamHits = False )
TrackV0Monitor().FullDetail = True
TrackV0Finder(KsMassCut = 300,
              AddExtraInfo = True,
              ExcludePVTracks = False)
              
from Configurables import TrackPVFinder
GaudiSequencer("RecoVertexSeq").Members.append( TrackPVFinder() )
GaudiSequencer("MoniTrSeq").Members.append( TrackV0NtupleMonitor(V0Location = 'Rec/Vertex/TweakedV0') )

from Configurables import NTupleSvc
ntupleOutputFile = "tuples.root"
NTupleSvc().Output=["FILE1 DATAFILE='"+ntupleOutputFile+"' TYP='ROOT' OPT='NEW'"]

def doMyAlignChanges():
    from Configurables import TESCheck
    TESCheck().Stop = False
    TESCheck().Enable = False
    
    from Configurables import TrackEventFitter, VeloTrackSelector
    from TrackFitter.ConfiguredFitters import ConfiguredStraightLineFitter
    TrackEventFitter('FitMatch').Fitter.MaxNumberOutliers = 0
    TrackEventFitter('FitSeed').Fitter.MaxNumberOutliers = 0
    TrackEventFitter('RefitSeed').Fitter.MaxNumberOutliers = 0
    TrackEventFitter('FitForward').Fitter.MaxNumberOutliers = 0
    TrackEventFitter('FitVelo').Fitter.MaxNumberOutliers = 0
    TrackEventFitter('FitVeloTT').Fitter.MaxNumberOutliers = 0

    GaudiSequencer("RecoVertexSeq").Members = [ TrackPVFinder(),
                                                tweakedV0Finder,
                                                TrackV0Finder() ]

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


appendPostConfigAction(doMyAlignChanges)
