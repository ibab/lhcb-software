#!/usr/bin/env gaudirun.py
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
__version__ = 'CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.2 $'
# =============================================================================


from HltLine.HltLinesConfigurableUser import *

#//---------------------------
#// HLT Photon Alley
#//--------------------------


class Hlt1PhotonLinesConf(HltLinesConfigurableUser):
   __slots__ = { 'Track_PtCut'     : 650.   # for global optimization 1
               , 'Track_IPCut3D'   : 0.15   # for global optimization 2
               , 'Pho_IsPho'       : -0.1   # for global optimization 3 optional
               , 'Pho_EtCut'       : 2500.
               , 'PhoTra_PtCut'    : 1250.
               }

   def __apply_configuration__(self):
        from HltLine.HltLine import Hlt1Line   as Line
        from HltLine.HltLine import Hlt1Member as Member
        from HltLine.HltLine import Hlt1Tool   as Tool
        from HltLine.HltLine import hlt1Lines, addHlt1Prop, rmHlt1Prop 
        from Hlt1Lines.HltL0Candidates import convertL0Candidates
        from Configurables import HltTrackFunctionFactory
        from Configurables import HltAntiEleConf
        from Configurables import L0ConfirmWithT
        from Configurables import PatConfirmTool
        from HltLine.HltReco import RZVelo, Velo, PV2D
        from HltLine.HltDecodeRaw import DecodeIT, DecodeECAL

        ####
        #from Configurables import L0Calo2CaloTool
        #l0c2c = L0Calo2CaloTool()
        #l0c2c.DuplicateClustersOnTES = True
        ####

        TRACK_PT_CUT = str(self.getProp('Track_PtCut'))
        TRACK_IP_CUT = str(self.getProp('Track_IPCut3D'))
        IS_PHOTON    = str(self.getProp('Pho_IsPho'))
        L0ET_CUT     = str(self.getProp('Pho_EtCut'))
        PHOTRA_PT_CUT = str(self.getProp('PhoTra_PtCut'))

        commonSeq1 =  [ Member ('TF', 'L0ET' , FilterDescriptor = ['L0ET,>,'+L0ET_CUT]) 
                      , DecodeIT
                      , Member ('TF', 'AntiEle' , FilterDescriptor = ['AntiEleConf,>,0.5'] 
                               , tools = [ Tool( HltTrackFunctionFactory,
                                 tools = [ Tool( HltAntiEleConf, 
                                 tools = [ Tool( L0ConfirmWithT,  particleType = 2 ,
                                 tools = [ Tool( PatConfirmTool,  nSigmaX=10, nSigmaTx=10,nSigmaY=10,nSigmaTy=10 )])])])]
                               )
                      , DecodeECAL
                      , Member ('TF', 'Photon' , FilterDescriptor = ['IsPhoton,>,'+IS_PHOTON]
                               , HistogramUpdatePeriod = 0
                               , HistoDescriptor = { 'IsPhoton' : ('IsPhoton',-2.,2.,400), 'IsPhotonBest' : ('IsPhotonBest',-2.,2.,400)}
                               )

                      , RZVelo, PV2D.ignoreOutputSelection()
                      , Member ('TF', 'RZVelo'  # 2D IP selection
                               , FilterDescriptor = ['rIP_PV2D,||[],0.10,3.0']
                               , HistogramUpdatePeriod = 0
                               , HistoDescriptor = { 'rIP' : ('rIP',-1.,3.,400), 'rIPBest' : ('rIPBest',-1.,3.,400)}
                               )
                      , Member ('TU', 'Velo' , RecoName = 'Velo')
                      , Member ('TF', 'Velo' #  3D IP selection
                               , FilterDescriptor = ['IP_PV2D,||[],'+TRACK_IP_CUT+',3.0']
                               , HistogramUpdatePeriod = 0
                               , HistoDescriptor = { 'IP' : ('IP',-1.,3.,400), 'IPBest' : ('IPBest',-1.,3.,400) }
                               )
                      , Member ('TU' ,'FirstForward' , RecoName = 'Forward') 
                      ]
        commonSeq2 =  [ Member ('TF', 'FirstForward'
                               , FilterDescriptor = ['PT,>,'+TRACK_PT_CUT]
                               )
                      , Velo
                      , Member ('TF', 'SecondVelo' 
                               , FilterDescriptor = ['IP_PV2D,||[],'+TRACK_IP_CUT+',3.0']
                               , HistogramUpdatePeriod = 0
                               , HistoDescriptor = { 'IP' : ('IP',-1.,3.,400), 'IPBest' : ('IPBest',-1.,3.,400) }
                               )
                      , Member ('TU' ,'SecondForward' , RecoName = 'Forward')
                      , Member ('TF', 'SecondForward'
                               , FilterDescriptor = ['PT,>,'+TRACK_PT_CUT]
                               , HistogramUpdatePeriod = 0
                               , HistoDescriptor =  { 'PT'     : ('PT',0.,8000.,100), 'PTBest' : ('PTBest',0.,8000.,100) }
                               )
                      , Member ('VM2', 'DiTrack' # two track vertex: DOCA
                               , InputSelection1 = '%TFFirstForward'
                               , InputSelection2 = '%TFSecondForward'
                               , FilterDescriptor = [ 'DOCA,<,0.2']
                               , HistogramUpdatePeriod = 0
                               , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100), 'DOCABest':('DOCABest',0.,3.,100) }
                               )
                      , Member ('VF', 'DiTrack' #two track vertex: DZ
                               , FilterDescriptor = [ 'VertexDz_PV2D,>,0.']
                               , HistoDescriptor = { 'VertexDz_PV2D':('VertexDz_PV2D',-3.,3.,100), 'VertexDz_PV2DBest':('VertexDz_PV2D',-3.,3.,100) }
                               )
                      ]

        from Hlt1Lines.HltL0Candidates import L0Channels
        if 'Photon' in L0Channels() :
            Line ('PhotonTrack' 
                  , prescale = self.prescale
                  , L0DU = "L0_CHANNEL('Photon')"
                  , algos = [ convertL0Candidates('Photon') ] + commonSeq1
                          + [ Member ('TF', 'Forward'
                                      , FilterDescriptor = ['PT,>,'+PHOTRA_PT_CUT]
                                      , HistogramUpdatePeriod = 0
                                      , HistoDescriptor = { 'PT' : ('PT',0.,6000.,300), 'PTBest' : ('PTBest',0.,6000.,300)}
                                     )
                            , Member ('VM2', 'PhoTra'
                                   , InputSelection1 = '%TFPhoton'
                                   , InputSelection2 = '%TFForward'
                                   , OutputSelection = '%Decision'
                                   )
                            ]
                  , postscale = self.postscale
                 )
        if 'Electron' in L0Channels() :
            Line ('PhotonFromEleTrack' 
                  , prescale = self.prescale
                  , L0DU = "L0_CHANNEL('Electron')"
                  , algos = [ convertL0Candidates('Electron') ] + commonSeq1
                          + [ Member ('TF', 'Forward'
                                      , FilterDescriptor = ['PT,>,'+PHOTRA_PT_CUT]
                                      , HistogramUpdatePeriod = 0
                                      , HistoDescriptor = { 'PT' : ('PT',0.,6000.,300), 'PTBest' : ('PTBest',0.,6000.,300)}
                                     )
                            , Member ('VM2', 'PhoTra'
                                   , InputSelection1 = '%TFPhoton'
                                   , InputSelection2 = '%TFForward'
                                   , OutputSelection = '%Decision'
                                   )
                            ]
                  , postscale = self.postscale
                 )

        if 'Photon' in L0Channels() :
            Line ('PhotonDiTrack' 
                  , prescale = self.prescale
                  , L0DU = "L0_CHANNEL('Photon')"
                  , algos = [ convertL0Candidates('Photon') ] + commonSeq1 + commonSeq2
                          + [ Member ( 'AddPhotonToVertex', 'DiTrackDecision' # add photon track to ditrack vertex to save all objects into summary
                                     , InputSelection1 = '%VFDiTrack'
                                     , InputSelection2 = '%TFPhoton'
                                     , OutputSelection = '%Decision'
                                     )
                            ]
                  , postscale = self.postscale
                 )

        if 'Electron' in L0Channels() :
            Line ('PhotonFromEleDiTrack' 
                  , prescale = self.prescale
                  , L0DU = "L0_CHANNEL('Electron')"
                  , algos = [ convertL0Candidates('Electron') ] + commonSeq1 + commonSeq2
                          + [ Member ( 'AddPhotonToVertex', 'DiTrackDecision' # add photon track to ditrack vertex to save all objects into summary
                                     , InputSelection1 = '%VFDiTrack'
                                     , InputSelection2 = '%TFPhoton'
                                     , OutputSelection = '%Decision'
                                     )
                            ]
                  , postscale = self.postscale
                 )
