#!/usr/bin/env python
# =============================================================================
# $Id: HltReco.py,v 1.10 2009-07-03 10:13:45 graven Exp $
# =============================================================================
## @file HltConf/HltReco.py
#  Collection of predefined algorithms to perform reconstruction
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2009-04-03
# =============================================================================
#
# the (ordered) list of (configurables corresponding to the) algorithms needed to 
# obtain the Hlt1 selection 'PV2D' can be obtained by doing 
#
#   from HltConf.HltReco import PV2D
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
# 

__all__ = ( 'PV2D'            # bindMembers instance with algorithms needed to get 'PV2D'
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
from Configurables import PatPV2D, PatPV3D, PatForward, PatForwardTool
from Configurables import Tf__PatVeloRTracking, Tf__PatVeloSpaceTracking
from Configurables import PVOfflineTool
from Configurables import HltTrackFilter, HltVertexFilter, HltTrackUpgrade
from HltLine.HltLine import bindMembers
from Configurables import TrackEventFitter, TrackMasterFitter, TrackKalmanFilter, TrackMasterExtrapolator  #@gk for TrackFit
from TrackFitter.ConfiguredFitters import ConfiguredFastFitter
from Configurables import PatSeeding, PatSeedingTool, PatMatch, CreateFastTrackCollection #@gk for PatSeeding
from Configurables import PatDownstream
from Configurables import TrackEventCloneKiller, TrackCloneFinder

#############################################################################################
# Configure pattern recognition algorithms
#############################################################################################

####TODO: split into Hlt1 specific part,
####      and in the stuff which runs at the Hlt1/Hlt2 boundary,
####      and the Hlt2 specific part...


#### Velo Tracking
patVeloR = Tf__PatVeloRTracking('HltRecoRZVelo' , OutputTracksName = "Hlt/Track/RZVelo" ) 

recoVelo = Tf__PatVeloSpaceTracking('HltRecoVelo'
                                   , InputTracksName = patVeloR.OutputTracksName
                                   , OutputTracksName = "Hlt/Track/Velo" )

#### Forward Tracking
recoForward = PatForward( 'HltRecoForward'
                        , InputTracksName = recoVelo.OutputTracksName
                        , OutputTracksName = "Hlt/Track/Forward" )
PatForwardTool( MinMomentum = 1000., MinPt = 1000., AddTTClusterName = "" ) 
recoForward.addTool(PatForwardTool, name='PatForwardTool')
recoForward.PatForwardTool.AddTTClusterName = "PatAddTTCoord"

#### Seeding 
recoSeeding = PatSeeding('HltSeeding' , OutputTracksName = "Hlt/Track/Seeding") 
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

patPV2D = PatPV2D( 'HltRecoPV2D' , InputTracksName = patVeloR.OutputTracksName
                                 , OutputVerticesName = "Hlt/Vertex/PV2D" )  

recoPV3D =  PatPV3D('Hlt1RecoPV3D' )
recoPV3D.addTool( PVOfflineTool, name = 'PVOfflineTool' )
recoPV3D.PVOfflineTool.InputTracks = [ recoVelo.OutputTracksName ]

### Downstream tracking
PatDownstream = PatDownstream()
PatDownstream.InputLocation=recoSeeding.OutputTracksName
PatDownstream.OutputLocation="Hlt/Track/SeedTT"
PatDownstream.UseForward = True
PatDownstream.SeedFilter = True

FitSeeding = TrackEventFitter('FitSeedTTTracks')
FitSeeding.TracksInContainer  = PatDownstream.OutputLocation
FitSeeding.TracksOutContainer = "Hlt/Track/FitSeedTT"
FitSeeding.addTool(TrackMasterFitter, name = 'Fitter')
ConfiguredFastFitter( getattr(FitSeeding,'Fitter'))

##### Hlt selections

prepareForward = HltTrackFilter( 'HltPrepareForward' 
		, InputSelection = "TES:Hlt/Track/Long" #@gk temporarily fixed
                               , OutputSelection = "Forward1"
                               , RequirePositiveInputs = False )

# why does Hlt1PrepareRZVelo::initialize  trigger an init of ToolSvc.OTRawBankDecoder???
prepareRZVelo = HltTrackFilter( 'Hlt1PrepareRZVelo'
                              , InputSelection   = "TES:" + patVeloR.OutputTracksName
                              , RequirePositiveInputs = False
                              , AddInfo = False
                              , FilterDescriptor = ["IsBackward,<,0.5"]
                              , OutputSelection     = "RZVelo" )

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
                             , InputSelection = "TES:" + PatPV2D('HltRecoPV2D').OutputVerticesName
                             , RequirePositiveInputs = False
                             , OutputSelection   = "PV2D" )


#############################################################################################
# Define the reconstruction sequence 
#############################################################################################

# first define sequencers for velo tracking
recoRZVeloTracksSequence = GaudiSequencer( 'HltRecoRZVeloTracksSequence', MeasureTime = True
                                         #, Members = [ decodeVeloLite, patVeloR ] )
                                         , Members = [ patVeloR ] )

recoRZPVSequence = GaudiSequencer( 'HltRecoRZPVSequence' , MeasureTime = True, IgnoreFilterPassed = True
                                 , Members = [ patPV2D ] )

