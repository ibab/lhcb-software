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
          , 'Velo'            # bindMembers instance with algorithms needed to get 'Velo'
	  , 'Hlt1Seeding'
          , 'VeloCandidates'
          , 'MaxOTHits'
          )

MaxOTHits = 15000

CommonMatchVeloMuonOptions = {"MaxChi2DoFX" : 10,
                              "XWindow" : 200,
                              "YWindow" : 200  }
CommonForwardTrackingOptions = { "MaxChi2" : 40,
                                 "MaxChi2Track" : 40,
                                 "MinHits" : 12,  
                                 "MinOTHits" : 14 } 
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
from HltTrackNames import HltSharedRZVeloTracksName, HltSharedVeloTracksName, HltSharedTracksPrefix 
from HltTrackNames import Hlt1TracksPrefix, _baseTrackLocation, Hlt1SeedingTracksName  
from Configurables import Tf__PatVeloSpaceTracking, Tf__PatVeloSpaceTool
from Configurables import FastVeloHitManager, DecodeVeloRawBuffer

#### Velo Tracking
patVeloR = Tf__PatVeloRTracking('HltRecoRZVelo', OutputTracksName = _baseTrackLocation(HltSharedTracksPrefix,HltSharedRZVeloTracksName) ) 
recoVelo = FastVeloTracking( 'FastVeloHlt', OutputTracksName = _baseTrackLocation(HltSharedTracksPrefix,HltSharedVeloTracksName))
recoVelo.HLT1Only = True   

##### Hlt selections
from Configurables import Hlt__TrackFilter as HltTrackFilter
prepare3DVelo = HltTrackFilter( 'Hlt1Prepare3DVelo'
                              , InputSelection   = "TES:" + recoVelo.OutputTracksName
                              , RequirePositiveInputs = False
                              , Code = [ '~TrBACKWARD' ] 
                              , OutputSelection     = "Velo" )

#############################################################################################
# Define the reconstruction sequence 
#############################################################################################
from HltLine.HltDecodeRaw import DecodeVELO
from Configurables import DecodeVeloRawBuffer

### define exported symbols (i.e. these are externally visible, the rest is NOT)
#This is the part which is shared between Hlt1 and Hlt2
MinimalRZVelo = bindMembers( None, [DecodeVELO, patVeloR ] ).setOutputSelection( patVeloR.OutputTracksName )

MinimalVelo = bindMembers( None, [DecodeVELO, recoVelo ] ).setOutputSelection( recoVelo.OutputTracksName )

Velo = bindMembers( None, [ MinimalVelo, prepare3DVelo ] ).setOutputSelection( 'Velo' )


# ==============================================================================
# Hlt1Seeding, used by MicroBias
# ==============================================================================
from Configurables import PatSeeding, PatSeedingTool
from HltLine.HltDecodeRaw import DecodeIT
ps = PatSeeding('Hlt1MBSeeding' , OutputTracksName = _baseTrackLocation(Hlt1TracksPrefix,Hlt1SeedingTracksName))
ps.addTool(PatSeedingTool, name="PatSeedingTool")
ps.PatSeedingTool.MaxOTHits = MaxOTHits

Hlt1Seeding = bindMembers( None, [ DecodeIT, ps ] )
