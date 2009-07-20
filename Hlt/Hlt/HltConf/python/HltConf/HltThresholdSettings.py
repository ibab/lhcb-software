__all__ = ( 'Miriam_20090430', 'FEST', 'SettingsForDataType' )

def SettingsForDataType( x ) :
    _dataType2Settings = { 'DC06' : Miriam_20090430 ,
                           'MC09' : Miriam_20090430 }  # temporary
    return _dataType2Settings[x] if x in _dataType2Settings else None

#########################################################################################
# HLT1
#########################################################################################
#
# Miriam's Hlt1 bandwidth division for 2*10^32 @ 40 MHz, done on DC06 MC
# @author miriam.gandelman@cern.ch
#
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
                           , 'DiMuon_MassCut'     : 2500
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
#
# FEST settings Gerhard.Raven@nikhef.nl
# @author Gerhard.Raven@nikhef.nl
#
FEST = {}
FEST.update(  { 'VE' : { 'Prescale' : { '.*'            : 1 } }
              , 'XP' : { 'Prescale' : { 'Hlt1XPress'    : 0.0025
                                      , 'Hlt1VeloAlign' : 1 } 
                                      }
              , 'PA' : { 'Prescale' : { 'Hlt1Physics'    : 1 
                                      , 'Hlt1Random'     : 1 # @OnlineEnv.AcceptRate
                                      , 'Hlt1Tell1Error' : 0
                                      }
                       }
              }
           )

#########################################################################################
# HLT2 for 2*10^32 @ 40 MHz, done on DC06 MC
#
# @author Compiled by Patrick.Koppenburg@cern.ch
#
# TOPO cuts by <p.spradlin1@physics.ox.ac.uk>
# LEPT cuts by
#
####
# Leptonic scenario
#
Hlt2_Leptonic = { 'TOPO' : { 'ComRobAllTrkPtLL' : 800.0
                           , 'ComRobAllTrkPVIPLL' : 0.025
                           , 'ComRobPairMinDocaUL' : 0.1
                           , 'ComRobTrkMaxPtLL' : 1500.0
                           , 'ComTFVtxPVDispChi2LL' : 169.0
                           , 'RobustPointingUL' : 0.12
                           , 'TFPointUL' : 0.2
                           , 'CharmRobustPointUL' : 0.04
                           , 'CharmTFPointUL' : 0.04
                           , 'Prescale["Hlt2TopoTF4BodyCharmSignal"]' : 0.0
                           , 'Prescale["Hlt2TopoTF4BodyReq4Yes"]' : 0.2
                           , 'Prescale["Hlt2TopoTF4BodyReq3Yes"]' : 0.2
                           , 'Prescale["Hlt2TopoTF4BodyReq2Yes"]' : 0.2
                           }
                , 'LEPT' : { }
                  }
Hlt2_Leptonic.update(Miriam_20090430)
####
# hadronic scenario
#
Hlt2_Hadronic = { 'TOPO' : { 'ComRobAllTrkPtLL' : 400.0
                           , 'ComRobAllTrkPVIPLL' : 0.025
                           , 'ComRobPairMinDocaUL' : 0.15
                           , 'ComRobTrkMaxPtLL' : 1500.0
                           , 'ComTFVtxPVDispChi2LL' : 196.0
                           , 'RobustPointingUL' : 0.28
                           , 'TFPointUL' : 0.16
                           , 'CharmRobustPointUL' : 0.04
                           , 'CharmTFPointUL' : 0.04
                           , 'Prescale["Hlt2TopoTF4BodyCharmSignal"]' : 0.2
                           , 'Prescale["Hlt2TopoTF4BodyReq4Yes"]' : 0.2
                           , 'Prescale["Hlt2TopoTF4BodyReq3Yes"]' : 0.2
                           , 'Prescale["Hlt2TopoTF4BodyReq2Yes"]' : 0.2
                           }
                , 'LEPT' : { }
                  }
Hlt2_Hadronic.update(Miriam_20090430)
####
# charming scenario
#
Hlt2_Charming = { 'TOPO' : { 'ComRobAllTrkPtLL' : 400.0
                           , 'ComRobAllTrkPVIPLL' : 0.025
                           , 'ComRobPairMinDocaUL' : 0.15
                           , 'ComRobTrkMaxPtLL' : 500.0
                           , 'ComTFVtxPVDispChi2LL' : 196.0
                           , 'RobustPointingUL' : 0.24
                           , 'TFPointUL' : 0.08
                           , 'CharmRobustPointUL' : 0.12
                           , 'CharmTFPointUL' : 0.12
                           , 'Prescale["Hlt2TopoTF4BodyCharmSignal"]' : 0.4
                           , 'Prescale["Hlt2TopoTF4BodyReq4Yes"]' : 0.0
                           , 'Prescale["Hlt2TopoTF4BodyReq3Yes"]' : 0.0
                           , 'Prescale["Hlt2TopoTF4BodyReq2Yes"]' : 0.0
                           }
                , 'LEPT' : { }
                  }
Hlt2_Charming.update(Miriam_20090430)
####
# Optimised scenario
# @author miriam.gandelman@cern.ch
#
Hlt2_Optimised = { 'TOPO' : { },
                   'LEPT' : {} }
Hlt2_Optimised.update(Miriam_20090430)
