#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: HltPhotonLines.py,v 1.7 2009-02-18 15:16:39 graven Exp $
# =============================================================================
## @file
#  Configuration of Photon Lines
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-08-25
# =============================================================================
'''
 script to configure Photon trigger lines
'''
# =============================================================================
__author__  = 'Gerhard Raven Gerhard.Raven@nikhef.nl'
__version__ = 'CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.7 $'
# =============================================================================

from Gaudi.Configuration import * 
from LHCbKernel.Configuration import *

from Configurables import GaudiSequencer
from Configurables import PatMatchTool
from Configurables import HltTrackFunctionFactory
from Configurables import HltAntiEleConf
from Configurables import L0ConfirmWithT
from Configurables import PatConfirmTool
from Configurables import HltTrackUpgrade

from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member
from HltConf.HltLine import Hlt1Tool   as Tool
from HltConf.HltLine import hlt1Lines, addHlt1Prop, rmHlt1Prop 

#//---------------------------
#// HLT Photon Alley
#//--------------------------
from HltConf.HltL0Candidates import *


class HltPhotonLinesConf(LHCbConfigurableUser):
   __slots__ = { 'Prescale'        : 1
               , 'Postscale'       : 1
               , 'Track_PtCut'     : 650.   # for global optimization 1
               , 'Track_IPCut3D'   : 0.15   # for global optimization 2
               , 'Pho_IsPho'       : -0.1   # for global optimization 3 optional
               , 'Pho_EtCut'       : 2500.
               }

   def __apply_configuration__(self):

        TRACK_PT_CUT = str(self.getProp('Track_PtCut'))
        TRACK_IP_CUT = str(self.getProp('Track_IPCut3D'))
        IS_PHOTON    = str(self.getProp('Pho_IsPho'))
        L0ET_CUT     = str(self.getProp('Pho_EtCut'))

        commonSeq =   [ Member ('TF', 'L0ET' , FilterDescriptor = ['L0ET,>,'+L0ET_CUT]) 
                      , Member ('TF', 'AntiEle' , FilterDescriptor = ['AntiEleConf,>,0.5'] 
                               , tools = [ Tool( HltTrackFunctionFactory,
                                 tools = [ Tool( HltAntiEleConf, 
                                 tools = [ Tool( L0ConfirmWithT,  particleType = 2 ,
                                 tools = [ Tool( PatConfirmTool,  nSigmaX=10, nSigmaTx=10,nSigmaY=10,nSigmaTy=10 )])])])]
                               )
                      , Member ('TF', 'Photon' , FilterDescriptor = ['IsPhoton,>,'+IS_PHOTON])
                      , GaudiSequencer('Hlt1RecoRZVeloSequence')
                      , Member ('TF', 'RZVelo'
                               , InputSelection     = 'RZVelo'
                               , FilterDescriptor = ['rIP_PV2D,||[],0.10,3.0']
                               , HistogramUpdatePeriod = 0
                               , HistoDescriptor = { 'rIP' : ('rIP',-1.,3.,400), 'rIPBest' : ('rIPBest',-1.,3.,400)}
                               )
                      , Member ('TU', 'Velo' , RecoName = 'Velo') #  Velo Reco
                      , Member ('TF', 'Velo' #  3D IP selection
                               , FilterDescriptor = ['IP_PV2D,||[],'+TRACK_IP_CUT+',3.0']
                               , HistogramUpdatePeriod = 0
                               , HistoDescriptor = { 'IP' : ('IP',-1.,3.,400), 'IPBest' : ('IPBest',-1.,3.,400) }
                               )
                      , HltTrackUpgrade( 'Hlt1RecoVelo' )
                      , Member ('TF', 'SecondVelo' 
                               , InputSelection     = 'Velo'
                               , FilterDescriptor = ['IP_PV2D,||[],'+TRACK_IP_CUT+',3.0']
                               , HistogramUpdatePeriod = 0
                               , HistoDescriptor = { 'IP' : ('IP',-1.,3.,400), 'IPBest' : ('IPBest',-1.,3.,400) }
                               )
                      , Member ('TU' ,'Forward' , RecoName = 'Forward') #  upgrade to Forward 
                      , Member ('TF', 'Forward' #  Pt cut (call it pretrigger sice could not implement veloTT)
                               , FilterDescriptor = ['PT,>,'+TRACK_PT_CUT]
                               , HistogramUpdatePeriod = 0
                               , HistoDescriptor =  { 'PT'     : ('PT',0.,8000.,100), 'PTBest' : ('PTBest',0.,8000.,100) }
                               )
                      , Member ('VM2', 'DiTrack' # two track vertex: DOCA
                               , InputSelection1 = '%TFForward'
                               , InputSelection2 = '%TFSecondVelo'
                               , FilterDescriptor = [ 'DOCA,<,0.2']
                               , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100), 'DOCABest':('DOCABest',0.,3.,100) }
                               )
                      , Member ('VF', 'DiTrack' #two track vertex: DZ
                               , FilterDescriptor = [ 'VertexDz_PV2D,>,0.']
                               , HistoDescriptor = { 'VertexDz_PV2D':('VertexDz_PV2D',-3.,3.,100), 'VertexDz_PV2DBest':('VertexDz_PV2D',-3.,3.,100) }
                               )
                      , Member ('VU', 'DiTrack' , RecoName = 'Forward')
                      , Member ('VF', 'SecondDiTrack' , FilterDescriptor = [ 'VertexMinPT,>,'+TRACK_PT_CUT])
                      ]

        Line ('Photon' 
              , L0DU = "L0_CHANNEL('Photon')"
              , algos = [ convertL0Candidates('Photon') ] + commonSeq
                      + [ Member ( 'AddPhotonToVertex', 'DiTrackDecision' # add photon track to ditrack vertex to save all objects into summary
                                 , InputSelection1 = '%VFSecondDiTrack'
                                 , InputSelection2 = '%TFPhoton'
                                 , OutputSelection = '%Decision'
                                 )
                        ]
             )

        Line ('PhotonFromEle' 
              , L0DU = "L0_CHANNEL('Electron')"
              , algos = [ convertL0Candidates('Electron') ] + commonSeq
                      + [ Member ( 'AddPhotonToVertex', 'DiTrackDecision' # add photon track to ditrack vertex to save all objects into summary
                                 , InputSelection1 = '%VFSecondDiTrack'
                                 , InputSelection2 = '%TFPhoton'
                                 , OutputSelection = '%Decision'
                                 )
                        ]
             )

