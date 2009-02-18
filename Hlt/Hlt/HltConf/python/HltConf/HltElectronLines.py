#!/usr/bin/env gaudirun.py

from Gaudi.Configuration import * 
from LHCbKernel.Configuration import *

from Configurables import GaudiSequencer
from Configurables import HltTrackUpgrade
from Configurables import HltTrackUpgradeTool
from Configurables import PatMatchTool
from Configurables import L0ConfirmWithT
from Configurables import ElectronSeedTool, PatConfirmTool

from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member
from HltConf.HltLine import bindMembers
from HltConf.HltLine import Hlt1Tool   as Tool
from HltConf.HltL0Candidates import *


class HltElectronLinesConf(LHCbConfigurableUser) :

    # steering variables
    __slots__ = { 'Compan_PtCut'         :  1000.    # for global optimization 1
                , 'Ele_IPCut'            :  0.1      # for global optimization 2
                , 'SingleEle_PtCut'      :  3000.
                , 'Jpsi_LowMassCut'      :  2400.
                , 'Jpsi_HighMassCut'     :  3200.
                }

    def __apply_configuration__(self) :
        
        RZVelo = GaudiSequencer('Hlt1RecoRZVeloSequence')
        RecoVelo = HltTrackUpgrade('Hlt1RecoVelo')

        COMPAN_PTCUT    = str(self.getProp('Compan_PtCut'))
        SINGLEELE_PTCUT = str(self.getProp('SingleEle_PtCut'))
        LOWMASS         = str(self.getProp('Jpsi_LowMassCut'))
        HIGHMASS        = str(self.getProp('Jpsi_HighMassCut'))

        ##### common bodies IP
        
        IP_CUT = str(self.getProp('Ele_IPCut'))
        prepareElectronWithIP = [ Member ( 'TF', 'L0Electrons', FilterDescriptor = [ 'L0ET,>,2600.' ] )
                                , Member ( 'TU', 'TConf', RecoName = 'TConf',
                                              tools = [ Tool( HltTrackUpgradeTool,
                                              tools = [ Tool( L0ConfirmWithT,  particleType = 2 ) ] ) ] )
                                , RZVelo
                                , Member ( 'TF', 'RZVelo', InputSelection = 'RZVelo', FilterDescriptor = [ 'RZVeloTMatch_%TUTConf,||<,60.' ] )
                                , Member ( 'TU', 'Velo', RecoName = 'Velo' )
                                , Member ( 'TM', 'VeloT', InputSelection1 = '%TUVelo' , InputSelection2 = '%TUTConf' , MatchName = 'VeloT')
                                , Member ( 'TF', 'VeloT', FilterDescriptor = [ 'IP_PV2D,||[],'+IP_CUT+',3.' ])
                                ]

        companionTrackWithIP  = [ RecoVelo
                                , Member ( 'TF', 'CompanionVelo', InputSelection    = 'Velo',
                                           FilterDescriptor = [ 'IP_PV2D,||[],'+IP_CUT+',3.', 'DOCA_%TFVeloT,<,0.2' ])
                                , Member ( 'TU', 'CompanionForward', RecoName = 'Forward' )
                                , Member ( 'TF', 'CompanionForward',  FilterDescriptor = ['PT,>,'+COMPAN_PTCUT])
                                , Member ( 'VM2', 'DiElectron', InputSelection1 = '%TFVeloT', InputSelection2 = '%TFCompanionForward',
                                           FilterDescriptor = ['DOCA,<,0.2'])
                                ]

        ##### common bodies no IP

        IP_CUT = "0.0"
        prepareElectronNoIP   = [ Member ( 'TF', 'L0Electrons', FilterDescriptor = [ 'L0ET,>,2600.' ] )
                                , Member ( 'TU', 'TConf', RecoName = 'TConf',
                                              tools = [ Tool( HltTrackUpgradeTool,
                                              tools = [ Tool( L0ConfirmWithT,  particleType = 2 ) ] ) ] )
                                , RZVelo
                                , Member ( 'TF', 'RZVelo', InputSelection = 'RZVelo', FilterDescriptor = [ 'RZVeloTMatch_%TUTConf,||<,60.' ] )
                                , Member ( 'TU', 'Velo', RecoName = 'Velo' )
                                , Member ( 'TM', 'VeloT', InputSelection1 = '%TUVelo' , InputSelection2 = '%TUTConf' , MatchName = 'VeloT')
                                , Member ( 'TF', 'VeloT', FilterDescriptor = [ 'IP_PV2D,||[],'+IP_CUT+',3.' ])
                                ]

        companionTrackNoIP    = [ RecoVelo
                                , Member ( 'TF', 'CompanionVelo', InputSelection    = 'Velo',
                                           FilterDescriptor = [ 'IP_PV2D,||[],'+IP_CUT+',3.', 'DOCA_%TFVeloT,<,0.2' ])
                                , Member ( 'TU', 'CompanionForward', RecoName = 'Forward' )
                                , Member ( 'TF', 'CompanionForward',  FilterDescriptor = ['PT,>,'+COMPAN_PTCUT])
                                , Member ( 'VM2', 'DiElectron', InputSelection1 = '%TFVeloT', InputSelection2 = '%TFCompanionForward',
                                           FilterDescriptor = ['DOCA,<,0.2'])
                                ]

        ##### Lines
        
        Line ('SingleElectron'
             , L0DU = "L0_CHANNEL('Electron')"
             , algos = [ convertL0Candidates('Electron') ] + prepareElectronWithIP
                     + [ Member ( 'TF','Decision'
                                , OutputSelection = '%Decision'
                                , FilterDescriptor = ['PT,>,'+SINGLEELE_PTCUT]
                                , HistogramUpdatePeriod = 0
                                , HistoDescriptor = { 'PT' : ('PT',0.,8000.,100), 'PTBest' : ('PTBest',0.,8000.,100)}
                                )
                       ]
             )

        Line ('SingleElectronFromPi0'
             , L0DU = "L0_CHANNEL('LocalPi0')"
             , algos = [ convertL0Candidates('LocalPi0') ] + prepareElectronWithIP
                     + [ Member ( 'TF','Decision'
                                , OutputSelection = '%Decision'
                                , FilterDescriptor = ['PT,>,'+SINGLEELE_PTCUT]
                                , HistogramUpdatePeriod = 0
                                , HistoDescriptor = { 'PT' : ('PT',0.,8000.,100), 'PTBest' : ('PTBest',0.,8000.,100)}
                                )
                       ]
             )

        Line( 'ElectronTrackWithIP' 
             , L0DU = "L0_CHANNEL('Electron')"
             , algos = [ convertL0Candidates('Electron') ] + prepareElectronWithIP + companionTrackWithIP
                     + [ Member ( 'VF', 'VertexCut'
                                , FilterDescriptor = [ 'VertexPointing_PV2D,<,0.5', 'VertexDz_PV2D,>,0.' ]
                                , OutputSelection = '%Decision'
                                )
                       ]
            )

        Line( 'ElectronFromPi0TrackWithIP' 
             , L0DU = "L0_CHANNEL('LocalPi0')"
             , algos = [ convertL0Candidates('LocalPi0') ] + prepareElectronWithIP + companionTrackWithIP
                     + [ Member ( 'VF', 'VertexCut'
                                , FilterDescriptor = [ 'VertexPointing_PV2D,<,0.5', 'VertexDz_PV2D,>,0.' ]
                                , OutputSelection = '%Decision'
                                )
                       ]
            )

        Line( 'ElectronTrackNoIP' 
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
            )
        
        Line( 'ElectronFromPi0TrackNoIP' 
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
            )
