## Exclusive reconstruction of D0 -> h- h'+ modes, adapted directly from
## the former Hlt2CharmHadD02HHLines.py configurable.
##
## Defines sets of stages for 6 lines:
##      'D02KK'
##      'D02KPi'
##      'D02PiPi'
##      'D02KKWideMass'
##      'D02KPiWideMass'
##      'D02PiPiWideMass'
##
## @author Patrick Spradlin
__author__  = [ 'Patrick Spradlin' ]


from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm

class CharmHadD02HHLines : # {
    def __init__( self ) : # {
        ## Just to prevent typos in copying the strings.
        self.__slotname = "D02HH"

        ## Slots for this set of lines; to be appended to the master set of
        ##   slots for the directory.
        self.slotDict = { self.__slotname : {
                          'Pair_AMINDOCA_MAX'        : 0.10 * mm
                        , 'Trk_Max_APT_MIN'          : 1500.0 * MeV
                        , 'D0_BPVVDCHI2_MIN'         : 25.0       # neuter
                        , 'D0_BPVDIRA_MIN'           : 0.99985    # neuter
                        #, 'D0_acosBPVDIRA_MAX'       : 17.3 * mrad
                        , 'D0_VCHI2PDOF_MAX'         : 10.0       # neuter
                        , 'D0_PT_MIN'                : 2000.0 * MeV
                        , 'Comb_AM_MIN'              : 1715.0 * MeV
                        , 'Comb_AM_MAX'              : 2015.0 * MeV
                        ## These should be removed to input particle filtering
                        , 'Trk_ALL_PT_MIN'           : 800.0 * MeV
                        , 'Trk_ALL_P_MIN'            : 5.0  * GeV
                        , 'Trk_ALL_MIPCHI2DV_MIN'    : 4.0
                    }
                }

        # The tagger config
        self.slotDict['D0_TAG_CPV'] = { 
                                        'DeltaM_AM_MIN'            :  130.0 * MeV,
                                        'DeltaM_MIN'               :  130.0 * MeV,
                                        'DeltaM_AM_MAX'            :  165.0 * MeV,
                                        'DeltaM_MAX'               :  160.0 * MeV,
                                        'TagVCHI2PDOF_MAX'         :  25.0
                                      }  

        # The lifetime unbiased lines, these are all tagged, TOS linked to the
        # HLT1 lifetime unbiased lines
        self.slotDict['D02HH_LTUNB'] = {
                                        'TisTosSpec'               : "Hlt1CalibTracking.*Decision%TOS",
                                        'Trk_ALL_PT_MIN'           :  800 * MeV,
                                        'Trk_Max_APT_MIN'          :  1200 * MeV,
                                        'D0_VCHI2PDOF_MAX'         :  10.0,
                                        'D0_BPVLTIME_MIN'          :  0.25 * picosecond,
                                        'D0_BPVDIRA_MIN'           :  0.99,
                                        'Pair_AMINDOCA_MAX'        :  0.1 * mm,
                                        'D0_PT_MIN'                :  2000 * MeV,
                                        'AM_MIN'                   :  1774 * MeV,
                                        'AM_MAX'                   :  1954 * MeV,
                                        'Mass_M_MIN'               :  1804.0 * MeV,
                                        'Mass_M_MAX'               :  1924.0 * MeV,
                                        }

        self.__stages = { }
    # }


    def slots(self) : # {
        return self.slotDict
    # }


    def stages(self) : # {
        ## Define the sets of stages for the lines.
        ## NOTE!!!  This method should only be called from within the
        ##    __apply_configuration__() method of a class that inherits from
        ##    Hlt2LinesConfigurableUser.
        ## I would much prefer that this is done in the class constructor.
        ## However, the import from Stages will not work outside of the
        ##   __apply_configuration__ method of the Hlt2LinesConfigurableUser
        ##   that uses these lines.
        if len(self.__stages) == 0 : # {
            from Stages import D02HH_D0ToKmPip, D02HH_D0ToKmKp, D02HH_D0ToPimPip
            from Stages import D02HH_D0ToKmPip_LTUNB, D02HH_D0ToKmKp_LTUNB, D02HH_D0ToPimPip_LTUNB
            from Stages import TagDecay,SharedSoftTagChild_pi

            ## Hmm, having several MassFilter with the same name seems strange,
            ## but supposedly it does something sensible.
            ## <grumble, grumble, inscrutable obfuscated wrapper nonsense>
            self.__stages = {   'D02KKTurbo'   : [D02HH_D0ToKmKp]
                              , 'D02KPiTurbo'  : [D02HH_D0ToKmPip]
                              , 'D02PiPiTurbo' : [D02HH_D0ToPimPip]
            }
            # Now add the tagged lines
            self.__stages['Dst_2D0Pi_D02KKTurbo']      = [TagDecay('D0_TAG_CPV', ["D*(2010)+ -> D0 pi+","D*(2010)- -> D0 pi-"],
                                                             inputs = [ self.__stages['D02KK'][0],
                                                                        SharedSoftTagChild_pi ])]
            self.__stages['Dst_2D0Pi_D02PiPiTurbo']    = [TagDecay('D0_TAG_CPV', ["D*(2010)+ -> D0 pi+","D*(2010)- -> D0 pi-"],
                                                             inputs = [ self.__stages['D02PiPi'][0],
                                                                         SharedSoftTagChild_pi ])] 
            self.__stages['Dst_2D0Pi_D02KPiCF']   = [TagDecay('D0_TAG_CPV', ["[D*(2010)+ -> D0 pi+]cc"],
                                                             inputs = [ self.__stages['D02KPi'][0],
                                                                        SharedSoftTagChild_pi ])]
            self.__stages['Dst_2D0Pi_D02KPiDCS']  = [TagDecay('D0_TAG_CPV', ["[D*(2010)- -> D0 pi-]cc"],
                                                            inputs = [ self.__stages['D02KPi'][0],
                                                                       SharedSoftTagChild_pi ])]
            # The tagged LTUNB lines
            self.__stages['Dst_2D0Pi_D02KK_LTUNBTurbo']      = [TagDecay('D0_TAG_CPV', ["D*(2010)+ -> D0 pi+","D*(2010)- -> D0 pi-"],
                                                                   inputs = [ D02HH_D0ToKmKp_LTUNB,
                                                                              SharedSoftTagChild_pi ])]
            self.__stages['Dst_2D0Pi_D02PiPi_LTUNBTurbo']    = [TagDecay('D0_TAG_CPV', ["D*(2010)+ -> D0 pi+","D*(2010)- -> D0 pi-"],
                                                                   inputs = [ D02HH_D0ToPimPip_LTUNB,
                                                                              SharedSoftTagChild_pi ]) ] 
            self.__stages['Dst_2D0Pi_D02KPiCF_LTUNBTurbo']   = [TagDecay('D0_TAG_CPV', ["[D*(2010)+ -> D0 pi+]cc"],
                                                                   inputs = [ D02HH_D0ToKmPip_LTUNB,
                                                                              SharedSoftTagChild_pi ])]
            self.__stages['Dst_2D0Pi_D02KPiDCS_LTUNBTurbo']  = [TagDecay('D0_TAG_CPV', ["[D*(2010)- -> D0 pi-]cc"],
                                                                   inputs = [ D02HH_D0ToKmPip_LTUNB,
                                                                              SharedSoftTagChild_pi ])]
        # }

        return self.__stages
    # }
# }
