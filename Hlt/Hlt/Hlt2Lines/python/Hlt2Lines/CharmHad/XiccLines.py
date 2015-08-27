## Exclusive reconstruction of Xi_cc^+ and Xi_cc^++ to various decay modes,
##   including wrong-charge combinations for background studies.
## Adapted directly from the stripping configurable StrippingXicc.py.
##
## Defines sets of stages for lots of lines:
##   Signal mode lines:
##      'Xiccp2D0PpKmPip_D02KmPipTurbo'
##      'Xiccpp2D0PpKmPipPip_D02KmPipTurbo'
##      'Xiccp2DpPpKm_Dp2KmPipPipTurbo'
##      'Xiccpp2DpPpKmPip_Dp2KmPipPipTurbo'
##      'Xiccp2LcpKmPip_Lcp2PpKmPipTurbo'
##      'Xiccpp2LcpKmPipPip_Lcp2PpKmPipTurbo'
##      'Xiccp2Xic0Pip_Xic0ToPpKmKmPipTurbo'
##      'Xiccpp2Xic0PipPip_Xic0ToPpKmKmPipTurbo'
##      'Xiccp2XicpPimPip_Xicp2PpKmPipTurbo'
##      'Xiccpp2XicpPip_Xicp2PpKmPipTurbo'
##   'Wrong charge' lines for background studies:
##      'Xiccp2LcpKmPim_Lcp2PpKmPipTurbo'
##      'Xiccpp2LcpKmPimPip_Lcp2PpKmPipTurbo'
##      'Xiccp2DpPpKp_Dp2KmPipPipTurbo'
##      'Xiccpp2DpPpKpPip_Dp2KmPipPipTurbo'
##      'Xiccpp2DpPpKmPim_Dp2KmPipPipTurbo'
##      'Xiccp2Xic0Pim_Xic0ToPpKmKmPipTurbo'
##      'Xiccpp2Xic0PimPip_Xic0ToPpKmKmPipTurbo'
##      'Xiccp2XicpPimPim_Xicp2PpKmPipTurbo'
##      'Xiccpp2XicpPim_Xicp2PpKmPipTurbo'
##      'Xiccp2D0PpKmPim_D02KmPipTurbo'
##      'Xiccpp2D0PpKmPimPip_D02KmPipTurbo'
##   Doubly Cabibbo suppressed modes for background studies:
##      'Xiccp2LcpKpPim_Lcp2PpKmPipTurbo'
##      'Xiccpp2LcpKpPimPip_Lcp2PpKmPipTurbo'
##      'Xiccp2D0PpKpPim_D02KmPipTurbo'
##      'Xiccpp2D0PpKpPimPip_D02KmPipTurbo'
##
##
## @author Patrick Spradlin
__author__  = [ 'Patrick Spradlin' ]


from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
import math

