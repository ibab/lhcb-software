###################################################
# All job-options relevant to the BeamGas Alleys: #
# Set the sequences and configure the algorithms  #
###################################################

from Gaudi.Configuration import *
from Configurables import GaudiSequencer

from Configurables import DecodeVeloRawBuffer, Tf__PatVeloRTracking
from Configurables import BeamGasTrigCheckL0TracksBXType, BeamGasTrigExtractClusters, BeamGasTrigClusterCut, BeamGasTrigVertexCut

############################################
# Set the sequences of the beam gas alleys #
# 1) BeamGas overlapped with PP sub-alley  #
# 2) BeamGas only sub-alley                #
############################################

ApplicationMgr().TopAlg += [ GaudiSequencer('BeamGasAlleys') ]
GaudiSequencer('BeamGasAlleys').MeasureTime  = True
GaudiSequencer('BeamGasAlleys').ShortCircuit = False # ==> always execute both sub alleys
GaudiSequencer('BeamGasAlleys').Members += [ GaudiSequencer('BeamGasWithPPSequence') ]
GaudiSequencer('BeamGasAlleys').Members += [ GaudiSequencer('BeamGasOnlySequence') ]


######################################################################
# 1) BeamGas overlapped with PP sub-alley                            #
# Set the content of the sequence and configure the added algorithms #
######################################################################

BeamGasWithPPSeq              = GaudiSequencer('BeamGasWithPPSequence')
BeamGasWithPPSeq.ShortCircuit = True #Allow exiting of the sequence before the last algorithm

BeamGasWithPPSeq.Members += [GaudiSequencer('RZVeloTracksSequence')]
# Here we are producing the RZ tracks ourselves by adding this sequence
# (which includes just DecodeVeloBuffer and RZTracking1)
# When running in HLT, the BeamGasAlley sholud not include it

BeamGasWithPPSeq.Members += [BeamGasTrigCheckL0TracksBXType('L0TracksBXBeamGasWithPP')]
BeamGasWithPPSeq.Members += ['BeamGasTrigClusterCut']
BeamGasWithPPSeq.Members += ['BeamGasTrigExtractClusters']
BeamGasWithPPSeq.Members += [ Tf__PatVeloRTracking('RZTracking2') ]
BeamGasWithPPSeq.Members += ['BeamGasTrigVertexCut']

#-----------------------------------------------------
# Set the properties of these algs in the same order
#-----------------------------------------------------

RZVeloTracksSeq = GaudiSequencer( 'RZVeloTracksSequence', MeasureTime = True
                                  ,Members = [ DecodeVeloRawBuffer() , Tf__PatVeloRTracking('RZTracking1') ] )      
RZTracking1 = Tf__PatVeloRTracking('RZTracking1')
RZTracking1.OutputTracksName = "Hlt/Track/RZVelo"


algConf_CheckL0TracksBXType_BGWithPP = BeamGasTrigCheckL0TracksBXType('L0TracksBXBeamGasWithPP')                                    
algConf_CheckL0TracksBXType_BGWithPP.CheckL0Decision = True
algConf_CheckL0TracksBXType_BGWithPP.L0DULocation = "Trig/L0/L0DUReport"
algConf_CheckL0TracksBXType_BGWithPP.CheckTracks = True
algConf_CheckL0TracksBXType_BGWithPP.PVRTTracksLocation = "Hlt/Track/RZVelo"
algConf_CheckL0TracksBXType_BGWithPP.ChechBXType = True
algConf_CheckL0TracksBXType_BGWithPP.ODINLocation = "DAQ/ODIN"
algConf_CheckL0TracksBXType_BGWithPP.BXTypeAccept1 = 0
algConf_CheckL0TracksBXType_BGWithPP.BXTypeAccept2 = 0
algConf_CheckL0TracksBXType_BGWithPP.OutputLevel = INFO
#print algConf_CheckL0TracksBXType_BGWithPP  
  

algConf_ClusterCut = BeamGasTrigClusterCut()
algConf_ClusterCut.SensorsBegin = 22
algConf_ClusterCut.SensorsEnd   = 39
algConf_ClusterCut.FracUnusedClustersCut = 0.27
algConf_ClusterCut.OutputLevel = INFO
#print algConf_ClusterCut	


algConf_ExtractClusters = BeamGasTrigExtractClusters()
algConf_ExtractClusters.OutputLocation = "Raw/Velo/UnusedRLiteClusters"
algConf_ExtractClusters.OutputLevel = INFO
#print algConf_ExtractClusters


