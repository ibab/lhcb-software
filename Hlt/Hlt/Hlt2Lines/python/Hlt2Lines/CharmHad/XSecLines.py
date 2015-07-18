from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

# Lines for XSection measurements, specifically for the 2015 Early Measurement campaign
# These are tuned to be as close as possible to the stripping lines

class CharmHadXSecLines() :
    def localcuts(self) : 
        return { # Now the combiner for the CPV lines 
                 'D02HH_XSec'   : {
                                  'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",
                                  'Pair_AMINDOCA_MAX'        : 0.10 * mm,
                                  'Trk_Max_APT_MIN'          : 500.0 * MeV, 
                                  'D0_BPVVDCHI2_MIN'         : 49.0,       # neuter
                                  'D0_BPVDIRA_MIN'           : 0.99985,    # neuter
                                  'D0_VCHI2PDOF_MAX'         : 10.0,       # neuter
                                  'D0_PT_MIN'                : 0.0 * MeV, 
                                  'Comb_AM_MIN'              : 1775.0 * MeV, 
                                  'Comb_AM_MAX'              : 1955.0 * MeV, 
                                  ## These should be removed to input particle filtering
                                  'Trk_ALL_PT_MIN'           : 250.0 * MeV, 
                                  'Trk_ALL_P_MIN'            : 2.0  * GeV, 
                                  'Trk_ALL_MIPCHI2DV_MIN'    : 16.0, 
                                  ## Add mass filter after combination cut
                                  'Mass_M_MIN'               :  1784.0 * MeV,
                                  'Mass_M_MAX'               :  1944.0 * MeV,
                                },
                 'D02HHHH_XSec' : {
                                 'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",
                                 'Trk_ALL_PT_MIN'           :  200 * MeV,
                                 'Trk_ALL_MIPCHI2DV_MIN'    :  4,
                                 'AM_34'                    :  (139.5 + 139.5) * MeV,
                                 'AM_4'                     :  (139.5) * MeV,
                                 'AM_MIN'                   :  1774 * MeV,
                                 'AM_MAX'                   :  1954 * MeV,
                                 'ASUMPT_MIN'               :  0.0 * MeV,
                                 'ADOCA_MAX'                :  0.1 * mm,
                                 'ACHI2DOCA_MAX'            :  99999999.0,
                                 'VCHI2PDOF_MAX'            :  10.0,
                                 'BPVDIRA_MIN'              :  0.99985,
                                 'BPVLTIME_MIN'             :  0.1*picosecond,
                                 'Mass_M_MIN'               :  1784.0 * MeV,
                                 'Mass_M_MAX'               :  1944.0 * MeV,
                                 'AMOM_MIN'                 :  0 * MeV,
                                 'DPT_MIN'                  :  0 * MeV,
                                 'DMOM_MIN'                 :  0 * MeV,
                                 'BPVVDCHI2_MIN'            :  9,
                                },
                 'Dpm2HHH_XSec' : {
                                 'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",
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
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  1959 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  1949.0 * MeV,
                                },
                 'Ds2HHH_XSec' : { 
                                 'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",
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
                                 'TisTosSpec'               : "Hlt1TrackAllL0Decision%TOS",
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
                 'D0_TAG_XSec' : {
                                 'DeltaM_AM_MIN'            :  130.0 * MeV,
                                 'DeltaM_MIN'               :  130.0 * MeV,
                                 'DeltaM_AM_MAX'            :  165.0 * MeV,
                                 'DeltaM_MAX'               :  160.0 * MeV,
                                 'TagVCHI2PDOF_MAX'         :  25.0
                                },
                 'D_TAG_NEUTRAL_XSec' : {
                                 'DeltaM_AM_MIN'            :  130.0 * MeV,
                                 'DeltaM_MIN'               :  130.0 * MeV,
                                 'DeltaM_AM_MAX'            :  165.0 * MeV,
                                 'DeltaM_MAX'               :  160.0 * MeV,
                                },
                 'Sigmac_TAG_XSec' : { 
                                 'DeltaM_AM_MIN'            :  150.0 * MeV,
                                 'DeltaM_MIN'               :  155.0 * MeV,
                                 'DeltaM_AM_MAX'            :  205.0 * MeV,
                                 'DeltaM_MAX'               :  200.0 * MeV,
                                 'TagVCHI2PDOF_MAX'         :  25.0
                                },
                }
    
    def locallines(self):
        from Stages import MassFilter,TagDecay,TagDecayWithNeutral
        from Stages import SharedSoftTagChild_pi,SharedNeutralLowPtChild_pi0,SharedNeutralLowPtChild_gamma
        from Stages import XSec_D02KPi, XSec_D02K3Pi
        from Stages import XSec_DpToKmPipPip, XSec_DpToKmKpPim
        from Stages import XSec_DspToKmKpPim, XSec_DspToPimPipPip
        from Stages import XSec_LcpToKmPpPip, XSec_LcpToPimPpPip, XSec_LcpToKmPpKp
        
        stages = {# First the D2HHH lines
                  'Dpm2KPiPi_XSecTurbo'       : [MassFilter('Dpm2KPiPi_XSec',nickname='Dpm2HHH_XSec', 
                                                      inputs=[XSec_DpToKmPipPip],shared=True)],
                  'Dpm2KKPi_XSecTurbo'        : [MassFilter('Dpm2KKPi_XSec', nickname = 'Dpm2HHH_XSec',
                                                      inputs=[XSec_DpToKmKpPim],shared=True)],
                  'Ds2KKPi_XSecTurbo'         : [MassFilter('Ds2KKPi_XSec', nickname = 'Ds2HHH_XSec', 
                                                      inputs=[XSec_DspToKmKpPim],shared=True)],
                  'Ds2PiPiPi_XSecTurbo'       : [MassFilter('Ds2PiPiPi_XSec', nickname = 'Ds2HHH_XSec',
                                                      inputs=[XSec_DspToPimPipPip],shared=True)],
                  # Now the Lc2HHH lines, untagged, CF shared to reuse in tagged lines
                  # Because of the mass window these also catch neutral Xi_c baryons
                  'Lc2KPPi_XSecTurbo'        : [MassFilter('Lc2KPPi_XSec', nickname = 'Lc2HHH_XSec',
                                                      inputs=[XSec_LcpToKmPpPip],shared=True)],
                  'Lc2KPK_XSecTurbo'         : [MassFilter('Lc2KPK_XSec', nickname = 'Lc2HHH_XSec',
                                                      inputs=[XSec_LcpToKmPpKp],shared=True)],
                  'Lc2PiPPi_XSecTurbo'       : [MassFilter('Lc2PiPPi_XSec', nickname = 'Lc2HHH_XSec',
                                                      inputs=[XSec_LcpToPimPpPip],shared=True)],
                  # The untagged D->KPi line
                  'D02KPi_XSecTurbo'         : [MassFilter('D02KPi_XSec',nickname='D02HH_XSec',
                                                      inputs=[XSec_D02KPi],shared=True)]
                 }
        # Now the Sigma_c0,++->Lambda_c(pKpi)pi line
        # Because of the mass window this also catches some excited Xi_c
        stages['Sigmac_2LcPi_XSecTurbo']      = [TagDecay('Sigmac_2LcPi_XSec',
                                                     ["[Sigma_c0 -> Lambda_c+ pi-]cc",
                                                      "[Sigma_c++ -> Lambda_c+ pi+]cc"],
                                                     inputs = [ stages["Lc2KPPi_XSecTurbo"][0],
                                                                SharedSoftTagChild_pi ], 
                                                     nickname = 'Sigmac_TAG_XSec', shared=True) ]
        # Now the D*->D0pi line for the D0->KPi case
        stages['Dst_2D0Pi_D02KPi_XSecTurbo']  = [TagDecay('Dst_2D0Pi_D02KPi_XSec',
                                                     ["[D*(2010)+ -> D0 pi+]cc"],
                                                     inputs = [ stages["D02KPi_XSecTurbo"][0],
                                                                SharedSoftTagChild_pi ], 
                                                     nickname = 'D0_TAG_XSec', shared=True) ]

        # Now the D*->D0pi line for the D0->K3Pi case
        stages['Dst_2D0Pi_D02K3Pi_XSecTurbo'] = [TagDecay('Dst_2D0Pi_D02K3Pi_XSec',
                                                     ["[D*(2010)+ -> D0 pi+]cc"],
                                                     inputs = [ MassFilter('D02K3Pi_XSec',nickname='D02HHHH_XSec',
                                                                           inputs=[XSec_D02K3Pi],shared=True),
                                                                SharedSoftTagChild_pi ], 
                                                     nickname='D0_TAG_XSec', shared=True) ]

        # Now the lines tagged with a photon or pi0
        stages['Dst_2D0Pi0_D02KPi_XSecTurbo']    = [TagDecayWithNeutral('Dst_2D0Pi0_D02KPi_XSec',
                                                                   ["D*(2007)0 -> D0 pi0","D*(2007)0 -> D~0 pi0"],
                                                                   inputs = [ stages["D02KPi_XSecTurbo"][0],
                                                                   SharedNeutralLowPtChild_pi0 ], 
                                                                   nickname='D_TAG_NEUTRAL_XSec', shared=True) ]
        stages['Dst_2D0Gamma_D02KPi_XSecTurbo']  = [TagDecayWithNeutral('Dst_2D0Gamma_D02KPi_XSec',
                                                                   ["D*(2007)0 -> D0 gamma","D*(2007)0 -> D~0 gamma"],
                                                                   inputs = [ stages["D02KPi_XSecTurbo"][0],
                                                                   SharedNeutralLowPtChild_gamma ],  
                                                                   nickname='D_TAG_NEUTRAL_XSec', shared=True) ]
        stages['Dst_2DsGamma_Ds2KKPi_XSecTurbo'] = [TagDecayWithNeutral('Dst_2DsGamma_Ds2KKPi_XSec',
                                                                   ["[D*_s+ -> D_s+ gamma]cc"],
                                                                   inputs = [ stages["Ds2KKPi_XSecTurbo"][0],
                                                                   SharedNeutralLowPtChild_gamma ], 
                                                                   nickname='D_TAG_NEUTRAL_XSec',shared=True) ]  
 

        return stages
