## Inclusive charm hadron lines that rely on small Delta m signatures of
## decays of short-lived charmed hadrons to long-lived charmed hadrons,
## e.g., D*(2010)+ -> D0 pi+.
##
## @author Patrick Spradlin, Emanuele Michielin
__author__  = [ 'Patrick Spradlin', 'Emanuele Michielin' ]

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm

class CharmHadDst2PiD02HHXBDTLines : # {
    def __init__( self ) : # {
        ## Slots for this set of lines; to be appended to the master set of
        ##   slots for the directory.
        self.slotDict = {
                ## Configuration for two-body seeds shared by inclusive lines
                'InclHc2HHX'         : {
                    'Trk_TRCHI2DOF_MAX'         : 2.0
                  , 'Trk_PT_MIN'                : 200.0 * MeV
                  , 'Trk_MIPCHI2DV_MIN'         : 7.0
                  , 'D0_VCHI2PDOF_MAX'          : 50
                  , 'D0_BPVVDCHI2_MIN'          : 20
                  , 'D0_BPVCORRM_MAX'           : 2400.0 * MeV
                  , 'D0_comb_PT_MIN'             : 1900 * MeV
                  , 'TisTosSpec': { "Hlt1Track.*Decision%TOS":0 }
                }
                ## Configuration for Hc + pion combinatorics shared by lines
                , 'InclHcst2PiHc2HHX'  : {
                    'Spi_TRCHI2DOF_MAX'         : 3.0       
                  , 'Spi_PT_MIN'                : 150.0 * MeV   
                  , 'Dst_VCHI2PDOF_MAX'         : 50.0
                  , 'Dst_PT_MIN'                : 2.0 * GeV
                  , 'Dst_M_MAX'                 : 2700.0 * MeV
                  , 'Dst_D0_DeltaM_MAX'         : 500.0 * MeV
                }
                ## Configuration for D*+ specific filtering
                , 'InclDst2PiD02HHX'   : {
                    'D0_BPVVDCHI2_MIN'          : 20
                  , 'D0_BPVCORRM_MAX'           : 2100.0 * MeV
                  , 'Dst_M_MAX'                 : 2300.0 * MeV
                  , 'Dst_D0_DeltaM_MAX'         : 350.0 * MeV
                  , 'BDT_Lookup_Filename'       : "Hlt2Dst2PiD02HHX_BDTParams_v1r0.txt"
                  , 'BDT_Threshold'             : 1.33

                  , 'BDT_Lookup_VarMap' : {
                                "D0FD"          : "CHILDFUN(BPVVD, 1)"
                                , "SLPCOSTHETA" : "LV02"
                                , "TRSUMPT"     : "CHILDFUN(CHILDFUN(PT, 1) + CHILDFUN(PT, 2), 1)"
                                , "DSTFD"       : "BPVVD"
                                , "SLPPT"       : "CHILDFUN(PT, 2)"
                                , "D0CHI2"      : "CHILDFUN(VFASPF(VCHI2), 1)"
                                , "DSTDOCA"     : "DOCAMAX"
                              }
                }
                ## Configuration for Sigc specific filtering
                , 'InclSigc2PiLc2HHX'  : {
                    'D0_BPVVDCHI2_MIN'          : 20
                  , 'D0_BPVCORRM_MAX'           : 2400.0 * MeV
                  , 'Dst_M_MAX'                 : 2700.0 * MeV
                  , 'Dst_D0_DeltaM_MAX'         : 500.0 * MeV
                  , 'BDT_Lookup_Filename'       : "Hlt2Sigc2PiLc2HHX_BDTParams_v1r0.txt"
                  , 'BDT_Threshold'             : 1.345

                  , 'BDT_Lookup_VarMap' : {
                                "D0FD"          : "CHILDFUN(BPVVD, 1)"
                                , "SLPCOSTHETA" : "LV02"
                                , "TRSUMPT"     : "CHILDFUN(CHILDFUN(PT, 1) + CHILDFUN(PT, 2), 1)"
                                , "DSTFD"       : "BPVVD"
                                , "SLPPT"       : "CHILDFUN(PT, 2)"
                                , "D0CHI2"      : "CHILDFUN(VFASPF(VCHI2), 1)"
                                , "DSTDOCA"     : "DOCAMAX"
                              }
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
            from Stages import InclHcst2PiHc2HHX
            from Stages import InclHcst2PiHc2HHXFilter

            ## Final kinematic filtering
            InclDst2PiD02HHX = InclHcst2PiHc2HHXFilter( 'Dst2PiD0'
                , inputs = [ InclHcst2PiHc2HHX ]
                , nickname = 'InclDst2PiD02HHX' )

            InclSigc2PiLc2HHX = InclHcst2PiHc2HHXFilter( 'Sigc2PiLc'
                , inputs = [ InclHcst2PiHc2HHX ]
                , nickname = 'InclSigc2PiLc2HHX' )


            from Stages import BDTFilter
            Dstp2D0PiInclFilt = BDTFilter( 'FiltBDT', [ InclDst2PiD02HHX ], self.slots()['InclDst2PiD02HHX'] )
            Sigc2LcPiInclFilt = BDTFilter( 'FiltBDT', [ InclSigc2PiLc2HHX ], self.slots()['InclSigc2PiLc2HHX'] )


            self.__stages = { 'InclDst2PiD02HHXBDT'  :  [ Dstp2D0PiInclFilt ]
                              , 'InclSigc2PiLc2HHXBDT'  :  [ Sigc2LcPiInclFilt ]  }
        # }

        return self.__stages
    # }
# }

