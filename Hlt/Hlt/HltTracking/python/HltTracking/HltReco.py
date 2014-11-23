#!/usr/bin/env python
# =============================================================================
# $Id: HltReco.py,v 1.6 2010-09-07 12:30:12 graven Exp $
# =============================================================================
## @file HltTracking/HltReco.py
#  Collection of predefined algorithms to perform reconstruction
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2009-04-03
# =============================================================================
#
# the (ordered) list of (configurables corresponding to the) algorithms needed to 
# obtain the Hlt1 selection 'Velo' can be obtained by doing 
#
#   from HltTracking.HltReco import Velo
#   Velo.members() 
#
# in addition, the 'outputselection' of the last member can be 
# obtained by doing:
#
#   Velo.outputSelection()
#
# which should return 'Velo' of course...
# The returned object can be used in the 'algos' list used by Hlt1Line,
# and, given that 'outputSelection' is properly set, partakes in the automated
# daisychaining present in Hlt1Line. In case you do _not_ want this
# daisychaining, you can do:
#
#  Velo.ignoreOutputSelection()
#
# which 'skips' Velo for the daisychaining (but will insert the right configurables)
#

__all__ = ( 'MinimalRZVelo'   # bindMembers instance with algorithms needed to get 'MinimalRZVelo' 
          , 'MinimalVelo'
          , 'RevivedVelo'
          , 'Velo'            # bindMembers instance with algorithms needed to get 'Velo'
          , 'RevivedForward'
          , 'Hlt1Seeding'
          , 'VeloCandidates'
          , 'MaxOTHits'
          )

MaxOTHits = 15000 #--> move to CommonForwardOptions

CommonMatchVeloMuonOptions = {"MaxChi2DoFX" : 10,
                              "XWindow" : 200,
                              "YWindow" : 200  }

CommonpETOptions = {"minMomentum" : 2000.0 ,
                    "DxGroupFactor" : 0.0,
                    "maxPseudoChi2" : 256.0 ,
                    "maxSolutionsPerTrack" : 1,
                    "fitTracks" : False,
                    "maxChi2" : 256.}

CommonVeloTTFitterOptions = {"NumberFitIterations" : 1,
                             "MaxNumberOutliers" : 1,
                             "Extrapolator.ExtraSelector" : "TrackSimpleExtraSelector",
                             "StateAtBeamLine" : False}

CommonVeloTTOptions = {"minMomentum" : 2000.0,
                       "minPT" : 200.0 ,
                       "PassTracks" : True,
                       "centralHoleSize" : 33,
                       "PassHoleSize" : 45,
                       "maxPseudoChi2" : 256.0,
                       "DxGroupFactor" : 0.0,
                       "maxSolutionsPerTrack" : 1,
                       "fitTracks" : False,
                       "maxChi2" : 256.}

CommonForwardOptions = { "MaxOTHits" : 150000 }
    

CommonForwardTrackingOptions = { "MaxChi2" : 40,
                                 "MaxChi2Track" : 40,
                                 "MinHits" : 12,  
                                 "MinOTHits" : 14,
                                 "SecondLoop" : True,
                                 "FlagUsedSeeds" : True }
 
ForwardTrackingOptions_MomEstimate = { "UseMomentumEstimate" : True
                                       ,"UseWrongSignWindow" : True
                                       ,"WrongSignPT" : 2000
                                       ,"Preselection" : True }


CommonForwardTrackingOptions_EarlyData = { "MaxChi2" : 40, 
                                 "MaxChi2Track" : 40, 
                                 "MinHits" : 12,  
                                 "MinOTHits" : 14,
                                 "MinXPlanes" : 4,
                                 "MinPlanes" : 8,
                                 "MaxSpreadX" : 1.5,
                                 "MaxSpreadY" : 3.0  }
