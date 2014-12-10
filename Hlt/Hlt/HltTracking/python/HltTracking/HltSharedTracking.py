#!/usr/bin/env python
# =============================================================================
## @file HltTracking/HltSharedTracking.py
#  was HltReco.py
#  Collection of predefined algorithms to perform reconstruction
#  @author Sebastian Neubert sebastian.neubert@cern.ch, Gerhard Raven Gerhard.Raven@nikhef.nl
# =============================================================================
#

__all__ = ( 'MinimalVelo'
          , 'RevivedVelo'
          , 'RevivedForward'
          , 'Hlt1Seeding'
          , 'HltHPTTracking'
          , 'VeloTTTracking'  
          )


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

from HltTrackNames import Hlt1TrackLoc, HltSharedTrackLoc, Hlt2TrackLoc

from Configurables import Tf__PatVeloSpaceTracking, Tf__PatVeloSpaceTool
from Configurables import FastVeloHitManager, DecodeVeloRawBuffer
from Configurables import TrackStateInitAlg, TrackStateInitTool

#### Velo Tracking

# the full Velo reconstruction
def recoVelo(OutputLocation=HltSharedTrackLoc["Velo"]):
    recoVelo = FastVeloTracking( 'FastVeloHlt', OutputTracksName = OutputLocation) 
    recoVelo.HLT1Only = False 
    recoVelo.HLT2Complement = False 
    recoVelo.StatPrint = True
    return recoVelo
 

#### filter the Velo output
from Configurables import TrackListRefiner, HltRecoConf
filterVelo = TrackListRefiner('VeloFilter',
                              inputLocation = HltSharedTrackLoc["Velo"],
                              outputLocation = HltSharedTrackLoc["VeloSelection"])
from Configurables import  LoKi__Hybrid__TrackSelector as LoKiTrackSelector
filterVelo.addTool(LoKiTrackSelector,name="VeloSelector")
filterVelo.Selector = LoKiTrackSelector(name="VeloSelector")
veloSelector = filterVelo.Selector
veloSelector.Code = HltRecoConf().getProp("VeloSelectionCut")
veloSelector.StatPrint = True


#### VeloTT Tracking
from HltTracking.HltRecoConf import VeloTTOptions, VeloTTToolOptions
from Configurables import PatVeloTTHybrid, PatVeloTTHybridTool
recoVeloTT = PatVeloTTHybrid( 'PatVeloTTHlt', 
                        InputTracksName = HltSharedTrackLoc["VeloSelection"],
                        OutputTracksName = HltSharedTrackLoc["VeloTTHPT"],
                        **VeloTTOptions )
recoVeloTT.addTool(PatVeloTTHybridTool(**VeloTTToolOptions), name="PatVeloTTTool")
recoVeloTT.PatVeloTTTool.StatPrint = True


#### Forward Tracking
from Configurables import PatForward, PatForwardTool
from Configurables import HltRecoConf
recoForward = PatForward( 'RecoForwardHlt'
                          , InputTracksName  = recoVeloTT.OutputTracksName 
                          , OutputTracksName = HltSharedTrackLoc["ForwardHPT"]
                          , maxOTHits=HltRecoConf().getProp("Forward_MaxOTHits") )

# apply modifications on top
from HltTracking.HltRecoConf import CommonForwardTrackingOptions, ForwardTrackingOptions_MomEstimate
opts = CommonForwardTrackingOptions
opts.update(ForwardTrackingOptions_MomEstimate)
recoForward.addTool(PatForwardTool(**opts), name='PatForwardTool')

# update configuration with HLT specific tweaks
recoForward.PatForwardTool.MinMomentum = HltRecoConf().getProp("Forward_HPT_MinP")
recoForward.PatForwardTool.MinPt = HltRecoConf().getProp("Forward_HPT_MinPt")
recoForward.PatForwardTool.StatPrint = True
recoForward.StatPrint = True


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
RevivedForward = bindMembers(None,DecodeTT.members() + DecodeIT.members() + [ DecodeTRACK ]).setOutputSelection( HltSharedTrackLoc["Forward"] )

# TODO: put selection revive/redo here (ask Sebastian)
# for now always redo:
bm_members =  DecodeVELO.members() + [recoVelo(), filterVelo] 
bm_members += DecodeTT.members() + [recoVeloTT] 
bm_members += DecodeIT.members() + [recoForward]

HltHPTTracking = bindMembers(None, bm_members).setOutputSelection( recoForward.OutputTracksName )

#VeloTT tracking
vt_members = DecodeVELO.members() + [ recoVelo(), filterVelo ]
vt_members += DecodeTT.members()  + [ recoVeloTT ]
VeloTTTracking = bindMembers(None, vt_members).setOutputSelection( recoVeloTT.OutputTracksName ) 

# ==============================================================================
# Hlt1Seeding, used by MicroBias
# ==============================================================================
from Configurables import PatSeeding, PatSeedingTool
from HltLine.HltDecodeRaw import DecodeIT
ps = PatSeeding('Hlt1MBSeeding' , OutputTracksName = Hlt1TrackLoc["Seeding"])
ps.addTool(PatSeedingTool, name="PatSeedingTool")
ps.PatSeedingTool.MaxOTHits = HltRecoConf().getProp("Forward_MaxOTHits")

Hlt1Seeding = bindMembers( None, [ DecodeIT, ps ] )
