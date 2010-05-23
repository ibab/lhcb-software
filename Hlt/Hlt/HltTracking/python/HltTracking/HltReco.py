#!/usr/bin/env python
# =============================================================================
# $Id: HltReco.py,v 1.4 2010-05-23 19:38:40 gligorov Exp $
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

__all__ = ('MinimalRZVelo'   # bindMembers instance with algorithms needed to get 'MinimalRZVelo' 
          , 'RZVelo'          # bindMembers instance with algorithms needed to get 'RZVelo'
          , 'MinimalVelo'
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
from Configurables import Tf__PatVeloSpaceTracking, Tf__PatVeloSpaceTool

#### Velo Tracking
patVeloR = Tf__PatVeloRTracking('HltRecoRZVelo', OutputTracksName = _baseTrackLocation(HltSharedTracksPrefix,HltSharedRZVeloTracksName) ) 

recoVelo         = Tf__PatVeloSpaceTracking('Hlt1And2RecoVelo'
                                   , InputTracksName  = "Hlt/Track/RZVelo" 
                                   , OutputTracksName = "Hlt/Track/Velo" )
    
recoVelo.addTool( Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool" )
recoVelo.PatVeloSpaceTool.MarkClustersUsed=True

##### Hlt selections

# why does Hlt1PrepareRZVelo::initialize  trigger an init of ToolSvc.OTRawBankDecoder???
prepareRZVelo = HltTrackFilter( 'Hlt1PrepareRZVelo'
                              , InputSelection   = "TES:" + patVeloR.OutputTracksName
                              , RequirePositiveInputs = False
                              , AddInfo = False
                              , FilterDescriptor = ["IsBackward,<,0.5"]
                              , OutputSelection     = "RZVelo" )


prepare3DVelo = HltTrackFilter( 'Hlt1Prepare3DVelo'
                              , InputSelection   = "TES:" + recoVelo.OutputTracksName
                              , RequirePositiveInputs = False
                              , AddInfo = False
                              , FilterDescriptor = ["IsBackward,<,0.5"]
                              , OutputSelection     = "Velo" )

#############################################################################################
# Define the reconstruction sequence 
#############################################################################################

from HltLine.HltDecodeRaw import DecodeVELO
from Configurables import DecodeVeloRawBuffer

### define exported symbols (i.e. these are externally visible, the rest is NOT)
#This is the part which is shared between Hlt1 and Hlt2
MinimalRZVelo   = bindMembers( None, [DecodeVELO, patVeloR ] )
RZVelo   = bindMembers( None, [ MinimalRZVelo, prepareRZVelo ] )

MinimalVelo = bindMembers( None, [                  MinimalRZVelo , recoVelo ] )
Velo     = bindMembers( None, [                  MinimalVelo, prepare3DVelo ] )

from Configurables import PatSeeding
from HltLine.HltDecodeRaw import DecodeIT
Hlt1Seeding = bindMembers( None, [ DecodeIT,
                                   PatSeeding('Hlt1MBSeeding'
                                              ,OutputTracksName = _baseTrackLocation(Hlt1TracksPrefix,Hlt1SeedingTracksName))
                                   ] )