recoRZVeloSequence = GaudiSequencer ( 'HltRecoRZVeloSequence', MeasureTime = True
                                    , Members = 
                                    [  recoRZVeloTracksSequence
                                    ,  recoRZPVSequence ] )

# define basic reco sequence, this should be run in any case
trackRecoSequence = GaudiSequencer( 'HltTrackRecoSequence'
                                  ,  Members =
                                  [  recoRZVeloSequence
				                  ,  recoVelo
                                  ,  recoPV3D # this aborts the remainder of the sequence if no primary -- do we really want that??
				  ] )

# Now we add different algorithms for long track reco based on the different reconstruction scenarios
# first we want to decide if the seeding should run

#if only Forward is run, we can write directly to Hlt/Track/Long
# Otherwise we just have to add our stuff
from HltConf.HltDecodeRaw import DecodeTT, DecodeIT
if not RunSeeding:
                    recoCopy.InputLocations = ["Hlt/Track/Forward"]
                    trackRecoSequence.Members += DecodeTT.members() + DecodeIT.members() + [ recoForward , recoCopy]
else:
                    trackRecoSequence.Members += DecodeTT.members() + DecodeIT.members() + [ recoForward , recoSeeding , recoMatch , recoCopy]

#### now we add the fit 
if RunFastFit    :  trackRecoSequence.Members += [ fastKalman]
if RunCloneKiller:  trackRecoSequence.Members += [ cloneKiller ]

#### and last prepareForward 
## TODO: remove as soon as an Hlt2 monitoring line exists for this...
trackRecoSequence.Members += [ prepareForward]


####TODO
###           HltTrackRecoSequence                  GaudiSequencer           
###              HltRecoRZVelo                      Tf::PatVeloRTracking     
###              HltRecoVelo                        Tf::PatVeloSpaceTracking
###              HltRecoForward                     PatForward               
###              HltRecoPVSequence                  GaudiSequencer             IgnoreFilterPassed
###                    HltRecoPV2D                  PatPV2D                  
###                    Hlt1RecoPV3D                 PatPV3D            


hlt1RecoRZVeloTracksSequence = GaudiSequencer( 'Hlt1RecoRZVeloTracksSequence' , MeasureTime = True
                                             , Members = [ recoRZVeloTracksSequence , prepareRZVelo ] )

hlt1RecoRZPVSequence = GaudiSequencer( 'Hlt1RecoRZPVSequence', MeasureTime = True
                                     , Members = [ recoRZPVSequence , preparePV2D ] )

recoRZVelo = GaudiSequencer( 'Hlt1RecoRZVeloSequence' , MeasureTime = True
                           , Members = [ recoRZVeloSequence , prepareRZVelo, preparePV2D ] ) 

#hlt1RecoSequence = GaudiSequencer( 'Hlt1RecoSequence', MeasureTime = True
#                                 , Members = 
#                                 [ hlt1RecoRZVeloTracksSequence
#                                 , hlt1RecoRZPVSequence
#                                 , reco1Velo
#                                 ,  recoPV3D # this aborts the remainder of the sequence if no primary -- do we really want that??
#                                 , recoFwd ] )

recoSeq = GaudiSequencer('HltRecoSequence', MeasureTime = True
                        , Members =
                        [ trackRecoSequence
                        # //,GaudiSequencer('HltCaloRecoSequence')  // In HltChargedProtoPAlg for the time being
                        ] )

importOptions('$HLTCONFROOT/options/TsaTool.opts')
#// --------------------- HltCalosequence // In HltChargedProtoPAlg for the time being
#// Options for Calo reconstruction
importOptions( '$HLTCONFROOT/options/units.opts' )
importOptions( '$CALORECOROOT/options/HltCaloSeq.opts' )
GaudiSequencer('HltCaloRecoSequence', Members = [ GaudiSequencer('RecoCALOSeq') ] )

#/// @todo This cannot work, as tracking must have been done to process the rest.
#//recoSeq.IgnoreFilterPassed = True; // process both track and calo independently




### define exported symbols (i.e. these are externally visible, the rest is NOT)
from HltConf.HltDecodeRaw import DecodeVELO
#Forward1 = bindMembers( None, [ DecodeVELO, patVeloR, recoVelo, recoForward , prepareForward ] )
PV2D     = bindMembers( None, [ DecodeVELO, patVeloR, patPV2D, preparePV2D ] )
RZVelo   = bindMembers( None, [ DecodeVELO, patVeloR, prepareRZVelo ] )
#Forward1 = bindMembers( None, [ patVeloR, recoVelo, recoForward , prepareForward ] )
#PV2D     = bindMembers( None, [ patVeloR, patPV2D, preparePV2D ] )
#RZVelo   = bindMembers( None, [ patVeloR, prepareRZVelo ] )
Velo     = bindMembers( None, [                  RZVelo , reco1Velo ] )
Forward  = bindMembers( None, [                                Velo,  recoFwd ] )

# warning: Seed is _not_ selfcontained, and relies on Forward having run...
Seed     = bindMembers( None, [ recoSeeding, PatDownstream ] )
SeedKF   = bindMembers( None, [ Seed, FitSeeding ] )

HltSeedSequence = GaudiSequencer("HltSeedSequence", MeasureTime = True, Members = Seed.members() )
HltRecoSequence = recoSeq