class CharmHadXiccLines : # {
    def __init__( self ) : # {
        ## Slots for this set of lines; to be appended to the master set of
        ##   slots for the directory.
        self.slotDict = {
            "Xicc2HcH"   : {
                  'Trk_ALL_MIPCHI2DV_MIN': -1.0
                , 'AM_MIN'               :  3.1 * GeV
                , 'AM_MAX'               :  4.0 * GeV
                , 'ASUMPT_MIN'           :  2.0 * GeV
                , 'VCHI2PDOF_MAX'        : 20.0
                , 'BPVDIRA_MIN'          :  0.0
                , 'BPVVDCHI2_MIN'        : -1.0
                , 'BPVLTIME_MIN'         : -1.0e3 * picosecond
                , 'TisTosSpec'           : "Hlt1.*Track.*Decision%TOS"
            }
            , "Xicc2HcHH"   : {
                  'Trk_1of2_PT_MIN'      :  250.0 * MeV
                , 'Comb_AM_MIN'          :    3.1 * GeV
                , 'Comb_AM_MAX'          :    4.0 * GeV
                , 'Comb_ACHI2DOCA_MAX'   :   10.0
                , 'Comb_APT_MIN'         :    2.0 * GeV
                , 'Xicc_VCHI2PDOF_MAX'   :   30.0
                , 'Xicc_VZ1VZdiff_MIN'   :    0.01 * mm
                , 'Xicc_BPVVDCHI2_MIN'   :   -1.0
                , 'Xicc_acosBPVDIRA_MIN' : math.pi / 2.0
                , 'TisTosSpec'           : "Hlt1.*Track.*Decision%TOS"
            }
            , "Xicc2HcHHH"  : {
                  'Trk_2of3_PT_MIN'      :  250.0 * MeV
                , 'Trk_1of3_PT_MIN'      :  250.0 * MeV
                , 'Comb_AM_MIN'          :    3.1 * GeV
                , 'Comb_AM_MAX'          :    4.0 * GeV
                , 'Comb_ACHI2DOCA_MAX'   :   10.0
                , 'Comb_APT_MIN'         :    2.0 * GeV
                , 'Xicc_VCHI2PDOF_MAX'   :   60.0
                , 'Xicc_VZ1VZdiff_MIN'   :    0.01 * mm
                , 'Xicc_BPVVDCHI2_MIN'   :   -1.0
                , 'Xicc_acosBPVDIRA_MIN' : math.pi / 2.0
                , 'TisTosSpec'           : "Hlt1.*Track.*Decision%TOS"
            }
            , "Xicc2HcHHHH" : {
                  'Trk_3of4_PT_MIN'      :  250.0 * MeV
                , 'Trk_2of4_PT_MIN'      :  250.0 * MeV
                , 'Trk_1of4_PT_MIN'      :  250.0 * MeV
                , 'Comb_AM_MIN'          :    3.1 * GeV
                , 'Comb_AM_MAX'          :    4.0 * GeV
                , 'Comb_ACHI2DOCA_MAX'   :   10.0
                , 'Comb_APT_MIN'         :    2.0 * GeV
                , 'Xicc_VCHI2PDOF_MAX'   :   60.0
                , 'Xicc_VZ1VZdiff_MIN'   :    0.01 * mm
                , 'Xicc_BPVVDCHI2_MIN'   :   -1.0
                , 'Xicc_acosBPVDIRA_MIN' : math.pi / 2.0
                , 'TisTosSpec'           : "Hlt1.*Track.*Decision%TOS"
            }
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
            from Stages import SharedPromptChild_pi, SharedPromptChild_K
            from Stages import SharedPromptChild_p
            from Stages import Lc2HHH_LcpToKmPpPip
            from Stages import Xic2HHH_XicpToKmPpPip
            from Stages import Xic02PKKPi
            from Stages import D2HHH_DpToKmPipPip
            from Stages import D02HH_D0ToKmPip

            from Stages import Xicc2HcHHCombiner, Xicc2HcHHHCombiner
            from Stages import Xicc2HcHHHHCombiner
            from Stages import DetachedV0HCombiner


            ## Decay modes to Lambda_c+
            ## ------------------------------------------------------------ ##
            ## Signal modes
            XiccpToLcpKmPip = Xicc2HcHHCombiner( 'Comb'
                , decay = '[Xi_cc+ -> Lambda_c+ K- pi+]cc'
                , inputs = [ Lc2HHH_LcpToKmPpPip, SharedPromptChild_K, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHH' )
            XiccppToLcpKmPipPip = Xicc2HcHHHCombiner( 'Comb'
                , decay = '[Xi_cc++ -> Lambda_c+ K- pi+ pi+]cc'
                , inputs = [ Lc2HHH_LcpToKmPpPip, SharedPromptChild_K, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHHH' )

            ## 'Wrong charge' combinations for background distributions
            XiccpToLcpKmPim = Xicc2HcHHCombiner( 'Comb'
                , decay = '[Xi_cc+ -> Lambda_c+ K- pi-]cc'
                , inputs = [ Lc2HHH_LcpToKmPpPip, SharedPromptChild_K, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHH' )
            XiccppToLcpKmPimPip = Xicc2HcHHHCombiner( 'Comb'
                , decay = '[Xi_cc++ -> Lambda_c+ K- pi+ pi-]cc'
                , inputs = [ Lc2HHH_LcpToKmPpPip, SharedPromptChild_K, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHHH' )


            ## Doubly Cabibbo suppressed combinations for backgrounds
            XiccpToLcpKpPim = Xicc2HcHHCombiner( 'Comb'
                , decay = '[Xi_cc+ -> Lambda_c+ K+ pi-]cc'
                , inputs = [ Lc2HHH_LcpToKmPpPip, SharedPromptChild_K, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHH' )
            XiccppToLcpKpPimPip = Xicc2HcHHHCombiner( 'Comb'
                , decay = '[Xi_cc++ -> Lambda_c+ K+ pi- pi+]cc'
                , inputs = [ Lc2HHH_LcpToKmPpPip, SharedPromptChild_K, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHHH' )


            ## Decay modes to D+
            ## ------------------------------------------------------------ ##
            ## Signal modes
            XiccpToDpPpKm = Xicc2HcHHCombiner( 'Comb'
                , decay = '[Xi_cc+ -> D+ p+ K-]cc'
                , inputs = [ D2HHH_DpToKmPipPip, SharedPromptChild_p, SharedPromptChild_K ]
                , nickname = 'Xicc2HcHH' )
            XiccppToDpPpKmPip = Xicc2HcHHHCombiner( 'Comb'
                , decay = '[Xi_cc++ -> D+ p+ K- pi+]cc'
                , inputs = [ D2HHH_DpToKmPipPip, SharedPromptChild_p,
                             SharedPromptChild_K, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHHH' )

            ## 'Wrong charge' combinations for background distributions
            XiccpToDpPpKp = Xicc2HcHHCombiner( 'Comb'
                , decay = '[Xi_cc+ -> D+ p+ K+]cc'
                , inputs = [ D2HHH_DpToKmPipPip, SharedPromptChild_p, SharedPromptChild_K ]
                , nickname = 'Xicc2HcHH' )
            XiccppToDpPpKpPip = Xicc2HcHHHCombiner( 'Comb'
                , decay = '[Xi_cc++ -> D+ p+ K+ pi+]cc'
                , inputs = [ D2HHH_DpToKmPipPip, SharedPromptChild_p,
                             SharedPromptChild_K, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHHH' )
            XiccppToDpPpKmPim = Xicc2HcHHHCombiner( 'Comb'
                , decay = '[Xi_cc++ -> D+ p+ K- pi-]cc'
                , inputs = [ D2HHH_DpToKmPipPip, SharedPromptChild_p,
                             SharedPromptChild_K, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHHH' )


            ## Decay modes to D0
            ## ------------------------------------------------------------ ##
            ## Signal modes
            XiccpToD0PpKmPip = Xicc2HcHHHCombiner( 'Comb'
                , decay = '[Xi_cc+ -> D0 p+ K- pi+]cc'
                , inputs = [ D02HH_D0ToKmPip, SharedPromptChild_p,
                             SharedPromptChild_K, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHHH' )
            XiccppToD0PpKmPipPip = Xicc2HcHHHHCombiner( 'Comb'
                , decay = '[Xi_cc++ -> D0 p+ K- pi+ pi+]cc'
                , inputs = [ D02HH_D0ToKmPip, SharedPromptChild_p,
                             SharedPromptChild_K, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHHHH' )

            ## 'Wrong charge' combinations for background distributions
            XiccpToD0PpKmPim = Xicc2HcHHHCombiner( 'Comb'
                , decay = '[Xi_cc+ -> D0 p+ K- pi-]cc'
                , inputs = [ D02HH_D0ToKmPip, SharedPromptChild_p,
                             SharedPromptChild_K, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHHH' )
            XiccppToD0PpKmPimPip = Xicc2HcHHHHCombiner( 'Comb'
                , decay = '[Xi_cc++ -> D0 p+ K- pi+ pi-]cc'
                , inputs = [ D02HH_D0ToKmPip, SharedPromptChild_p,
                             SharedPromptChild_K, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHHHH' )

            ## Doubly Cabibbo suppressed combinations for backgrounds
            XiccpToD0PpKpPim = Xicc2HcHHHCombiner( 'Comb'
                , decay = '[Xi_cc+ -> D0 p+ K+ pi-]cc'
                , inputs = [ D02HH_D0ToKmPip, SharedPromptChild_p,
                             SharedPromptChild_K, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHHH' )
            XiccppToD0PpKpPimPip = Xicc2HcHHHHCombiner( 'Comb'
                , decay = '[Xi_cc++ -> D0 p+ K+ pi+ pi-]cc'
                , inputs = [ D02HH_D0ToKmPip, SharedPromptChild_p,
                             SharedPromptChild_K, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHHHH' )


            ## Decay modes to Xi_c+
            ## ------------------------------------------------------------ ##
            ## Signal modes
            XiccpToXicpPimPip = Xicc2HcHHCombiner( 'Comb'
                , decay = '[Xi_cc+ -> Xi_c+ pi+ pi-]cc'
                , inputs = [ Xic2HHH_XicpToKmPpPip, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHH' )
            XiccppToXicpPip = DetachedV0HCombiner( 'Comb'
                , decay = '[Xi_cc++ -> Xi_c+ pi+]cc'
                , inputs = [ Xic2HHH_XicpToKmPpPip, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcH' )

            ## 'Wrong charge' combinations for background distributions
            XiccpToXicpPimPim = Xicc2HcHHCombiner( 'Comb'
                , decay = '[Xi_cc+ -> Xi_c+ pi- pi-]cc'
                , inputs = [ Xic2HHH_XicpToKmPpPip, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHH' )
            XiccppToXicpPim = DetachedV0HCombiner( 'Comb'
                , decay = '[Xi_cc++ -> Xi_c+ pi-]cc'
                , inputs = [ Xic2HHH_XicpToKmPpPip, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcH' )


            ## Decay modes to Xi_c0
            ## ------------------------------------------------------------ ##
            ## Signal modes
            XiccpToXic0Pip = DetachedV0HCombiner( 'Comb'
                , decay = '[Xi_cc+ -> Xi_c0 pi+]cc'
                , inputs = [ Xic02PKKPi, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcH' )
            XiccppToXic0PipPip = Xicc2HcHHCombiner( 'Comb'
                , decay = '[Xi_cc++ -> Xi_c0 pi+ pi+]cc'
                , inputs = [ Xic02PKKPi, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHH' )

            ## 'Wrong charge' combinations for background distributions
            XiccpToXic0Pim = DetachedV0HCombiner( 'Comb'
                , decay = '[Xi_cc+ -> Xi_c0 pi-]cc'
                , inputs = [ Xic02PKKPi, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcH' )
            XiccppToXic0PimPip = Xicc2HcHHCombiner( 'Comb'
                , decay = '[Xi_cc++ -> Xi_c0 pi+ pi-]cc'
                , inputs = [ Xic02PKKPi, SharedPromptChild_pi ]
                , nickname = 'Xicc2HcHH' )


            ## The stages dictionary should be a clear two-column list from
            ##   which the lines defined in this module can be directly read.
            self.__stages = {
                ## Signal modes
                'Xiccp2LcpKmPip_Lcp2PpKmPipTurbo'     : [XiccpToLcpKmPip],
                'Xiccpp2LcpKmPipPip_Lcp2PpKmPipTurbo' : [XiccppToLcpKmPipPip],
                'Xiccp2DpPpKm_Dp2KmPipPipTurbo'       : [XiccpToDpPpKm],
                'Xiccpp2DpPpKmPip_Dp2KmPipPipTurbo'   : [XiccppToDpPpKmPip],
                'Xiccp2D0PpKmPip_D02KmPipTurbo'       : [XiccpToD0PpKmPip],
                'Xiccpp2D0PpKmPipPip_D02KmPipTurbo'   : [XiccppToD0PpKmPipPip],
                'Xiccp2XicpPimPip_Xicp2PpKmPipTurbo'  : [XiccpToXicpPimPip],
                'Xiccpp2XicpPip_Xicp2PpKmPipTurbo'    : [XiccppToXicpPip],
                'Xiccp2Xic0Pip_Xic0ToPpKmKmPipTurbo' : [XiccpToXic0Pip],
                'Xiccpp2Xic0PipPip_Xic0ToPpKmKmPipTurbo' : [XiccppToXic0PipPip],

                ## 'Wrong charge' background combinations
                'Xiccp2LcpKmPim_Lcp2PpKmPipTurbo'     : [XiccpToLcpKmPim],
                'Xiccpp2LcpKmPimPip_Lcp2PpKmPipTurbo' : [XiccppToLcpKmPimPip],
                'Xiccp2DpPpKp_Dp2KmPipPipTurbo'       : [XiccpToDpPpKp],
                'Xiccpp2DpPpKpPip_Dp2KmPipPipTurbo'   : [XiccppToDpPpKpPip],
                'Xiccpp2DpPpKmPim_Dp2KmPipPipTurbo'   : [XiccppToDpPpKmPim],
                'Xiccp2D0PpKmPim_D02KmPipTurbo'       : [XiccpToD0PpKmPim],
                'Xiccpp2D0PpKmPimPip_D02KmPipTurbo'   : [XiccppToD0PpKmPimPip],
                'Xiccp2XicpPimPim_Xicp2PpKmPipTurbo'  : [XiccpToXicpPimPim],
                'Xiccpp2XicpPim_Xicp2PpKmPipTurbo'    : [XiccppToXicpPim],
                'Xiccp2Xic0Pim_Xic0ToPpKmKmPipTurbo' : [XiccpToXic0Pim],
                'Xiccpp2Xic0PimPip_Xic0ToPpKmKmPipTurbo' : [XiccppToXic0PimPip],

                ## Doubly Cabibbo suppressed background combinations
                'Xiccp2LcpKpPim_Lcp2PpKmPipTurbo'    : [XiccpToLcpKpPim],
                'Xiccpp2LcpKpPimPip_Lcp2PpKmPipTurbo' : [XiccppToLcpKpPimPip],
                'Xiccp2D0PpKpPim_D02KmPipTurbo'       : [XiccpToD0PpKpPim],
                'Xiccpp2D0PpKpPimPip_D02KmPipTurbo'   : [XiccppToD0PpKpPimPip]
            }
        # }

        return self.__stages
    # }
# }
