#!/usr/bin/env python
# =============================================================================
# $Id: HltReco.py,v 1.2 2010-04-19 08:03:57 albrecht Exp $
# =============================================================================
## @file HltTracking/HltReco.py
#  Collection of predefined algorithms to perform reconstruction
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2009-04-03
# =============================================================================
#
# the (ordered) list of (configurables corresponding to the) algorithms needed to 
# obtain the Hlt1 selection 'PV2D' can be obtained by doing 
#
#   from HltTracking.HltReco import PV2D
#   PV2D.members() 
#
# in addition, the 'outputselection' of the last member can be 
# obtained by doing:
#
#   PV2D.outputSelection()
#
# which should return 'PV2D' of course...
# The returned object can be used in the 'algos' list used by Hlt1Line,
# and, given that 'outputSelection' is properly set, partakes in the automated
# daisychaining present in Hlt1Line. In case you do _not_ want this
# daisychaining, you can do:
#
#  PV2D.ignoreOutputSelection()
#
# which 'skips' PV2D for the daisychaining (but will insert the right configurables)
#

__all__ = ('MinimalRZVelo'   # bindMembers instance with algorithms needed to get 'MinimalRZVelo' 
          , 'RZVelo'          # bindMembers instance with algorithms needed to get 'RZVelo'
          , 'Velo'            # bindMembers instance with algorithms needed to get 'Velo'
	  , 'Hlt1Seeding'
          )
#############################################################################################
# Import Configurables
#############################################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import PatPV3D
from Configurables import Tf__PatVeloGeneric, Tf__PatVeloRTracking, Tf__PatVeloGeneralTracking
from Configurables import PVOfflineTool
from Configurables import HltTrackFilter, HltVertexFilter, HltTrackUpgrade
from HltLine.HltLine import bindMembers
from Configurables import PatSeeding, PatSeedingTool

# J. Albrecht 2010-04-16
# globally set the resolutions for ST
#when syntax errors there are fixed take from 
#importOptions( "$STTOOLSROOT/options/Brunel_EarlyData.opts" )
from Configurables import STOfflinePosition, STOnlinePosition

STOnlinePosition().ErrorVec=[0.289,0.339,0.322]
STOnlinePosition().APE = 0.197

ITOnl = STOnlinePosition("ITLiteClusterPosition")
ITOnl.ErrorVec=[0.28,0.20,0.3]
ITOnl.APE=0.05
#ITOnl.DetType = "IT"

STPos = STOfflinePosition("ITClusterPosition")
STPos.LinSharingCorr2 = 0.61
STPos.LinSharingCorr2 = 0.61
STPos.CubicSharingCorr2 = 7.4
STPos.LinSharingCorr4 = 0.63
STPos.DetType = "IT"
STPos.ErrorVec = [0.28, 0.20, 0.3, 0.2]
STPos.APE = 0.05

STOfflinePosition().ErrorVec = [0.289, 0.339, 0.322, 0.459]
STOfflinePosition().APE = 0.197

#############################################################################################
# Configure pattern recognition algorithms
#############################################################################################

#This is the one unavoidable piece of hardcoding since this is the piece
#shared between Hlt1 and Hlt2
from HltTrackNames import HltSharedRZVeloTracksName, HltSharedTracksPrefix 
from HltTrackNames import Hlt1TracksPrefix, _baseTrackLocation, Hlt1SeedingTracksName  

#### Velo Tracking
patVeloR = Tf__PatVeloRTracking('HltRecoRZVelo', OutputTracksName = _baseTrackLocation(HltSharedTracksPrefix,HltSharedRZVeloTracksName) ) 

#We have a choice of which 3D Velo tracking (sans upgrading of 2D tracks) to use...
#recoVeloOpen = Tf__PatVeloGeneric("PatVeloGeneric", Output = "Hlt/Track/VeloOpen")
#recoVeloOpenOutput = recoVeloOpen.Output
#recoVeloOpen = Tf__PatVeloGeneralTracking('HltRecoVeloGeneral', OutputTracksLocation = "Hlt/Track/VeloOpen" )
#recoVeloOpenOutput = recoVeloOpen.OutputTracksLocation 

#### Primary vertex algorithms...

#recoPV3DOpen =  PatPV3D('HltRecoPV3DOpen' )
#PV3DOpenTool = PVOfflineTool('PVOfflineTool') 
#PV3DOpenTool.InputTracks = [recoVeloOpenOutput  ]
#recoPV3DOpen.addTool( PV3DOpenTool )
#recoPV3DOpen.OutputVerticesName = "Hlt/Vertex/PV3DOpen"

##### Hlt selections

# why does Hlt1PrepareRZVelo::initialize  trigger an init of ToolSvc.OTRawBankDecoder???
prepareRZVelo = HltTrackFilter( 'Hlt1PrepareRZVelo'
                              , InputSelection   = "TES:" + patVeloR.OutputTracksName
                              , RequirePositiveInputs = False
                              , AddInfo = False
                              , FilterDescriptor = ["IsBackward,<,0.5"]
                              , OutputSelection     = "RZVelo" )

#prepareVeloOpen = HltTrackFilter( 'Hlt1PrepareVeloOpen'
#                                , InputSelection = "TES:" + recoVeloOpenOutput
#                                , RequirePositiveInputs = False
#                                , AddInfo = False
#                                , OutputSelection = "VeloOpen") 

reco1Velo = HltTrackUpgrade( 'Hlt1RecoVelo'
                           , InputSelection = prepareRZVelo.OutputSelection
                           , OutputSelection = "Velo"
                           , RecoName = "Velo"
                           , HistogramUpdatePeriod = 0 )

#recoFwd = HltTrackUpgrade( 'Hlt1RecoForward'
#                         , InputSelection = reco1Velo.OutputSelection
#                         , OutputSelection = "Forward"
#                         , RecoName = "Forward"
#                         , HistogramUpdatePeriod = 0 )

#preparePV3DOpen = HltVertexFilter( 'Hlt1PreparePV3DOpen'
#                                 , InputSelection = "TES:" + recoPV3DOpen.OutputVerticesName
#                                 , RequirePositiveInputs = False
#                                 , OutputSelection = "PV3DOpen" )
#############################################################################################
# Define the reconstruction sequence 
#############################################################################################

from HltLine.HltDecodeRaw import DecodeVELO
from Configurables import DecodeVeloRawBuffer

### define exported symbols (i.e. these are externally visible, the rest is NOT)
#This is the part which is shared between Hlt1 and Hlt2
MinimalRZVelo   = bindMembers( None, [DecodeVELO, patVeloR ] )
#The Hlt1 parts
RZVelo   = bindMembers( None, [ MinimalRZVelo, prepareRZVelo ] )
Velo     = bindMembers( None, [                  RZVelo , reco1Velo ] )

from Configurables import PatSeeding
from HltLine.HltDecodeRaw import DecodeIT
Hlt1Seeding = bindMembers( None, [ DecodeIT,
                                   PatSeeding('Hlt1MBSeeding'
                                              ,OutputTracksName = _baseTrackLocation(Hlt1TracksPrefix,Hlt1SeedingTracksName))
                                   ] )

# Debug things for an open VELO
#VeloOpen = bindMembers( None, [ DecodeVeloRawBuffer(), recoVeloOpen, prepareVeloOpen ] )
#PV3DOpen = bindMembers( None, [ DecodeVeloRawBuffer(), recoVeloOpen, prepareVeloOpen, recoPV3DOpen, preparePV3DOpen ] )
