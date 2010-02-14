#!/usr/bin/env python
# =============================================================================
# $Id: HltReco.py,v 1.21 2010-02-14 10:22:41 graven Exp $
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

__all__ = ( 'PV2D'            # bindMembers instance with algorithms needed to get 'PV2D'
          , 'PV3D'            # bindMembers instance with algorithms needed to get 'PV3D'
          , 'RZVelo'          # bindMembers instance with algorithms needed to get 'RZVelo'
          , 'Velo'            # bindMembers instance with algorithms needed to get 'Velo'
          , 'Forward'         # bindMembers instance with algorithms needed to get 'Forward'
          , 'Seed'            # run Seeding
          , 'SeedKF'
          , 'HltRecoSequence' # Sequencer used after Hlt1, and start of Hlt2
	      , 'HltSeedSequence' 
          )
#############################################################################################
# Switches for different reco scenarios
#############################################################################################

RunSeeding = False
RunFastFit = False
RunCloneKiller = False

#############################################################################################
# Import Configurables
#############################################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import PatPV2DFit3D, PatPV3D, PatForward, PatForwardTool
from Configurables import Tf__PatVeloGeneric, Tf__PatVeloRTracking, Tf__PatVeloSpaceTracking, Tf__PatVeloSpaceTool, Tf__PatVeloGeneralTracking
from Configurables import PVOfflineTool
from Configurables import HltTrackFilter, HltVertexFilter, HltTrackUpgrade
from HltLine import bindMembers
from Configurables import TrackEventFitter, TrackMasterFitter, TrackKalmanFilter, TrackMasterExtrapolator  #@gk for TrackFit
from TrackFitter.ConfiguredFitters import ConfiguredFastFitter
from Configurables import PatSeeding, PatSeedingTool, PatMatch, CreateFastTrackCollection #@gk for PatSeeding
from Configurables import PatDownstream
from Configurables import TrackEventCloneKiller, TrackCloneFinder

#############################################################################################
# Configure pattern recognition algorithms
#############################################################################################

####TODO: split into Hlt1 specific part,
####      and the Hlt2 specific part...


#### Velo Tracking
patVeloR = Tf__PatVeloRTracking('HltRecoRZVelo', OutputTracksName = "Hlt/Track/RZVelo" ) 

recoVelo = Tf__PatVeloSpaceTracking('HltRecoVelo'
                                   , InputTracksName = patVeloR.OutputTracksName
                                   , OutputTracksName = "Hlt/Track/Velo" )

#We have a choice of which 3D Velo tracking (sans upgrading of 2D tracks) to use...
recoVeloOpen = Tf__PatVeloGeneric("PatVeloGeneric", Output = "Hlt/Track/VeloOpen")
recoVeloOpenOutput = recoVeloOpen.Output
#recoVeloOpen = Tf__PatVeloGeneralTracking('HltRecoVeloGeneral', OutputTracksLocation = "Hlt/Track/VeloOpen" )
#recoVeloOpenOutput = recoVeloOpen.OutputTracksLocation 