CommonSeedingTrackingOptions_EarlyData = { "OTNHitsLowThresh" : 12,
                                           "MinTotalPlanes" : 7,
                                           "MaxMisses" : 2,
                                           "MaxTrackChi2LowMult" : 10,
                                           "MaxFinalTrackChi2" : 20,
                                           "MaxFinalChi2" : 30,
                                           "MaxTrackChi2" : 40,
                                           "MaxChi2HitIT" : 10,
                                           "MaxChi2HitOT" : 30}    
CommonDownstreamTrackingOptions_EarlyData = { "xPredTol2" : 20.0,
                                              "TolMatch" : 1.5,
                                              "TolUV" : 2.0,
                                              "maxWindowSize" : 10.0,
                                              "MaxChisq" : 20.0,
                                              "MaxDistance" : 0.3,
                                              "deltaP" : 2.0,
                                              "errorZMagnet" : 30.0 }      

############################################################################################
# Option to decide which pattern to use
############################################################################################
#############################################################################################
# Import Configurables
#############################################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import Tf__PatVeloGeneric, Tf__PatVeloRTracking, FastVeloTracking
from Configurables import PVOfflineTool
from HltLine.HltLine import bindMembers
from Configurables import PatSeeding, PatSeedingTool

# New tune JvT 2010-07-14
# globally set the resolutions for ST
#when syntax errors there are fixed take from 
#importOptions( "$STTOOLSROOT/options/Brunel_EarlyData.opts" )
from Configurables import STOfflinePosition, STOnlinePosition

STOnlinePosition().ErrorVec=[0.257, 0.245, 0.277]
STOnlinePosition().APE = 0.197

ITOnl = STOnlinePosition("ITLiteClusterPosition")
ITOnl.ErrorVec=[0.253,0.236,0.273]
ITOnl.APE=0.0758
#ITOnl.DetType = "IT"

STPos = STOfflinePosition("ITClusterPosition")
STPos.LinSharingCorr2 = -0.0152  
STPos.CubicSharingCorr2 = 12.33 
STPos.CubicSharingCorr3 = 4.369 
STPos.LinSharingCorr4 = 0.530 
STPos.DetType = "IT"
STPos.ErrorVec =[ 0.253, 0.236, 0.273, 0.185 ]
STPos.APE = 0.0758

STOfflinePosition().ErrorVec = [0.257, 0.245, 0.277, 0.208]
STOfflinePosition().APE = 0.197

from Configurables import OTRawBankDecoder
OTRawBankDecoder().TimeWindow = ( -8.0, 56.0 ) # add units: ns!!


#############################################################################################
# Configure pattern recognition algorithms
#############################################################################################

#This is the one unavoidable piece of hardcoding since this is the piece
#shared between Hlt1 and Hlt2
#from HltTrackNames import HltSharedRZVeloTracksName, HltSharedVeloTracksName, HltSharedTracksPrefix 

#from HltTrackNames import HltSharedVeloLocation, HltSharedVeloTTLocation, HltSharedForwardLocation
from HltTrackNames import Hlt1TrackLoc, HltSharedTrackLoc, Hlt2TrackLoc

#from HltTrackNames import Hlt1TracksPrefix, _baseTrackLocation, Hlt1SeedingTracksName  
from Configurables import Tf__PatVeloSpaceTracking, Tf__PatVeloSpaceTool
from Configurables import FastVeloHitManager, DecodeVeloRawBuffer
from Configurables import TrackStateInitAlg, TrackStateInitTool

#### Velo Tracking

# deprecated algorithm
# patVeloR = Tf__PatVeloRTracking('HltRecoRZVelo', OutputTracksName = _baseTrackLocation(HltSharedTracksPrefix,HltSharedRZVeloTracksName) ) 
#

# the full Velo reconstruction
def recoVelo(OutputLocation=HltSharedTrackLoc["Velo"]):
    recoVelo = FastVeloTracking( 'FastVeloHlt', OutputTracksName = OutputLocation) 
    recoVelo.HLT1Only = False 
    recoVelo.HLT2Complement = False 
    recoVelo.StatPrint = True
    return recoVelo
  

