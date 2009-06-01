
from Gaudi.Configuration import * 


from HltLine.HltLinesConfigurableUser import *
from HltLine.HltLine import Hlt1Line   as Line
from HltLine.HltLine import Hlt1Member as Member
from HltLine.HltLine import bindMembers
from HltLine.HltLine import Hlt1Tool   as Tool
from Hlt1Lines.HltL0Candidates import *


class HltElectronLinesConf(HltLinesConfigurableUser) :

    # steering variables
    __slots__ = { 'Prescale'             : {}
                , 'Postscale'            : {}
                , 'Compan_PtCut'         :  1000.    # for global optimization 1
                , 'Ele_IPCut'            :  0.1      # for global optimization 2
                , 'SingleEle_PtCut'      :  3000.    # for global optimization 3 optional
                , 'Jpsi_LowMassCut'      :  2400.
                , 'Jpsi_HighMassCut'     :  3200.
                , 'Ele_EtCut'            :  2600.
                }

    def __apply_configuration__(self) :
        from Configurables import HltTrackUpgradeTool
        from Configurables import L0ConfirmWithT
        from HltConf.HltReco import RZVelo, Velo, PV2D
        
        COMPAN_PTCUT    = str(self.getProp('Compan_PtCut'))
        SINGLEELE_PTCUT = str(self.getProp('SingleEle_PtCut'))
        LOWMASS         = str(self.getProp('Jpsi_LowMassCut'))
        HIGHMASS        = str(self.getProp('Jpsi_HighMassCut'))
        L0ET_CUT        = str(self.getProp('Ele_EtCut'))

        ##### common bodies IP
        IP_CUT = str(self.getProp('Ele_IPCut'))
        prepareElectronWithIP = [ Member ( 'TF', 'L0Electrons', FilterDescriptor = [ 'L0ET,>,'+L0ET_CUT ] )
                                , Member ( 'TU', 'TConf', RecoName = 'TConf',
                                              tools = [ Tool( HltTrackUpgradeTool,
                                              tools = [ Tool( L0ConfirmWithT,  particleType = 2 ) ] ) ] )
                                , RZVelo, PV2D.ignoreOutputSelection()
                                , Member ( 'TF', 'RZVelo', FilterDescriptor = [ 'RZVeloTMatch_%TUTConf,||<,60.' ] )
                                , Member ( 'TU', 'Velo', RecoName = 'Velo' )
                                , Member ( 'TM', 'VeloT', InputSelection1 = '%TUVelo' , InputSelection2 = '%TUTConf' , MatchName = 'VeloT')
                                , Member ( 'TF', 'VeloT', FilterDescriptor = [ 'IP_PV2D,||[],'+IP_CUT+',3.' ])
                                ]

        companionTrackWithIP  = [ Velo
                                , Member ( 'TF', 'CompanionVelo',
                                           FilterDescriptor = [ 'IP_PV2D,||[],'+IP_CUT+',3.', 'DOCA_%TFVeloT,<,0.2' ])
                                , Member ( 'TU', 'CompanionForward', RecoName = 'Forward' )
                                , Member ( 'TF', 'CompanionForward',  FilterDescriptor = ['PT,>,'+COMPAN_PTCUT])
                                , Member ( 'VM2', 'DiElectron', InputSelection1 = '%TFVeloT', InputSelection2 = '%TFCompanionForward',
                                           FilterDescriptor = ['DOCA,<,0.2'])
                                ]

        ##### common bodies no IP
        IP_CUT = "0.0"
        prepareElectronNoIP   = [ Member ( 'TF', 'L0Electrons', FilterDescriptor = [ 'L0ET,>,'+L0ET_CUT ] )
                                , Member ( 'TU', 'TConf', RecoName = 'TConf',
                                              tools = [ Tool( HltTrackUpgradeTool,
                                              tools = [ Tool( L0ConfirmWithT,  particleType = 2 ) ] ) ] )
                                , RZVelo, PV2D.ignoreOutputSelection()
                                , Member ( 'TF', 'RZVelo', FilterDescriptor = [ 'RZVeloTMatch_%TUTConf,||<,60.' ] )
                                , Member ( 'TU', 'Velo', RecoName = 'Velo' )
                                , Member ( 'TM', 'VeloT', InputSelection1 = '%TUVelo' , InputSelection2 = '%TUTConf' , MatchName = 'VeloT')
                                , Member ( 'TF', 'VeloT', FilterDescriptor = [ 'IP_PV2D,||[],'+IP_CUT+',3.' ])
                                ]

        companionTrackNoIP    = [ Velo
                                , Member ( 'TF', 'CompanionVelo', 
                                           FilterDescriptor = [ 'IP_PV2D,||[],'+IP_CUT+',3.', 'DOCA_%TFVeloT,<,0.2' ])
                                , Member ( 'TU', 'CompanionForward', RecoName = 'Forward' )
                                , Member ( 'TF', 'CompanionForward',  FilterDescriptor = ['PT,>,'+COMPAN_PTCUT])
                                , Member ( 'VM2', 'DiElectron', InputSelection1 = '%TFVeloT', InputSelection2 = '%TFCompanionForward',
                                           FilterDescriptor = ['DOCA,<,0.2'])
                                ]

        ##### Lines
        Line ('SingleElectron'
                 , prescale = self.prescale
                 , L0DU = "L0_CHANNEL('Electron')"
                 , algos = [ convertL0Candidates('Electron') ] + prepareElectronWithIP
                         + [ Member ( 'TF','Decision'
                                    , OutputSelection = '%Decision'
                                    , FilterDescriptor = ['PT,>,'+SINGLEELE_PTCUT]
                                    , HistogramUpdatePeriod = 0
                                    , HistoDescriptor = { 'PT' : ('PT',0.,8000.,100), 'PTBest' : ('PTBest',0.,8000.,100)}
                                    )
                           ]
                 , postscale = self.postscale
                 )

        Line ('SingleElectronFromPi0'
             , prescale = self.prescale
             , L0DU = "L0_CHANNEL('LocalPi0')"
             , algos = [ convertL0Candidates('LocalPi0') ] + prepareElectronWithIP
                     + [ Member ( 'TF','Decision'
                                , OutputSelection = '%Decision'
                                , FilterDescriptor = ['PT,>,'+SINGLEELE_PTCUT]
                                , HistogramUpdatePeriod = 0
                                , HistoDescriptor = { 'PT' : ('PT',0.,8000.,100), 'PTBest' : ('PTBest',0.,8000.,100)}
                                )
                       ]
             , postscale = self.postscale
             )

        Line( 'ElectronTrackWithIP' 
             , prescale = self.prescale
             , L0DU = "L0_CHANNEL('Electron')"
             , algos = [ convertL0Candidates('Electron') ] + prepareElectronWithIP + companionTrackWithIP
                     + [ Member ( 'VF', 'VertexCut'
                                , FilterDescriptor = [ 'VertexPointing_PV2D,<,0.5', 'VertexDz_PV2D,>,0.' ]
                                , OutputSelection = '%Decision'
                                )
                       ]
             , postscale = self.postscale
            )

        Line( 'ElectronFromPi0TrackWithIP' 
             , prescale = self.prescale
             , L0DU = "L0_CHANNEL('LocalPi0')"
             , algos = [ convertL0Candidates('LocalPi0') ] + prepareElectronWithIP + companionTrackWithIP
                     + [ Member ( 'VF', 'VertexCut'
                                , FilterDescriptor = [ 'VertexPointing_PV2D,<,0.5', 'VertexDz_PV2D,>,0.' ]
                                , OutputSelection = '%Decision'
                                )
                       ]
             , postscale = self.postscale
            )

        Line( 'ElectronTrackNoIP' 
             , prescale = self.prescale
             , L0DU = "L0_CHANNEL('Electron')"
             , algos = [ convertL0Candidates('Electron') ] + prepareElectronNoIP + companionTrackNoIP 
                     + [ Member ( 'VF', 'VertexCut'
                                , FilterDescriptor = [ 'VertexPointing_PV2D,<,0.5', 'VertexDz_PV2D,>,0.' ]
                                )
                       , Member ( 'VF', 'MassCut'
                                , FilterDescriptor = [ 'VertexDiElectronMass,[],'+LOWMASS+','+HIGHMASS ]
                                , OutputSelection = '%Decision'
                                )
                       ]
             , postscale = self.postscale
            )
        
        Line( 'ElectronFromPi0TrackNoIP' 
             , prescale = self.prescale
             , L0DU = "L0_CHANNEL('LocalPi0')"
             , algos = [ convertL0Candidates('LocalPi0') ] + prepareElectronNoIP + companionTrackNoIP 
                     + [ Member ( 'VF', 'VertexCut'
                                , FilterDescriptor = [ 'VertexPointing_PV2D,<,0.5', 'VertexDz_PV2D,>,0.' ]
                                )
                       , Member ( 'VF', 'MassCut'
                                , FilterDescriptor = [ 'VertexDiElectronMass,[],'+LOWMASS+','+HIGHMASS ]
                                , OutputSelection = '%Decision'
                                )
                       ]
             , postscale = self.postscale
            )
