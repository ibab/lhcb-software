#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: HltHadronLines.py,v 1.8 2008-10-30 10:08:46 hernando Exp $
# =============================================================================
## @file
#  Configuration of Hadron Lines
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-08-25
# =============================================================================
"""
 script to configure Hadron trigger lines
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.8 $"
# =============================================================================

from Gaudi.Configuration import * 

from Configurables import GaudiSequencer
from Configurables import HltL0CaloPrepare
from Configurables import HltTrackUpgrade

from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import bindMembers
from HltConf.HltLine import Hlt1Member as Member
from HltConf.HltLine import Hlt1Tool   as Tool
from HltConf.HltLine import hlt1Lines, addHlt1Prop, rmHlt1Prop 

importOptions('$HLTCONFROOT/options/HltRecoSequence.py')


prepMainHadron = bindMembers( 'MainHadronPrep',
                              [ HltL0CaloPrepare('L0HadronDecision', CaloMakerTool="HadronSeedTool", CaloType = 'Hadron', MinEt = 3500.0 )
                                , GaudiSequencer('Hlt1RecoRZVeloSequence')
                                , Member ( 'TF' ,'RZVelo'
                                           , InputSelection = 'RZVelo'
                                           , FilterDescriptor = ['Calo2DChi2_L0HadronDecision,<,4']
                                           , HistogramUpdatePeriod = 1
                                           , HistoDescriptor = { 'Calo2DChi2_L0HadronDecision':('Calo2DChi2_L0HadronDecision',0.,100.,20), 'Calo2DChi2_L0HadronDecision':('Calo2DChi2_L0HadronDecision',0.,100.,20) }
                                           )
                                , Member ( 'TU', 'Velo', RecoName = 'Velo')
                                , Member ( 'TF', 'Velo'
                                           , FilterDescriptor = [ 'IP_PV2D,||>,0.1', 'Calo3DChi2_L0HadronDecision,<,4' ]
                                           , HistogramUpdatePeriod = 1
                                           , HistoDescriptor = { 'IP_PV2D':('IP_PV2D',-0.1,3.,100), 'IP_PV2DBest':('IP_PV2DBest',-0.1,3.,100), 'Calo3DChi2_L0HadronDecision':('Calo3DChi2_L0HadronDecision',-0.1,50.,100), 'Calo3DChi2_L0HadronDecision':('Calo3DChi2_L0HadronDecision',-0.1,50.,100) }
                                           )
                                , Member ( 'TM' , 'VeloCalo'
                                           , InputSelection1 = '%TUVelo'
                                           , InputSelection2 = 'L0HadronDecision'
                                           , MatchName = 'VeloCalo' , MaxQuality = 4.
                                           , HistogramUpdatePeriod = 1
                                           )
                                , GaudiSequencer('HltDecodeT')
                                , GaudiSequencer('HltDecodeTT')
                                , Member ( 'TU', 'GuidedForward'
                                           , InputSelection  = '%TMVeloCalo'
                                           , RecoName = 'GuidedForward'
                                           , HistogramUpdatePeriod = 1
                                           )
                                ])


Line ( 'SingleHadron' 
       , L0DU  = "L0_CHANNEL('Hadron')"
       , algos = 
       [ prepMainHadron 
         , Member ( 'TF' , 'Decision'
                    , OutputSelection = '%Decision'
                    , FilterDescriptor = ['PT,>,5000.']
                    , HistogramUpdatePeriod = 1
                    , HistoDescriptor = { 'PT':('PT',0.,6000.,100), 'PTBest':('PTBest',0.,6000.,100) }
                    )
         ])

Line ('DiHadron' 
      , L0DU  = "L0_CHANNEL('Hadron')"
      , algos =  
      [ prepMainHadron
        , Member ( 'TF', 'GuidedForward'
                   , FilterDescriptor = ['PT,>,2500.']
                   , HistogramUpdatePeriod = 1
                   , HistoDescriptor = { 'PT':('PT',0.,6000.,100), 'PTBest':('PTBest',0.,6000.,100) }
                   )
        , HltTrackUpgrade( 'Hlt1RecoVelo' )
        , Member ( 'TF', 'Velo'
                   , InputSelection = HltTrackUpgrade('Hlt1RecoVelo')
                   ## TODO: is this line still needed?
                   , FilterDescriptor = [ 'IP_PV2D,||>,0.1', 'MatchIDsFraction_%TFGuidedForward,<,0.9' ]
                   , HistogramUpdatePeriod = 1
                   , HistoDescriptor = { 'IP_PV2D':('IP_PV2D',0.,3.,100), 'IP_PV2DBest':('IP_PV2DBest',0.,3.,100) }
                   )
        , Member ( 'VM2', 'Velo'
                   , InputSelection1 = '%TFGuidedForward'
                   , InputSelection2 = '%TFVelo'
                   , FilterDescriptor = [ 'DOCA,<,0.2']
                   , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100), 'DOCABest':('DOCABest',0.,3.,100) }
                   )
        , Member ( 'VF' ,'Velo'
                   , FilterDescriptor = [ 'VertexDz_PV2D,>,0.']
                   , HistoDescriptor = { 'VertexDz_PV2D':('VertexDz_PV2D',-3.,3.,100), 'VertexDz_PV2DBest':('VertexDz_PV2D',-3.,3.,100) }
                   )
        , Member ( 'VU', 'Forward' , RecoName = 'Forward')
        , Member ( 'VF', 'Decision'
                   , OutputSelection = '%Decision'
                   , FilterDescriptor = [ 'VertexMinPT,>,1000.', 'VertexPointing_PV2D,<,0.4' ]
                   , HistogramUpdatePeriod = 1
                   , HistoDescriptor = { 'VertexMinPT':('VertexMinPT',0.,6000.,100), 'VertexMinPTBest':('VertexMinPTBest',0.,6000.,100), 'VertexPointing_PV2D':('VertexPointing_PV2D',0.,1.,100), 'VertexPointing_PV2DBest':('VertexPointing_PV2DBest',0.,1.,100) }
                   )
        ])
