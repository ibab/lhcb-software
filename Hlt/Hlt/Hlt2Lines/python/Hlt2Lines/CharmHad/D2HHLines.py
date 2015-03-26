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
                              
                 'D2HEtaPrime':{'D_MassWinLow'            : 1500 * MeV,
                              'D_MassWinHigh'             : 2300 * MeV,
                              'D_PT'                      : 2000 * MeV,
                              'Vertex_Chi2'               : 9,                              
                              'Neutral_MassWinLow'        : 900  * MeV, #Neutral = Eta_prime
                              'Neutral_MassWinHigh'       : 990  * MeV,
                              'Neutral_PT'                : 900  * MeV,                                                            
                              'Daug_PT'                   : 1300 * MeV,                              
                              'Daug_P'                    : 1500 * MeV,                                                            
                              'Track_Chi2'                : 3,
                              'AMAXDOCA'                  : 0.05  * mm,
                              'Trk_MIPCHI2DV_MIN'         : 6.0,                              
                              'Gamma_PT'                  : 600  * MeV,
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
                }
    
    def locallines(self):
        from Stages import MassFilter
        from Stages import D2PiH0_eeg, D2KH0_eeg, D2PiH0_pipig, D2KH0_pipig, DStar2PiD0_eeg, DStar2PiD0_ee 
        stages = {'D2PiPi0_eegamma'           : [ D2PiH0_eeg('D2HPi0')    ],
                  'D2KPi0_eegamma'            : [ D2KH0_eeg('D2HPi0')     ],
                  'D2PiEta_eegamma'           : [ D2PiH0_eeg('D2HEta')    ],
                  'D2KEta_eegamma'            : [ D2KH0_eeg('D2HEta')     ],
                  'D2PiEta_pipigamma'         : [ D2PiH0_pipig('D2HEta')  ],
                  'D2KEta_pipigamma'          : [ D2KH0_pipig('D2HEta')   ],
                  'D2PiEtaPrime_pipigamma'    : [ D2PiH0_pipig('D2HEtaPrime')  ],
                  'D2KEtaPrime_pipigamma'     : [ D2KH0_pipig('D2HEtaPrime')   ],
                  
                  'Dstar2PiD0_eegamma'        : [ DStar2PiD0_eeg('Dstar2PiD0_eegamma')],
                  'Dstar2PiD0_ee'             : [ DStar2PiD0_ee('Dstar2PiD0_ee') ]                                                
                 }
        return stages
