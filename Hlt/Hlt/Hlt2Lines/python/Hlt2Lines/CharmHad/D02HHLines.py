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
    def __init__(self, slotname = "D02HH") : # {
        ## Maybe just hard-code the slot name?
        self.__slotname = slotname

        ## Slots for this set of lines; to be appended to the master set of
        ##   slots for the directory.
        self.slotDict = { self.__slotname : {
                          'Trk_PT_MIN'               : 800.0 * MeV
                        , 'Trk_P_MIN'                : 5.0  * GeV
                        , 'Trk_MIPCHI2DV_MIN'        : 2.0        # neuter
                        , 'Trk_TRCHI2DOF_MAX'        : 3.0        # neuter
                        , 'Pair_AMINDOCA_MAX'        : 0.10 * mm
                        , 'Trk_Max_APT_MIN'          : 1500.0 * MeV
                        , 'D0_BPVVDCHI2_MIN'         : 25.0       # neuter
                        , 'D0_BPVDIRA_MIN'           : 0.99985    # neuter
                        #, 'D0_acosBPVDIRA_MAX'       : 17.3 * mrad
                        , 'D0_VCHI2PDOF_MAX'         : 10.0       # neuter
                        , 'D0_PT_MIN'                : 2000.0 * MeV
                        ## Should these be in separate config blocks?
                        , 'Mass_M_MIN'               : 1815.0 * MeV
                        , 'Mass_M_MAX'               : 1915.0 * MeV
                        , 'WideMass_M_MIN'           : 1715.0 * MeV
                        , 'WideMass_M_MAX'           : 2015.0 * MeV
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
            from Stages import MassFilter, DetachedD02HHCombiner

            ## Move to the shared filtered input particles when they make sense
            ##    to me.
            from Inputs import Hlt2NoPIDsPions, Hlt2NoPIDsKaons
            D02KPiWideMass  = DetachedD02HHCombiner( 'D02KPi', decay = "[D0 -> K- pi+]cc"
                                    , inputs = [ Hlt2NoPIDsPions, Hlt2NoPIDsKaons ]
                                    , slotName = self.__slotname
                                    , shared = True )
            D02KKWideMass   = DetachedD02HHCombiner( 'D02KK', decay = "D0 -> K- K+"
                                    , inputs = [ Hlt2NoPIDsKaons ]
                                    , slotName = self.__slotname
                                    , shared = True )
            D02PiPiWideMass = DetachedD02HHCombiner( 'D02PiPi', decay = "D0 -> pi- pi+"
                                    , inputs = [ Hlt2NoPIDsPions ]
                                    , slotName = self.__slotname
                                    , shared = True )
        
            ## Hmm, having several MassFilter with the same name seems strange,
            ## but supposedly it does something sensible.
            ## <grumble, grumble, inscrutable obfuscated wrapper nonsense>
            D02KPi  = MassFilter( self.__slotname, inputs = [D02KPiWideMass] )
            D02KK   = MassFilter( self.__slotname, inputs = [D02KKWideMass] )
            D02PiPi = MassFilter( self.__slotname, inputs = [D02PiPiWideMass] )

            self.__stages = {   'D02KK'           : [D02KK]
                              , 'D02KPi'          : [D02KPi]
                              , 'D02PiPi'         : [D02PiPi]
                              , 'D02KKWideMass'   : [D02KKWideMass]
                              , 'D02KPiWideMass'  : [D02KPiWideMass]
                              , 'D02PiPiWideMass' : [D02PiPiWideMass]
            }
        # }

        return self.__stages
    # }
# }
