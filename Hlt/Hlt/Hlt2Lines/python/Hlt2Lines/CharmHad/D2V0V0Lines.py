from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

## Temporary local definition of particle masses.  Used to adapt TagDecay cut
## variables from from Delta M to Q.
_local_m_pip = 139.57018 * MeV

class CharmHadD2V0V0Lines():
    def localcuts(self):
        return {
                 # The combiner for the CPV lines, separate for 2LL, LLDD, and 2DD
                 'D2KS0KS0_LL' : {
                                 'KS0_ALL_PT_MIN'           :  500.0 * MeV,
                                 'KS0_ALL_MIPCHI2DV_MIN'    :  9.0,
                                 'BPVVDCHI2_MIN'            :  20.0,
                                 'BPVLTIME_MIN'             :  0.2 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9994,
                                 'ASUMPT_MIN'               :  1500 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  1959 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  1949.0 * MeV,
                                 'TisTosSpec'               : "Hlt1.*Track.*Decision%TOS"
                                },
                  'D2KS0KS0_DD' : {
                                 'KS0_ALL_PT_MIN'           :  750.0 * MeV,
                                 'KS0_ALL_MIPCHI2DV_MIN'    :  4.0,
                                 'BPVVDCHI2_MIN'            :  10.0,
                                 'BPVLTIME_MIN'             :  0.2 * picosecond,
                                 'BPVDIRA_MIN'              :  0.9994,
                                 'ASUMPT_MIN'               :  2000 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  1959 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  1949.0 * MeV,
                                 'TisTosSpec'               : "Hlt1.*Track.*Decision%TOS"
                                }, 
                  # The tagger config
                  'D02V0V0_TAG_CPV' : { 'Q_AM_MIN'                 :  130.0 * MeV - _local_m_pip,
                                        'Q_M_MIN'                  :  130.0 * MeV - _local_m_pip,
                                        'Q_AM_MAX'                 :  165.0 * MeV - _local_m_pip,
                                        'Q_M_MAX'                  :  160.0 * MeV - _local_m_pip,
                                        'TagVCHI2PDOF_MAX'         :  25.0
                                      }
                }
    
    def locallines(self):
        from Stages import MassFilter, DetachedV0V0Combiner
        from Stages import CharmHadSharedKsLL, CharmHadSharedKsDD
        from Stages import TagDecay, SharedSoftTagChild_pi

        ## If either KS0 is DD,  use the DD dictionary, otherwise use LL.
        D2KS0KS0_2LLComb = DetachedV0V0Combiner( 'Comb'
                , decay = "D0 -> KS0 KS0"
                , inputs = [ CharmHadSharedKsLL ]
                , nickname = 'D2KS0KS0_LL' )

        D2KS0KS0_2LL = MassFilter('D2KS0KS0_LL', inputs=[ D2KS0KS0_2LLComb ])


        D2KS0KS0_LLDDComb = DetachedV0V0Combiner( 'Comb'
                , decay = "D0 -> KS0 KS0"
                , inputs = [ CharmHadSharedKsLL, CharmHadSharedKsDD ]
                , lldd = True, nickname = 'D2KS0KS0_DD')

        D2KS0KS0_LLDD = MassFilter('D2KS0KS0_DD', inputs = [ D2KS0KS0_LLDDComb ])


        D2KS0KS0_2DDComb = DetachedV0V0Combiner( 'Comb'
                , decay = "D0 -> KS0 KS0"
                , inputs = [ CharmHadSharedKsDD ]
                , nickname = 'D2KS0KS0_DD' )

        D2KS0KS0_2DD = MassFilter('D2KS0KS0_DD', inputs = [D2KS0KS0_2DDComb])

        # Now the tagged lines
        Dstp2D0Pip_D2KS0KS0_2LL = TagDecay('D02V0V0_TAG_CPV'
                        , ["D*(2010)- -> D0 pi-","D*(2010)+ -> D0 pi+"]
                        , inputs = [ D2KS0KS0_2LL, SharedSoftTagChild_pi ]
                        , ReFitPVs = True) 

        Dstp2D0Pip_D2KS0KS0_LLDD = TagDecay('D02V0V0_TAG_CPV'
                        , ["D*(2010)- -> D0 pi-","D*(2010)+ -> D0 pi+"]
                        , inputs = [ D2KS0KS0_LLDD, SharedSoftTagChild_pi ]
                        , ReFitPVs = True) 

        Dstp2D0Pip_D2KS0KS0_2DD = TagDecay('D02V0V0_TAG_CPV'
                        , ["D*(2010)- -> D0 pi-","D*(2010)+ -> D0 pi+"]
                        , inputs = [ D2KS0KS0_2DD, SharedSoftTagChild_pi ]
                        , ReFitPVs = True)

        stages = {
                  # First untagged
                  'D02KS0KS0_KS0LLTurbo'        : [D2KS0KS0_2LL],
                  'D02KS0KS0_KS0LL_KS0DDTurbo'  : [D2KS0KS0_LLDD],
                  'D02KS0KS0_KS0DDTurbo'        : [D2KS0KS0_2DD],
                  # Now tagged
                  'Dstp2D0Pip_D02KS0KS0_KS0LLTurbo' : [Dstp2D0Pip_D2KS0KS0_2LL],
                  'Dstp2D0Pip_D02KS0KS0_KS0LL_KS0DDTurbo' : [Dstp2D0Pip_D2KS0KS0_LLDD],
                  'Dstp2D0Pip_D02KS0KS0_KS0DDTurbo' : [Dstp2D0Pip_D2KS0KS0_2DD]
                 }
        return stages
