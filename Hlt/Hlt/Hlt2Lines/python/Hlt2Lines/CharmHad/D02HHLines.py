## Exclusive reconstruction of D0 -> h- h'+ modes, adapted directly from
## the former Hlt2CharmHadD02HHLines.py configurable.
##
## Defines sets of stages for lots of lines:
##      'D02KmKpTurbo'
##      'D02KmPipTurbo'
##      'D02PimPipTurbo'
##      'Dstp2D0Pip_D02KmKpTurbo'
##      'Dstp2D0Pip_D02PimPipTurbo'
##      'Dstp2D0Pip_D02KmPipTurbo'
##      'Dstp2D0Pip_D02KpPimTurbo'
##      'Dstp2D0Pip_D02KmKp_LTUNBTurbo'
##      'Dstp2D0Pip_D02PimPip_LTUNBTurbo'
##      'Dstp2D0Pip_D02KmPip_LTUNBTurbo'
##      'Dstp2D0Pip_D02KpPim_LTUNBTurbo'
##
## @author Patrick Spradlin
__author__  = [ 'Patrick Spradlin' ]


from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm


## Temporary local definition of pion mass.  Here to ease the transition
## of cut variables from Delta M to Q.
_local_m_pip = 139.57018 * MeV
_local_m_mup = 105.65837 * MeV


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
                                        'Q_AM_MIN'          :  130.0 * MeV - _local_m_pip,
                                        'Q_M_MIN'           :  130.0 * MeV - _local_m_pip,
                                        'Q_AM_MAX'          :  165.0 * MeV - _local_m_pip,
                                        'Q_M_MAX'           :  160.0 * MeV - _local_m_pip,
                                        'TagVCHI2PDOF_MAX'  :  25.0
                                      }  

        # The muon tagger config
        self.slotDict['DSTP_MUTAG_CPV'] = { 
                                        'Q_AM_MIN'                 :  100.0 * MeV - _local_m_mup,
                                        'Q_M_MIN'                  :  100.0 * MeV - _local_m_mup,
                                        'Q_AM_MAX'                 :  5000. * MeV - _local_m_mup,
                                        'Q_M_MAX'                  :  5000. * MeV - _local_m_mup,
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
            from Stages import D0_TAG_CPV_Dstp2D0Pip_D02KmPip
            from Stages import TagDecay, SharedSoftTagChild_pi, SharedSoftTagChild_mu

            ## Create D*+ -> D0 pi+

            ## I (P.S.) want to make this very easy to trace, but some
            ##   automation would help to ensure consistency.  Need to
            ##   consider better patterns.  For now, i have gone with making
            ##   it as clear as possible.

            ## Hmm, having several MassFilter with the same name seems strange,
            ##   but the automatic name mangling does something sensible since
            ##   these are not shared instances.
            ## <grumble, grumble, inscrutable obfuscated wrapper nonsense>

            ## D0 -> K pi reconstructed as "[D0 -> K- pi+]cc
            ## D0_TAG_CPV_Dstp2D0Pip_D02KmPip is a shared instance defined in
            ##   Stages.py because it is also used in CharmSpectroscopyLines.py
            Dstp2D0Pip_D02KpPim = TagDecay('CharmHadDstp2D0Pip_D02KpPim'
                        , ["[D*(2010)- -> D0 pi-]cc"]
                        , inputs = [ D02HH_D0ToKmPip, SharedSoftTagChild_pi ]
                        , shared = True
                        , nickname = 'D0_TAG_CPV'
                        , ReFitPVs = True)

            ## Self-conjugate final state reconstructed only as D0
            Dstp2D0Pip_D02KmKp  = TagDecay('CharmHadDstp2D0Pip_D02KmKp'
                        , decay = ["D*(2010)+ -> D0 pi+","D*(2010)- -> D0 pi-"]
                        , inputs = [ D02HH_D0ToKmKp, SharedSoftTagChild_pi ]
                        , shared = True
                        , nickname = 'D0_TAG_CPV'
                        , ReFitPVs = True)

            Dstp2D0Pip_D02PimPip = TagDecay('CharmHadDstp2D0Pip_D02PimPip'
                        , decay = ["D*(2010)+ -> D0 pi+","D*(2010)- -> D0 pi-"]
                        , inputs = [ D02HH_D0ToPimPip, SharedSoftTagChild_pi ]
                        , shared = True
                        , nickname = 'D0_TAG_CPV'
                        , ReFitPVs = True)

            ## Muon tagged lines
            MuTag_DstpMu_Dstp2D0Pip_D02KpPim = TagDecay('DSTP_MUTAG_CPV'
                        , ["[B0 -> D*(2010)- mu+]cc","[B0 -> D*(2010)- mu-]cc"]
                        , inputs = [ Dstp2D0Pip_D02KpPim, SharedSoftTagChild_mu ]
                        , ReFitPVs = True)
            MuTag_DstpMu_Dstp2D0Pip_D02KmPip = TagDecay('DSTP_MUTAG_CPV'
                        , ["[B0 -> D*(2010)- mu+]cc","[B0 -> D*(2010)- mu-]cc"]
                        , inputs = [ D0_TAG_CPV_Dstp2D0Pip_D02KmPip, SharedSoftTagChild_mu ]
                        , ReFitPVs = True)
            MuTag_DstpMu_Dstp2D0Pip_D02KmKp  = TagDecay('DSTP_MUTAG_CPV'
                        , decay = ["[B0 -> D*(2010)- mu+]cc","[B0 -> D*(2010)- mu-]cc"]
                        , inputs = [ Dstp2D0Pip_D02KmKp, SharedSoftTagChild_mu ]
                        , ReFitPVs = True)
            MuTag_DstpMu_Dstp2D0Pip_D02PimPip = TagDecay('DSTP_MUTAG_CPV'
                        , decay = ["[B0 -> D*(2010)- mu+]cc","[B0 -> D*(2010)- mu-]cc"]
                        , inputs = [ Dstp2D0Pip_D02PimPip, SharedSoftTagChild_mu ]
                        , ReFitPVs = True)

        
            ## For decay-time-PDF unbiased D0s.
            Dstp2D0Pip_D02KmPip_LTUNB = TagDecay('D0_TAG_CPV'
                        , ["[D*(2010)+ -> D0 pi+]cc"]
                        , inputs = [ D02HH_D0ToKmPip_LTUNB, SharedSoftTagChild_pi ]
                        , ReFitPVs = True)

            Dstp2D0Pip_D02KpPim_LTUNB = TagDecay('D0_TAG_CPV'
                        , ["[D*(2010)- -> D0 pi-]cc"]
                        , inputs = [ D02HH_D0ToKmPip_LTUNB, SharedSoftTagChild_pi ]
                        , ReFitPVs = True)

            Dstp2D0Pip_D02KmKp_LTUNB = TagDecay('D0_TAG_CPV'
                        , ["D*(2010)+ -> D0 pi+","D*(2010)- -> D0 pi-"]
                        , inputs = [ D02HH_D0ToKmKp_LTUNB, SharedSoftTagChild_pi ]
                        , ReFitPVs = True)

            Dstp2D0Pip_D02PimPip_LTUNB = TagDecay('D0_TAG_CPV'
                        , ["D*(2010)+ -> D0 pi+","D*(2010)- -> D0 pi-"]
                        , inputs = [ D02HH_D0ToPimPip_LTUNB, SharedSoftTagChild_pi ]
                        , ReFitPVs = True)


            ## The stages dictionary should be a clear two-column list from
            ##   which the lines defined in this module can be directly read.
            self.__stages = {
                  'D02KmKpTurbo'                    : [D02HH_D0ToKmKp]
                , 'D02KmPipTurbo'                   : [D02HH_D0ToKmPip]
                , 'D02PimPipTurbo'                  : [D02HH_D0ToPimPip]

                ## Now add the tagged lines
                , 'Dstp2D0Pip_D02KmKpTurbo'         : [ Dstp2D0Pip_D02KmKp ]
                , 'Dstp2D0Pip_D02PimPipTurbo'       : [ Dstp2D0Pip_D02PimPip ]
                , 'Dstp2D0Pip_D02KmPipTurbo'        : [ D0_TAG_CPV_Dstp2D0Pip_D02KmPip ]
                , 'Dstp2D0Pip_D02KpPimTurbo'        : [ Dstp2D0Pip_D02KpPim ]

                ## The muon tagged D* lines
                , 'MuTag_DstpMu_Dstp2D0Pip_D02KpPimTurbo' : [ MuTag_DstpMu_Dstp2D0Pip_D02KpPim ]
                , 'MuTag_DstpMu_Dstp2D0Pip_D02KmPipTurbo' : [ MuTag_DstpMu_Dstp2D0Pip_D02KmPip ]
                , 'MuTag_DstpMu_Dstp2D0Pip_D02KmKpTurbo'  : [ MuTag_DstpMu_Dstp2D0Pip_D02KmKp ]
                , 'MuTag_DstpMu_Dstp2D0Pip_D02PimPipTurbo': [ MuTag_DstpMu_Dstp2D0Pip_D02PimPip ]

                ## The tagged LTUNB lines
                , 'Dstp2D0Pip_D02KmKp_LTUNBTurbo'   : [ Dstp2D0Pip_D02KmKp_LTUNB ]
                , 'Dstp2D0Pip_D02PimPip_LTUNBTurbo' : [ Dstp2D0Pip_D02PimPip_LTUNB ]
                , 'Dstp2D0Pip_D02KmPip_LTUNBTurbo'  : [ Dstp2D0Pip_D02KmPip_LTUNB ]
                , 'Dstp2D0Pip_D02KpPim_LTUNBTurbo'  : [ Dstp2D0Pip_D02KpPim_LTUNB ]
            }
        # }

        return self.__stages
    # }
# }
