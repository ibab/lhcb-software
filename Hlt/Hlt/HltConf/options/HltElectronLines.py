#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: HltElectronLines.py,v 1.3 2008-09-24 13:16:37 graven Exp $
# =============================================================================
## @file
#  Configuration of Photon Lines
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-09-24
# =============================================================================
'''
 script to configure Electron trigger lines
'''
# =============================================================================
__author__  = 'Gerhard Raven Gerhard.Raven@nikhef.nl'
__version__ = 'CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.3 $'
# =============================================================================

from Gaudi.Configuration import * 

from Configurables import GaudiSequencer
from Configurables import HltL0CaloPrepare
from Configurables import HltTrackUpgrade

from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member
from HltConf.HltLine import bindMembers
from HltConf.HltLine import Hlt1Tool   as Tool
from HltConf.HltLine import hlt1Lines, addHlt1Prop, rmHlt1Prop 



prepElectron =  bindMembers( 'PrepElectron', 
  [ Member ('L0CaloPrepare' 
           , InputSelection = HltL0CaloPrepare().getDefaultProperties()['InputSelection']
           , CaloType = 'Electron', MinEt = 2600.0
           )
  , GaudiSequencer('Hlt1RecoRZVeloSequence')
  , Member ('TF', 'Hlt1EleSingleTFRZVelo'  # // select RZVelo tracks with an IP and matched to L0Calo
           , InputSelection     = 'RZVelo'
           , FilterDescriptor = [ 'Calo2DChi2_L0ElectronDecision,<,4.' ]
           , HistogramUpdatePeriod = 0
           , HistoDescriptor = {  'Calo2DChi2'     : ('Calo2DChi2',0.,20.,100), 'Calo2DChi2Best' : ('Calo2DChi2Best',0.,20.,100) }
           )
  , Member ( 'TU', 'Velo', RecoName = 'Velo') # // Velo reconstruction of selected RZVelo
  , Member ( 'TF', 'Velo' # // Select Velo tracks with an IP and matched to L0Calo
           , HistogramUpdatePeriod = 0
           , FilterDescriptor = [ 'IP_PV2D,||[],0.1,3.', 'Calo3DChi2_L0ElectronDecision,<,4.' ]
           , HistoDescriptor = { 'IP' :           ('IP',-1.,3.,400), 'IPBest' :       ('IPBest',-1.,3.,400), 'Calo3DChi2':    ('Calo3DChi2',0.,20.,100), 'Calo3DChi2Best':('Calo3DChi2Best',0.,20.,100) }
           )
  , GaudiSequencer('HltDecodeT')
  , GaudiSequencer('HltDecodeTT')
  , Member ( 'TU', 'Forward' # // Make forward the selected velo tracks
           , RecoName = 'Forward'
           )
  , Member ( 'TF','Forward' # // Select forward track with a given Pt
           , FilterDescriptor = ['PT,>,1300.']
           , HistogramUpdatePeriod = 0
           , HistoDescriptor = { 'PT'     : ('PT',0.,6000.,100), 'PTBest' : ('PTBest',0.,6000.,100) }
           )
  ])


Line ('SingleElectron'
     , L0 = ['Electron']
     , algos = 
     [ prepElectron
     , Member( 'TF','Hlt1ElectronSingleDecision'
             , InputSelection  = prepElectron.outputSelection()
             , FilterDescriptor = ['PT,>,3000.']
             , HistogramUpdatePeriod = 0
             , HistoDescriptor = { 'PT' : ('PT',0.,8000.,100), 'PTBest' : ('PTBest',0.,8000.,100)}
             )
     ])


Line( 'ElectronTrack' 
    , L0 = [ 'Electron' ]
    , algos = 
    [ prepElectron
    , HltTrackUpgrade( 'Hlt1RecoVelo' )
    , Member ( 'TF', 'CompanionVelo' # select velo tracks as companions 
             , InputSelection    = 'Velo'
             , HistogramUpdatePeriod = 0
             , FilterDescriptor = [ 'IP_PV2D,||[],0.1,3.', 'DOCA_Hlt1EleTFForward,<,0.2' ]
             , HistoDescriptor = { 'IP': ( 'IP',-1.,.3,600), 'IPBest':   ( 'IPBest',-1.,3.,600), 'DOCA':     ( 'DOCA',0.,1.,100), 'DOCABest': ( 'DOCABest',0.,1.,100) }
             )
    , Member ( 'TU', 'CompanionForward' # // Do forward the companion tracks
             , RecoName = 'Forward'
             )
    , Member ( 'TF', 'CompanionForward' # // // Select companion tracks if they have a fiven Pt
             , FilterDescriptor = ['PT,>,1000.']
             , HistogramUpdatePeriod = 0
             , HistoDescriptor = { 'PT'     : ('PT',0.,8000.,100), 'PTBest' : ('PTBest',0.,8000.,100)}
             )
    , Member ( 'VM2', 'DiElectron' # // // Make vertices with the forward companion tracks 
             , InputSelection1   = prepElectron.outputSelection()
             , InputSelection2   = '%TFCompanionForward'
             , FilterDescriptor = ['DOCA,<,0.2']
             , HistogramUpdatePeriod = 0
             , HistoDescriptor = { 'DOCA' :     ('DOCA',0.,1.,100), 'DOCABest' : ('DOCABest',0.,0.5,100) }
             )
    , Member ( 'VF', 'Decision' # // select vertices if Pt, poiting, and distance
             , FilterDescriptor = [ 'VertexPointing_PV2D,<,0.5', 'VertexDz_PV2D,>,0.' ]
             , HistogramUpdatePeriod = 0
             , HistoDescriptor = { 'VertexPointing'    : ('VertexPointing',0.,1.,100), 'VertexPointingBest': ('VertexPointingBest',0.,1.,100), 'VertexDz'          : ('VertexDz',-10.,50.,100), 'VertexDzBest'      : ('VertexDzBest',-10.,50.,100) }
             )
    ])
