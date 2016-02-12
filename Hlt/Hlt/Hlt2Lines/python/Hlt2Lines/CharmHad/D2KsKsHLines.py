# -*- coding: utf-8 -*-
## @file
#  Set of Hlt2-lines suitable for the study of charm decay channels with
#  two K0s and one hadron in the final state.
#
#  @author Maurizio MARTINELLI maurizio.martinelli@cern.ch
#=============================================================================
""" Set of Hlt2-lines suitable for the study of charm decay channels with
    four particles in the final state.
"""
#=============================================================================
__author__  = "Maurizio MARTINELLI maurizio.martinelli@cern.ch"
from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm, mrad
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
import copy

class CharmHadD2KsKsHLines(Hlt2LinesConfigurableUser):
    def localcuts(self) :
        CommonKsKsH = {
        'KS0_ALL_PT_MIN'        : 500.0 * MeV,
        'KS0_ALL_MIPCHI2DV_MIN' : 9.0,
        'Trk_ALL_PT_MIN'        : 250 * MeV,
        'Trk_ALL_MIPCHI2DV_MIN' : 3,
        'AM_MIN'                : 1740. * MeV,
        'AM_MAX'                : 1990. * MeV,
        'AM_3'                  : 139.57018 * MeV,
        'PT12_MIN'              : 1500 * MeV,
        'ASUMPT_MIN'            : 1500 * MeV,
        'VCHI2PDOF_MAX'         : 20,
        'acosBPVDIRA_MAX'       : 34.6 * mrad,
        'BPVLTIME_MIN'          : 0.4 * picosecond,
        'DPT_MIN'               : 1800 * MeV,
        'DMOM_MIN'              : 20000 * MeV,
        'TisTosSpec'            : [],#"Hlt1.*Track.*Decision%TOS",
        }

        Dp2KsKsH = copy.deepcopy(CommonKsKsH)
        Dp2KsKsHMass = { 'Mass_M_MIN' : 1765 * MeV,
                         'Mass_M_MAX' : 1965 * MeV }

        Ds2KsKsH = copy.deepcopy(CommonKsKsH)
        Ds2KsKsH.update({   'AM_MIN'                : 1840. * MeV,
                            'AM_MAX'                : 2090. * MeV,
                            'BPVLTIME_MIN'          : 0.2 * picosecond} )
        Ds2KsKsHMass = {    'Mass_M_MIN' : 1865. * MeV,
                            'Mass_M_MAX' : 2065. * MeV }

        Lc2KsKsH = copy.deepcopy(CommonKsKsH)
        Lc2KsKsH.update({   'AM_MIN'                : 2181. * MeV,
                            'AM_MAX'                : 2391. * MeV,
                            'BPVLTIME_MIN'          : 0.1 * picosecond} )
        Lc2KsKsHMass = {    'Mass_M_MIN' : 2156. * MeV,
                            'Mass_M_MAX' : 2366. * MeV }

        cuts = { 'Dp2KsKsH'       : Dp2KsKsH,
                 'Ds2KsKsH'       : Ds2KsKsH,
                 'Lc2KsKsH'       : Lc2KsKsH,
                 'Dp2KsKsHMass'   : Dp2KsKsHMass,
                 'Ds2KsKsHMass'   : Ds2KsKsHMass,
                 'Lc2KsKsHMass'   : Lc2KsKsHMass }

        return cuts

    def locallines(self):
        from Stages import HKsKsCombiner, MassFilter
        from Stages import CharmHadSharedKsLL, CharmHadSharedKsDD
        from Stages import SharedDetachedDpmChild_pi, SharedDetachedDpmChild_K, SharedDetachedLcChild_p

        Dp2KsKsPip_LL =   HKsKsCombiner('Dp2KsKsPip_LL', decay="[D+ -> KS0 KS0 pi+]cc",
                           inputs=[CharmHadSharedKsLL, SharedDetachedDpmChild_pi],
                           nickname='Dp2KsKsH')
        Dp2KsKsPip_LLDD = HKsKsCombiner('Dp2KsKsPip_LLDD', decay="[D+ -> KS0 KS0 pi+]cc",
                           inputs=[CharmHadSharedKsLL, CharmHadSharedKsDD, SharedDetachedDpmChild_pi],
                           nickname='Dp2KsKsH', lldd = True)
        Dp2KsKsPip_DD =   HKsKsCombiner('Dp2KsKsPip_DD', decay="[D+ -> KS0 KS0 pi+]cc",
                           inputs=[CharmHadSharedKsDD, SharedDetachedDpmChild_pi],
                           nickname='Dp2KsKsH')
        Dp2KsKsKp_LL =    HKsKsCombiner('Dp2KsKsKp_LL', decay="[D+ -> KS0 KS0 K+]cc",
                           inputs=[CharmHadSharedKsLL, SharedDetachedDpmChild_K],
                           nickname='Dp2KsKsH')
        Dp2KsKsKp_LLDD =  HKsKsCombiner('Dp2KsKsKp_LLDD', decay="[D+ -> KS0 KS0 K+]cc",
                           inputs=[CharmHadSharedKsLL, CharmHadSharedKsDD, SharedDetachedDpmChild_K],
                           nickname='Dp2KsKsH', lldd = True)
        Dp2KsKsKp_DD =    HKsKsCombiner('Dp2KsKsKp_DD', decay="[D+ -> KS0 KS0 K+]cc",
                           inputs=[CharmHadSharedKsDD, SharedDetachedDpmChild_K],
                           nickname='Dp2KsKsH')
        Dsp2KsKsPip_LL =   HKsKsCombiner('Dsp2KsKsPip_LL', decay="[D_s+ -> KS0 KS0 pi+]cc",
                           inputs=[CharmHadSharedKsLL, SharedDetachedDpmChild_pi],
                           nickname='Ds2KsKsH')
        Dsp2KsKsPip_LLDD = HKsKsCombiner('Dsp2KsKsPip_LLDD', decay="[D_s+ -> KS0 KS0 pi+]cc",
                           inputs=[CharmHadSharedKsLL, CharmHadSharedKsDD, SharedDetachedDpmChild_pi],
                           nickname='Ds2KsKsH', lldd = True)
        Dsp2KsKsPip_DD =   HKsKsCombiner('Dsp2KsKsPip_DD', decay="[D_s+ -> KS0 KS0 pi+]cc",
                           inputs=[CharmHadSharedKsDD, SharedDetachedDpmChild_pi],
                           nickname='Ds2KsKsH')
        Dsp2KsKsKp_LL =    HKsKsCombiner('Dsp2KsKsKp_LL', decay="[D_s+ -> KS0 KS0 K+]cc",
                           inputs=[CharmHadSharedKsLL, SharedDetachedDpmChild_K],
                           nickname='Ds2KsKsH')
        Dsp2KsKsKp_LLDD =  HKsKsCombiner('Dsp2KsKsKp_LLDD', decay="[D_s+ -> KS0 KS0 K+]cc",
                           inputs=[CharmHadSharedKsLL, CharmHadSharedKsDD, SharedDetachedDpmChild_K],
                           nickname='Ds2KsKsH', lldd = True)
        Dsp2KsKsKp_DD =    HKsKsCombiner('Dsp2KsKsKp_DD', decay="[D_s+ -> KS0 KS0 K+]cc",
                           inputs=[CharmHadSharedKsDD, SharedDetachedDpmChild_K],
                           nickname='Ds2KsKsH')
        Lcp2KsKsPp_LL =   HKsKsCombiner('Lcp2KsKsPp_LL', decay="[Lambda_c+ -> KS0 KS0 p+]cc",
                           inputs=[CharmHadSharedKsLL, SharedDetachedLcChild_p],
                           nickname='Lc2KsKsH')
        Lcp2KsKsPp_LLDD = HKsKsCombiner('Lcp2KsKsPp_LLDD', decay="[Lambda_c+ -> KS0 KS0 p+]cc",
                           inputs=[CharmHadSharedKsLL, CharmHadSharedKsDD, SharedDetachedLcChild_p],
                           nickname='Lc2KsKsH', lldd = True)
        Lcp2KsKsPp_DD =   HKsKsCombiner('Lcp2KsKsPp_DD', decay="[Lambda_c+ -> KS0 KS0 p+]cc",
                           inputs=[CharmHadSharedKsDD, SharedDetachedLcChild_p],
                           nickname='Lc2KsKsH')

        Dp2KS0KS0Pip_KS0LL = MassFilter('Dp2KsKsHMass',inputs=[Dp2KsKsPip_LL])
        Dp2KS0KS0Pip_KS0LLDD = MassFilter('Dp2KsKsHMass',inputs=[Dp2KsKsPip_LLDD])
        Dp2KS0KS0Pip_KS0DD = MassFilter('Dp2KsKsHMass',inputs=[Dp2KsKsPip_DD])
        Dp2KS0KS0Kp_KS0LL = MassFilter('Dp2KsKsHMass',inputs=[Dp2KsKsKp_LL])
        Dp2KS0KS0Kp_KS0LLDD = MassFilter('Dp2KsKsHMass',inputs=[Dp2KsKsKp_LLDD])
        Dp2KS0KS0Kp_KS0DD = MassFilter('Dp2KsKsHMass',inputs=[Dp2KsKsKp_DD])

        Dsp2KS0KS0Pip_KS0LL = MassFilter('Ds2KsKsHMass',inputs=[Dsp2KsKsPip_LL])
        Dsp2KS0KS0Pip_KS0LLDD = MassFilter('Ds2KsKsHMass',inputs=[Dsp2KsKsPip_LLDD])
        Dsp2KS0KS0Pip_KS0DD = MassFilter('Ds2KsKsHMass',inputs=[Dsp2KsKsPip_DD])
        Dsp2KS0KS0Kp_KS0LL = MassFilter('Ds2KsKsHMass',inputs=[Dsp2KsKsKp_LL])
        Dsp2KS0KS0Kp_KS0LLDD = MassFilter('Ds2KsKsHMass',inputs=[Dsp2KsKsKp_LLDD])
        Dsp2KS0KS0Kp_KS0DD = MassFilter('Ds2KsKsHMass',inputs=[Dsp2KsKsKp_DD])

        Lcp2KS0KS0Pp_KS0LL = MassFilter('Lc2KsKsHMass',inputs=[Lcp2KsKsPp_LL])
        Lcp2KS0KS0Pp_KS0LLDD = MassFilter('Lc2KsKsHMass',inputs=[Lcp2KsKsPp_LLDD])
        Lcp2KS0KS0Pp_KS0DD = MassFilter('Lc2KsKsHMass',inputs=[Lcp2KsKsPp_DD])

        stages = {
                  'Dp2KS0KS0Pip_KS0LLTurbo'   : [Dp2KS0KS0Pip_KS0LL],
                  'Dp2KS0KS0Pip_KS0LLDDTurbo' : [Dp2KS0KS0Pip_KS0LLDD],
                  'Dp2KS0KS0Pip_KS0DDTurbo'   : [Dp2KS0KS0Pip_KS0DD],

                  'Dp2KS0KS0Kp_KS0LLTurbo'   : [Dp2KS0KS0Kp_KS0LL],
                  'Dp2KS0KS0Kp_KS0LLDDTurbo' : [Dp2KS0KS0Kp_KS0LLDD],
                  'Dp2KS0KS0Kp_KS0DDTurbo'   : [Dp2KS0KS0Kp_KS0DD],

                  'Dsp2KS0KS0Pip_KS0LLTurbo'   : [Dsp2KS0KS0Pip_KS0LL],
                  'Dsp2KS0KS0Pip_KS0LLDDTurbo' : [Dsp2KS0KS0Pip_KS0LLDD],
                  'Dsp2KS0KS0Pip_KS0DDTurbo'   : [Dsp2KS0KS0Pip_KS0DD],

                  'Dsp2KS0KS0Kp_KS0LLTurbo'   : [Dsp2KS0KS0Kp_KS0LL],
                  'Dsp2KS0KS0Kp_KS0LLDDTurbo' : [Dsp2KS0KS0Kp_KS0LLDD],
                  'Dsp2KS0KS0Kp_KS0DDTurbo'   : [Dsp2KS0KS0Kp_KS0DD],

                  'Lcp2KS0KS0Pp_KS0LLTurbo'   : [Lcp2KS0KS0Pp_KS0LL],
                  'Lcp2KS0KS0Pp_KS0LLDDTurbo' : [Lcp2KS0KS0Pp_KS0LLDD],
                  'Lcp2KS0KS0Pp_KS0DDTurbo'   : [Lcp2KS0KS0Pp_KS0DD],
                 }

        return stages
