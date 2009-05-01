
__all__ = ( 'Miriam_20090430', 'FEST', 'SettingsForDataType' )

def SettingsForDataType( x ) :
    _dataType2Settings = { '2009' : Miriam_20090430 }
    return _dataType2Settings[x] if x in _dataType2Settings else None



Miriam_20090430 = { 'HA' : { 'SingleHadron_PTCut' : 4450
                           , 'HadMain_IPCut'      : 0.09
                           , 'HadMain_PTCut'      : 1600
                           , 'HadCompanion_DZCut' : 0.05
                           , 'HadCompanion_IPCut' : 0.13
                           , 'HadCompanion_PTCut' : 950
                           , 'HadCompanion_PointingCut' : 1.0 
                           }
                  , 'PH' : { 'Pho_EtCut'          : 2800.0
                           , 'Track_PtCut'        : 650
                           , 'Track_IPCut3D'      : 0.15
                           , 'Pho_IsPho'          : -0.082 
                           }
                  , 'EL' : { 'SingleEle_PtCut'    : 2820.0
                           , 'Compan_PtCut'       : 2750
                           , 'Ele_IPCut'          : 0.13 
                           }
                  , 'MU' : { 'DiMuon_SumPtCut'    : 1480.0
                           , 'Muon_PtCut'         : 6200
                           , 'MuonIP_PtCut'       : 1800
                           , 'Muon_IPMinCut'      : 0.07
                           , 'DiMuon_DOCACut'     : 1.6
                           , 'DiMuon_MassCut'     : 2300
                           , 'DiMuonIP_MassCut'   : 300
                           , 'DiMuon_IPCut'       : 0.08
                           , 'Prescale' : { 'Hlt1MuTrack4JPsi' : 0. }
                           , 'MuTrackMuPt'        : 950
                           , 'MuTrackMuIP'        : 0.07
                           , 'MuTrackTrPt'        : 550
                           , 'MuTrackTrIP'        : 0.1
                           , 'MuTrackDoca'        : 0.2
                           , 'MuTrackDZ'          : 2.0
                           , 'MuTrackDimuMass'    : 1000
                           , 'MuTrackPoint'       : 0.4
                           , 'MuTrackTrChi2'      : 10 
                           }
                  }

from copy import deepcopy
FEST = deepcopy(Miriam_20090430)
FEST.update(  { 'VE' : { 'Prescale' : { '.*'            : 1 } }
              , 'XP' : { 'Prescale' : { 'Hlt1VeloAlign' : 1 } }
              , 'PA' : { 'Prescale' : { 'Hlt1Physics'   : 1 
                                      , 'Hlt1Random'    : 1 # @OnlineEnv.AcceptRate
                                      }
                       }
              }
           )

