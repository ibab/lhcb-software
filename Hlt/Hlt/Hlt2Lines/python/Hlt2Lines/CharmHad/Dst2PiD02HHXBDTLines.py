## Inclusive charm hadron lines that rely on small Delta m signatures of
## decays of short-lived charmed hadrons to long-lived charmed hadrons,
## e.g., D*(2010)+ -> D0 pi+.
##
## @author Patrick Spradlin, Emanuele Michielin
__author__  = [ 'Patrick Spradlin', 'Emanuele Michielin' ]

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm

class CharmHadDst2PiD02HHXBDTLines : # {
    def __init__(self, slotname = "Dst2PiD02HHXBDT") : # {
        ## Maybe just hard-code the slot name?
        self.__slotname = slotname

        ## Slots for this set of lines; to be appended to the master set of
        ##   slots for the directory.
        self.slotDict = { self.__slotname : {
                    'Trk_TRCHI2DOF_MAX'         : 5.0
                  , 'Trk_PT_MIN'                : 200.0 * MeV
                  , 'Trk_MIPCHI2DV_MIN'         : 4.0
                  , 'Spi_TRCHI2DOF_MAX'         : 100.0         # no cut
                  , 'Spi_PT_MIN'                : 000.0 * MeV   # no cut
                  , 'D0_VCHI2PDOF_MAX'          : 100
                  , 'D0_BPVVDCHI2_MIN'          : 20
                  , 'D0_BPVCORRM_MAX'           : 2100.0 * MeV
                  , 'Dst_VCHI2PDOF_MAX'         : 100.0
                  , 'Dst_PT_MIN'                : 2.0 * GeV
                  , 'Dst_M_MAX'                 : 2500.0 * MeV
                  , 'Dst_D0_DeltaM_MAX'         : 800.0 * MeV

                  , 'BDT_Lookup_Filename'       : "Hlt2Dst2PiD02HHX_BDTParams_v0r0.txt"
                  , 'BDT_Threshold'             : 1.37

                  , 'BDT_Lookup_VarMap' : {
                                "D0FD"          : "CHILDFUN(BPVVD, 1)"
                                , "SLPCOSTHETA" : "LV02"
                                , "TRSUMPT"     : "CHILDFUN(CHILDFUN(PT, 1) + CHILDFUN(PT, 2), 1)"
                                , "DSTFD"       : "BPVVD"
                                , "SLPPT"       : "CHILDFUN(PT, 2)"
                                , "D0CHI2"      : "CHILDFUN(VFASPF(VCHI2), 1)"
                                , "DSTDOCA"     : "DOCAMAX"
                              }

                  , 'TisTosSpec': { "Hlt1Track.*Decision%TOS":0 }
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
            from Stages import DetachedD02HHInclCombiner, Dstp2D0PiInclCombiner, BDTFilter

            ## Move to the shared filtered input particles when they make sense
            ##    to me.
            from Inputs import Hlt2NoPIDsPions, Hlt2NoPIDsKaons
            d0DecayDesc = [ "D0 -> K+ pi-", "D0 -> K- pi+"
                            , "D0 -> K+ pi+", "D0 -> K- pi-"
                            , "D0 -> K+ K-", "D0 -> pi+ pi-"
                          ]
            D02HHIncl = DetachedD02HHInclCombiner( 'CombD0', d0DecayDesc, [Hlt2NoPIDsPions, Hlt2NoPIDsKaons], self.__slotname )

            dstDecayDesc = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
            Dstp2D0PiInclComb = Dstp2D0PiInclCombiner( 'CombDst', dstDecayDesc, [D02HHIncl, Hlt2NoPIDsPions ], self.__slotname )

            Dstp2D0PiInclFilt = BDTFilter( 'FiltBDT', [ Dstp2D0PiInclComb ], self.slots()[self.__slotname] )


            self.__stages = { 'Dst2PiD02HHXBDT'  :  [ Dstp2D0PiInclFilt ]  }
        # }

        return self.__stages
    # }
# }

