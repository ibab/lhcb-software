# -*- coding: utf-8 -*-
## @file
#
#  Set of Hlt2-lines for:
#   - D(s) -> h h0 with h0= pi0, eta, eta' 
#     where pi0/eta -> e+ e- gamma and eta/eta'-> pi+ pi- gamma
#   - D(s) -> eta(') h,
#     with eta(') -> pi+ pi- X (X = pi0, eta)
#
#  @author Tom HADAVIZADEH tom.hadavizadeh@physics.ox.ac.uk
#  @author Simone STRACKA simone.stracka@cern.ch
#                                                                       
#

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm 
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2HHLines() :
    def localcuts(self) : 
        return { 'D2HPi0' : { 'D_MassWinLow'              : 1500 * MeV,
                              'D_MassWinHigh'             : 2300 * MeV,
                              'D_PT'                      : 2000 * MeV,
                              'Vertex_Chi2'               : 25,                              
                              'Neutral_MassWinLow'        : 70   * MeV, #Neutral = Pi0
                              'Neutral_MassWinHigh'       : 210  * MeV,
                              'Neutral_PT'                : 200  * MeV,                                                            
                              'Daug_PT'                   : 350  * MeV,
                              'Daug_P'                    : 350  * MeV,                                                           
                              'Track_Chi2'                : 5,
                              'Trk_MIPCHI2DV_MIN'         : 6.0,                               
                              'Gamma_PT'                  : 300  * MeV,
                              'Gamma_P'                   : 1000 * MeV},
                              
                 'D2HEta' : { 'D_MassWinLow'              : 1500 * MeV,
                              'D_MassWinHigh'             : 2300 * MeV,
                              'D_PT'                      : 2000 * MeV,
                              'Vertex_Chi2'               : 9,                              
                              'Neutral_MassWinLow'        : 500  * MeV, #Neutral = Eta
                              'Neutral_MassWinHigh'       : 600  * MeV,
                              'Neutral_PT'                : 500  * MeV,                                                            
                              'Daug_PT'                   : 1000 * MeV,
                              'Daug_P'                    : 1000 * MeV,                                                            
                              'Track_Chi2'                : 3,
                              'AMAXDOCA'                  : 0.05  * mm,
                              'Trk_MIPCHI2DV_MIN'         : 6.0,                              
                              'Gamma_PT'                  : 300  * MeV,
                              'Gamma_P'                   : 1500 * MeV},                              
                                             
                 'Dstar2PiD0_eegamma' :{'D_MassWinLow'    : 1500 * MeV,
                              'D_MassWinHigh'             : 2300 * MeV,
                              'D_PT'                      : 2000 * MeV,
                              'Vertex_Chi2'               : 9,                              
                              'Neutral_MassWinLow'        : 1814 * MeV, #Neutral = D0 mass =1864
                              'Neutral_MassWinHigh'       : 1914 * MeV,
                              'Neutral_PT'                : 700  * MeV,                                                            
                              'Daug_PT'                   : 1300 * MeV,                              
                              'Daug_P'                    : 1500 * MeV,                                                            
                              'Track_Chi2'                : 3,
                              'AMAXDOCA'                  : 0.05  * mm,
                              'Trk_MIPCHI2DV_MIN'         : 6.0,                              
                              'Gamma_PT'                  : 600  * MeV,
                              'Gamma_P'                   : 1500 * MeV},
                                                            
                 'Dstar2PiD0_ee':{'D_MassWinLow'         : 1500 * MeV,
                              'D_MassWinHigh'             : 2300 * MeV,
                              'D_PT'                      : 2000 * MeV,
                              'Vertex_Chi2'               : 9,                              
                              'Neutral_MassWinLow'        : 1814 * MeV, #Neutral = D0 mass =1864
                              'Neutral_MassWinHigh'       : 1914 * MeV,
                              'Neutral_PT'                : 700  * MeV,                                                            
                              'Daug_PT'                   : 1300 * MeV,                              
                              'Daug_P'                    : 1500 * MeV,
                              'Elec_PT'                   : 350  * MeV,                              
                              'Elec_P'                    : 500  * MeV,                                                            
                              'Track_Chi2'                : 3,
                              'Trk_MIPCHI2DV_MIN'         : 6.0},                              
                                                          
                 'Conv_Photon' : {'Elec_PT'               : 350  * MeV,
                                  'Trk_MIPCHI2DV_MIN'     : 6.0,
                                  'Track_Chi2'            : 5         }, 
                                  
                 'D2EtaPi_hhgamma' : {
                    'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",            
                    'Trk_PT_MIN'        :  300 * MeV,
                    'Trk_MIPCHI2DV_MIN' :  3,
                    'AM_MIN'            : 1550 * MeV, 
                    'AM_MAX'            : 2550 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2500 * MeV,
                    'APT_MIN'           : 1500.0 * MeV , # 2000 in stripping
                    'BPVLTIME_MIN'      : 0.10 * picosecond,  # ~ 0.15 picosecond in the stripping
                    'VCHI2PDOF_MAX'     : 5,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 800 * MeV, #
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2550 * MeV, #
                    'AMEta_MIN'         : 450 * MeV,
                    'AMEta_MAX'         : 800 * MeV,
                                                    },
                 'D2EtaK_hhgamma' : {
                    'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",            
                    'Trk_PT_MIN'        :  300 * MeV,
                    'Trk_MIPCHI2DV_MIN' :  3,
                    'AM_MIN'            : 1550 * MeV, 
                    'AM_MAX'            : 2550 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2500 * MeV,
                    'APT_MIN'           : 1500.0 * MeV,  # 2000 in stripping
                    'BPVLTIME_MIN'      : 0.10 * picosecond,   # picosecond , ~ 0.15 picosecond in the stripping
                    'VCHI2PDOF_MAX'     : 5,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 800 * MeV, #
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2550 * MeV, #
                    'AMEta_MIN'         : 450 * MeV,
                    'AMEta_MAX'         : 800 * MeV,
                                                    },
                 'D2EtaPrimePi_hhgamma' : {
                    'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",            
                    'Trk_PT_MIN'        :  300 * MeV,
                    'Trk_MIPCHI2DV_MIN' :  3,
                    'AM_MIN'            : 1550 * MeV, 
                    'AM_MAX'            : 2550 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2500 * MeV,
                    'APT_MIN'           : 1500.0 * MeV,  # 2000 in stripping
                    'BPVLTIME_MIN'      : 0.10 * picosecond,   # picosecond , ~ 0.15 picosecond in the stripping
                    'VCHI2PDOF_MAX'     : 5,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 1150 * MeV, #
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2550 * MeV, #
                    'AMEta_MIN'         : 200 * MeV,
                    'AMEta_MAX'         : 1150 * MeV,
                                                    },
                 'D2EtaPrimeK_hhgamma' : {
                    'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",            
                    'Trk_PT_MIN'        :  300 * MeV,
                    'Trk_MIPCHI2DV_MIN' :  3,
                    'AM_MIN'            : 1550 * MeV, 
                    'AM_MAX'            : 2550 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2500 * MeV,
                    'APT_MIN'           : 1500.0 * MeV,  # 2000 in stripping
                    'BPVLTIME_MIN'      : 0.10 * picosecond,   # picosecond , ~ 0.15 picosecond in the stripping
                    'VCHI2PDOF_MAX'     : 5,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 1150 * MeV,
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2550 * MeV, #
                    'AMEta_MIN'         : 800 * MeV,
                    'AMEta_MAX'         : 1150 * MeV,
                                                    },
                 'D2EtaPi_3hR' : {
                    'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",            
                    'Trk_PT_MIN'        :  300 * MeV,
                    'Trk_MIPCHI2DV_MIN' :  3,
                    'AM_MIN'            : 1550 * MeV,
                    'AM_MAX'            : 2550 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2500 * MeV,
                    'APT_MIN'           : 1500.0 * MeV,
                    'BPVLTIME_MIN'      : 0.10 * picosecond,   # picosecond
                    'VCHI2PDOF_MAX'     : 5,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 1050 * MeV, #
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2550 * MeV,
                    'AMEta_MIN'         : 450 * MeV,
                    'AMEta_MAX'         : 1150 * MeV,
                    },
                 'D2EtaK_3hR' : {
                    'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",            
                    'Trk_PT_MIN'        :  300 * MeV,
                    'Trk_MIPCHI2DV_MIN' :  3,
                    'AM_MIN'            : 1550 * MeV,
                    'AM_MAX'            : 2550 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2500 * MeV,
                    'APT_MIN'           : 1500.0 * MeV,
                    'BPVLTIME_MIN'      : 0.10 * picosecond,   # picosecond
                    'VCHI2PDOF_MAX'     : 5,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 700 * MeV, #
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2550 * MeV,
                    'AMEta_MIN'         : 450 * MeV,
                    'AMEta_MAX'         : 700 * MeV,
                    },
                 'D2EtaPi_3hM' : {
                    'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",            
                    'Trk_PT_MIN'        :  300 * MeV,
                    'Trk_MIPCHI2DV_MIN' :  3,
                    'AM_MIN'            : 1550 * MeV,
                    'AM_MAX'            : 2550 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2500 * MeV,
                    'APT_MIN'           : 1500.0 * MeV,
                    'BPVLTIME_MIN'      : 0.10 * picosecond,   # picosecond
                    'VCHI2PDOF_MAX'     : 5,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 1050 * MeV, #
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2550 * MeV,
                    'AMEta_MIN'         : 450 * MeV,
                    'AMEta_MAX'         : 1150 * MeV,
                    },
                 'D2EtaK_3hM' : {
                    'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",            
                    'Trk_PT_MIN'        :  300 * MeV,
                    'Trk_MIPCHI2DV_MIN' :  3,
                    'AM_MIN'            : 1550 * MeV,
                    'AM_MAX'            : 2550 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2500 * MeV,
                    'APT_MIN'           : 1500.0 * MeV,
                    'BPVLTIME_MIN'      : 0.10 * picosecond,   # picosecond
                    'VCHI2PDOF_MAX'     : 5,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 700 * MeV, #
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2550 * MeV,
                    'AMEta_MIN'         : 450 * MeV,
                    'AMEta_MAX'         : 700 * MeV,
                    },
                 'D2EtaPrimePi_3h' : {
                    'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",            
                    'Trk_PT_MIN'        :  300 * MeV,
                    'Trk_MIPCHI2DV_MIN' :  3,
                    'AM_MIN'            : 1550 * MeV,
                    'AM_MAX'            : 2550 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2500 * MeV,
                    'APT_MIN'           : 1500.0 * MeV,
                    'BPVLTIME_MIN'      : 0.10 * picosecond,   # picosecond
                    'VCHI2PDOF_MAX'     : 5,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 650 * MeV, #
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2550 * MeV,
                    'AMEta_MIN'         : 750 * MeV,
                    'AMEta_MAX'         : 1150 * MeV,
                    },
                 'D2EtaPrimeK_3h' : {
                    'TisTosSpec'        : "Hlt1TrackAllL0Decision%TOS",         #'TisTosSpec'               : "Hlt1.*Decision%TOS"
                    'Trk_PT_MIN'        :  300 * MeV,
                    'Trk_MIPCHI2DV_MIN' :  3,
                    'AM_MIN'            : 1550 * MeV,
                    'AM_MAX'            : 2550 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2500 * MeV,
                    'APT_MIN'           : 1500.0 * MeV,
                    'BPVLTIME_MIN'      : 0.10 * picosecond,   # picosecond 
                    'VCHI2PDOF_MAX'     : 5,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 650 * MeV, #
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2550 * MeV,
                    'AMEta_MIN'         : 750 * MeV,
                    'AMEta_MAX'         : 1150 * MeV,
                    },
                 'DiPion_forD2HH' : {
                    'Trk_PT_MIN'               :  300 *  MeV, #350 * MeV,
                    'Trk_MIPCHI2DV_MIN'        :  3,
                    'AM_MAX'                   : 1150 * MeV,     
                    'ASUMPT_MIN'               :  0.0 * MeV,
                    'ADOCA_MAX'                :  0.1 * mm,
                    'VCHI2_MAX'                :  20.0,
                    'BPVVD_MIN'                :  0.0, #
                    'BPVCORRM_MAX'             : 3.5 * GeV, #
                    'BPVVDCHI2_MIN'            :  0.0,
                    },                                                                 
                 }
    
    def locallines(self):
        from Stages import MassFilter
        from Stages import D2PiH0_eeg, D2KH0_eeg, DStar2PiD0_eeg, DStar2PiD0_ee 
        from Stages import D2RhoHG_3Body_Combiner
        from Stages import D2HH_RhoToPipPim
        from Stages import SharedNoPIDDetachedChild_pi
        from Stages import SharedNoPIDDetachedChild_K
        from Stages import SharedNeutralLowPtChild_gamma
        from Stages import SharedNeutralLowPtChild_pi0R
        from Stages import SharedNeutralLowPtChild_pi0M
        from Stages import SharedNeutralLowPtChild_eta
            
        stages = {'D2PiPi0_eegammaTurbo'           : [ D2PiH0_eeg('D2HPi0')    ],
                  'D2KPi0_eegammaTurbo'            : [ D2KH0_eeg('D2HPi0')     ],
                  'D2PiEta_eegammaTurbo'           : [ D2PiH0_eeg('D2HEta')    ],
                  'D2KEta_eegammaTurbo'            : [ D2KH0_eeg('D2HEta')     ],
                  
                  'Dstar2PiD0_eegammaTurbo'        : [ DStar2PiD0_eeg('Dstar2PiD0_eegamma')],
                  'Dstar2PiD0_eeTurbo'             : [ DStar2PiD0_ee('Dstar2PiD0_ee') ],
                  
                  'D2EtaPi_hhgammaTurbo'           : [D2RhoHG_3Body_Combiner('D2EtaPi_hhgamma',
                                                                        decay = ["D+ -> rho(770)0 pi+ gamma","D- -> rho(770)0 pi- gamma"],
                                                                        inputs = [D2HH_RhoToPipPim, 
                                                                                  SharedNoPIDDetachedChild_pi,
                                                                                  SharedNeutralLowPtChild_gamma ])], 
                  'D2EtaK_hhgammaTurbo'            : [D2RhoHG_3Body_Combiner('D2EtaK_hhgamma',
                                                                        decay = ["D+ -> rho(770)0 K+ gamma","D- -> rho(770)0 K- gamma"],
                                                                        inputs = [D2HH_RhoToPipPim, 
                                                                                  SharedNoPIDDetachedChild_K,
                                                                                  SharedNeutralLowPtChild_gamma ])],                  
                  'D2EtaPrimePi_hhgammaTurbo'      : [D2RhoHG_3Body_Combiner('D2EtaPrimePi_hhgamma',
                                                                        decay = ["D+ -> rho(770)0 pi+ gamma","D- -> rho(770)0 pi- gamma"],
                                                                        inputs = [D2HH_RhoToPipPim, 
                                                                                  SharedNoPIDDetachedChild_pi,
                                                                                  SharedNeutralLowPtChild_gamma ])],
                  
                  'D2EtaPrimeK_hhgammaTurbo'       : [D2RhoHG_3Body_Combiner('D2EtaPrimeK_hhgamma',
                                                                        decay = ["D+ -> rho(770)0 K+ gamma","D- -> rho(770)0 K- gamma"],
                                                                        inputs = [D2HH_RhoToPipPim, 
                                                                                  SharedNoPIDDetachedChild_K,
                                                                                  SharedNeutralLowPtChild_gamma ])],
                  'D2EtaPi_3hRTurbo'                : [D2RhoHG_3Body_Combiner('D2EtaPi_3hR',
                                                                        decay = ["D+ -> rho(770)0 pi+ pi0","D- -> rho(770)0 pi- pi0"],
                                                                        inputs = [D2HH_RhoToPipPim, 
                                                                                  SharedNoPIDDetachedChild_pi,
                                                                                  SharedNeutralLowPtChild_pi0R ])],
                  'D2EtaK_3hRTurbo'                 : [D2RhoHG_3Body_Combiner('D2EtaK_3hR',
                                                                        decay = ["D+ -> rho(770)0 K+ pi0","D- -> rho(770)0 K- pi0"],
                                                                        inputs = [D2HH_RhoToPipPim, 
                                                                                  SharedNoPIDDetachedChild_K,
                                                                                  SharedNeutralLowPtChild_pi0R ])],
                  'D2EtaPi_3hMTurbo'                : [D2RhoHG_3Body_Combiner('D2EtaPi_3hM',
                                                                        decay = ["D+ -> rho(770)0 pi+ pi0","D- -> rho(770)0 pi- pi0"],
                                                                        inputs = [D2HH_RhoToPipPim, 
                                                                                  SharedNoPIDDetachedChild_pi,
                                                                                  SharedNeutralLowPtChild_pi0M ])],
                  'D2EtaK_3hMTurbo'                 : [D2RhoHG_3Body_Combiner('D2EtaK_3hM',
                                                                        decay = ["D+ -> rho(770)0 K+ pi0","D- -> rho(770)0 K- pi0"],
                                                                        inputs = [D2HH_RhoToPipPim, 
                                                                                  SharedNoPIDDetachedChild_K,
                                                                                  SharedNeutralLowPtChild_pi0M ])],
                  'D2EtaPrimePi_3hTurbo'           : [D2RhoHG_3Body_Combiner('D2EtaPrimePi_3h',
                                                                        decay = ["D+ -> rho(770)0 pi+ eta","D- -> rho(770)0 pi- eta"],
                                                                        inputs = [D2HH_RhoToPipPim, 
                                                                                  SharedNoPIDDetachedChild_pi,
                                                                                  SharedNeutralLowPtChild_eta ])],
                  'D2EtaPrimeK_3hTurbo'            : [D2RhoHG_3Body_Combiner('D2EtaPrimeK_3h',
                                                                        decay = ["D+ -> rho(770)0 K+ eta","D- -> rho(770)0 K- eta"],
                                                                        inputs = [D2HH_RhoToPipPim, 
                                                                                  SharedNoPIDDetachedChild_K,
                                                                                  SharedNeutralLowPtChild_eta ])],
                  }
        return stages


