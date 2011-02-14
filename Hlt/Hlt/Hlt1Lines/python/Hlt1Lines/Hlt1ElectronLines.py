
from Gaudi.Configuration import * 


from HltLine.HltLinesConfigurableUser import *


class Hlt1ElectronLinesConf(HltLinesConfigurableUser) :

    # steering variables
    __slots__ = { 'Ele_EtCut'              :  2600.    # L0 ET cut
                , 'Ele_EtCutSingleNoIP'    :  5000.
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
        from HltLine.HltDecodeRaw import DecodeIT, DecodeTT, DecodeVELO, DecodeECAL
        from HltTracking.Hlt1TrackUpgradeConf import Forward, FitTrack, RadCor
        from HltTracking.Hlt1TrackMatchConf import MatchCallback
              
