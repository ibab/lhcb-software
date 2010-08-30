
from Gaudi.Configuration import * 


from HltLine.HltLinesConfigurableUser import *


class Hlt1ElectronLinesConf(HltLinesConfigurableUser) :

    # steering variables
    __slots__ = { 'Ele_EtCut'              :  2600.    # L0 ET cut
                , 'EleIP_IPCut'            :  0.1      # single electron with IP cut
                , 'EleIP_PtCut'            :  3000.    
                , 'Ele_PtCut'              :  10000.   # single electron without IP cut
                , 'EleIPCompanion_PtCut'   :  1000.    # di-electron with IP cut
                , 'EleCompanion_PtCut'     :  1000.    # di-electron without IP cut           
                , 'DiEle_LowMassCut'       :  2400.
                , 'DiEle_HighMassCut'      :  -1.0     # do not user upper mass cut    
                , 'DiEle_PtCut'            :  2500.    # pt cut for di-electron
                }

#
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt1Line   as Line
        from HltLine.HltLine import Hlt1Member as Member
        from HltLine.HltLine import bindMembers
        from HltLine.HltLine import Hlt1Tool   as Tool
        from Hlt1Lines.HltL0Candidates import convertL0Candidates
        from Configurables import HltTrackUpgradeTool
        from Configurables import L0ConfirmWithT
        from HltTracking.HltReco import RZVelo, Velo
        from HltTracking.HltPVs  import PV3D
        from HltLine.HltDecodeRaw import DecodeIT, DecodeTT, DecodeVELO, DecodeECAL
        from Hlt1Lines.HltFastTrackFit import setupHltFastTrackFit
        from Configurables import HltTrackUpgradeTool, PatForwardTool, HltGuidedForward, PatConfirmTool
        from Hlt1Lines.HltConfigurePR import ConfiguredPR
              

        ####
        #from Configurables import L0Calo2CaloTool
        #l0c2c = L0Calo2CaloTool()
        #l0c2c.DuplicateClustersOnTES = True
        ####
        
        
    

        ##### common bodies IP
        prepareElectronWithIP = [ Member ( 'TF', 'L0Electrons', FilterDescriptor = [ 'L0ET,>,'+str(self.getProp('Ele_EtCut')) ] )
                                  , DecodeIT
                                  , Member ( 'TU', 'TConf', RecoName = 'TEleConf'
                                             , tools = [ Tool( HltTrackUpgradeTool
                                               , tools = [ Tool( L0ConfirmWithT
                                                 , 'TEleConf'
                                                 , particleType = 2
                                                 , tools = [ Tool( PatConfirmTool
                                                   , tools = [ConfiguredPR( "PatSeeding" )]
                                                                   )]
                                                                 )]
                                                               )]
                                             )
                                  , Velo, PV3D().ignoreOutputSelection()
                                  , DecodeTT
                                  , Member ( 'TM', 'VeloT', InputSelection1 = 'Velo' , InputSelection2 = '%TUTConf' , MatchName = 'VeloT')
                                  , Member ( 'TF', 'VeloT', FilterDescriptor = [ 'IP_PV3D,||>,'+str(self.getProp('EleIP_IPCut'))]
                                            , HistogramUpdatePeriod = 0
                                            , HistoDescriptor = { 'IP' : ('IP',-1.,3.,400), 'IPBest' : ('IPBest',-1.,3.,400) }
                                            )
                                  ]

        companionTrackWithIP  = [ Velo
                                  , Member ( 'TF', 'CompanionVelo',
                                             FilterDescriptor = [ 'IP_PV3D,||>,'+str(self.getProp('EleIP_IPCut')), 'DOCA_%TFVeloT,<,0.2' ]
                                            , HistogramUpdatePeriod = 0
                                            , HistoDescriptor = { 'IP' : ('IP',-1.,3.,400), 'IPBest' : ('IPBest',-1.,3.,400) }
                                            )
                                  , Member ( 'TU', 'CompanionForward'
                                             , RecoName = 'Forward'
                                             , tools = [ Tool( HltTrackUpgradeTool
                                                               ,tools = [ConfiguredPR( "Forward" )] )]
                                             )
                                  , Member ( 'TF', 'CompanionForward',  FilterDescriptor = ['PT,>,'+str(self.getProp('EleIPCompanion_PtCut'))]
                                            , HistogramUpdatePeriod = 0
                                            , HistoDescriptor =  { 'PT'     : ('PT',0.,8000.,100), 'PTBest' : ('PTBest',0.,8000.,100) }
                                            )
                                  , Member ( 'VM2', 'DiElectron', InputSelection1 = '%TFVeloT', InputSelection2 = '%TFCompanionForward',
                                             FilterDescriptor = ['DOCA,<,0.2']
                                            , HistogramUpdatePeriod = 0
                                            , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100), 'DOCABest':('DOCABest',0.,3.,100) }
                                            )
                                  ]
        
        ##### common bodies no IP
        prepareElectronNoIP   = [ Member ( 'TF', 'L0Electrons', FilterDescriptor = [ 'L0ET,>,'+str(self.getProp('Ele_EtCut')) ] )
                                  , DecodeIT
                                  , Member ( 'TU', 'TConf', RecoName = 'TEleConf',
                                             tools = [ Tool( HltTrackUpgradeTool
                                             , tools = [ Tool( L0ConfirmWithT
                                             , 'TEleConf'
                                             ,  particleType = 2
                                             , tools = [ Tool( PatConfirmTool
                                               , tools = [ConfiguredPR( "PatSeeding" )]
                                                               )]
                                                            )]
                                                          )]
                                             )
                                  , Velo, PV3D().ignoreOutputSelection()
                                  , DecodeTT
                                  , Member ( 'TM', 'VeloT', InputSelection1 = 'Velo' , InputSelection2 = '%TUTConf' , MatchName = 'VeloT')
                                  ]

        companionTrackNoIP    = [ Velo
                                , Member ( 'TF', 'CompanionVelo', 
                                           FilterDescriptor = [ 'DOCA_%TMVeloT,<,0.15' ])
                                , Member ( 'TU', 'CompanionForward'
                                           , RecoName = 'Forward'
                                           , tools = [ Tool( HltTrackUpgradeTool
                                                             ,tools = [ConfiguredPR( "Forward" )] )]
                                           )
                                , Member ( 'TF', 'CompanionForward',  FilterDescriptor = ['PT,>,'+str(self.getProp('EleCompanion_PtCut'))]
                                          , HistogramUpdatePeriod = 0
                                          , HistoDescriptor =  { 'PT'     : ('PT',0.,8000.,100), 'PTBest' : ('PTBest',0.,8000.,100) }
                                          )
                                , Member ( 'VM2', 'DiElectron', InputSelection1 = '%TMVeloT', InputSelection2 = '%TFCompanionForward',
                                           FilterDescriptor = ['DOCA,<,0.2']
                                          , HistogramUpdatePeriod = 0
                                          , HistoDescriptor = { 'DOCA':('DOCA',0.,3.,100), 'DOCABest':('DOCABest',0.,3.,100) }
                                          )
                                ]

        ##### Lines
        from Hlt1Lines.HltL0Candidates import L0Channels
        from Hlt1Lines.Hlt1GECs import Hlt1GEC
        if 'Electron' in L0Channels() :
            Line ('SingleElectronWithIP'
                     , prescale = self.prescale
                     , L0DU = "L0_CHANNEL('Electron')"
                     , algos = [ Hlt1GEC(),convertL0Candidates('Electron') ] + prepareElectronWithIP
                             + [ Member ( 'TF','PTCut'
                                        , FilterDescriptor = ['PT,>,'+str(self.getProp('EleIP_PtCut'))]
                                        , HistogramUpdatePeriod = 0
                                        , HistoDescriptor = { 'PT' : ('PT',0.,8000.,100), 'PTBest' : ('PTBest',0.,8000.,100)}
                                        )
                               , Member ( 'TU', 'FitTrack', RecoName = 'FitTrack', callback = setupHltFastTrackFit )
                               , Member ( 'TF', 'Decision'
                                        , FilterDescriptor = ['FitIP_PV3D,||>,'+str(self.getProp('EleIP_IPCut'))]
                                        , OutputSelection = '%Decision'
                                        )         
                               ]
                     , postscale = self.postscale
                     )

        if 'Electron' in L0Channels() :
            Line ('SingleElectronNoIP'
                     , prescale = self.prescale
                     , L0DU = "L0_CHANNEL('Electron')"
                     , algos = [ Hlt1GEC(),convertL0Candidates('Electron') ] + prepareElectronNoIP
                             + [ Member ( 'TF','PTCut'
                                        , FilterDescriptor = ['PT,>,'+str(self.getProp('Ele_PtCut'))]
                                        , HistogramUpdatePeriod = 0
                                        , HistoDescriptor = { 'PT' : ('PT',0.,20000.,100), 'PTBest' : ('PTBest',0.,20000.,100)}
                                        , OutputSelection = '%Decision'
                                        )       
                               ]
                     , postscale = self.postscale
                     )

            Line( 'ElectronTrackWithIP' 
                 , prescale = self.prescale
                 , L0DU = "L0_CHANNEL('Electron')"
                 , algos = [ Hlt1GEC(),convertL0Candidates('Electron') ] + prepareElectronWithIP + companionTrackWithIP
                         + [ Member ( 'VF', 'VertexCut'
                                    , FilterDescriptor = [ 'VertexPointing_PV3D,<,0.5', 'VertexDz_PV3D,>,0.' ]
                                    )
                           , Member ( 'VU', 'FitTrack', RecoName = 'FitTrack', callback = setupHltFastTrackFit )
                           , Member ( 'VF', 'FitTrack'
                                    , FilterDescriptor =  [ 'FitVertexMinIP_PV3D,||>,'+str(self.getProp('EleIP_IPCut'))]
                                    , OutputSelection = '%Decision'
                                    )
                           ]
                 , postscale = self.postscale
                )

            Line( 'ElectronTrackNoIP' 
                 , prescale = self.prescale
                 , L0DU = "L0_CHANNEL('Electron')"
                 , algos = [ Hlt1GEC(),convertL0Candidates('Electron') ] + prepareElectronNoIP + companionTrackNoIP 
                         + [ DecodeECAL
                           , Member ( 'VU', 'RadCor' , RecoName = 'RadCor', tools = [ Tool( HltTrackUpgradeTool ) ] )
                           , Member ( 'VF', 'VertexPT', FilterDescriptor = [ 'VertexPT,>,'+str(self.getProp('DiEle_PtCut')) ] )
                           , Member ( 'VF', 'MassCut'
                                    , FilterDescriptor = [ 'VertexDiElectronMass,>,%s'%self.getProp('DiEle_LowMassCut') ] + (
                                                         [ 'VertexDiElectronMass,<,%s'% self.getProp('DiEle_HighMassCut') ] if self.getProp('DiEle_HighMassCut') > 0 else [] )
                                    , HistogramUpdatePeriod = 0
                                    , HistoDescriptor = { 'VertexDiElectronMass':('VertexDiElectronMass',0.,3.,100), 'VertexDiElectronMassBest':('VertexDiElectronMassBest',0.,3.,100) }
                                    , OutputSelection = '%Decision'
                                    )
                           ]
                 , postscale = self.postscale
                )
        
