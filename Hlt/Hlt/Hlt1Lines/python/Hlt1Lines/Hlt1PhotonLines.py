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
__version__ = 'CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.9 $'
# =============================================================================


from HltLine.HltLinesConfigurableUser import *

#//---------------------------
#// HLT Photon Alley
#//--------------------------


class Hlt1PhotonLinesConf(HltLinesConfigurableUser):
   __slots__ = { 'Pho_IsPho'         : -0.1   
               , 'Pho_EtCut'         : 2800.
               , 'Track_PtCut'       : 1250.
               , 'Track_IPCut'       : 0.15
               , 'DiTrack_PtCut'     : 650.  
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
        from HltTracking.HltReco import Velo
        from HltTracking.HltPVs  import PV3D
        from HltLine.HltDecodeRaw import DecodeIT, DecodeECAL
        from Hlt1Lines.HltFastTrackFit import setupHltFastTrackFit

        from Configurables import HltTrackUpgradeTool, PatForwardTool
        from Hlt1Lines.HltConfigurePR import ConfiguredPR
              

        ####
        #from Configurables import L0Calo2CaloTool
        #l0c2c = L0Calo2CaloTool()
        #l0c2c.DuplicateClustersOnTES = True
        ####




        commonSeq1 =  [ Member ('TF', 'L0ET' , FilterDescriptor = ['L0ET,>,'+str(self.getProp('Pho_EtCut'))]) 
                      , DecodeIT
                      , Member ('TF', 'AntiEle' , FilterDescriptor = ['AntiEleConf,>,0.5'] 
                               , tools = [ Tool( HltTrackFunctionFactory,
                                 tools = [ Tool( HltAntiEleConf, 
                                 tools = [ Tool( L0ConfirmWithT,  particleType = 2 ,
                                 tools = [ Tool( PatConfirmTool,  nSigmaX=10, nSigmaTx=10,nSigmaY=10,nSigmaTy=10, tools = [ConfiguredPR( "PatSeeding" )]
                                                 )])])])]
                               )
                      , DecodeECAL
                      , Member ('TF', 'Photon' , FilterDescriptor = ['IsPhoton,>,'+str(self.getProp('Pho_IsPho'))]
                               , HistogramUpdatePeriod = 0
                               , HistoDescriptor = { 'IsPhoton' : ('IsPhoton',-2.,2.,400), 'IsPhotonBest' : ('IsPhotonBest',-2.,2.,400)}
                               )
                      , Velo, PV3D().ignoreOutputSelection()
                      , Member ('TF', 'Velo'
                               , FilterDescriptor = ['IP_PV3D,||>,'+str(self.getProp('Track_IPCut'))]
                               , HistogramUpdatePeriod = 0
                               , HistoDescriptor = { 'IP' : ('IP',-1.,3.,400), 'IPBest' : ('IPBest',-1.,3.,400) }
                               )
                      , Member ('TU' ,'FirstForward'
                                , RecoName = 'Forward'
                                , tools = [ Tool( HltTrackUpgradeTool
                                                  ,tools = [ConfiguredPR( "Forward" )] )]
                                ) 
                      ]
        commonSeq2 =  [ Member ('TF', 'FirstForward'
                               , FilterDescriptor = ['PT,>,'+str(self.getProp('DiTrack_PtCut'))]
                               )
                      , Velo
                      , Member ('TF', 'SecondVelo' 
                               , FilterDescriptor = ['IP_PV3D,||>,'+str(self.getProp('Track_IPCut'))]
                               , HistogramUpdatePeriod = 0
                               , HistoDescriptor = { 'IP' : ('IP',-1.,3.,400), 'IPBest' : ('IPBest',-1.,3.,400) }
                               )
                      , Member ('TU' ,'SecondForward'
                                , RecoName = 'Forward'
                                , tools = [ Tool( HltTrackUpgradeTool
                                                  ,tools = [ConfiguredPR( "Forward" )] )]
                                )
                      , Member ('TF', 'SecondForward'
                               , FilterDescriptor = ['PT,>,'+str(self.getProp('DiTrack_PtCut'))]
                               , HistogramUpdatePeriod = 0
                               , HistoDescriptor =  { 'PT'     : ('PT',0.,8000.,100), 'PTBest' : ('PTBest',0.,8000.,100) }
                               )
                      , Member ('VM2', 'DiTrack' 
                               , InputSelection1 = '%TFFirstForward'
                               , InputSelection2 = '%TFSecondForward'
                               , FilterDescriptor = [ 'DOCA,<,0.2']
                               , HistogramUpdatePeriod = 0
                               , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100), 'DOCABest':('DOCABest',0.,3.,100) }
                               )
                      , Member ('VF', 'DiTrack' 
                               , FilterDescriptor = [ 'VertexDz_PV3D,>,0.']
                               , HistoDescriptor = { 'VertexDz_PV3D':('VertexDz_PV3D',-3.,3.,100), 'VertexDz_PV3DBest':('VertexDz_PV3D',-3.,3.,100) }
                               )
                      , Member ('VU', 'FitTrack',  RecoName = 'FitTrack', callback = setupHltFastTrackFit )
                      , Member ('VF', 'FitTrack'
                               , FilterDescriptor = [ 'FitVertexMinIP_PV3D,||>,'+str(self.getProp('Track_IPCut'))]
                               , HistoDescriptor = { 'FitVertexMinIP_PV3D':('FitVertexMinIP_PV3D',-3.,3.,100), 'FitVertexMinIP_PV3DBest':('FitVertexMinIP_PV3D',-3.,3.,100) }
                               )
                      ]

        from Hlt1Lines.HltL0Candidates import L0Channels
        if 'Photon' in L0Channels() :
            Line ('PhotonTrack' 
                  , prescale = self.prescale
                  , L0DU = "L0_CHANNEL('Photon')"
                  , algos = [ convertL0Candidates('Photon') ] + commonSeq1
                          + [ Member ('TF', 'Forward'
                                     , FilterDescriptor = ['PT,>,'+str(self.getProp('Track_PtCut'))]
                                     , HistogramUpdatePeriod = 0
                                     , HistoDescriptor = { 'PT' : ('PT',0.,6000.,300), 'PTBest' : ('PTBest',0.,6000.,300)}
                                     )
                            , Member ( 'TU', 'FitTrack', RecoName = 'FitTrack', callback = setupHltFastTrackFit )
                            , Member ( 'TF', 'FitTrack'
                                     , FilterDescriptor = ['FitIP_PV3D,||>,'+str(self.getProp('Track_IPCut'))]
                                     )  
                            , Member ('VM2', 'PhoTra'
                                     , InputSelection1 = '%TFPhoton'
                                     , InputSelection2 = '%TFFitTrack'
                                     , OutputSelection = '%Decision'
                                     )
                            ]
                  , postscale = self.postscale
                 )
        if 'Electron' in L0Channels() :
            Line ('PhotonTrackFromEle' 
                  , prescale = self.prescale
                  , L0DU = "L0_CHANNEL('Electron')"
                  , algos = [ convertL0Candidates('Electron') ] + commonSeq1
                          + [ Member ('TF', 'Forward'
                                     , FilterDescriptor = ['PT,>,'+str(self.getProp('Track_PtCut'))]
                                     , HistogramUpdatePeriod = 0
                                     , HistoDescriptor = { 'PT' : ('PT',0.,6000.,300), 'PTBest' : ('PTBest',0.,6000.,300)}
                                     )
                            , Member ( 'TU', 'FitTrack', RecoName = 'FitTrack', callback = setupHltFastTrackFit )
                            , Member ( 'TF', 'FitTrack'
                                     , FilterDescriptor = ['FitIP_PV3D,||>,'+str(self.getProp('Track_IPCut'))]
                                     )
                            , Member ('VM2', 'PhoTra'
                                     , InputSelection1 = '%TFPhoton'
                                     , InputSelection2 = '%TFFitTrack'
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
                          + [ Member ( 'AddPhotonToVertex', 'DiTrackDecision'
                                     , InputSelection1 = '%VFFitTrack'
                                     , InputSelection2 = '%TFPhoton'
                                     , OutputSelection = '%Decision'
                                     )
                            ]
                  , postscale = self.postscale
                 )

        if 'Electron' in L0Channels() :
            Line ('PhotonDiTrackFromEle' 
                  , prescale = self.prescale
                  , L0DU = "L0_CHANNEL('Electron')"
                  , algos = [ convertL0Candidates('Electron') ] + commonSeq1 + commonSeq2
                          + [ Member ( 'AddPhotonToVertex', 'DiTrackDecision'
                                     , InputSelection1 = '%VFFitTrack'
                                     , InputSelection2 = '%TFPhoton'
                                     , OutputSelection = '%Decision'
                                     )
                            ]
                  , postscale = self.postscale
                 )
