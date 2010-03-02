#!/usr/bin/env python
# =============================================================================
# $Id: HltReco.py,v 1.28 2010-03-02 11:09:11 gligorov Exp $
# =============================================================================
## @file HltLine/HltReco.py
#  Collection of predefined algorithms to perform reconstruction
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2009-04-03
# =============================================================================
#
# the (ordered) list of (configurables corresponding to the) algorithms needed to 
# obtain the Hlt1 selection 'PV2D' can be obtained by doing 
#
#   from HltLine.HltReco import PV2D
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
from HltLine import bindMembers
from Configurables import PatSeeding, PatSeedingTool

#############################################################################################
# Configure pattern recognition algorithms
#############################################################################################

####TODO: split into Hlt1 specific part,
####      and the Hlt2 specific part...

#This is the one unavoidable piece of hardcoding since this is the piece
#shared between Hlt1 and Hlt2
from HltTrackNames import HltSharedRZVeloTracksName, HltSharedTracksPrefix, _baseTrackLocation  

#### Velo Tracking
patVeloR = Tf__PatVeloRTracking('HltRecoRZVelo', OutputTracksName = _baseTrackLocation(HltSharedTracksPrefix,HltSharedRZVeloTracksName) ) 

#We have a choice of which 3D Velo tracking (sans upgrading of 2D tracks) to use...
recoVeloOpen = Tf__PatVeloGeneric("PatVeloGeneric", Output = "Hlt/Track/VeloOpen")
recoVeloOpenOutput = recoVeloOpen.Output
#recoVeloOpen = Tf__PatVeloGeneralTracking('HltRecoVeloGeneral', OutputTracksLocation = "Hlt/Track/VeloOpen" )
#recoVeloOpenOutput = recoVeloOpen.OutputTracksLocation 

#### Primary vertex algorithms...

recoPV3DOpen =  PatPV3D('HltRecoPV3DOpen' )
PV3DOpenTool = PVOfflineTool('PVOfflineTool') 
PV3DOpenTool.InputTracks = [recoVeloOpenOutput  ]
recoPV3DOpen.addTool( PV3DOpenTool )
recoPV3DOpen.OutputVerticesName = "Hlt/Vertex/PV3DOpen"

##### Hlt selections

# why does Hlt1PrepareRZVelo::initialize  trigger an init of ToolSvc.OTRawBankDecoder???
prepareRZVelo = HltTrackFilter( 'Hlt1PrepareRZVelo'
                              , InputSelection   = "TES:" + patVeloR.OutputTracksName
                              , RequirePositiveInputs = False
                              , AddInfo = False
                              , FilterDescriptor = ["IsBackward,<,0.5"]
                              , OutputSelection     = "RZVelo" )

prepareVeloOpen = HltTrackFilter( 'Hlt1PrepareVeloOpen'
                                , InputSelection = "TES:" + recoVeloOpenOutput
                                , RequirePositiveInputs = False
                                , AddInfo = False
                                , OutputSelection = "VeloOpen") 

reco1Velo = HltTrackUpgrade( 'Hlt1RecoVelo'
                           , InputSelection = prepareRZVelo.OutputSelection
                           , OutputSelection = "Velo"
                           , RecoName = "Velo"
                           , HistogramUpdatePeriod = 0 )

recoFwd = HltTrackUpgrade( 'Hlt1RecoForward'
                         , InputSelection = reco1Velo.OutputSelection
                         , OutputSelection = "Forward"
                         , RecoName = "Forward"
                         , HistogramUpdatePeriod = 0 )

preparePV3DOpen = HltVertexFilter( 'Hlt1PreparePV3DOpen'
                                 , InputSelection = "TES:" + recoPV3DOpen.OutputVerticesName
                                 , RequirePositiveInputs = False
                                 , OutputSelection = "PV3DOpen" )
#############################################################################################
# Define the reconstruction sequence 
#############################################################################################

from HltDecodeRaw import DecodeVELO
from Configurables import DecodeVeloRawBuffer

### define exported symbols (i.e. these are externally visible, the rest is NOT)
#This is the part which is shared between Hlt1 and Hlt2
MinimalRZVelo   = bindMembers( None, [DecodeVELO, patVeloR ] )
#The Hlt1 parts
RZVelo   = bindMembers( None, [ MinimalRZVelo, prepareRZVelo ] )
Velo     = bindMembers( None, [                  RZVelo , reco1Velo ] )

from Configurables import PatSeeding
from HltDecodeRaw import DecodeIT
Hlt1Seeding = bindMembers( None, [ DecodeIT,
                                   PatSeeding('Hlt1MBSeeding'
                                              ,OutputTracksName = 'Hlt1/Tracks/Seeding')
                                   ] )

# Debug things for an open VELO
VeloOpen = bindMembers( None, [ DecodeVeloRawBuffer(), recoVeloOpen, prepareVeloOpen ] )
PV3DOpen = bindMembers( None, [ DecodeVeloRawBuffer(), recoVeloOpen, prepareVeloOpen, recoPV3DOpen, preparePV3DOpen ] )
