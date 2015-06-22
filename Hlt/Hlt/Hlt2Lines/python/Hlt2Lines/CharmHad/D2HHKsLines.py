from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2HHKshLines(Hlt2LinesConfigurableUser):
    def localcuts(self) :
        return {
                 # generic D0 --> HHKs cuts
                 'D02HHKsh' : {
                             'TisTosSpec'               : "Hlt1.*MVA.*Decision%TOS",
                             'AM_MIN'                   :  1740. * MeV,
                             'AM_MAX'                   :  1990. * MeV,
                             'ASUMPT_MIN'               :  1500 * MeV,
                             'D_PT_MIN'                 :  1800 * MeV,
                             'D_BPVDIRA_MIN'            :  0.9994,
                             'D_BPVVDCHI2_MIN'          :  20.0,
                             'D_VCHI2PDOF_MAX'          :  20.0,
                             'D_BPVLTIME_MIN'           :  0.1 * picosecond,
                             'HHMaxDOCA'                :  0.200 * mm,
                             'HHMass_MAX'               :  1500. * MeV,
                                          }
                ,'D02KshPiPi_LL' : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'D02KshPiPi_DD' : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'D02KshKPi_LL'  : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'D02KshKPi_DD'  : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'D02KshPiK_LL'  : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'D02KshPiK_DD'  : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'D02KshKK_LL'   : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'D02KshKK_DD'   : {
                                     'Mass_M_MIN'       : 1765 * MeV,
                                     'Mass_M_MAX'       : 1965 * MeV
                                   }
                ,'Dst2D0pi_D02HHKsh' : {
                                        'DeltaM_AM_MIN'    : 135 * MeV,
                                        'DeltaM_AM_MAX'    : 175 * MeV,
                                        'DeltaM_MIN'       : 135 * MeV,
                                        'DeltaM_MAX'       : 165 * MeV,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                        'TisTosSpec'       : "Hlt1.*MVA.*Decision%TOS",
                                       }
                            
        }

    def locallines(self):
        from Stages import MassFilter
        from Stages import D02KsPiPi_LL, D02KsPiPi_DD
        from Stages import D02KsKPi_LL, D02KsKPi_DD
        from Stages import D02KsKK_LL, D02KsKK_DD
        from Stages import TagDecay
        from Stages import SharedSoftTagChild_pi


        ## We can probably use only D*-tagged HHKs candidates, so let's make
        ## the corresponding D*-tagged lines.
        ##  
        ## Recall that the first argument of Dst2D0pi is the "name" used in
        ## the dictionary of cuts and the second argument is the "d0" list to
        ## be used.
        ##  
        ## All of the following lines use a common dictionary
        ## "name":  Dst2D0pi_D02HHKsh
        ## as the channels should be sufficiently similar to use common cuts.
        Dstp2D0Pip_D02KS0PimPip_KS0LL = TagDecay('Dst2D0pi_D02HHKsh'
                , decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"]
                , inputs = [D02KsPiPi_LL, SharedSoftTagChild_pi])

        Dstp2D0Pip_D02KS0PimPip_KS0DD = TagDecay('Dst2D0pi_D02HHKsh'
                , decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"]
                , inputs = [D02KsPiPi_DD, SharedSoftTagChild_pi])

        ## These lines will produce
        ## D*+ --> D0,pi+; D0 --> D0 --> K-,pi+,Kshort candidates 
        ## and also D*- --> D0bar,pi-;  D0bar --> K+,pi-,Kshort candidates;
        ## Ignoring CPV in K0 and K0bar decays to Kshort, these are CP
        ## conjugate decays.
        Dstp2D0Pip_D02KS0KmPip_KS0LL  = TagDecay('Dst2D0pi_D02HHKsh'
                , decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D~0 pi-"]
                , inputs = [D02KsKPi_LL, SharedSoftTagChild_pi])

        Dstp2D0Pip_D02KS0KmPip_KS0DD  = TagDecay('Dst2D0pi_D02HHKsh'
                , decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D~0 pi-"]
                , inputs = [D02KsKPi_DD, SharedSoftTagChild_pi])

        ## These lines will produce
        ## D*+ --> D0,pi+; D0 --> D0 --> K+,pi-,Kshort candidates
        ## and also D*- --> D0bar,pi-;  D0bar --> K-,pi+,Kshort candidates;
        ## Ignoring CPV in K0 and K0bar decays to Kshort, these are CP
        ## conjugate decays.
        ##
        ## Note that the code reuses the D02KsKPi_XX inputs where
        ## D0 --> Ks,K-,pi+ and D0bar --> Ks,K+,pi-.  To allow this reuse,
        ## with the charge assignments wanted here, the descriptors say that
        ## the D*+ decays to D0bar,pi+ and the D*- decays to D0,pi-.
        ## This is OK because the descriptor is not encoded in the candidate
        ## beyond Hlt.
        Dstp2D0Pip_D02KS0KpPim_KS0LL  = TagDecay('Dst2D0pi_D02HHKsh'
                , decay = ["D*(2010)+ -> D~0 pi+", "D*(2010)- -> D0 pi-"]
                , inputs = [D02KsKPi_LL, SharedSoftTagChild_pi])

        Dstp2D0Pip_D02KS0KpPim_KS0DD  = TagDecay('Dst2D0pi_D02HHKsh'
                , decay = ["D*(2010)+ -> D~0 pi+", "D*(2010)- -> D0 pi-"]
                , inputs = [D02KsKPi_DD, SharedSoftTagChild_pi])

        Dstp2D0Pip_D02KS0KmKp_KS0LL  = TagDecay('Dst2D0pi_D02HHKsh'
                , decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"]
                , inputs = [D02KsKK_LL, SharedSoftTagChild_pi])

        Dstp2D0Pip_D02KS0KmKp_KS0DD  = TagDecay('Dst2D0pi_D02HHKsh'
                , decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"]
                , inputs = [D02KsKK_DD, SharedSoftTagChild_pi])


        ## The stages dictionary should be a clear two-column list from
        ##   which the lines defined in this module can be directly read.
        stages = {'D02KS0PimPip_KS0LLTurbo'       : [MassFilter('D02KshPiPi_LL',inputs=[D02KsPiPi_LL])],
                  'D02KS0PimPip_KS0DDTurbo'       : [MassFilter('D02KshPiPi_DD',inputs=[D02KsPiPi_DD])],
                  'D02KS0KmPip_KS0LLTurbo'        : [MassFilter('D02KshKPi_LL',inputs=[D02KsKPi_LL])],
                  'D02KS0KmPip_KS0DDTurbo'        : [MassFilter('D02KshKPi_DD',inputs=[D02KsKPi_DD])],
                  'D02KS0KmKp_KS0LLTurbo'         : [MassFilter('D02KshKK_LL',inputs=[D02KsKK_LL])],
                  'D02KS0KmKp_KS0DDTurbo'         : [MassFilter('D02KshKK_DD',inputs=[D02KsKK_DD])],

                  'Dstp2D0Pip_D02KS0PimPip_KS0LL' : [Dstp2D0Pip_D02KS0PimPip_KS0LL],
                  'Dstp2D0Pip_D02KS0PimPip_KS0DD' : [Dstp2D0Pip_D02KS0PimPip_KS0DD],

                  'Dstp2D0Pip_D02KS0KmPip_KS0LL'  : [Dstp2D0Pip_D02KS0KmPip_KS0LL],
                  'Dstp2D0Pip_D02KS0KmPip_KS0DD'  : [Dstp2D0Pip_D02KS0KmPip_KS0DD],

                  'Dstp2D0Pip_D02KS0KpPim_KS0LL'  : [Dstp2D0Pip_D02KS0KpPim_KS0LL],
                  'Dstp2D0Pip_D02KS0KpPim_KS0DD'  : [Dstp2D0Pip_D02KS0KpPim_KS0DD],

                  'Dstp2D0Pip_D02KS0KmKp_KS0LL'   : [Dstp2D0Pip_D02KS0KmKp_KS0LL],
                  'Dstp2D0Pip_D02KS0KmKp_KS0DD'   : [Dstp2D0Pip_D02KS0KmKp_KS0DD]
                 }
        

        return stages
