from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2HHKsLines(Hlt2LinesConfigurableUser):
    __slots__ = {'Prescale'                 : {},
                 'Postscale'                : {}
                ,'TrackGEC' : {'NTRACK_MAX'      : 10000}
                ,'Common' : {'TisTosSpec'   : "Hlt1Track.*Decision%TOS",
                             'Trk_TRCHI2DOF_MAX'        :  3.0,
                             'Trk_PT_MIN'               :  200 * MeV,
                             'Trk_P_MIN'                :  1000 * MeV,
                             'Trk_ALL_TRCHI2DOF_MAX'    :  3.0,
                             'Trk_ALL_P_MIN'            :  1000 * MeV,
                             'VCHI2PDOF_MAX'            :  20.0,
                             'Trk_ALL_MIPCHI2DV_MIN'    :  1.0,
                             'Trk_2OF3_MIPCHI2DV_MIN'   :  4.0,
                             'Trk_1OF3_MIPCHI2DV_MIN'   :  4.0,
                             'Kaon_PIDK_MIN'            :  0,
                             'Pion_PIDK_MAX'            :  10,
                             'Proton_PIDp_MIN'          :  0,
                             'AM_MIN'                   :  1779 * MeV,
                             'AM_MAX'                   :  2436 * MeV,
                             'D_ASUMPT_MIN'             :  0 * MeV,
                             'D_BPVDIRA_MIN'            :  0.9994,
                             'D_BPVVDCHI2_MIN'          :  20.0,
                             'D_VCHI2PDOF_MAX'          :  20.0,
                             'D_BPVLTIME_MIN'           :  0.2 * picosecond,
                             'D_Mass_M_MIN'             :  1789.0 * MeV,
                             'D_Mass_M_MAX'             :  2049.0 * MeV
                             , 'MaxDOCA'                :  0.200 * mm
                             , 'HHMass_MAX'             :  1450. * MeV
                             , 'KshhTFHHTrkPLL'         :  1500.0 * MeV
                             , 'KshhTFHHTrkChi2UL'       : 5.0        # unitless -- 14 Jan: Mat changed from 20.0 to 5.0
                             , 'KshhTFHHVtxChi2UL'       : 10.0       # unitless -- 14 Jan: Mat changed from 20.0 to 10.0
                             , 'KshhTFHHPtLL'            : 1000.0 * MeV
                             , 'KshhTFHHFDLL'            : 2.0 * mm        # -- 26 Feb: Mat changed from 1mm to 2mm
                             , 'KshhTFHHTightTrkChi2UL'  : 2.0        # unitless -- 27 Feb: Mat adds for use in DD filter
                             , 'KshhTFHHTightVtxChi2UL'  : 5.0        # unitless -- 27 Feb: Mat adds for use in DD filter
                             , 'KshhTFHHTightFDLL'       : 4.0 * mm       # -- 27 Feb: Mat adds for use in DD filter
                             , 'KshhTFHHTightPtLL'       : 1000.0 * MeV # -- 27 Feb: Mat adds for use in DD filter
                             , 'KshhTFHHTightTrkPVIPChi2LL' : 12.5     # unitless -- 28 Feb: Mat adds for use in DD filter
                             , 'KshhTFKsLLTrkPtLL'       : 200.0  * MeV
                             , 'KshhTFKsLLTrkPVIPChi2LL' : 9.0        # unitless
                             , 'KshhTFKsLLTrkChi2UL'     : 2.5       # unitless;  changed to 10 from 20 111219 mds  to 2.5 120121
                             , 'KshhTFKsLLMothSymMassWin': 30.0 * MeV  #  mds, increased to 30 MeV from 15 120315
                             , 'KshhTFKsLLVtxChi2UL'     : 20.0       # unitless change from 30 to 20 120116
                             , 'KshhTFKsLLVtxPVDispZLL'  : -1000.0 * mm
                             , 'KshhTFKsLLVtxPVDispZUL'  : 650.0 * mm
                             , 'KshhTFKsLLVtxPVDispChi2LL' : 400.0    # unitlessfrom 100 to 300 120114 to 400 120116
                             , 'KshhTFKsLLBpvlTimeLL'    : 2.0        # initial value 120327 is 2.0 ps
                             , 'KshhTFKsLLDiraLL'        : 0.9997     # unitless
                             , 'KshhTFKsDDTrkPLL'        : 2000.0 * MeV
                             , 'KshhTFKsDDTrkPtLL'       : 200.0  * MeV
                             , 'KshhTFKsDDTrkPVIPChi2LL' : 9.0        # unitless changed from 4.0 to 9.0 120328 mds
                             , 'KshhTFKsDDTrkChi2UL'     : 10.0       # unitless -- 14 Jan: Mat changed from 20.0 to 10.0
                             , 'KshhTFKsDDSymMassWin'    : 40.0 * MeV # changed from 80 * MeV 111219 mds
                             , 'KshhTFKsDDVtxChi2UL'     : 20.0       # unitless -- 14 Jan: Mat changed from 30 to 20
                             , 'KshhTFKsDDVtxPVDispZLL'  : 400.0 * mm
                             , 'KshhTFKsDDVtxPVDispZUL'  : 2300.0 * mm
                             , 'KshhTFKsDDVtxPVDispChi2LL' : 100.0    # unitless
                             , 'KshhTFKsDDDiraLL'        : 0.9999     # unitless -- 14 Jan: Mat changed from 0.9997 to 0.9999
                             , 'KshhTFD0VtxChi2UL'        : 20.0       # unitless
                             , 'KshhTFD0PtLL'             : 2000.0 * MeV    # -- 14 Jan: Mat changed from 500 to 2000
                             , 'KshhTFD0DiraLL'           : 0.999        # unitless -- 14 Jan: Mat changed from 0.999 to 0.0; and back to 0.999 120115 mds
                             , 'KshhTFDwKsLLSymMassWin'  : 100.0 * MeV  # restored to 100 MeV 120315 mds
                             , 'KshhTFDwKsDDSymMassWin'  : 120.0 * MeV  # restored to 120 MeV 120315 mds
                             , 'Dz2HHKs_Mass_LL_MIN'     : 1795.0 * MeV # 1865 - 70
                             , 'Dz2HHKs_Mass_LL_MAX'     : 1935.0 * MeV # 1865 - 70
                             , 'Dz2HHKs_Mass_DD_MIN'     : 1795.0 * MeV # 1865 - 70
                             , 'Dz2HHKs_Mass_DD_MAX'     : 1935.0 * MeV # 1865 - 70
                             , 'KshhTFD0BpvlTimeLL'      : 0.3        # initial value 120329 is 0.2 ps
                             , 'KshhTFD0PVIPCHI2UL'      : 20        # inital value 120329 20
                             , 'ComRobUseGEC'            : True       # do or do not
                             , 'ComRobGEC'               : 180        # max number of tracks
                             , 'HLT1FILTER'               : ''
                             , 'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 }
                             , 'name_prefix'              : 'CharmHadD02HHKs'
                            },  ## end of 'Common' : { ... }
                 'SharedDetachedCFChild_K' : {
                                             'PID_LIM'                  :  0,
                                             'Trk_ALL_PT_MIN'           :  200 * MeV,
                                             'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                                          },
                 'SharedDetachedCFChild_pi' : {
                                             'PID_LIM'                  :  10,
                                             'Trk_ALL_PT_MIN'           :  200 * MeV,
                                             'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                                          },
                 'SharedDetachedSCSChild_K' : {
                                             'PID_LIM'                  :  0,
                                             'Trk_ALL_PT_MIN'           :  200 * MeV,
                                             'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                                          },
                 'SharedDetachedSCSChild_pi' : {
                                             'PID_LIM'                  :  10,
                                             'Trk_ALL_PT_MIN'           :  200 * MeV,
                                             'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                                          },
                 'SharedDetachedDCSChild_K' : {
                                             'PID_LIM'                  :  0,
                                             'Trk_ALL_PT_MIN'           :  200 * MeV,
                                             'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                                          },
                 'SharedDetachedDCSChild_pi' : {
                                             'PID_LIM'                  :  10,
                                             'Trk_ALL_PT_MIN'           :  200 * MeV,
                                             'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                                          }
        }

    def __apply_configuration__(self):
##
## notation:  Ksh  <--> Kshort
##            LL, DD  <-->  both Kshort daughters are long or downstream tracks
        from Stages import MassFilterD_LL,MassFilterD_DD     
        from Stages import Dz2KshPiPi_LL, Dz2KshPiPi_DD, Dz2KshKPi_LL, Dz2KshKPi_DD
        from Stages import Dz2KshKK_LL, Dz2KshKK_DD
        from Stages import D02KshPiPi_LL, D02KshPiPi_DD, D02KshKPi_LL, D02KshKPi_DD
        from Stages import D02KshKK_LL, D02KshKK_DD
        stages = {'Dz2KshPiPi_LL'   : [MassFilterD_LL('Dz2KshPiPi_LL',inputs=[Dz2KshPiPi_LL])],
                  'Dz2KshPiPi_DD'   : [MassFilterD_DD('Dz2KshPiPi_DD',inputs=[Dz2KshPiPi_DD])],
                  'Dz2KshKPi_LL'      : [MassFilterD_LL('Dz2KshKPi_LL',inputs=[Dz2KshKPi_LL])],
                  'Dz2KshKPi_DD'      : [MassFilterD_DD('Dz2KshKPi_DD',inputs=[Dz2KshKPi_DD])],
                  'Dz2KshKK_LL'       : [MassFilterD_LL('Dz2KshKK_LL',inputs=[Dz2KshKK_LL])],
                  'Dz2KshKK_DD'       : [MassFilterD_DD('Dz2KshKK_DD',inputs=[Dz2KshKK_DD])],
                  'D02KshPiPi_LL'     : [MassFilterD_LL('D02KshPiPi_LL',inputs=[D02KshPiPi_LL])],
                  'D02KshPiPi_DD'     : [MassFilterD_DD('D02KshPiPi_DD',inputs=[D02KshPiPi_DD])],
                  'D02KshKPi_LL'      : [MassFilterD_LL('D02KshKPi_LL',inputs=[D02KshKPi_LL])],
                  'D02KshKPi_DD'      : [MassFilterD_DD('D02KshKPi_DD',inputs=[D02KshKPi_DD])],
                  'D02KshKK_LL'       : [MassFilterD_LL('D02KshKK_LL',inputs=[D02KshKK_LL])],
                  'D02KshKK_DD'       : [MassFilterD_DD('D02KshKK_DD',inputs=[D02KshKK_DD])] }

        from HltLine.HltLine import Hlt2Line
        for nickname, algos in self.algorithms(stages).iteritems():
            Hlt2Line(nickname, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
