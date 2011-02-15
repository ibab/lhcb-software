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
        pass