Tf__PatVeloSpaceTracking("HltRecoVelo").addTool( Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool" )
Tf__PatVeloSpaceTracking("HltRecoVelo").PatVeloSpaceTool.MarkClustersUsed=True

recoVeloGeneral = Tf__PatVeloGeneralTracking('HltRecoVeloGeneral'
                                   , OutputTracksLocation = "Hlt/Track/Velo" )

#### Forward Tracking
recoForward = PatForward( 'HltRecoForward'
                        , InputTracksName = recoVelo.OutputTracksName
                        , OutputTracksName = "Hlt/Track/Forward" )
PatForwardTool( MinMomentum = 1000., MinPt = 1000., AddTTClusterName = "" ) 
recoForward.addTool(PatForwardTool, name='PatForwardTool')
recoForward.PatForwardTool.AddTTClusterName = "PatAddTTCoord"

#### Seeding 
recoSeeding = PatSeeding('HltSeeding', OutputTracksName = "Hlt/Track/Seeding") 
recoSeeding.addTool(PatSeedingTool, name="PatSeedingTool")
recoSeeding.PatSeedingTool.UseForward = True
recoSeeding.PatSeedingTool.ForwardCloneMergeSeg = True
recoSeeding.PatSeedingTool.InputTracksName = recoForward.OutputTracksName

#### Matching
recoMatch = PatMatch('HltMatch'
                     , VeloInput = recoVelo.OutputTracksName
                     , SeedInput = recoSeeding.OutputTracksName 
		             , MatchOutput = "Hlt/Track/Match")

#### FastKalmanFitter
fastKalman = TrackEventFitter('Hlt2FastTrackFit' ,TracksInContainer  = "Hlt/Track/Long")
fastKalman.addTool(TrackMasterFitter, name = 'Fitter')
fitter = ConfiguredFastFitter( getattr(fastKalman,'Fitter'))

#### CreateFastTrackCollection
recoCopy = CreateFastTrackCollection('TrackCopy'
		, InputLocations = [ recoForward.OutputTracksName, recoMatch.MatchOutput ]
		, OutputLocation = "Hlt/Track/Long"
		, SlowContainer = True)

#### FastCloneFlagging
cloneKiller = TrackEventCloneKiller('FastCloneKiller'
		, TracksInContainers = ["Hlt/Track/Long"]
		, SkipSameContainerTracks = False
		, CopyTracks = False)
cloneKiller.addTool(TrackCloneFinder, name = 'CloneFinderTool')
cloneKiller.CloneFinderTool.RestrictedSearch = True

#### Primary vertex algorithms...

patPV2D = PatPV2DFit3D( 'HltRecoPV2D' , InputTracksName = patVeloR.OutputTracksName
                                      , OutputVerticesName = "Hlt/Vertex/PV2D" )  
patPV2D.addTool(PVOfflineTool, name = 'PVOfflineTool')
patPV2D.PVOfflineTool.PVFitterName='LSAdaptPV3DFitter'


recoPV3D =  PatPV3D('HltRecoPV3D' )
recoPV3D.addTool( PVOfflineTool, name = 'PVOfflineTool' )
recoPV3D.PVOfflineTool.InputTracks = [ recoVelo.OutputTracksName ]

recoPV3DOpen =  PatPV3D('HltRecoPV3DOpen' )
PV3DOpenTool = PVOfflineTool('PVOfflineTool') 
PV3DOpenTool.InputTracks = [recoVeloOpenOutput  ]
recoPV3DOpen.addTool( PV3DOpenTool )
recoPV3DOpen.OutputVerticesName = "Hlt/Vertex/PV3DOpen"

### Downstream tracking
PatDownstream = PatDownstream()
PatDownstream.InputLocation=recoSeeding.OutputTracksName
PatDownstream.OutputLocation="Hlt/Track/SeedTT"
#PatDownstream.UseForward = True
#PatDownstream.SeedFilter = True
PatDownstream.RemoveUsed = True
PatDownstream.RemoveAll = True


FitSeeding = TrackEventFitter('FitSeedTTTracks')
FitSeeding.TracksInContainer  = PatDownstream.OutputLocation
FitSeeding.TracksOutContainer = "Hlt/Track/FitSeedTT"
FitSeeding.addTool(TrackMasterFitter, name = 'Fitter')
ConfiguredFastFitter( getattr(FitSeeding,'Fitter'))

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

prepareVelo = HltTrackFilter('HltPrepareVelo'
                            , InputSelection = "TES:" + recoVelo.OutputTracksName
                            , OutputSelection = "Velo1"
                            , RequirePositiveInputs = False )

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

preparePV2D = HltVertexFilter( 'Hlt1PreparePV2D'
                             , InputSelection = "TES:" + PatPV2DFit3D('HltRecoPV2D').OutputVerticesName
                             , RequirePositiveInputs = False
                             , FilterDescriptor = ["VertexZPosition,>,-5000","VertexTransversePosition,>,-1"]
                             , HistoDescriptor = {'VertexZPosition': ( 'PV2D: VertexZPosition',-200.,200.,200),
                                                  'VertexZPositionBest': ( 'PV2D: Highest VertexZPosition',-200,200.,100),
                                                  'VertexTransversePosition': ( 'PV2D: VertexTransversePosition',0,1,50),
                                                  'VertexTransversePositionBest': ( 'PV2D: Highest VertexTransversePosition',0,1,50)
                                                    }
                             , OutputSelection   = "PV2D" )

preparePV3DOpen = HltVertexFilter( 'Hlt1PreparePV3DOpen'
                                 , InputSelection = "TES:" + recoPV3DOpen.OutputVerticesName
                                 , RequirePositiveInputs = False
                                 , OutputSelection = "PV3DOpen" )
#############################################################################################
# Define the reconstruction sequence 
#############################################################################################

from HltDecodeRaw import DecodeVELO

# define basic reco sequence, this should be run in any case
trackRecoSequence = GaudiSequencer( 'HltTrackRecoSequence'
                                  ,  Members = DecodeVELO.members() +  [ patVeloR, recoVelo, recoVeloGeneral ] )

# Now we add different algorithms for long track reco based on the different reconstruction scenarios
# first we want to decide if the seeding should run

#if only Forward is run, we can write directly to Hlt/Track/Long
# Otherwise we just have to add our stuff
from HltDecodeRaw import DecodeTT, DecodeIT
if not RunSeeding:
                    recoCopy.InputLocations = ["Hlt/Track/Forward"]
                    trackRecoSequence.Members += DecodeTT.members() + DecodeIT.members() + [ recoForward, recoCopy]
else:
                    trackRecoSequence.Members += DecodeTT.members() + DecodeIT.members() + [ recoForward, recoSeeding, recoMatch, recoCopy]

#### now we add the fit 
if RunFastFit    :  trackRecoSequence.Members += [ fastKalman]
if RunCloneKiller:  trackRecoSequence.Members += [ cloneKiller ]




from Configurables import DecodeVeloRawBuffer

### define exported symbols (i.e. these are externally visible, the rest is NOT)
PV2D     = bindMembers( None, [ DecodeVELO, patVeloR, patPV2D, preparePV2D ] )
PV3D     = bindMembers( None, [ DecodeVELO, patVeloR, recoVelo, recoPV3D ] )
RZVelo   = bindMembers( None, [ DecodeVELO, patVeloR, prepareRZVelo ] )
Velo     = bindMembers( None, [                  RZVelo , reco1Velo ] )
VeloOpen = bindMembers( None, [ DecodeVeloRawBuffer(), recoVeloOpen, prepareVeloOpen ] )
PV3DOpen = bindMembers( None, [ DecodeVeloRawBuffer(), recoVeloOpen, prepareVeloOpen, recoPV3DOpen, preparePV3DOpen ] )
Forward  = bindMembers( None, [                                Velo,  recoFwd ] )

# FIXME TODO WARNING: Seed is _not_ selfcontained, and relies on Forward having run...
Seed     = bindMembers( None, [ recoSeeding, PatDownstream ] )
SeedKF   = bindMembers( None, [ Seed, FitSeeding ] )

HltSeedSequence = GaudiSequencer("HltSeedSequence", MeasureTime = True, Members = Seed.members() )
HltRecoSequence = bindMembers(None, trackRecoSequence.Members )
