from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

# Lines for XSection measurements, specifically for the 2015 Early Measurement campaign
# These are tuned to be as close as possible to the stripping lines

class CharmHadXSecLines() :
    def localcuts(self) : 
        return { # Now the combiner for the CPV lines 
                 'Dpm2HHH_XSec' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  10.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  50.0,
                                 'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  400.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  25.0,
                                 'BPVVDCHI2_MIN'            :  16.0,
                                 'BPVLTIME_MIN'             :  0.15 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9994,
                                 'ASUMPT_MIN'               :  0 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  1959 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  1949.0 * MeV,
                                },
                 'Ds2HHH_XSec' : { 
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  10.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  50.0,
                                 'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  25.0,
                                 'BPVVDCHI2_MIN'            :  16.0,
                                 'BPVLTIME_MIN'             :  0.15 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9994,
                                 'ASUMPT_MIN'               :  0 * MeV,
                                 'AM_MIN'                   :  1879 * MeV,
                                 'AM_MAX'                   :  2059 * MeV,
                                 'Mass_M_MIN'               :  1889.0 * MeV,
                                 'Mass_M_MAX'               :  2049.0 * MeV,
                                },
                 'Lc2HHH_XSec' : {
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4.0, 
                                 'Trk_2OF3_MIPCHI2DV_MIN'   :  10.0,
                                 'Trk_1OF3_MIPCHI2DV_MIN'   :  50.0,
                                 'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  25.0,
                                 'BPVVDCHI2_MIN'            :  4.0,
                                 'BPVLTIME_MIN'             :  0.075 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9994,
                                 'ASUMPT_MIN'               :  0 * MeV,
                                 'AM_MIN'                   :  2201 * MeV,
                                 'AM_MAX'                   :  2553 * MeV,
                                 'Mass_M_MIN'               :  2211.0 * MeV,
                                 'Mass_M_MAX'               :  2543.0 * MeV,
                                },
                 'D02HH_XSec'    : {
                                 'Trk_PT_MIN'               : 250.0 * MeV,
                                 'Trk_P_MIN'                : 1.0  * GeV, 
                                 'Trk_MIPCHI2DV_MIN'        : 4.0,        # neuter
                                 'Pair_AMINDOCA_MAX'        : 0.10 * mm,
                                 'Trk_Max_APT_MIN'          : 0.0 * MeV,
                                 'D0_BPVVDCHI2_MIN'         : 25.0 ,      # neuter
                                 'D0_BPVDIRA_MIN'           : 0.99985,    # neuter
                                 'D0_VCHI2PDOF_MAX'         : 10.0,       # neuter
                                 'D0_PT_MIN'                : 0.0 * MeV, 
                                 'Mass_M_MIN'               : 1785.0 * MeV, 
                                 'Mass_M_MAX'               : 1945.0 * MeV,
                                 'WideMass_M_MIN'           : 1775.0 * MeV, 
                                 'WideMass_M_MAX'           : 1955.0 * MeV 
                                },
                 'D0_TAG_XSec' : {
                                 'DeltaM_AM_MIN'            :  130.0 * MeV,
                                 'DeltaM_MIN'               :  130.0 * MeV,
                                 'DeltaM_AM_MAX'            :  165.0 * MeV,
                                 'DeltaM_MAX'               :  160.0 * MeV,
                                 'TagVCHI2PDOF_MAX'         :  100.0
                                },
                 'Sigmac_TAG_XSec' : { 
                                 'DeltaM_AM_MIN'            :  150.0 * MeV,
                                 'DeltaM_MIN'               :  155.0 * MeV,
                                 'DeltaM_AM_MAX'            :  205.0 * MeV,
                                 'DeltaM_MAX'               :  200.0 * MeV,
                                 'TagVCHI2PDOF_MAX'         :  100.0
                                }, 
                }
    
    def locallines(self):
        from Stages import MassFilter,TagDecay
        from Stages import D2KPiPi_SS,D2KKPi_OS,D2PiPiPi
        from Stages import Lc2KPPi, Lc2PiPPi
        from Stages import SharedSoftTagChild_pi
        
        stages = {# First the D2HHH lines
                  'Dpm2KPiPi_XSec'       : [MassFilter('Dpm2HHH_XSec', 
                                                      inputs=[D2KPiPi_SS('Dpm2HHH_XSec')])],
                  'Dpm2KKPi_XSec'        : [MassFilter('Dpm2HHH_XSec',
                                                      inputs=[D2KKPi_OS('Dpm2HHH_XSec')])],
                  'Ds2KKPi_XSec'         : [MassFilter('Ds2HHH_XSec', 
                                                      inputs=[D2KKPi_OS('Ds2HHH_XSec')])],
                  'Ds2PiPiPi_XSec'       : [MassFilter('Ds2HHH_XSec',
                                                      inputs=[D2PiPiPi('Ds2HHH_XSec')])],
                  # Now the Lc2HHH lines, untagged, CF shared to reuse in tagged lines
                  # Because of the mass window these also catch neutral Xi_c baryons
                  'Lc2KPPi_XSec'        : [MassFilter('Lc2KPPi_XSec', nickname = 'Lc2HHH_XSec',
                                                      inputs=[Lc2KPPi('Lc2HHH_XSec')],shared=True)],
                  'Lc2PiPPi_XSec'       : [MassFilter('Lc2HHH_XSec',
                                                      inputs=[Lc2PiPPi('Lc2HHH_XSec')])]
                 }
        # Now the Sigma_c0,++->Lambda_c(pKpi)pi line
        # Because of the mass window this also catches some excited Xi_c
        stages['Sigmac_2LcPi_XSec']     = TagDecay('Sigmac_TAG_XSec', ["[Sigma_c0 -> Lambda_c+ pi-]cc",
                                                                      "[Sigma_c++ -> Lambda_c+ pi+]cc"],
                                                  inputs = [ stages["Lc2KPPi_XSec"][0],
                                                             SharedSoftTagChild_pi ]), 
 
        return stages
