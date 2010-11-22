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
          )
############################################################################################
# Option to decide which pattern to use
############################################################################################
useFastVelo = False
#############################################################################################
# Import Configurables
#############################################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import PatPV3D
from Configurables import Tf__PatVeloGeneric, Tf__PatVeloRTracking, FastVeloTracking
from Configurables import PVOfflineTool
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

if useFastVelo :
    recoVelo = FastVeloTracking( 'FastVeloHlt', OutputTracksName = "Hlt/Track/Velo" )
    recoVelo.MinRSensor = 4
    recoVelo.BestEfficiency = False
else :
   recoVelo         = Tf__PatVeloSpaceTracking('Hlt1And2RecoVelo'
                                               , InputTracksName  = "Hlt/Track/RZVelo" 
                                               , OutputTracksName = "Hlt/Track/Velo" )
   recoVelo.addTool( Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool" )
   recoVelo.PatVeloSpaceTool.MarkClustersUsed=True

##### Hlt selections
from Configurables import HltTrackFilter
prepare3DVelo = HltTrackFilter( 'Hlt1Prepare3DVelo'
                              , InputSelection   = "TES:" + recoVelo.OutputTracksName
                              , RequirePositiveInputs = False
                              , AddInfo = False
                              , FilterDescriptor = ["IsBackward,<,0.5"]
                              , OutputSelection     = "Velo" )

#from Configurables import LoKi__HltUnit
#prepare3DVelo = LoKi__HltUnit('Hlt1Prepare3DVelo', Code = "TrSOURCE('%s', ~TrBACKWARD) >> TrREGISTER('Velo') >> ~TrEMPTY" % recoVelo.OutputTracksName )

#############################################################################################
# Define the reconstruction sequence 
#############################################################################################

from HltLine.HltDecodeRaw import DecodeVELO
from Configurables import DecodeVeloRawBuffer

### define exported symbols (i.e. these are externally visible, the rest is NOT)
#This is the part which is shared between Hlt1 and Hlt2
MinimalRZVelo   = bindMembers( None, [DecodeVELO, patVeloR ] ).setOutputSelection( patVeloR.OutputTracksName )

if useFastVelo :
    MinimalVelo     = bindMembers( None, [DecodeVELO, recoVelo ] ).setOutputSelection( recoVelo.OutputTracksName )
else :
    MinimalVelo     = bindMembers( None, [MinimalRZVelo, recoVelo ] ).setOutputSelection( recoVelo.OutputTracksName )

Velo            = bindMembers( None, [                    MinimalVelo, prepare3DVelo ] ).setOutputSelection('Velo')

from Configurables import PatSeeding
from HltLine.HltDecodeRaw import DecodeIT
Hlt1Seeding = bindMembers( None, [ DecodeIT,
                                   PatSeeding('Hlt1MBSeeding'
                                              ,OutputTracksName = _baseTrackLocation(Hlt1TracksPrefix,Hlt1SeedingTracksName))
                                   ] )

# ==============================================================================
# Define & Register symbols for streamer framework
# ==============================================================================
from Configurables import LoKi__Hybrid__CoreFactory as Hlt1Factory
from Configurables import Hlt__Track2Candidate

_factory = Hlt1Factory ( "Hlt1Factory" )

## append Track->Candidate converter to the end of velo recontruction. tmp?
_veloTracks = Hlt__Track2Candidate (
    'Velo2Candidates' ,
    Code            = "~TrBACKWARD"    , ## skip backward tracks 
    InputSelection  = "Hlt/Track/Velo" ,
    OutputSelection = 'VeloCandidates'
    )
_Velo = bindMembers ( None ,
                      [ Velo , _veloTracks ] ).setOutputSelection('VeloCandidates')

#
## register the symbols for factory:
#  
_factory.Lines += [
    ## ``Gate'' or ``Cause''  : reconstruct Velo & convert to Candidates
    "veloReco   = execute ( %s ) " % [ m.getFullName() for m in _Velo.members() ] , 
    ## ``Source'' : get Velo-track candidates (use the "cause")
    "veloTracks = veloReco  * TC_SELECTION( '%s' ) " % _veloTracks.OutputSelection 
    ]

# ==============================================================================
# The END 
# ==============================================================================