#### VeloTT Tracking
from Configurables import PatVeloTTHybrid, PatVeloTTHybridTool
recoVeloTT = PatVeloTTHybrid( 'PatVeloTTHlt', 
                        InputTracksName = HltSharedTrackLoc["Velo"],
                        OutputTracksName = Hlt1TrackLoc["VeloTTHPT"],
                        fitTracks=False )
recoVeloTT.addTool(PatVeloTTHybridTool, name="PatVeloTTTool")
recoVeloTT.PatVeloTTTool.getProperties().update(CommonVeloTTOptions)
recoVeloTT.PatVeloTTTool.StatPrint = True

#### Forward Tracking
from Configurables import PatForward, PatForwardTool
recoForward = PatForward( 'RecoForwardHlt'
                          , InputTracksName  = recoVeloTT.OutputTracksName 
                          , OutputTracksName = Hlt1TrackLoc["ForwardHPT"] 
                          , maxOTHits=MaxOTHits)
recoForward.addTool(PatForwardTool, name='PatForwardTool')
recoForward.PatForwardTool.getProperties().update(CommonForwardTrackingOptions)
recoForward.PatForwardTool.getProperties().update(ForwardTrackingOptions_MomEstimate)
#### TODO: WHERE DO WE PUT THESE VALUES???
recoForward.PatForwardTool.MinMomentum = 3000
recoForward.PatForwardTool.MinPt = 500
recoForward.PatForwardTool.StatPrint = True

##### Hlt selections
from Configurables import Hlt__TrackFilter as HltTrackFilter
prepare3DVelo = HltTrackFilter( 'Hlt1Prepare3DVelo'
                              , InputSelection   = "TES:" + HltSharedTrackLoc["Velo"]
                              , RequirePositiveInputs = False
                              , Code = [ '~TrBACKWARD' ] 
                              , OutputSelection     = "Velo" )


#############################################################################################
# Define modules for the reconstruction sequence 
#############################################################################################
from HltLine.HltDecodeRaw import DecodeVELO, DecodeTRACK, DecodeTT, DecodeIT
from Configurables import DecodeVeloRawBuffer, Hlt2Conf

### define exported symbols (i.e. these are externally visible, the rest is NOT)
#This is the part which is shared between Hlt1 and Hlt2
MinimalVelo = bindMembers( None, [DecodeVELO, recoVelo(OutputLocation=HltSharedTrackLoc["Velo"] ) ] ).setOutputSelection( HltSharedTrackLoc["Velo"] )
RevivedVelo = bindMembers(None, [DecodeVELO, DecodeTRACK]).setOutputSelection( HltSharedTrackLoc["Velo"] )
RevivedForward = bindMembers(None,DecodeTT.members() + DecodeIT.members() + [ DecodeTRACK ]).setOutputSelection( Hlt1TrackLoc["Forward"] )

# put selection revive/redo here
# for now always redo:
bm_members =  DecodeVELO.members() + [recoVelo()]
bm_members += DecodeTT.members() + [recoVeloTT] 
bm_members += DecodeIT.members() + [recoForward]
HltTracking = bindMembers(None, bm_members).setOutputSelection( recoForward.OutputTracksName )

#VeloTT tracking
vt_members = DecodeVELO.members() + [ recoVelo() ]
vt_members += DecodeTT.members()  + [ recoVeloTT ]
VeloTTTracking = bindMembers(None, vt_members).setOutputSelection( recoVeloTT.OutputTracksName ) 

# ==============================================================================
# Hlt1Seeding, used by MicroBias
# ==============================================================================
from Configurables import PatSeeding, PatSeedingTool
from HltLine.HltDecodeRaw import DecodeIT
ps = PatSeeding('Hlt1MBSeeding' , OutputTracksName = Hlt1TrackLoc["Seeding"])
ps.addTool(PatSeedingTool, name="PatSeedingTool")
ps.PatSeedingTool.MaxOTHits = MaxOTHits

Hlt1Seeding = bindMembers( None, [ DecodeIT, ps ] )