RZTracking2 = Tf__PatVeloRTracking('RZTracking2')
RZTracking2.HitManagerName = "SecondDefaultVeloRHitManager"
#ToolSvc.SecondDefaultVeloRHitManager.LiteClusterLocation = "Raw/Velo/UnusedRLiteClusters"
RZTracking2.OutputTracksName = "Hlt/Track/RZVeloBeamGas"
RZTracking2.ZVertexMin = -1500.
RZTracking2.ZVertexMax = 1500.
RZTracking2.OutputLevel = INFO
#print RZTracking2


algConf_VertexCut = BeamGasTrigVertexCut()
algConf_VertexCut.OutputLevel = INFO
algConf_VertexCut.RZTracksInputLocation = "Hlt/Track/RZVeloBeamGas"
algConf_VertexCut.MaxBinValueCut   = 5 #Triggering cut 
algConf_VertexCut.HistoBinWidth    = 10
algConf_VertexCut.ZExclusionRangeLow  = -265.
algConf_VertexCut.ZExclusionRangeUp   = 265.
algConf_VertexCut.OutputLevel = INFO
#HltAlgorithm related
algConf_VertexCut.MinCandidates    = 1 #Should be > 0 and <= MaxBinValueCut+1 (by default = 1)
#algConf_VertexCut.OutputSelectionName = ... #By default = name of the algorithm
print algConf_VertexCut


######################################################################
# 2) BeamGas only sub-alley (no overlapped pp collision)             #
# Set the content of the sequence and configure the added algorithms #
######################################################################

BeamGasOnlySeq = GaudiSequencer('BeamGasOnlySequence')
BeamGasOnlySeq.MeasureTime = True
BeamGasOnlySeq.ShortCircuit = True #Allow exiting the sequence before the last algorithm

BeamGasOnlySeq.Members += [BeamGasTrigCheckL0TracksBXType('L0TracksBXBeamGasOnly')]
BeamGasOnlySeq.Members += [ Tf__PatVeloRTracking('RZTrackingBGOnly') ]
BeamGasOnlySeq.Members += ['BeamGasTrigVertexCut/BeamGasTrigVertexCutBGOnly']

#-----------------------------------------------------
# Set the properties of these algs in the same order
#-----------------------------------------------------	
algConf_CheckL0TracksBXType_BGOnly = BeamGasTrigCheckL0TracksBXType('L0TracksBXBeamGasOnly')

algConf_CheckL0TracksBXType_BGOnly.CheckL0Decision = False
#algConf_CheckL0TracksBXType_BGOnly.L0DULocation = "Trig/L0/L0DUReport"
algConf_CheckL0TracksBXType_BGOnly.CheckTracks = False
#algConf_CheckL0TracksBXType_BGOnly.PVRTTracksLocation = "Hlt/Track/RZVelo"
algConf_CheckL0TracksBXType_BGOnly.ChechBXType = True
algConf_CheckL0TracksBXType_BGOnly.ODINLocation = "DAQ/ODIN"
algConf_CheckL0TracksBXType_BGOnly.BXTypeAccept1 = 1
algConf_CheckL0TracksBXType_BGOnly.BXTypeAccept2 = 2
algConf_CheckL0TracksBXType_BGOnly.OutputLevel = INFO
#print algConf_CheckL0TracksBXType_BGOnly 


RZTrackingBGOnly = Tf__PatVeloRTracking('RZTrackingBGOnly')
RZTrackingBGOnly.OutputTracksName = 'Hlt/Track/RZVeloBeamGasOnly'
RZTrackingBGOnly.ZVertexMin = -1500.0
RZTrackingBGOnly.ZVertexMax = 1500.0
RZTrackingBGOnly.OutputLevel = INFO


algConf_VertexCutBGOnly = BeamGasTrigVertexCut('BeamGasTrigVertexCutBGOnly')
algConf_VertexCutBGOnly.RZTracksInputLocation = "Hlt/Track/RZVeloBeamGasOnly"
algConf_VertexCutBGOnly.MaxBinValueCut     = 5
algConf_VertexCutBGOnly.HistoBinWidth      = 10
algConf_VertexCutBGOnly.ZExclusionRangeLow = 0. 
algConf_VertexCutBGOnly.ZExclusionRangeUp  = 0.   
algConf_VertexCutBGOnly.OutputLevel = INFO
#HltAlgorithm related
algConf_VertexCutBGOnly.MinCandidates    = 1 #Should be > 0 and <= MaxBinValueCut+1 (by default = 1)
#algConf_VertexCutBGOnly.OutputSelectionName = ... #By default = name of the algorithm
print algConf_VertexCutBGOnly

