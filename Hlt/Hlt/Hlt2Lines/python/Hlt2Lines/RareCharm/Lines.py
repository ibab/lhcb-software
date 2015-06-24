##
#  @author F. Dettori Francesco.Dettori@cern.ch
#          A. Contu   Andrea.Contu@cern.ch
#          B. Viaud   viaud@lal.in2p3.fr
#  @date 2015-03-07
#
#  Please contact the abovementioned responsibles before editing this file
# 
##


from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class RareCharmLines(Hlt2LinesConfigurableUser):
    __slots__ = {'_stages' : {},
                 'Prescale' : {},
                 'Common' : {#two body lines
                            'XTrackChi2'         : 5               # adimensional
                            ,'XminP'              : 4000 * MeV      # MeV
                            ,'XminPT'             : 750 * MeV       # MeV
                            ,'XmaxPT'             : 1100 *MeV       # MeV
                            ,'XminIPChi2'         : 3               # adimensional
                            ,'XmaxIPChi2'         : 8               # adimensional

                            ,'DMassWin'           : 70. * MeV       # MeV
                            ,'DMassWinSig'        : 300. * MeV      # MeV
                            ,'D0MinPT'            : 1800. * MeV     # MeV
                            ,'doca'               : 1.0 * mm        # mm

                            ,'DVChi2'             : 10         # adimensional
                            ,'DDira'              : 0.9997     # adimensional
                            ,'DMinFlightChi2'     : 20         # adimensional
                            ,'DMaxIPChi2'         : 15         # adimensional
                            ,'ePIDe'              :  2         # adimensional
                            #three body lines
                            ,'TrkPt_Hmumu'                 : 300.0 * MeV
                            ,'TrkP_Hmumu'                  : 3000.0 * MeV ## 1
                            ,'TrkPVIPChi2_Hmumu'           : 2.0      # unitless
                            ,'TrkPVIPChi2MAX_Hmumu'       : 17.0      #0 unitless
                            ,'TrkPtMAX_Hmumu'             : 0.0 * MeV
                            ,'TrkChi2_Hmumu'               : 4.0      # unitless
                            ,'PairMinDoca_Hmumu'           : 0.1 * mm
                            ,'PairMaxDoca_Hmumu'           : 0.25* mm
                            ,'VtxPVDispChi2_Hmumu'         : 20.0    #20 unitless
                            ,'VtxChi2_Hmumu'               : 20.0     # unitless
                            ,'DIPChi2_Hmumu'               : 36.0     #4  unitless
                            ,'DSumPt_Hmumu'                : 500.0 * MeV ## 1
                            ,'DDira_Hmumu'              : 0.9999     # adimensional
                            ,'MCOR_MAX_Hmumu'              : 3500.0 * MeV
                            ,'Sig_M_MIN'                   : 1800.0 * MeV
                            ,'Sig_M_MAX'                   : 2050.0 * MeV
                            ,'WideMass_M_MIN_Hmumu'              : 1700.0 * MeV
                            ,'WideMass_M_MAX_Hmumu'              : 2100.0 * MeV
                            ,'Lambda_c_Sig_M_MIN'                   : 2200.0 * MeV
                            ,'Lambda_c_Sig_M_MAX'                   : 2370.0 * MeV
                            ,'Lambda_c_WideMass_M_MIN'              : 1700.0 * MeV
                            ,'Lambda_c_WideMass_M_MAX'              : 2300.0 * MeV
                            #four body lines
                            ,'TrkPt_HHmumu'                 : 300.0 * MeV
                            , 'TrkP_HHmumu'                  : 3000.0 * MeV
                            , 'TrkPVIPChi2_HHmumu'           : 2.0      # unitless
                            , 'TrkPVIPChi2MAX_HHmumu'           : 12.0      # !!unitless
                            , 'TrkPtMAX_HHmumu'                 : 0.0 * MeV
                            , 'TrkChi2_HHmumu'               : 4.0      # unitless
                            , 'PairMinDoca_HHmumu'           : 0.1 * mm
                            , 'PairMaxDoca_HHmumu'           : 0.2 * mm
                            , 'VtxPVDispChi2_HHmumu'         : 36.0    #  unitless
                            , 'VtxChi2_HHmumu'               : 15.0     # unitless
                            , 'DIPChi2_HHmumu'               : 25.0     #  unitless
                            , 'DSumPt_HHmumu'                : 3000.0 * MeV
                            , 'DDira_HHmumu'              : 0.9999     # adimensional                    
                            , 'MCOR_MAX_HHmumu'              : 3500.0 * MeV
                            , 'Sig_M_MIN'                   : 1800.0 * MeV
                            , 'Sig_M_MAX'                   : 1950.0 * MeV
                            , 'WideMass_M_MIN_HHmumu'              : 1700.0 * MeV
                            , 'WideMass_M_MAX_HHmumu'              : 2100.0 * MeV
                            #for Lines with Electrons
                            ,'WideMass_M_MIN_Hee'              : 1500.0 * MeV
                            ,'WideMass_M_MAX_Hee'              : 2100.0 * MeV
                            , 'WideMass_M_MIN_HHee'              : 1500.0 * MeV
                            , 'WideMass_M_MAX_HHee'              : 2100.0 * MeV
                            , 'VtxPVDispChi2_XeeORmue'         : 49.0     #  unitless
                            , 'TrkPVIPChi2_XeeORmue'           : 3.0      # unitless
                            },
                   'TwoMuonForD2XXH' : {
                              'Trk_PT_MIN_mumuX'                :  300.0 * MeV# 500.0 /300
                            , 'Trk_P_MIN_mumuX'                 : 3000.0 * MeV # 5000.0 /3000
                            , 'Trk_MIPCHI2DV_MIN_mumuX'         :    2.0      # 2.0 /0  unitless
                            , 'Trk_TRCHI2DOF_MAX_mumuX'         :    4.0      # unitless
                            , 'Pair_AMINDOCA_MAX_mumuX'         :    0.1 * mm
                            , 'Pair_BPVVDCHI2_MIN_mumuX'        :    9.0      # 0.0 unitless
                            , 'Pair_BPVVD_MIN_mumuX'            :    0.0 * mm
                            , 'Pair_SumAPT_MIN_mumuX'           : 0.0 * MeV
                            , 'Pair_BPVCORRM_MAX_mumuX'         : 3500.0 * MeV
                              },
                 'EMu' : {'DMassWin'           : 200. * MeV       # MeV
                             },
                 'TwoElectronForD2XXH' : {
                              'Trk_PT_MIN_eeX'                :  300.0 * MeV# 500.0 /300
                            , 'Trk_P_MIN_eeX'                 : 3000.0 * MeV # 5000.0 /3000
                            , 'Trk_MIPCHI2DV_MIN_eeX'         :    3.0      # 2.0 /0  unitless
                            , 'Trk_TRCHI2DOF_MAX_eeX'         :    4.0      # unitless
                            , 'Pair_AMINDOCA_MAX_eeX'         :    0.1 * mm
                            , 'Pair_BPVVDCHI2_MIN_eeX'        :    20.0      # 0.0 unitless
                            , 'Pair_BPVVD_MIN_eeX'            :    0.0 * mm
                            , 'Pair_SumAPT_MIN_eeX'           : 0.0 * MeV
                            , 'Pair_BPVCORRM_MAX_eeX'         : 3500.0 * MeV
                              },
                  'TwoMuElForD2XXH' : {
                              'Trk_PT_MIN_mueX'                :  300.0 * MeV# 500.0 /300
                            , 'Trk_P_MIN_mueX'                 : 3000.0 * MeV # 5000.0 /3000
                            , 'Trk_MIPCHI2DV_MIN_mueX'         :    3.0      # 2.0 /0  unitless
                            , 'Trk_TRCHI2DOF_MAX_mueX'         :    4.0      # unitless
                            , 'Pair_AMINDOCA_MAX_mueX'         :    0.1 * mm
                            , 'Pair_BPVVDCHI2_MIN_mueX'        :    20.0      # 0.0 unitless
                            , 'Pair_BPVVD_MIN_mueX'            :    0.0 * mm
                            , 'Pair_SumAPT_MIN_mueX'           : 0.0 * MeV
                            , 'Pair_BPVCORRM_MAX_mueX'         : 3500.0 * MeV
                            , 'ePIDe'                          : 2.        # adimensional
                   },
                 'D2PiMueFilter'  :  { 'Sig_M_MIN'                   : 1600.0 * MeV
                                       , 'Sig_M_MAX'                   : 1950.0 * MeV
                                       },
                 'D2KMueFilter'   :  { 'Sig_M_MIN'                   : 1600.0 * MeV
                                       , 'Sig_M_MAX'                   : 1950.0 * MeV
                                       },
                 'D2PieeFilter'  :  { 'Sig_M_MIN'                   : 1600.0 * MeV
                                       , 'Sig_M_MAX'                   : 1950.0 * MeV
                                       },
                 'D2KeeFilter'   :  { 'Sig_M_MIN'                   : 1600.0 * MeV
                                       , 'Sig_M_MAX'                   : 1950.0 * MeV
                                       },
                 'D02PiPiMueFilter':  { 'Sig_M_MIN'                   : 1600.0 * MeV
                                       , 'Sig_M_MAX'                   : 1950.0 * MeV
                                       },
                 'D02KKMueFilter':  { 'Sig_M_MIN'                   : 1600.0 * MeV
                                       , 'Sig_M_MAX'                   : 1950.0 * MeV
                                       },
                 'D02KPiMueFilter' :  { 'Sig_M_MIN'                   : 1600.0 * MeV
                                       , 'Sig_M_MAX'                   : 1950.0 * MeV
                                       },
                 'D02PiPieeFilter':  { 'Sig_M_MIN'                   : 1600.0 * MeV
                                       , 'Sig_M_MAX'                   : 1950.0 * MeV
                                       },
                 'D02KKeeFilter':  { 'Sig_M_MIN'                   : 1600.0 * MeV
                                     , 'Sig_M_MAX'                   : 1950.0 * MeV
                                     },
                 'D02KPieeFilter':  { 'Sig_M_MIN'                   : 1600.0 * MeV
                                      , 'Sig_M_MAX'                   : 1950.0 * MeV
                                      },

                 }
    
    def stages(self, nickname=""):
        if hasattr(self, '_stages') and self._stages:
            if nickname:
                return self._stages[nickname]
            else:
                return self._stages
        
        from Stages import (D02mumuComb,
                            D02pipiComb,
                            D02kpiComb,
                            D02kmuComb,
                            D02emuComb,
                            TwoMuonForD2XXH,
                            TwoElectronForD2XXH,
                            TwoMuElForD2XXH,
                            InPartFilterHXX_Kaons,
                            InPartFilterHXX_Pions,
                            InPartFilterHXX_Protons,
                            InPartFilterHHXX_Kaons,
                            InPartFilterHHXX_Pions,
                            D2PiMuMuComb,
                            D2KMuMuComb,
                            D2PiMuMuSSComb,
                            D2KMuMuSSComb,
                            D2PieeComb,
                            D2KeeComb,
                            D2PiMueComb,
                            D2KMueComb,
                            Lc2PMuMuComb,
                            Lc2PMuMuSSComb,
                            Lc2PeeComb,
                            Lc2PMueComb,
                            D02PiPiMuMuComb,
                            D02KKMuMuComb,
                            D02KPiMuMuComb,
                            D02PiPieeComb,
                            D02KKeeComb,
                            D02KPieeComb,
                            D02PiPiMueComb,
                            D02KKMueComb,
                            D02KPiMueComb,
                            D02KPiMuMuSSComb,
                            D2PiMuMuFilter,
                            D2KMuMuFilter,
                            D2PiMuMuSSFilter,
                            D2KMuMuSSFilter,
                            D2PieeFilter,
                            D2KeeFilter,
                            D2PiMueFilter,
                            D2KMueFilter,
                            Lc2PMuMuFilter,
                            Lc2PMuMuSSFilter,
                            Lc2PeeFilter,
                            Lc2PMueFilter,
                            D02PiPiMuMuFilter,
                            D02KKMuMuFilter,
                            D02KPiMuMuFilter,
                            D02PiPieeFilter,
                            D02KKeeFilter,
                            D02KPieeFilter,
                            D02PiPiMueFilter,
                            D02KKMueFilter,
                            D02KPiMueFilter,
                            D02KPiMuMuSSFilter)
        self._stages = { 'D02MuMu'   : [ D02mumuComb ],
                   'D02PiPi'  :  [ D02pipiComb ],
                   'D02KPi'  :  [ D02kpiComb ],
                   'D02KMu'  :  [ D02kmuComb ],
                   'D02EMu'  :  [ D02emuComb ],
                   'D2PiMuMuFilter' : [ D2PiMuMuFilter ],
                   'D2KMuMuFilter' : [ D2KMuMuFilter ],
                   'D2PieeFilter' : [ D2PieeFilter ],
                   'D2KeeFilter' : [ D2KeeFilter ],
                   'D2PiMueFilter' : [ D2PiMueFilter ],
                   'D2KMueFilter' : [ D2KMueFilter ],
                   'D2PiMuMuSSFilter' : [ D2PiMuMuSSFilter ],
                   'D2KMuMuSSFilter' : [ D2KMuMuSSFilter ],
                   'Lc2PMuMuFilter' : [ Lc2PMuMuFilter ],
                   'Lc2PMuMuSSFilter' : [ Lc2PMuMuSSFilter ],
                   'Lc2PeeFilter' : [ Lc2PeeFilter ],
                   'Lc2PMueFilter' : [ Lc2PMueFilter ],
                   'D02PiPiMuMuFilter' : [ D02PiPiMuMuFilter ],
                   'D02KKMuMuFilter' : [ D02KKMuMuFilter ],
                   'D02KPiMuMuFilter' : [ D02KPiMuMuFilter ],
                   'D02PiPieeFilter' : [ D02PiPieeFilter ],
                   'D02KKeeFilter' : [ D02KKeeFilter ],
                   'D02KPieeFilter' : [ D02KPieeFilter ],
                   'D02PiPiMueFilter' : [ D02PiPiMueFilter ],
                   'D02KKMueFilter' : [ D02KKMueFilter ],
                   'D02KPiMueFilter' : [ D02KPiMueFilter ],
                   'D02KPiMuMuSSFilter' : [ D02KPiMuMuSSFilter ],
                  }
        
        if nickname:
            return self._stages[nickname]
        else:
            return self._stages
    
    def __apply_configuration__(self):
        
        from HltLine.HltLine import Hlt2Line

        stages = self.stages()
        
        for (nickname, algos) in self.algorithms(stages).iteritems():
            Hlt2Line('RareCharm' + nickname,
                     prescale = self.prescale,
                     algos = algos,
                     postscale = self.postscale)



