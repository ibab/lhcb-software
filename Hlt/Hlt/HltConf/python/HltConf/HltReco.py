#!/usr/bin/env python
# =============================================================================
# $Id: HltReco.py,v 1.1 2009-04-03 18:49:18 graven Exp $
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
          , 'HltRecoSequence' # Sequencer used after Hlt1, and start of Hlt2
          )
#
#
#------------------------------
# Definition of the Hlt Reconstruction
#-----------------------------
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import PatPV2D, PatPV3D, PatForward, PatForwardTool
from Configurables import Tf__PatVeloRTracking, Tf__PatVeloSpaceTracking
from Configurables import PVOfflineTool
from Configurables import HltTrackFilter, HltVertexFilter, HltTrackUpgrade
from HltConf.HltLine import bindMembers


#----------------------- HltTrack sequence
# ---------------------
## make sure we continue, even if no PV is found
patVeloR = Tf__PatVeloRTracking('HltRecoRZVelo' , OutputTracksName = "Hlt/Track/RZVelo" ) 

recoRZVeloTracksSequence = GaudiSequencer( 'HltRecoRZVeloTracksSequence', MeasureTime = True
                                         , Members = [ patVeloR ] 
                                         )

patPV2D = PatPV2D( 'HltRecoPV2D' , InputTracksName = patVeloR.OutputTracksName
                                 , OutputVerticesName = "Hlt/Vertex/PV2D" 
                 )  
recoRZPVSequence = GaudiSequencer( 'HltRecoRZPVSequence' , MeasureTime = True, IgnoreFilterPassed = True
                                 , Members = [ patPV2D ] 
                                 )

recoRZVeloSequence = GaudiSequencer ( 'HltRecoRZVeloSequence', MeasureTime = True
                                    , Members = 
                                    [  recoRZVeloTracksSequence
                                    ,  recoRZPVSequence ] )

recoVelo = Tf__PatVeloSpaceTracking('HltRecoVelo'
                                   , InputTracksName = patVeloR.OutputTracksName
                                   , OutputTracksName = "Hlt/Track/Velo" )

prepareVelo = HltTrackFilter('HltPrepareVelo'
                            , InputSelection = "TES:" + recoVelo.OutputTracksName
                            , OutputSelection = "Velo1"
                            , RequirePositiveInputs = False )

recoForward = PatForward( 'HltRecoForward'
                        , InputTracksName = recoVelo.OutputTracksName
                        , OutputTracksName = "Hlt/Track/Forward" )

prepareForward = HltTrackFilter( 'HltPrepareForward' 
                               , InputSelection = "TES:" + recoForward.OutputTracksName
                               , OutputSelection = "Forward1"
                               , RequirePositiveInputs = False )


recoPV3D =  PatPV3D('Hlt1RecoPV3D' ) #TODO: check IPVOfflineTool config!!! InputTracksName = recoVelo.OutputTracksName )
#Hlt1RecoPV3D.PVOfflineTool.InputTracks = [ recoVelo.OutputTracksName ]
recoPV3D.addTool( PVOfflineTool, name = 'PVOfflineTool' )
recoPV3D.PVOfflineTool.InputTracks = [ recoVelo.OutputTracksName ]

trackRecoSequence = GaudiSequencer( 'HltTrackRecoSequence'
                                  ,  Members =
                                  [  recoRZVeloSequence
                                  ,  recoVelo
                                  ,  prepareVelo
                                  ,  recoPV3D # this aborts the remainder of the sequence if no primary -- do we really want that??
                                  ,  recoForward
                                  ,  prepareForward
                                  ] )

#------------------------------
# Definition of the Hlt1 Reconstruction
#-----------------------------

# why does Hlt1PrepareRZVelo::initialize  trigger an init of ToolSvc.OTRawBankDecoder???
prepareRZVelo = HltTrackFilter( 'Hlt1PrepareRZVelo'
                              , InputSelection   = "TES:" + patVeloR.OutputTracksName
                              , RequirePositiveInputs = False
                              , AddInfo = False
                              , FilterDescriptor = ["IsBackward,<,0.5"]
                              , OutputSelection     = "RZVelo" )

preparePV2D = HltVertexFilter( 'Hlt1PreparePV2D'
                             , InputSelection = "TES:" + PatPV2D('HltRecoPV2D').OutputVerticesName
                             , RequirePositiveInputs = False
                             , OutputSelection   = "PV2D" )

hlt1RecoRZVeloTracksSequence = GaudiSequencer( 'Hlt1RecoRZVeloTracksSequence' , MeasureTime = True
                                             , Members = [ recoRZVeloTracksSequence , prepareRZVelo ] )

hlt1RecoRZPVSequence = GaudiSequencer( 'Hlt1RecoRZPVSequence', MeasureTime = True
                                     , Members = [ recoRZPVSequence , preparePV2D ] )

recoRZVelo = GaudiSequencer( 'Hlt1RecoRZVeloSequence' , MeasureTime = True
                           , Members = [ recoRZVeloSequence , prepareRZVelo, preparePV2D ] ) 

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


#hlt1RecoSequence = GaudiSequencer( 'Hlt1RecoSequence', MeasureTime = True
#                                 , Members = 
#                                 [ hlt1RecoRZVeloTracksSequence
#                                 , hlt1RecoRZPVSequence
#                                 , reco1Velo
#                                 ,  recoPV3D # this aborts the remainder of the sequence if no primary -- do we really want that??
#                                 , recoFwd ] )

# Forward
#----------------
PatForwardTool( MinMomentum = 1000.
              , MinPt = 1000.
              , AddTTClusterName = "" )

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
Forward1 = bindMembers( None, [ patVeloR, recoVelo, recoForward , prepareForward ] )
PV2D     = bindMembers( None, [ patVeloR, patPV2D, preparePV2D ] )
RZVelo   = bindMembers( None, [ patVeloR, prepareRZVelo ] )
Velo     = bindMembers( None, [ RZVelo , reco1Velo ] )
Forward  = bindMembers( None, [ Velo,  recoFwd ] )
HltRecoSequence = recoSeq
